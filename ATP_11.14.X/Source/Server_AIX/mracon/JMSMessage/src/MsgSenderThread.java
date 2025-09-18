import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

/*import org.apache.log4j.DailyRollingFileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;*/

import logging.MessageTransactionLogging;

/**
 * 
 * @author 100454 This class Intializes the queues once and checks for the
 *         messages from the queues and if any messages push to the JMS MQ
 *         Appname is mracon Default service name is mracon if the user didn't
 *         passed any parameters
 * 
 * @modified by 104103
 */
class MsgSenderThread extends Thread {

	private final static Logger logger = LogManager.getLogger(MsgSenderThread.class);
	// Logger logger = Logger.getRootLogger();

	static String appName = "mracon";
	static String serviceName = null;
	String messageFromCQueue = null;
	boolean queuesInitialized = false;
	static MraconHandler mraconHandler;
	static MessageSender messageSender;
	static BossCommunicationManager bossCommunicationManager;
	static ApacheHttpClient apacheHttpClient;
	// LoopTask task = new LoopTask();
	// Timer timer = new Timer("TaskName");
	static boolean isQueueempty = false;
	static boolean logsCreate = false;
	static boolean appenderExist = true;
	static boolean isJMSConnEstablished = false;
	int sentMsgCount = 0;

	public void run() {
		
		while (true) {
			if (!queuesInitialized) {
				int intializeResult = mraconHandler.initialize(appName, serviceName);
				if (intializeResult == 01) {
					if (logsCreate) {
						logger.info("failed to create the queues and going to terminate and stopping java program.");
					}
					// log the failed to create the queues.
					mraconHandler.terminate();
					// Fine Call
					System.exit(-1);
				} else if (intializeResult == 02) {
					if (logsCreate) {
						logger.info("appname or server name is null so going to terminate and stopping java program");
					}
					mraconHandler.terminate();
					// Fine Call
					System.exit(-1);
				} else if (intializeResult == 00) {
					queuesInitialized = true;
					if (logsCreate) {
						logger.info("Queues Initialized Successfully");
					}
				} else {
					if (logsCreate) {
						logger.info("intializeResult is invalid  so going to terminate and stopping java program");
					}
					mraconHandler.terminate();
					// Fine Call
					System.exit(-1);
				}
			}
			if (queuesInitialized) {
				/*
				 * if(logsCreate){ logger.
				 * info("Start reading from the queue===from servicename="
				 * +serviceName); }
				 */
				isQueueempty = false;

				try {
					// isJMSConnEstablished
																// = false ;
					isJMSConnEstablished = messageSender.setup();
				while (!isQueueempty) {
					messageFromCQueue = "";
					if (isJMSConnEstablished) {
						logger.info("Current JMS connection status::::::" + isJMSConnEstablished);
						try {
							BossCommunicationManager.instance().reset();
							messageFromCQueue = mraconHandler.sendRequestMessage(serviceName);
							//logger.info("Online message sending to FAI : " + messageFromCQueue);
							

						} catch (Exception e) {
							logger.error(e.getMessage(), e);
						}
						// messageFromCQueue =
						// mraconHandler.sendRequestMessage(serviceName);
					} else {
						logger.info(":::: JMS Connection is not established");
						if (logsCreate) {
							messageSender.closeSession();
								logger.info("The Connection with the server was lost,Please Check Trying to reconnect..!!!");
						
							messageFromCQueue = mraconHandler.sendRequestMessage(serviceName);
							messageFromCQueue = messageFromCQueue.replaceAll("(\\r|\\n)", "");
								logger.info(serviceName);
							logger.info("Archive message sending to bossService : " + messageFromCQueue);

							if (((!messageFromCQueue.isEmpty()) && messageFromCQueue != null) && messageFromCQueue.length() > 500) {
								logger.debug(
											"Message Received Also Sending To Archive File ======> " + messageFromCQueue);
								BossCommunicationManager.instance().write(messageFromCQueue);
								
							} else {
								logger.info("Empty message or less byte of message received can not be logged into archive file....!!!!");
							}
								logger.info("Trying to setup connection again!");
								messageSender.closeSession();
								isJMSConnEstablished = messageSender.setup();
						}
					}
					if (messageFromCQueue.indexOf("Error:Timed Out on Receive,  error code:") != -1) {
						/*
						 * if(logsCreate){
						 * logger.info("Timed out error=============="); }
						 */
					} else if (messageFromCQueue
							.indexOf("Error on pteipc_receive OR Error in Receving Message") != -1) {
						if (logsCreate) {
							logger.info("There are no more messages in the queue and stopped reading from the queue");
						}
						isQueueempty = true;
					} else if (messageFromCQueue.equals("1") || messageFromCQueue == "1") {
						if (logsCreate) {
							logger.info(
									"There was an exit request from native call,So the send and recieve process will be stopped");
						}
						// Fine Call
						System.exit(-1);
					} else if (messageFromCQueue.equals("100") || messageFromCQueue == "100") {
						logsCreate = true;
						if (!appenderExist) {
							appenderExist = true;
							enableSenderLogger();
							MessageReciever.enableReceiverLogs();
							MessageSender.enableMessageSenderLogs();
						} /*
							 * else{ MessageReciever.enableReceiverLogs();
							 * MessageSender.enableMessageSenderLogs(); }
							 */
					} else if (messageFromCQueue.equals("101") || messageFromCQueue == "101") {
						logger.info("Received the Signal TRACE_OFF,So stopping the logging process=====");
						appenderExist = false;
						logsCreate = false;
					} else if (messageFromCQueue != null && !messageFromCQueue.equals("")) {
						
						if (messageFromCQueue.length() == Integer
								.parseInt(messageSender.getProp().getProperty("msgLength"))) {
							try {
								messageSender.sendMessage(messageFromCQueue);
							} catch (Exception e) {
								// TODO Auto-generated catch block
								logger.error(e.getMessage(), e);
							}
							sentMsgCount++;
						} else if (messageFromCQueue
								.length() == Integer.parseInt(messageSender.getProp().getProperty("msgLength")) - 1) {
							try {
								messageSender.sendMessage(messageFromCQueue + " ");
							} catch (Exception e) {
								// TODO Auto-generated catch block
								logger.error(e.getMessage(), e);
							}
							sentMsgCount++;
						} else {
							/*
							 * if(logsCreate){
							 * logger.info("The message is not "+messageSender.
							 * getProp().getProperty("msgLength")
							 * +" bytes so not pushing to JMS=="
							 * +messageFromCQueue); }
							 */
						}
						// logger.info("The message pushed to queue
						// successfully");
					} else {

						if (logsCreate) {
							logger.info("The Message from C Queue is empty:");
						}

						isQueueempty = true;
					}

				}
				if (sentMsgCount % 1000 == 0) {
					logger.info("sentMsgCount for the service " + serviceName + " Was===" + sentMsgCount);
				}
				messageSender.closeSession();
				} finally {
					messageSender.closeSession();
				}
				/*
				 * if(logsCreate){
				 * logger.info("Done with reading from the queue"); }
				 */
			} else {
				if (logsCreate) {
					logger.info("Queues didn't initialize properly going to terminate");
				}
				mraconHandler.terminate();
				// Fine Call
				System.exit(-1);
			}
		}
	}

	public static void main(String[] args) throws Exception {
		try {
			if (appenderExist) {
				logsCreate = true;
				enableSenderLogger();
				MessageReciever.enableReceiverLogs();
				MessageSender.enableMessageSenderLogs();
			}
			BossCommunicationManager.instance().hardReset();
			mraconHandler = new MraconHandler();
			messageSender = new MessageSender();
			MsgReceiverThread msgReceiverTask = new MsgReceiverThread();
			// enableSenderLogger();
			if (mraconHandler.getLibFileVersion() == MessageReciever.currentVersion) {
				if (logsCreate) {
					logger.info("The lib file version:" + MessageReciever.currentVersion
							+ " is matching with the java version,Continue with initialize.");
				}
			} else if (mraconHandler.getLibFileVersion() > MessageReciever.currentVersion) {
				String isInitialize = messageSender.getProp().getProperty("continueIfLibVersionGreater");
				if (isInitialize == null || isInitialize.equals("null")) {
					if (logsCreate) {
						logger.info("The lib file version:" + mraconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + MessageReciever.currentVersion
								+ ",So not going to initialize.");
					}
					// Fine Call
					System.exit(-1);
				} else if (!isInitialize.equals("true")) {
					if (logsCreate) {
						logger.info("The lib file version:" + mraconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + MessageReciever.currentVersion
								+ ",So not going to initialize.");
					}
					// Fine Call
					System.exit(-1);
				} else {
					if (logsCreate) {
						logger.info("The lib file version:" + mraconHandler.getLibFileVersion()
								+ " is greater than to the java version:" + MessageReciever.currentVersion
								+ ",Still based on config going to initialize.");
					}
				}
			} else if (mraconHandler.getLibFileVersion() < MessageReciever.currentVersion) {
				if (logsCreate) {
					logger.info("The lib file version:" + mraconHandler.getLibFileVersion()
							+ " is less than to the java version:" + MessageReciever.currentVersion
							+ ",So not going to initialize.");
				}
				// Fine Call
				System.exit(-1);
			}
			if (args.length > 0) {
				serviceName = args[0];
			} else {
				serviceName = appName;
			}
			if (logsCreate) {
				logger.info("appName====" + appName + "==serviceName===" + serviceName);
			}
			
			MsgSenderThread executingTask = new MsgSenderThread();
			executingTask.start();
			
			
			//msgReceiverTask.start();
			/*
			 * if(logsCreate){
			 * logger.info("MsgReceiverThread Started====================="); }
			 */
		} catch (Exception e) {
			if (logsCreate) {
				logger.info("Caught excetion in main method====" + e.getMessage());
			}
			// e.printStackTrace();
		}
	}

	public static void enableSenderLogger() {
		
		MessageTransactionLogging.enableLogger();
		
		/*
		String propFilePath = System.getenv("ASCENDENTROOT");
		try {
			if (propFilePath != null && logsCreate == true) {
				appenderExist = true;
				String loggerPath = propFilePath + "/trace/mracon.log";
				PatternLayout layout = new PatternLayout();
				// layout.setConversionPattern("%d
				// [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
				layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
				String datePattern = "'.'yyyy-MM-dd";
				DailyRollingFileAppender appender = new DailyRollingFileAppender(layout, loggerPath, datePattern);
				logger.addAppender(appender);
				// logger.info("enabled SenderLogger Logs===================");
				MessageReciever.enableReceiverLogs();
				MessageSender.enableMessageSenderLogs();
				
				 * BossCommunicationManager.enableBossCommunicationLogger();
				 * ApacheHttpClient.enableApacheHttpClientLogger();
				 
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	*/}
}
