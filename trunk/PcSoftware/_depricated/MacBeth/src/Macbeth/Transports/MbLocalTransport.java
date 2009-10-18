/**
 * File created by Jimmy
 * at 2003-dec-29 00:04:12
 */
package Macbeth.Transports;

import Macbeth.System.MbPacketTransport;
import Macbeth.System.MbPacket;
import Macbeth.System.MbModule;
import Macbeth.System.MbKernel;

/**
 * A local packet transport for the Macbeth kernel. This will
 * deliver packets locally within the Macbeth kernel.
 * @author Jimmy
 */
public class MbLocalTransport extends MbPacketTransport {

    /**
     * Creates a new instance of MbLocalTransport.
     * @param parentKernel Reference to the parent kernel object.
     */
    public MbLocalTransport(MbKernel parentKernel) {
        super(parentKernel);
    }

    /**
     * Tries to deliver a packet locally.
     * @param p The packet that needs to be delivered.
     * @return True if the packet has been delivered successfully.
     * False if this packet cannot be delivered locally (that is,
     * if it has a destination kernel that differs from "self").
     * @throws Macbeth.System.MbPacketTransport.MbPacketDeliveryException if the destination module
     * couldn't be found while trying to deliver locally.
     */
    public boolean deliverPacket(MbPacket p) throws MbPacketTransport.MbPacketDeliveryException {
        //we will only deliver locally if destination kernel is "self"
        if (p.getDestination().getKernel().equals("self")) {
            deliverPacketLocally(p);
            return true;
        } else {
            //throw new MbPacketDeliveryException("Can only deliver to \"self\". Destination kernel was: \"" + p.getDestination().kernel + "\"");
            return false;
        }
    }

    /**
     * Returns the name of this transport.
     * @return The name of this transport.
     */
    public String getName() {
        return "Local transport";
    }

    /**
     * Delivers a packet to the specified destination module.
     * This delivery will only be performed locally.
     * @param p The packet that it to be delivered.
     * @throws Macbeth.System.MbPacketTransport.MbPacketDeliveryException if the destination module
     * couldn't be found.
     */
    protected void deliverPacketLocally(MbPacket p) throws MbPacketTransport.MbPacketDeliveryException {
        String modulename = p.getDestination().getModule();
        MbModule module = null;
        //try to locate destination module locally
        module = parentKernel.getModule(modulename);
        //if module was found
        if (module!=null) {
            //tell module to receive this packet
            module.packetReceived(p);
        }
        //if module wasn't found
        else {
            //throw exception
            throw new MbPacketDeliveryException("Destination module '" + p.getDestination().getModule() + "' not found.");
        }
        //we have now delivered the packet successfully, so increase sent packets-counter
        packetsSent++;
    }
}
