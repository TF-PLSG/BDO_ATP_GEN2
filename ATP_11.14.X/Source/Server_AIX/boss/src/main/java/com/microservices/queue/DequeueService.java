package com.microservices.queue;

import org.springframework.stereotype.Service;

@Service
public class DequeueService {
    private final QueueContainer queueContainer;

    public DequeueService() {
        queueContainer = QueueContainer.getInstance();
    }

    public synchronized String dequeue() {
        return queueContainer.dequeue();
    }
}
