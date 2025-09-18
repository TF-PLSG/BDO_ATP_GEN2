import java.util.Date;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;


public class ScheduledTaskPool {

	public static void main(String[] args) throws InterruptedException {
		
		
		ScheduledExecutorService scheduledThreadPool = Executors.newScheduledThreadPool(5);
				
		//schedule to run after sometime
		System.out.println("Current Time = "+new Date());
		for(int i=0; i<1; i++){
			Thread.sleep(1000);
			
			MyTimerTask timerTask = new MyTimerTask("do processing");
			scheduledThreadPool.schedule(timerTask, 20, TimeUnit.SECONDS);
			
			/*WorkerThread worker = new WorkerThread("do processing");
			scheduledThreadPool.schedule(worker, 10, TimeUnit.SECONDS);*/
		}
		
		//add some delay to let some threads spawn by scheduler
		Thread.sleep(10000);
		
		scheduledThreadPool.shutdown();
		while(!scheduledThreadPool.isTerminated()){
			//wait for all tasks to finish
		}
		System.out.println("Finished all tasks");
	}

}