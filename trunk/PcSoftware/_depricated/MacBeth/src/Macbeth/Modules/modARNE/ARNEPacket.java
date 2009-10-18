/*
 * File created by Jimmy
 * at 2003-sep-03 13:39:58
 */
package Macbeth.Modules.modARNE;

import Macbeth.Utilities.UByte;

/**
 * Represents an ARNE packet that can be transmitted
 * or received to/from an ARNE network.
 * @author Jimmy
 */
public class ARNEPacket {

    /**
     * Maximum number of data bytes inside an ARNE packet.
     */
    public static final int MAX_ARNE_DATALEN = 15;

    /**
     * The ARNE header bytes. There are always two of them.
     */
    public UByte[] header;

    /**
     * The ARNE data bytes.
     */
    public UByte[] data;

    /**
     * Creates a new instance of ARNEPacket.
     */
    public ARNEPacket() {
        //always two header bytes
        header = new UByte[2];
        data = null;
    }

    /**
     * Sets the lenght of the data field in this packet.
     * @param size The number of data bytes.
     */
    public void setDataLen(int size) {
        data = new UByte[size];
        //first header byte contains startbits and data size
        header[0] = UByte.fromShort((short)(0xA0 | (0x0F & size)));
    }

    /**
     * Gets the length of the data field in this packet.
     * @return The number of data bytes.
     */
    public int getDataLen() {
        if (data!=null) {
            return data.length;
        } else {
            return 0;
        }
    }

    /**
     * Sets the node address to which this packet is to
     * be delivered. This address will be saved in the
     * ARNE header.
     * @param address The destination node address.
     */
    public void setNodeAddress(short address) {
        header[1] = UByte.fromShort(address);
    }

}