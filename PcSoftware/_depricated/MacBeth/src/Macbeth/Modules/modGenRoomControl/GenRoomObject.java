/*
 * File created by Jimmy
 * at 2003-sep-15 19:07:18
 */
package Macbeth.Modules.modGenRoomControl;

import Macbeth.Utilities.UByte;
import java.io.*;

/**
 * A representation of a room object (mostly lamps). Each
 * room object has a name, a byte value and mostly a
 * corresponding remote control button as well.
 * @author Jimmy, arune
 */
public class GenRoomObject {

    public String name;
    public String address;
    public UByte[] byteValue;
    public String remoteButton;
    public boolean displHTML;


    /**
     * Creates a new instance of RoomObject.
     * @param name The name of this object.
     * @param address The node-address at where this object is.
     * @param byteval The byte value of this object.
     * @param remotebutton The name of the remote button associated with this object.
     */
    public GenRoomObject(String name, String address, UByte[] byteval, String remotebutton, boolean displHTML) {
        this.name = name;
        this.address = address;
        this.byteValue = byteval;
        this.remoteButton = remotebutton;
        this.displHTML = displHTML;
    }
    public GenRoomObject() {
        this("", "", new UByte[0], "", false);
    }

}
