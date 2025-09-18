import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

class MsgReceiverThread{
	/**
	 * This is a TimerTask which will check the response Queue on JMS for 
	 * every 2 secs(Configurable) and sent the response message by calling JNI Call
	 * public native String receiveResponseMsg(String response);
	 */
	static MessageReciever messageReciever;
    LoopTask task = new LoopTask();
    Timer timer = new Timer("RecieverTask");
    
    public MsgReceiverThread(){
    	messageReciever = new MessageReciever();
    }
    
    public void start() {
	    timer.cancel();
	    timer = new Timer("RecieverTask");
	    Date executionDate = new Date();
	    Long delay = Long.parseLong(messageReciever.getProp().getProperty("queueCheckDelay"));
	    timer.scheduleAtFixedRate(task,executionDate,delay);
    }
    
    private class LoopTask extends TimerTask {
	    public void run() {
	    	//logger.info("Running for every 2 secs==========");
	    	messageReciever.readMessage();
	    }
    }	
   
   /* public static void main(String[] args){
    	logger.info("Starting Main Method==to recive messages==");
    	MsgReceiverThread executingTask = new MsgReceiverThread();
    	logger.info("MsgReceiverThread Got created=====================");
		executingTask.start();
	}*/
}
 
