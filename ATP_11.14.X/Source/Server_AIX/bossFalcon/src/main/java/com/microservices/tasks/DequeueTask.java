package com.microservices.tasks;

import com.microservices.actions.ArchiveLogger;
import com.microservices.helpers.StringUtilities;
import com.microservices.queue.DequeueService;

import lombok.extern.slf4j.Slf4j;

import org.springframework.beans.factory.DisposableBean;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
@Slf4j
public class DequeueTask implements DisposableBean, Runnable {
    private volatile boolean exit = false;
    private DequeueService dequeueService;
    private ArchiveLogger archiveLogger;

    public DequeueTask(@Autowired DequeueService dequeueService,
                       @Autowired ArchiveLogger archiveLogger) {
        Thread t = new Thread(this);
        this.dequeueService = dequeueService;
        this.archiveLogger = archiveLogger;

        t.start();
    }

    @Override
    public void run() {
        while (!exit) {
            try {
                String dequeuedData = dequeueService.dequeue();
                Thread.sleep(100);
                if (StringUtilities.isNotNullOrEmpty(dequeuedData)) {
                    processRequest(dequeuedData);
                }
            } catch (Exception ex) {
            	log.error(ex.getMessage(), ex);
            }
        }
    }

    private void processRequest(String dequeuedData) {
    	log.info("Dequeued Data: " + dequeuedData);
        archiveLogger.logData(dequeuedData);
    }

    @Override
    public void destroy() throws Exception {
        exit = true;
    }
}
