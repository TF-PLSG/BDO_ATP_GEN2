import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

public class TimeLimitedCodeBlock {
	
	private static Logger logger = LogManager.getLogger(TimeLimitedCodeBlock.class);
	public static void runWithTimeout(final Runnable runnable, long timeout, TimeUnit timeUnit) throws Exception {
		
		runWithTimeout(new Callable<Object>() {
			@Override
			public Object call() throws Exception {
				runnable.run();
				return null;
			}
		}, timeout, timeUnit);
	}

	public static <T> T runWithTimeout(Callable<T> callable, long timeout, TimeUnit timeUnit) throws Exception {
		logger.info("TimeLimitedCodeBlock runWithTimeout() method called.....!!");
		final ExecutorService executor = Executors.newSingleThreadExecutor(); /* newSingleThreadExecutor() returns ExecutorService with single
																				 thread worker,never increase its thread pool size more than one	*/																
		final Future<T> future = executor.submit(callable);
		executor.shutdown(); // This does not cancel the already-scheduled task.
		try {
			logger.info("future timeout value::::" + future.get(timeout, timeUnit));
			return future.get(timeout, timeUnit);
		} catch (TimeoutException e) {
			// remove this if you do not want to cancel the job in progress
			// or set the argument to 'false' if you do not want to interrupt
			// the thread
			logger.info("runWithTimeout() method catch block called.....!!"); 
			future.cancel(true);
			throw e;
		} catch (ExecutionException e) {
			// unwrap the root cause
			Throwable t = e.getCause();
			if (t instanceof Error) {
				throw (Error) t;
			} else if (t instanceof Exception) {
				throw (Exception) t;
			} else {
				throw new IllegalStateException(t);
			}
		}
	}
}