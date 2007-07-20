/**
 * File created by Jimmy
 * at 2003-dec-28 19:50:12
 */
package Macbeth.System;

import java.io.PrintStream;

/**
 * A packet transport plugs directly into the kernel and helps
 * it delivering packets via different transports. For instance,
 * a local transport might deliver a packet directly to the
 * destination module within the local kernel, while an internet
 * transport might send the packet to a kernel on a remote location.
 * Packet transports primarily send packets, but they can of
 * course also receive packets on their own, and when they do, they
 * can simply tell the kernel to put the new, received, packet onto
 * the standard packet queue by calling the packetReceived-method
 * declared in this class.
 */
public abstract class MbPacketTransport {
    //reference to our parent kernel object
    protected MbKernel parentKernel;
    //references to output streams
    protected PrintStream _debug,_info,_errors;

    //counts how many packets this transport has sent/received
    public int packetsSent = 0;
    public int packetsReceived = 0;

    /**
     * Creates a new instance of MbPacketTransport.
     * @param parentKernel Reference to the parent kernel object.
     */
    public MbPacketTransport(MbKernel parentKernel) {
        this.parentKernel = parentKernel;
    }

    /**
     * Will be called by the kernel when it has a new packet that
     * needs to be delivered. The kernel has a list of packet
     * transport systems, and typically it will try to invoke them
     * in the same order as they were added to that list. In other
     * words, there is no way you can tell whether one packet
     * transport will be invoked before another one. Therefore, it is
     * very important that you return false here if you cannot deliver
     * the current packet. The kernel will then try with the next
     * transport in list. If, and only if, none of the transports
     * could deliver the packet, the kernel will drop it.
     * @param p The packet that needs to be delivered.
     * @return False if this packet transport cannot perform delivery
     * on the specified packet. True if the packets has been delivered
     * successfully. When you return true, the packet is considered
     * delivered, and no further transports will be invoked.
     * @throws MbPacketDeliveryException if an error occured while trying
     * to deliver the packet.
     */
    public abstract boolean deliverPacket(MbPacket p) throws MbPacketDeliveryException;

    /**
     * Override this if you need to perform anything particular
     * during kernel startup.
     */
    public void startup() {
        //keep references to output streams
        _debug = parentKernel._debug;
        _info = parentKernel._info;
        _errors = parentKernel._errors;
    }

    /**
     * Override this if you need to perform anything particular
     * during kernel shutdown.
     */
    public void shutdown() {}

    /**
     * Returns the name of this transport. This is only used by
     * kernel to show user-friendly information.
     * @return The name of this transport.
     */
    public abstract String getName();

    /**
     * Call this method if you have received a new, incoming,
     * packet via your transport and need to handle it over to
     * the kernel. This is useful for bidirectional delivery
     * systems.
     * @param p The new packet that you have received.
     */
    protected void packetReceived(MbPacket p) {
        //put this packet onto the kernel packet queue
        parentKernel.packetReceived(p);
        //increase packets received-counter
        packetsReceived++;
    }

    /**
     * Thrown when a packet couldn't be delivered properly.
     */
    public class MbPacketDeliveryException extends Exception {
        /**
         * Creates a new instance of MbPacketDeliveryException
         * @param s The reason of this exception.
         */
        public MbPacketDeliveryException(String s) {
            super(s);
        }
    }
}
