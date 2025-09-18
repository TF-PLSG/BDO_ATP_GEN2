package framework.timerpool;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.function.Supplier;


public abstract class TimerJob {
    protected TimerJobStatus timerJobStatus;
    
    private static Logger logger = LogManager.getLogger(TimerJob.class);

    public abstract void onSuccess(Object object);

    public abstract void onTimeout(Object object);

    public abstract Supplier<Object> run();

    public void complete() {
        timerJobStatus = TimerJobStatus.COMPLETED;
    }

    public boolean hasCompleted() {
        return timerJobStatus == TimerJobStatus.COMPLETED;
    }

    public TimerJobStatus getJobStatus() {
        return timerJobStatus;
    }

    public void decommission() {
        if (!hasCompleted())
            complete();
        timerJobStatus = TimerJobStatus.READY_FOR_COMMISSIONING;
    }

    public void commission(long milliseconds) {
        CompletableFuture<Object> timer = CompletableFuture.supplyAsync(run());
        try {
            Object obj = timer.get(milliseconds, TimeUnit.MILLISECONDS);
            this.timerJobStatus = TimerJobStatus.COMPLETED;
            onSuccess(obj);
        } catch (TimeoutException ex) {
            this.timerJobStatus = TimerJobStatus.TIMED_OUT;
            onTimeout(null);
        } catch (InterruptedException e) {
            logger.error(e.getMessage(), e);
        } catch (ExecutionException e) {
            logger.error(e.getMessage(), e);
        }
    }
}
