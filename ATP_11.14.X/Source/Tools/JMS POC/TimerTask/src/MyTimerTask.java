import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;

public class MyTimerTask extends TimerTask {
	
	private String command;
	
	public MyTimerTask() {
		// TODO Auto-generated constructor stub
	}
	
	public MyTimerTask(String s) {
		this.command = s;
	}

	@Override
	public void run() {
		System.out.println("Timer Task Started At :" +new Date());
		completeTask();
		System.out.println("Timer Task Finished At :"+new Date());
		
	}

	private void completeTask() {
		try {
			// assuming this will take 2 seconds to complete the task
			Thread.sleep(2000);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}
	
	public static void main(String[] args) {
		TimerTask timerTask = new MyTimerTask();
	
	/*	
		Running Timer Task as a daemon thread
		public Timer(boolean isDaemon);
	*/	
		Timer timer = new Timer(true); 
		timer.scheduleAtFixedRate(timerTask, 2, 10*1000);
		System.out.println("Timer Task Started");
		
		// Cancel after sometime 
		try {
			Thread.sleep(30000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		timer.cancel();
		System.out.println("Timer Task Cancelled");
		
		try {
			Thread.sleep(30000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
