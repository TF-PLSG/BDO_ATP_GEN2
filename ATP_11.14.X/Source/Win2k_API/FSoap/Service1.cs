using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Threading;

namespace FSOAP
{
#if DEBUG
	public class FSOAP
	{
		[STAThread]
		static void Main()
		{
			EventLog m_eventLog = new EventLog();
			//m_eventLog.Log = "Application";
			m_eventLog.Source = "FSOAP";
//			if (!EventLog.SourceExists(m_eventLog.Source))
//			{
//				EventLog.CreateEventSource(m_eventLog.Source,m_eventLog.Log);
//			}
			//m_eventLog.Clear();
			m_eventLog.WriteEntry("FSOAP Server Started in debug mode");	

			AsynchronousSocketListener asyncSocketList = new AsynchronousSocketListener();
			asyncSocketList.StartListening(m_eventLog);

			Console.WriteLine("Press Enter to quit");
			Console.ReadLine();
			
		}
	}
#else
	public class FSOAP : System.ServiceProcess.ServiceBase
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		EventLog m_eventLog = new EventLog();
		public FSOAP()
		{
			// This call is required by the Windows.Forms Component Designer.
			InitializeComponent();			
			// TODO: Add any initialization after the InitComponent call
		}

		// The main entry point for the process
		static void Main()
		{
			System.ServiceProcess.ServiceBase[] ServicesToRun;
	
			// More than one user Service may run within the same process. To add
			// another service to this process, change the following line to
			// create a second service object. For example,
			//
			//   ServicesToRun = new System.ServiceProcess.ServiceBase[] {new Service1(), new MySecondUserService()};
			//
			ServicesToRun = new System.ServiceProcess.ServiceBase[] { new FSOAP() };

			System.ServiceProcess.ServiceBase.Run(ServicesToRun);
		}

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// FSOAP
			// 
			this.ServiceName = "FSOAP";

		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary>
		/// Set things in motion so your service can do its work.
		/// </summary>
		protected override void OnStart(string[] args)
		{
			AutoLog = false;
			string logName = "Application";
			string source = "FSOAP Source";
			if (!EventLog.SourceExists(source))
			{
				EventLog.CreateEventSource(source,logName);
			}
			m_eventLog.Source = source;
			m_eventLog.Log  = logName;
			//m_eventLog.Clear();
			m_eventLog.WriteEntry("FSOAP Server Started");			
			
			(new AsynchronousSocketListener()).StartListening(m_eventLog);
		}
 
		/// <summary>
		/// Stop this service.
		/// </summary>
		protected override void OnStop()
		{
			m_eventLog.WriteEntry("In onStop.");

		}
	}
#endif
}
