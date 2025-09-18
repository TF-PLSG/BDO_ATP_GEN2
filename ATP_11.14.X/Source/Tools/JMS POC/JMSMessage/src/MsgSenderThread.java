import java.io.IOException;

import org.apache.log4j.DailyRollingFileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;

/**
 * 
 * @author 100454
 * This class Intializes the queues once and checks for the messages from the queues 
 * and if any messages push to the JMS MQ
 * Appname is falcon
 * Default service name is falcon if the user didn't passed any parameters
 */
class MsgSenderThread extends Thread{
	
	private final static Logger logger = Logger.getLogger(MsgSenderThread.class);
	//Logger logger = Logger.getRootLogger();
	
	static String appName= "falcon";
	static String serviceName = null;
	String messageFromCQueue = null;
	boolean queuesInitialized = false;
	static FalconHandler falconHandler;
	static MessageSender messageSender;
    //LoopTask task = new LoopTask();
    //Timer timer = new Timer("TaskName");
    static boolean isQueueempty = false;
    static boolean logsCreate = false;
    static boolean appenderExist = true;
    static boolean isJMSConnEstablished = false;
    int sentMsgCount = 0;
    //private static ExecutorService executor = Executors.newSingleThreadExecutor();
    
   /* public void start() {
	    timer.cancel();
	    timer = new Timer("TaskName");
	    Date executionDate = new Date();
	    Long delay = Long.parseLong(messageSender.getProp().getProperty("queueCheckDelay"));
	    timer.scheduleAtFixedRate(task,executionDate,delay);
    }*/
    
    /*private class LoopTask extends TimerTask {
	    public void run() {
	    	if(!queuesInitialized){
	    		int intializeResult = falconHandler.initialize(appName,serviceName);
	        	//System.out.println("initialize called=========="+Thread.currentThread().getId());
	        	logger.info("intializeResult after intialize got invoked="+intializeResult);
	        	if(intializeResult == 01){
	        		logger.info("failed to create the queues and going to terminate and stopping java program.");
	        		//log the failed to create the queues.
	        		falconHandler.terminate();
	        		System.exit(-1);
	        	}else if(intializeResult == 02){
	        		logger.info("appname or server name is null so going to terminate and stopping java program");
	        		falconHandler.terminate();
	        		System.exit(-1);
	        	}else if(intializeResult == 00){
	        		queuesInitialized = true;
	        		logger.info("Queues Initialized Successfully");
	        	}else{
	        		logger.info("intializeResult is invalid  so going to terminate and stopping java program");
	        		falconHandler.terminate();
	        		System.exit(-1);
	        	}
	    	}
	    	if(queuesInitialized){
		    	logger.info("Start reading from the queue===from servicename="+serviceName);
		    	messageSender.setup();
		    	isQueueempty=false;
		    	logger.info("isQueueempty===Config Parameter==========="+isQueueempty);
	    		while(!isQueueempty){
	    			messageFromCQueue = "";
	    			//System.out.println("sendRequestMessage before called=========="+Thread.currentThread().getId());
			    	messageFromCQueue = falconHandler.sendRequestMessage(serviceName);
			    	//System.out.println("sendRequestMessage after called=========="+Thread.currentThread().getId());
			    	logger.info("The message read form C queues===="+messageFromCQueue);
			    	if(messageFromCQueue.indexOf("Error:Timed Out on Receive,  error code:") != -1){
			    		logger.info("Timed out error==============");
			    		//isQueueempty = true;
			    	}else if(messageFromCQueue.indexOf("Error on pteipc_receive OR Error in Receving Message") != -1){
			    		logger.info("There are no more messages in the queue and stopped reading from the queue");
			    		isQueueempty = true;
			    	}else if(messageFromCQueue != null && !messageFromCQueue.equals("")){
			    		logger.info("messageFromCQueue length==="+messageFromCQueue.length());
			    		if(messageFromCQueue.length() == Integer.parseInt(messageSender.getProp().getProperty("msgLength"))){
				    		messageSender.sendMessage(messageFromCQueue);
			    		}else{
			    			logger.info("The message is not "+messageSender.getProp().getProperty("msgLength")+" bytes so not pushing to JMS=="+messageFromCQueue);
			    		}
			    		//logger.info("The message pushed to queue successfully");
			    	}else{
			    		logger.info("The Message from C Queue is empty.");
			    		isQueueempty = true;
			    	}
		    	}
		    	messageSender.closeSession();
		    	logger.info("Done with reading from the queue");
	    	}else{
	    		logger.info("Queues didn't initialize properly going to terminate");
        		falconHandler.terminate();
        		System.exit(-1);
	    	}
	    }
    }	*/
    public void run(){    
    	while(true){
    		if(!queuesInitialized){
	    		int intializeResult = falconHandler.initialize(appName,serviceName);
	        	//System.out.println("initialize called=========="+Thread.currentThread().getId());
	    		if(intializeResult == 01){
	    			if(logsCreate){
	    				logger.info("failed to create the queues and going to terminate and stopping java program.");
	    			}
	    			//log the failed to create the queues.
	        		falconHandler.terminate();
	        		//Fine Call
	        		System.exit(-1);
	        	}else if(intializeResult == 02){
	        		if(logsCreate){
	        			logger.info("appname or server name is null so going to terminate and stopping java program");
	        		}
	        		falconHandler.terminate();
	        		//Fine Call
	        		System.exit(-1);
	        	}else if(intializeResult == 00){
	        		queuesInitialized = true;
	        		if(logsCreate){
	        			logger.info("Queues Initialized Successfully");
	        		}
	        	}else{
	        		if(logsCreate){
	        			logger.info("intializeResult is invalid  so going to terminate and stopping java program");
	        		}
	        		falconHandler.terminate();
	        		//Fine Call
	        		System.exit(-1);
	        	}
	    	}
	    	if(queuesInitialized){
	    		/*if(logsCreate){
	    			logger.info("Start reading from the queue===from servicename="+serviceName);
	    		}*/
		    	isQueueempty=false;
		    	isJMSConnEstablished = messageSender.setup();
		    	while(!isQueueempty){
	    			messageFromCQueue = "";
	    			//logger.info("Inside while loop ===messageFromCQueue ===" +messageFromCQueue);
	    			if(isJMSConnEstablished){
	    				messageFromCQueue = falconHandler.sendRequestMessage(serviceName);
	    			}else{
	    				if(logsCreate){
	    					messageSender.closeSession();
	    					logger.info("The Connection with the server was lost,Please Check!!!");
	    					logger.info("The Connection with the server was lost,Trying to reconnect!!!");
	    					isJMSConnEstablished = messageSender.setup();
	    				}
	    				//falconHandler.checkNetworkStatus("1");
	    				//call native to said conn not available
	    				
	    			}
			    	//System.out.println("sendRequestMessage after called=========="+Thread.currentThread().getId());
			    	/*if(logsCreate && (messageFromCQueue.length() == Integer.parseInt(messageSender.getProp().getProperty("msgLength")))){
			    		logger.info("The message read form C queues===="+messageFromCQueue);
			    		//System.out.println("The message read form C queues===="+messageFromCQueue);
			    	}  */ 	
			    	if(messageFromCQueue.indexOf("Error:Timed Out on Receive,  error code:") != -1){
			    		/*if(logsCreate){
			    			logger.info("Timed out error==============");
			    		}*/
			    	}else if(messageFromCQueue.indexOf("Error on pteipc_receive OR Error in Receving Message") != -1){
			    		if(logsCreate){
			    			logger.info("There are no more messages in the queue and stopped reading from the queue");
			    		}
			    		isQueueempty = true;
			    	}else if(messageFromCQueue.equals("1") || messageFromCQueue == "1"){
			    		if(logsCreate){
			    			logger.info("There was an exit request from native call,So the send and recieve process will be stopped");
			    		}
			    		//Fine Call
			    		System.exit(-1);
			    	}else if(messageFromCQueue.equals("100") || messageFromCQueue == "100"){
			    		logsCreate = true;
	    				if(!appenderExist){
	    					appenderExist = true;
	    					enableSenderLogger();
	    					MessageReciever.enableReceiverLogs();
	    					MessageSender.enableMessageSenderLogs();
	    				}/*else{
	    					MessageReciever.enableReceiverLogs();
	    					MessageSender.enableMessageSenderLogs();
	    				}*/
			    	}else if(messageFromCQueue.equals("101") || messageFromCQueue == "101"){
			    		logger.info("Received the Signal TRACE_OFF,So stopping the logging process=====");
			    		appenderExist = false;
	    				logsCreate = false;
			    	}else if(messageFromCQueue != null && !messageFromCQueue.equals("")){
			    		/*if(logsCreate){
			    			logger.info("messageFromCQueue length==="+messageFromCQueue.length());
			    		}*/
			    		/*if(logsCreate){
			    			logger.info("Message length from ATP Tool "+messageFromCQueue.length()+"msgLength From Prop File=="+Integer.parseInt(messageSender.getProp().getProperty("msgLength")));
			    		}*/
			    		if(messageFromCQueue.length() == Integer.parseInt(messageSender.getProp().getProperty("msgLength"))){
			    			messageSender.sendMessage(messageFromCQueue);
				    		sentMsgCount++;
			    		}else if(messageFromCQueue.length() == Integer.parseInt(messageSender.getProp().getProperty("msgLength"))-1){
			    			messageSender.sendMessage(messageFromCQueue+" ");
				    		sentMsgCount++;
			    		}else{
			    			/*if(logsCreate){
			    				logger.info("The message is not "+messageSender.getProp().getProperty("msgLength")+" bytes so not pushing to JMS=="+messageFromCQueue);
			    			}*/
			    		}
			    		//logger.info("The message pushed to queue successfully");
			    	}else{
			    		/*if(logsCreate){
			    			logger.info("The Message from C Queue is empty.");
			    		}*/
			    		isQueueempty = true;
			    	}
			    	
		    	}
		    	if(sentMsgCount%1000==0){
		    		logger.info("sentMsgCount for the service "+serviceName+" Was==="+sentMsgCount);
		    	}
		    	messageSender.closeSession();
		    	/*if(logsCreate){
		    		logger.info("Done with reading from the queue");
		    	}*/
	    	}else{
	    		if(logsCreate){	
	    			logger.info("Queues didn't initialize properly going to terminate");
	    		}
        		falconHandler.terminate();
        		//Fine Call
        		System.exit(-1);
	    	}
    	}
    }
    public static void main(String[] args) throws IOException{
    	try{
    		if(appenderExist){
    			logsCreate = true;
				enableSenderLogger();
				MessageReciever.enableReceiverLogs();
				MessageSender.enableMessageSenderLogs();
    		}
			falconHandler = new FalconHandler();
	    	messageSender = new MessageSender();
	    	MsgReceiverThread msgReceiverTask = new MsgReceiverThread();
	    	//enableSenderLogger();
	    	if(falconHandler.getLibFileVersion()== MessageReciever.currentVersion){
	    		if(logsCreate){
	    			logger.info("The lib file version:"+MessageReciever.currentVersion+" is matching with the java version,Continue with initialize.");
	    		}
	    	}else if(falconHandler.getLibFileVersion() > MessageReciever.currentVersion){
				String isInitialize= messageSender.getProp().getProperty("continueIfLibVersionGreater");
				if(isInitialize ==null || isInitialize.equals("null")){
					if(logsCreate){
						logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
					}
					//Fine Call
					System.exit(-1);
				}else if(!isInitialize.equals("true")){
					if(logsCreate){
						logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
					}
					//Fine Call
					System.exit(-1);
				}else{
					if(logsCreate){
						logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",Still based on config going to initialize.");
					}
				}
			}else if(falconHandler.getLibFileVersion() < MessageReciever.currentVersion) {
				if(logsCreate){
					logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is less than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
				}
				//Fine Call
				System.exit(-1);
			}
	    	if(args.length>0){
	    		serviceName =args[0];
	    	}else{
	    		serviceName =appName;
	    	}
	    	if(logsCreate){
	    		logger.info("appName===="+appName+"==serviceName==="+serviceName);
	    	}
	    	MsgSenderThread executingTask = new MsgSenderThread();
			executingTask.start();
			
	    	msgReceiverTask.start();
	    	/*if(logsCreate){
	    		logger.info("MsgReceiverThread Started=====================");
	    	}*/
    	}
    	catch (Exception e) {
    		if(logsCreate){
    			logger.info("Caught excetion in main method===="+e.getMessage());
    		}
			//e.printStackTrace();
		}
    	/*falconHandler = new FalconHandler();
    	messageSender = new MessageSender();
    	if(falconHandler.getLibFileVersion()== MessageReciever.currentVersion){
			logger.info("The lib file version:"+MessageReciever.currentVersion+" is matching with the java version,Continue with initialize.");
		}else if(falconHandler.getLibFileVersion() > MessageReciever.currentVersion){
			String isInitialize= messageSender.getProp().getProperty("continueIfLibVersionGreater");
			if(isInitialize ==null || isInitialize.equals("null")){
				logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
				System.exit(-1);
			}else if(!isInitialize.equals("true")){
				logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
				System.exit(-1);
			}else{
				logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is greater than to the java version:"+MessageReciever.currentVersion+",Still based on config going to initialize.");
			}
			
		}else if(falconHandler.getLibFileVersion() < MessageReciever.currentVersion) {
			logger.info("The lib file version:"+falconHandler.getLibFileVersion()+" is less than to the java version:"+MessageReciever.currentVersion+",So not going to initialize.");
			System.exit(-1);
		}
    	if(args.length>0){
    		serviceName =args[0];
    	}else{
    		serviceName =appName;
    	}
    	logger.info("appName===="+appName+"==serviceName==="+serviceName);
    	//MsgSenderThread executingTask = new MsgSenderThread();
		//executingTask.start();
*/    	
	}
    public static void enableSenderLogger(){
    	String propFilePath = System.getenv("ASCENDENTROOT");
		try{
			if(propFilePath !=null && logsCreate == true){
				appenderExist = true;
				String loggerPath = propFilePath+"/trace/falcon.log";
				PatternLayout layout =new PatternLayout();
				//layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");  
				layout.setConversionPattern("%d [%X{requestURIWithQueryString}] %-5p %c{2} - %m%n");
				String datePattern = "'.'yyyy-MM-dd";
				DailyRollingFileAppender appender = new DailyRollingFileAppender(layout,loggerPath,datePattern);    
				logger.addAppender(appender);
				//logger.info("enabled SenderLogger Logs===================");
				MessageReciever.enableReceiverLogs();
				MessageSender.enableMessageSenderLogs();
			}
		}catch (Exception e) {
			e.printStackTrace();
		}
    }
}
