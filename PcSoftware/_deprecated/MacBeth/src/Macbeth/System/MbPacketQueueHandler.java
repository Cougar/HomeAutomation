/*
 * MbPacketQueueHandler.java
 *
 * Created on den 24 augusti 2003, 15:11
 */
package Macbeth.System;

/**
 * A packet queue handler, which pulls packets from a packet
 * queue, and handles over each pulled packet to a packet handler
 * object.
 * @author Jimmy
 */
public class MbPacketQueueHandler extends Thread {
    private MbPacketHandler packetHandler;
    private MbPacketQueue packetQueue;
    private boolean runThread;

    /**
     * Creates a new instance of MbPacketQueueHandler.
     * @param packetqueue The packet queue to handle.
     * @param packethandler The packet handler that should be
     * invoked when packets need to be handled.
     */
    public MbPacketQueueHandler(MbPacketQueue packetqueue, MbPacketHandler packethandler) {
        super("MbPacketQueueHandler");
        packetQueue = packetqueue;
        packetHandler = packethandler;
    }

    /**
     * Start working with the queue.
     */
    public void startWorking() {
        runThread = true;
        //start thread (it will call our run-method in a new thread)
        start();
    }

    /**
     * Stop working with the queue.
     */
    public void stopWorking() {
        runThread = false;
    }

    /**
     * This is called by the thread when running.
     */
    public void run() {
        //loop while run-flag is set true
        while (runThread) {
            //if there are packets in the queue and packet handler is ready
            if (packetQueue.getSize()>0 && packetHandler.canHandlePacket()) {
                //pull packet from head of the queue and tell the packet handler to handle it
                packetHandler.handlePacket(packetQueue.dequeue());
            } else {
                //if queue is empty of packet handler not ready yet, try to get some sleep=)
                try {
                    sleep(5);
                } catch (InterruptedException e) {
                    //don't care
                }
            }
        }
    }
}
