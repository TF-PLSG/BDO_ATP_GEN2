using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Configuration;
using System.Diagnostics;
using System.Runtime.Remoting;
using System.Web;

namespace FSOAP
{
	public class StateObject 
	{	
		public Socket workSocket = null;	
		public const int BufferSize = 5024;	
		public byte[] buffer = new byte[BufferSize];		
		public StringBuilder sb = new StringBuilder();
	}

	public class AsynchronousSocketListener 
	{  
		public static string data = null;		
		public static ManualResetEvent allDone = new ManualResetEvent(false);
		public EventLog m_eventLog = null ;

		public  void StartListening(EventLog m_eventLog) 
		{
			// RemotingConfiguration.Configure( "FSOAP.exe.config" );

			IPHostEntry ipHostInfo = Dns.Resolve(Dns.GetHostName());
			IPAddress ipAddress = ipHostInfo.AddressList[0];
			String atpIPAddress =  ConfigurationSettings.AppSettings["atp.server"];	
			
			Int32 atpPort = 1600;
			try
			{
				atpPort = Int32.Parse(ConfigurationSettings.AppSettings["atp.port"]);
			}
			catch(Exception e)
			{				
				Console.WriteLine("Source : " + e.Source);
				Console.WriteLine("Message : " + e.Message);				
			}
			try
			{
				ipAddress = IPAddress.Parse(atpIPAddress);
			}
			catch(Exception e)
			{				
				Console.WriteLine("Source : " + e.Source);
				Console.WriteLine("Message : " + e.Message);
				ipAddress = ipHostInfo.AddressList[0];
			}
			
			EventLog.WriteEntry("FSOAP","FSOAP listening on the IP Address="+ atpIPAddress +" and port=" + atpPort, System.Diagnostics.EventLogEntryType.Information);
			
			atpConsoleTxnDisplay = ConfigurationSettings.AppSettings["atp.consoleTxnDisplay"];
			Console.WriteLine( "Console Txn Display: " + atpConsoleTxnDisplay );

			IPEndPoint localEndPoint = new IPEndPoint(ipAddress, atpPort);
			Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );			
			try 
			{
				/* Bind our new socket (listener) with our IP Address and Port
				 * that we will be listening on.
				 * Then, begin the listener.  The '100' represents maximum
				 * length of the pending connections queue; it is the number of
				 * incoming connections that can be queued for acceptance.  We
				 * should only have a single connection, so 100 is overkill.
				 */
				listener.Bind(localEndPoint);
				listener.Listen(100);

				Console.WriteLine("Waiting for a connection...");

				while (true) 
				{
					if ( allDone.Reset() )
					{
						/* Begin operation to accept an incoming connection attempt on listener.
						 * AsyncCallback says to call function "AcceptCallback" when a connection
						 * is made.
						 */
						listener.BeginAccept( 
							new AsyncCallback(AcceptCallback),
							listener );

						allDone.WaitOne();
					}
				}
			} 
			catch (Exception e) 
			{
				Console.WriteLine(e.ToString());				
			}

			Console.WriteLine("\nPress ENTER to continue...");
			Console.Read();
        
		}

		public  void AcceptCallback(IAsyncResult ar) 
		{
			try
			{
				allDone.Set();
				Socket listener = (Socket) ar.AsyncState;
				Socket handler = listener.EndAccept(ar);
			
				StateObject state = new StateObject();
				state.workSocket = handler;
				
				Console.WriteLine("Connected. Listening...");

				handler.BeginReceive( state.buffer, 0, StateObject.BufferSize, 0,
					new AsyncCallback(ReadCallback), state);
			}
			catch(Exception e)
			{
				Console.WriteLine(e.ToString());		
			}
		}

		public  void ReadCallback(IAsyncResult ar) 
		{
			StateObject state = (StateObject) ar.AsyncState;
			Socket handler = state.workSocket;
			int bytesRead = 0;
			try
			{
				// Read data from the client socket. 
				bytesRead = handler.EndReceive(ar);
			}
			catch(Exception e)
			{
				Console.WriteLine(e.ToString());		
				return;
			}
			try
			{
				if (bytesRead > 0) 
				{
					int dataLength =((state.buffer[0] << 8) | (state.buffer[1] & 0x000000ff)) & 0x0000ffff;				
					byte[] message = new byte[dataLength];

					if ( atpConsoleTxnDisplay == "on" )
					{
						System.Array.Copy(state.buffer,0,message,0,200);					/* debug_code */
						string debugData = Encoding.ASCII.GetString(message,0,200);			/* debug_code */
						Console.WriteLine( "\nTxn from ATP to FSoap: " + debugData + "\n");	/* debug_code */
					}

					/* Sanity Check - Length from message header should be reasonable. */
					if ( (dataLength > 10) && (dataLength < StateObject.BufferSize) )
					{
						System.Array.Copy(state.buffer,2,message,0,dataLength);
						string incomingData = Encoding.ASCII.GetString(message,0,dataLength);

						if ( atpConsoleTxnDisplay == "on" )
							Console.WriteLine(incomingData);

						FSOAP.WebReference.IncomingQueue service = new FSOAP.WebReference.IncomingQueue();

						string s = service.SendAsString(incomingData);
						byte[] byteData = Encoding.ASCII.GetBytes(s);
						byte[] outMessage = new byte[byteData.Length + 2];
						outMessage[0] = (byte)(byteData.Length >> 8);
						outMessage[1] = (byte)(byteData.Length);

						Array.Copy(byteData, 0, outMessage, 2, byteData.Length);					

						/* Send response from Fraudguard back to ATP. */
						Send(handler, outMessage, outMessage.Length);
					}
					else
					{
						Console.WriteLine("Read partial message off ATP socket: {0} bytes. Not sent to Fraudguard", bytesRead );

						System.Array.Copy(state.buffer,0,message,0,200);
						string debugData = Encoding.ASCII.GetString(message,0,200);
						Console.WriteLine( "\nTxn from ATP to FSoap: " + debugData + "\n");	
					}

					/* Cursively wait and handle requests from ATP. */
					state = new StateObject();
					state.workSocket = handler;
					handler.BeginReceive( state.buffer, 0, StateObject.BufferSize, 0,
						new AsyncCallback(ReadCallback), state);
				}
				else
				{
					Console.WriteLine("Disconnected. Waiting for a connection...");
				}
			}
			catch(Exception e)
			{
				Console.WriteLine(e.ToString());

				/*+--------------------------------+
				  | Do not exit on this exception. |
				  +--------------------------------+*/
				
				/* Print the message that is causing the problem. */
				int dataLength =((state.buffer[0] << 8) | (state.buffer[1] & 0x000000ff)) & 0x0000ffff;
				if ( (dataLength <= 0) || (dataLength > StateObject.BufferSize) )
					dataLength = 900;

				byte[] message = new byte[dataLength];
				System.Array.Copy(state.buffer,2,message,0,dataLength);
				string debugData = Encoding.ASCII.GetString(message,0,dataLength);
				Console.WriteLine( "\nTxn from ATP to FSoap: " + debugData + "\n");	

				/* Cursively "wait and handle" requests from ATP. */
				state = new StateObject();
				state.workSocket = handler;
				handler.BeginReceive( state.buffer, 0, StateObject.BufferSize, 0,
					new AsyncCallback(ReadCallback), state);
			}
		}

		public  void serviceCallBack(IAsyncResult AsyncResult)
		{
			FSOAP.WebReference.IncomingQueue pf = (FSOAP.WebReference.IncomingQueue) AsyncResult.AsyncState;
			FSOAP.WebReference.SendResult sendResult =  pf.EndSend(AsyncResult);
			System.Console.WriteLine( sendResult.ToString( ) );
		}    

		private  void Send(Socket handler, byte[] byteData, int bytesSent ) 
		{
			try 
			{
				// Convert the string data to byte data using ASCII encoding.
				//byte[] byteData = Encoding.ASCII.GetBytes(data);
				handler.Send(byteData, 0, bytesSent, SocketFlags.None);
				Console.WriteLine("Sent {0} bytes to client.", bytesSent);
			} 
			catch (Exception e) 
			{
				Console.WriteLine(e.ToString());
			}
		}

		[STAThread]
		public static int Main(String[] args) 
		{
			EventLog m_eventLog = new EventLog();
			m_eventLog.Log  = "Application";
			AsynchronousSocketListener asyncSocketList = new AsynchronousSocketListener();
			asyncSocketList.StartListening(m_eventLog);
			return 0;
		}
		public String atpConsoleTxnDisplay = "off";
	}
}
