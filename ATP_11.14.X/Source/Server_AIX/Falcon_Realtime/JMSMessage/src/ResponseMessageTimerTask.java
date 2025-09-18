import java.util.Timer;
import java.util.TimerTask;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.apache.log4j.spi.LoggerFactory;

public class ResponseMessageTimerTask {

	public static Logger logger = LogManager.getLogger(ResponseMessageTimerTask.class);

	static MessageReciever messageReciever;
	static Timer timer;

	public ResponseMessageTimerTask(int seconds) {
		timer = new Timer();
		timer.schedule(new QueueCheckTask(), seconds * 1000);
	}

	class QueueCheckTask extends TimerTask {

		@Override
		public void run() {
			System.out.println("Timer-On..!!");
			ResponseMessageTimerTask.onSuccessResponse();
			timer.cancel();

		}
	}

	public static void main(String[] args) {
		new ResponseMessageTimerTask(2);
		System.out.println(" TimerTask Created");
	}

	/*
	 * this method is responsible for mapping request and response message and
	 * process message accordingly either as a REALTIME or ONLINE or TIMEOUT Response
	 */
	public static void onSuccessResponse() {
		try {
			
		} catch (Exception e) {
			logger.info(e.getMessage());
		}
	}

	public static void onTimeout() {
		

	}

}
