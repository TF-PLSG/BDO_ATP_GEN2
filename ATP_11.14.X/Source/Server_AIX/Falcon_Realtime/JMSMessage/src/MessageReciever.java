import java.io.FileInputStream;
import java.io.IOException;
import java.net.ConnectException;
import java.util.Hashtable;
import java.util.Properties;

import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.Session;
import javax.jms.TextMessage;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import logging.MessageTransactionLogging;

/**
 * 
 * @author 100454 In setup() need to configure the OutputQ Based on environment
 * 
 *         This class will recieve the response from the output Queue from WAS
 *         and call the C using JNI public native String
 *         receiveResponseMsg(String response);
 * 
 * @modified by 104103
 */
public class MessageReciever {

	private final static Logger logger = LogManager.getLogger(MessageReciever.class);

	private Context jndiContext;
	private QueueConnection conn;
	private Session session;
	private Properties prop;
	private MessageConsumer msgConsumer;
	private Destination outputQ;
	static FalconHandler falconHandler;
	public static int currentVersion = 1;
	public static boolean logsenalble = false;
	public static boolean isConnAvailable = false;
	private MessageSender messageSender;

	private MessageRequestResponseMapper msgMapper;

	public Properties getProp() {
		return prop;
	}

	public void setProp(Properties prop) {
		this.prop = prop;
	}

	public MessageReciever() {

		falconHandler = new FalconHandler();
		prop = new Properties();
		try {
			String propFilePath = System.getenv("ASCENDENTROOT");
			enableReceiverLogs();
			// logger.info("propFilePath==============="+propFilePath);
			if (propFilePath != null) {
				prop.load(new FileInputStream(propFilePath + "/config/app_properties.properties"));
			} else {
				if (MsgSenderThread.logsCreate == true && logsenalble == true) {
					logger.info(
							"There is no properties file,Please set environment variable with name ASCENDENTROOT and value is propertyfile folder location");
				}
				// Fine Call
				System.exit(-1);
			}
			/*if (falconHandler.getLibFileVersion() == currentVersion) {
				if (MsgSenderThread.logsCreate == true && logsenalble == true) {
					logger.info("The lib file version:" + currentVersion
							+ " is matching with the java version,Continue with initialize.");
				}
			} else if (falconHandler.getLibFileVersion() > currentVersion) {
				String isInitialize = prop.getProperty("continueIfLibVersionGreater");
				if (isInitialize == null || isInitialize.equals("null")) {
					if (MsgSenderThread.logsCreate == true && logsenalble == true) {
						logger.info("The lib file version:" + falconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + MessageReciever.currentVersion
								+ ",So not going to initialize.");
					}
					// Fine Call
					System.exit(-1);
				} else if (!isInitialize.equals("true")) {
					if (MsgSenderThread.logsCreate == true && logsenalble == true) {
						logger.info("The lib file version:" + falconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + currentVersion
								+ ",So not going to initialize.");
					}
					// Fine Call
					System.exit(-1);
				} else {
					if (MsgSenderThread.logsCreate == true && logsenalble == true) {
						logger.info("The lib file version:" + falconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + currentVersion
								+ ",Still based on config going to initialize.");
					}
				}

			} else if (falconHandler.getLibFileVersion() < currentVersion) {
				if (MsgSenderThread.logsCreate == true && logsenalble == true) {
					logger.info("The lib file version:" + falconHandler.getLibFileVersion()
							+ " is less than to the java version:" + currentVersion + ",So not going to initialize.");
				}
				// Fine Call
				System.exit(-1);
			}*/
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void readMessage() throws Exception {
		isConnAvailable = setup();
		if (isConnAvailable) {
			String responseMsg = null;
			try {
				Message msg = null;
				boolean isanyMessages = false;
				while (msgConsumer.receiveNoWait() != null) {
					isanyMessages = true;
					msg = msgConsumer.receive();
					if (msg instanceof TextMessage) {
						responseMsg = ((TextMessage) msg).getText();
						if (responseMsg != null) {
							if (MsgSenderThread.logsCreate == true && logsenalble == true) {
								logger.info("recieved response message : " + responseMsg);
								msgMapper.responseMapper(responseMsg);
							}
							// falconHandler.receiveResponseMsg(responseMsg.toString());
						}
					}
				}
				if (!isanyMessages) {

					if (MsgSenderThread.logsCreate == true && logsenalble == true) {
						logger.info(
								"There are no response messages inside JMS Queue in predefined time current transaction processing as TIMEOUT ");
						falconHandler.receiveResponseMsg("TIMEOUT");

					}

				}

				/*
				 * System.out.println("Getting msg........."); Message msg =
				 * msgConsumer.receive(); System.out.println("Got it"); if (msg
				 * instanceof TextMessage) { txtMsg = ((TextMessage)
				 * msg).getText(); System.out.println("txtMsg==============" +
				 * txtMsg); }
				 */

			} catch (JMSException jmse) {
				if (MsgSenderThread.logsCreate == true && logsenalble == true) {
					logger.info("The Connection with the server was lost,Please Check!!!!");

				}
			}
			closeSession();
		} else {
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.info("The Connection with the server was lost,Please Check!!!");
			}
		}
	}

	private void closeSession() {
		try {
			session.close();
			conn.close();
		} catch (JMSException jmse) {
			logger.info("The Connection with the server was lost,Please Check!!!");
		}
	}

	private QueueConnectionFactory initFactory() {
		QueueConnectionFactory connFactory = null;
		try {
			if (jndiContext == null) {
				getInitialContext();
			}
			connFactory = (QueueConnectionFactory) jndiContext.lookup(prop.getProperty("connectionFactory"));
		} catch (NamingException ne) {
			// ne.printStackTrace();
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.error("JNDI Exception: " + ne.toString());
			}
			// Need to think again
			// System.exit(-1);
		} catch (ConnectException ce) {
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.error("JNDI Exception: " + ce.toString());
			}
			// Need to think again
			// System.exit(-1);
		}
		return connFactory;
	}

	private void getInitialContext() throws NamingException, ConnectException {
		Hashtable<String, String> properties = new Hashtable<String, String>();

		properties.put(Context.PROVIDER_URL,
				"iiop://" + prop.getProperty("serverHost") + ":" + prop.getProperty("serverPort"));
		properties.put(Context.INITIAL_CONTEXT_FACTORY, prop.getProperty("contextFactory"));
		try {
			jndiContext = new InitialContext(properties);
		} catch (NamingException ne) {
			throw (ne);
		}
	}

	private boolean setup() {
		QueueConnectionFactory ConnFactory = initFactory();

		outputQ = initDestination(prop.getProperty("readFromQueue"));
		// outputQ = initDestination(prop.getProperty("writeToQueue"));
		try {
			conn = ConnFactory.createQueueConnection();
			conn.start();
			session = conn.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
			msgConsumer = session.createConsumer(outputQ);
			return true;
		} catch (JMSException jmse) {
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.error("Error while creating connection/session/producer");
				logger.info("The Connection with the server was lost,Please Check!!!");
			}
			return false;
			// Commented
			// System.exit(-1);
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
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.error("JNDI Exception: " + ne.toString());
			}
			// System.exit(1);
		} catch (ConnectException ce) {
			if (MsgSenderThread.logsCreate == true && logsenalble == true) {
				logger.error("JNDI Exception: " + ce.toString());
			}
			// Need to think again
			// System.exit(-1);
		}
		return dest;
	}

	public static void enableReceiverLogs() {
		
		MessageTransactionLogging.enableLogger();
	}
}
