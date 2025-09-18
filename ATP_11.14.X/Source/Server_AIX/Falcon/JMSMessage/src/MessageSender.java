import java.io.FileInputStream;
import java.io.IOException;
import java.net.ConnectException;
import java.util.Hashtable;
import java.util.Properties;

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
				prop.load(new FileInputStream(propFilePath+"/config/properties.txt"));
			}else{
				if(MsgSenderThread.logsCreate){
					logger.info("There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
				}
				//Fine Call
				System.exit(-1);
			}
		} catch (IOException e) { e.printStackTrace(); } 
	}

	public void sendMessage(String message) {
		sendTextMessage(message);
	}

	public void closeSession(){
		if(isConnAvailable){
			try {
				logger.info("Closing session for the current thread");
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

	private void sendTextMessage(String txtMsg) {
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
				logger.info("The Connection with the server was lost,Please Check!!!");
			}
		}
	}

	private QueueConnectionFactory initFactory() {
		QueueConnectionFactory connFactory = null;
		try {
			if (jndiContext == null) {
				getInitialContext();
			}
			connFactory = (QueueConnectionFactory)jndiContext.lookup(prop.getProperty("connectionFactory"));
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
	}

	private void getInitialContext() throws NamingException, ConnectException {
		Hashtable<String,String> properties = new Hashtable<String,String>();
		properties.put(Context.PROVIDER_URL, "iiop://" + prop.getProperty("serverHost") + ":"+ prop.getProperty("serverPort"));
		properties.put(Context.INITIAL_CONTEXT_FACTORY,prop.getProperty("contextFactory"));
		
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
			QueueConnectionFactory ConnFactory = initFactory();
			Destination dest = initDestination(prop.getProperty("writeToQueue"));
			conn = ConnFactory.createQueueConnection();
			session = conn.createQueueSession(false, QueueSession.AUTO_ACKNOWLEDGE);
			msgProducer = session.createProducer(dest);
			isConnAvailable = true;
			return true;
		} catch (JMSException jmse) {
			if(MsgSenderThread.logsCreate == true){
				logger.info("The Connection with the server was lost,Please Check!!!");
			}
			return false;
			//Need to think
			//System.exit(-1);
		}catch (Exception e) {
			return false;
		}
	}

	private Destination initDestination(String destinationName) {
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
	}
	public static void enableMessageSenderLogs(){
		MessageTransactionLogging.enableLogger();
	}
}
