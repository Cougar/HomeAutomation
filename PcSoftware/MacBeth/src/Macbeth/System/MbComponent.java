/*
 * MbComponent.java
 *
 * Created on den 23 augusti 2003, 20:56
 */
package Macbeth.System;

/**
 * This class represents a Macbeth component. Components
 * are parts of the Macbeth system, each having its own
 * _unique_ name, a packet queue and a packet queue handler.
 * Extend this class further to create kernels or modules.
 * @author Jimmy
 */
abstract public class MbComponent implements MbPacketHandler,MbPacketReceiver {
    //A packet queue in which all packets will be put before they are processed.
    protected MbPacketQueue packetQueue;
    //A packet processor that takes care of packets in the queue.
    protected MbPacketQueueHandler packetQueueHandler;
    //is this component started?
    private boolean started;

    /**
     * Creates a new instance of MbComponent.
     */
    public MbComponent() {
        //create a new packet queue for us
        packetQueue = new MbPacketQueue();
        //create a handler for the packet queue
        packetQueueHandler = new MbPacketQueueHandler(packetQueue,this);
        //we're not started yet
        started = false;
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    abstract public String name();

    /**
     * Gets a short description of the component. This should
     * be kept short (1-2 lines) and should be formatted as
     * plain text.
     * @return A short description of the component.
     */
    abstract public String description();

    /**
     * Returns true if this component is started.
     * @return true if this component is started.
     */
    public boolean isStarted() {
        return started;
    }

    /**
     * Will be called when this component should start up. Most
     * initialization should be done here (rather than in the
     * component's constructor).
     * @throws MbStartupException When this component couldn't
     * be started up for some reason.
     */
    public void startup() throws MbStartupException {
        //start packet queue handler (it will be run in a separate thread)
        packetQueueHandler.startWorking();
        //this component is started now!
        started = true;
    }

    /**
     * Will be called when this component should shut down itself.
     * Default behaviour is to shut down immediately, deleting any
     * eventual packets waiting in our queue. If you want to change
     * this behaviour, just override this method. Keep in mind though,
     * that you cannot send any packets to other components here, they
     * might have been shut down already!
     */
    public void shutdown() {
        //shut down packet queue handler
        packetQueueHandler.stopWorking();
        //this component is no longer started!
        started = false;
    }

    /**
     * Will be called when someone sends a packet to us. Default behaviour
     * is to simply put the packet onto our packet queue. You could override
     * this if you, for instance, want to filter out certain packets before
     * putting them onto the queue. Note that you should not do the actual
     * packet handling here. That's what the handlePacket-method is for.
     * @param p The packet we're receiving.
     */
    public void packetReceived(MbPacket p) {
        packetQueue.enqueue(p);
    }

    /**
     * Checks if packet handler is ready.
     * @return True if we're ready and it's OK to call handlePacket, false otherwise.
     */
    public boolean canHandlePacket() {
        return true;
    }

    /**
     * Will be called by the packet queue handler for each packet it
     * takes off the queue. This means it is time to parse and take
     * care of this packet.
     * @param p The packet we need to handle.
     */
    abstract public void handlePacket(MbPacket p);

}
