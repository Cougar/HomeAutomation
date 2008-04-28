/*
 * MbPacketReceiver.java
 *
 * Created on den 24 augusti 2003, 23:59
 */
package Macbeth.System;

/**
 * Implement this interface if your class contains
 * functionality for receiving Macbeth packets. Note
 * that receiving a packet is not the same as handling
 * packets. Handling packets refers to the process
 * or parsing them and reacting to the data found
 * in their contents, while receiving simply refers
 * to the process of taking care of an incoming packet
 * (and putting it onto a packet queue or invoking a
 * handler).
 * @author Jimmy
 */
public interface MbPacketReceiver {
    /**
     * This is called when someone sends you a packet.
     */
    public void packetReceived(MbPacket p);
}