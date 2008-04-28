/*
 * File created by Jimmy
 * at 2004-jan-12 16:03:55
 */
package Macbeth.Modules.modControlMP;

/**
 * A representation of a RemoteMedia function. Each function
 * has a unique ID string and a corresponding remote control
 * button name.
 * @author Jimmy
 */
public class RemoteMediaFunction {

    /**
     * The ID string of this function. This is what should
     * be sent to the TCP server in RemoteMedia.
     */
    public String id;

    /**
     * The remote control button associated with this function.
     */
    public String remoteButton;

    /**
     * Creates a new instance of RoomObject.
     * @param id The ID of this function.
     * @param remotebutton The name of the remote button associated with this function.
     */
    public RemoteMediaFunction(String id, String remotebutton) {
        this.id = id;
        this.id = id;
        this.remoteButton = remotebutton;
    }

}
