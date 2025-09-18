import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.ConnectException;
import java.util.Hashtable;
import java.util.Properties;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSession;
import javax.jms.TextMessage;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import logging.MessageTransactionLogging;

/**
 * @author 100454
 *
 */
public class MessageSender {

	
	private Context jndiContext;
	private QueueConnection conn;
	private QueueSession session;
	private MessageProducer msgProducer;
	private Properties prop;
	private final static Logger logger = LogManager.getLogger(MessageSender.class);
	public static boolean logsEnabled = false;
	boolean isConnAvailable = false;
	
	public Properties getProp() {
		return prop;
	}

	public void setProp(Properties prop) {
		this.prop = prop;
	}

	public MessageSender(){
		prop = new Properties();
		try {   
			String propFilePath = System.getenv("ASCENDENTROOT");
			enableMessageSenderLogs();
			if(propFilePath !=null){
				prop.load(new FileInputStream(propFilePath+"/config/app_properties.properties"));
			}else{
				if(MsgSenderThread.logsCreate){
					logger.info("There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
				}
				//Fine Call
				System.exit(-1);
			}
		} catch (IOException e) { e.printStackTrace(); } 
	}

	public void sendMessage(String message) throws Exception {
		sendTextMessage(message);
	}
	public void closeSession(){
		if(isConnAvailable){
			logger.info("Closing session for the current thread");
			try {
				session.close();
				conn.close();
				/*if(MsgSenderThread.logsCreate){
					logger.info("JMS Session closed");
				}*/
			} catch (JMSException jmse) {
				if(MsgSenderThread.logsCreate == true){
					logger.info("The Connection with the server was lost,Please Check!!!");
				}
			}
		}
	}

	private void sendTextMessage(String txtMsg) throws Exception {
		if(isConnAvailable){
			try {
				/*if(MsgSenderThread.logsCreate){
					logger.info("message length=="+txtMsg.length());
				}*/
				TextMessage textMsg = session.createTextMessage();
				textMsg.setText(txtMsg);
				/*if(MsgSenderThread.logsCreate){
					logger.info("message length after set=="+txtMsg.length());
				}*/
				msgProducer.send(textMsg);
				if(MsgSenderThread.logsCreate && prop.getProperty("displayTransactionMsg").equals("1")){
					logger.info("Sent the message to the JMQ==i.e="+txtMsg);
				}
				//textMsg.acknowledge();
			} catch (JMSException jmse) {
				if(MsgSenderThread.logsCreate){
					//logger.error("Queue Full condition occured");
					logger.error("Failed to push the message ==="+txtMsg);
				}
				if(MsgSenderThread.logsCreate == true){
					logger.info("The Connection with the server was lost,Please Check!!!");
				}
			}
		}else{
			if(MsgSenderThread.logsCreate == true){
				System.out.println("The Connection with the server was lost,Transaction will be Archived....!!!");
				logger.info("The Connection with the server was lost,Transaction will be Archived....!!!");
				
				/**
				 * if connection with FAI Server is not available
				 * transaction will be archived once the FAI server is up
				 * and running the transaction file will be uploaded to the
				 * the FAI system through SFTP
				 */
				//BossCommunicationManager.instance().write(txtMsg);
			}
		}
	}

	private QueueConnectionFactory initFactory() throws FileNotFoundException, IOException {
		
	// <-- Changes In New Falcon Start Here --> 	
		
		final int connTimeoutPeriod = PropertyUtil.getInt("jms.jndiresolution.connection.timeout");
		logger.info("Connection timeout period " +connTimeoutPeriod);
		try {
			logger.info("MessageSender initFactory() try block calling.......");
			
	// <-- Changes In New Falcon Ends Here --> 		
			
			return TimeLimitedCodeBlock.runWithTimeout(()->{
				QueueConnectionFactory connFactory = null;
				logger.info("inside QueueConnectionFactory value of connFactory is :::" +connFactory);
				logger.info("jndiContext Value is ::::" +jndiContext);
				try {
					if (jndiContext == null) {
						logger.info("QueueConnectionFactory nested try{} block calling.......");
						getInitialContext();
					}
					if (jndiContext != null)
						connFactory = (QueueConnectionFactory)jndiContext.lookup(prop.getProperty("connectionFactory"));
					else 
						throw new ConnectException("JNDI Context couldn't be initialized");
				} catch (NamingException ne) {
					//ne.printStackTrace();
					if(MsgSenderThread.logsCreate){
						logger.error("JNDI Exception: " + ne.toString());
					}
					
					//Need to think again
					//System.exit(-1);
				} catch (ConnectException ce) {
					if(MsgSenderThread.logsCreate){
						logger.error("JNDI Exception: " + ce.toString());
					} 
					//Need to think again
					//System.exit(-1);
				}
				return connFactory;
			}, connTimeoutPeriod, TimeUnit.MILLISECONDS);
		} 
		catch (TimeoutException e) {
			logger.info("Timed out!");
			return null;
		}
		catch (Exception e) {
			logger.error(e.getMessage(), e);
			return null;
		}
	}

	private void getInitialContext() throws NamingException, ConnectException {
		Hashtable<String,String> properties = new Hashtable<String,String>();
		properties.put(Context.PROVIDER_URL, "iiop://" + prop.getProperty("serverHost") + ":"+ prop.getProperty("serverPort"));
		properties.put(Context.INITIAL_CONTEXT_FACTORY,prop.getProperty("contextFactory"));
		properties.put("com.ibm.CORBA.ConnectTimeout", prop.getProperty("context.initialize.timeout"));
		try {
			jndiContext = new InitialContext(properties);
		} catch (NamingException ne) {
			if(MsgSenderThread.logsCreate){
				logger.error("Error While getting Initial Context: " + ne.getMessage()+"Please start the server");
			} 
			//throw (ne);
		}
	}

	public boolean setup() {
		try {
			closeSession();
			logger.info("initFactory()");
			QueueConnectionFactory ConnFactory = initFactory();
			if (ConnFactory == null)
				return false;
			Destination dest = initDestination(prop.getProperty("writeToQueue"));
			logger.info("createQueueConnection()");
			conn = ConnFactory.createQueueConnection();
			logger.info("createQueueSession()");
			session = conn.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
			logger.info("createProducer()");
			msgProducer = session.createProducer(dest);
			isConnAvailable = true;
			return true;
		} catch (JMSException jmse) {
			if(MsgSenderThread.logsCreate == true){
				logger.info("The Connection with the server was lost,Please Check!!!");
			}// checking wait
			return false;
			//Need to think
			//System.exit(-1);
		}catch (Exception e) {
			logger.info(e.getMessage(), e);
			return false;
		}
	}

	private Destination initDestination(String destinationName) {
		final int connTimeoutPeriod = PropertyUtil.getInt("jms.jndiresolution.connection.timeout");
		try {
			logger.info("MessageSender initDestination()... ");
			return TimeLimitedCodeBlock.runWithTimeout(()->{
				Destination dest = null;
				try {
					if (jndiContext == null) {
						getInitialContext();
					}
					dest = (Destination) jndiContext.lookup(destinationName);
				} catch (NamingException ne) {
					if(MsgSenderThread.logsCreate){	
						logger.error("JNDI Exception: " + ne.toString());
					}
					//System.exit(1);
				} catch (ConnectException ce) {
					if(MsgSenderThread.logsCreate){
						logger.error("JNDI Exception: " + ce.toString());
					}
					//Need to think again
					//System.exit(-1);
				}
				return dest;
			}, connTimeoutPeriod, TimeUnit.MILLISECONDS);
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
			return null;
		}
	}
	public static void enableMessageSenderLogs(){
		MessageTransactionLogging.enableLogger();
	}
}
