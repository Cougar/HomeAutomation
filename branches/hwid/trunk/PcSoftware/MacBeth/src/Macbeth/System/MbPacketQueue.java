/*
 * MbPacketQueue.java
 *
 * Created on den 24 augusti 2003, 13:03
 */
package Macbeth.System;

import java.util.*;

/**
 * This class represents a packet-queue, onto which incoming
 * packets will be put, and from which kernels will fetch
 * packets for processing.
 * @author Jimmy
 */
public class MbPacketQueue {
    //queue is implemented using a linked list
    private LinkedList queue;

    /**
     * Creates a new instance of MbPacketQueue.
     */
    public MbPacketQueue() {
        queue = new LinkedList();
    }

    /**
     * Gets the size (=number of elements) of the queue.
     * @return The number of elements currently enqueued.
     */
    synchronized public int getSize() {
        return queue.size();
    }

    /**
     * Adds a packet to the queue.
     * @param p The packet that should be added. In case of null, packet is ignored.
     */
    synchronized public void enqueue(MbPacket p) {
        if (p!=null) {
            queue.add(p);
        }
    }

    /**
     * Extracts and removes a packet from the queue.
     * @return The extracted packet. If queue is empty, return value is null.
     */
    synchronized public MbPacket dequeue() {
        if (getSize()>0) {
            return (MbPacket)queue.removeFirst();
        } else {
            return null;
        }
    }
}
