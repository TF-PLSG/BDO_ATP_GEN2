package com.microservices.queue;

import org.springframework.stereotype.Service;

@Service
public class EnqueueService {
    private final QueueContainer queueContainer;

    public EnqueueService() {
        queueContainer = QueueContainer.getInstance();
    }

    public synchronized void enqueue(String message) {
        queueContainer.enqueue(message);
    }
}
