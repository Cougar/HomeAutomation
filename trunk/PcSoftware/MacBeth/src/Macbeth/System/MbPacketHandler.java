/*
 * MbPacketHandler.java
 *
 * Created on den 25 augusti 2003, 00:21
 */
package Macbeth.System;

/**
 * Implement this interface if your class can handle
 * Macbeth packets (this is not the same as receiving
 * packets - handling packets refers to the process
 * or parsing them and reacting to the data found
 * in their contents, while receiving simply refers
 * to the process of taking care of an incoming packet)
 * @author Jimmy
 */
public interface MbPacketHandler {
    /**
     * Must be called before calling handlePacket.
     * If this returns false, it means that handlePacket
     * cannot be called just yet.
     * @return True if it's OK to call handlePacket yet, False otherwise.
     */
    public boolean canHandlePacket();

    /**
     * This is called when you need to handle/parse a packet
     */
    public void handlePacket(MbPacket p);
}
