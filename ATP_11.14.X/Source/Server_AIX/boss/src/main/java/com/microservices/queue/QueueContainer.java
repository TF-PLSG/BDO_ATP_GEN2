package com.microservices.queue;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * Thread safe housing for the queue
 */
public class QueueContainer {
    // current instance
    private static QueueContainer thisInstance;
    private final ConcurrentLinkedQueue<String> theQueue;

    private QueueContainer() {
        theQueue = new ConcurrentLinkedQueue<>();
    }

    public static synchronized QueueContainer getInstance() {
        if (thisInstance == null)
            thisInstance = new QueueContainer();
        return thisInstance;
    }

    public synchronized void enqueue(String data) {
        theQueue.offer(data);  // return false if it fails to insert object
    }

    public synchronized String dequeue() {
        return theQueue.poll(); // return null if queue is empty
    }
}
