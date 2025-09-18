package framework.timerpooltest;

import framework.timerpool.TimerJob;

import java.util.function.Supplier;

public class PrintJob extends TimerJob {	
    @Override
    public void onSuccess(Object object) {
    	System.out.println("onSuccess");
        System.out.println(object);
    }

    @Override
    public void onTimeout(Object object) {
        System.out.println("Timedout!!!!!");
    }

    @Override
    public Supplier<Object> run() {
        return () -> {
            try {
                Thread.sleep(2000);
                
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return 0;
        };
    }

    public static void main(String[] args) {
        PrintJob printJob = new PrintJob();
        printJob.commission(2000);
    }
}
