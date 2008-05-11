/*
 * ServerDataHandler.java
 *
 * Created on den 26 augusti 2003, 00:57
 */
package Macbeth.Net;

/**
 * Implement this interface if your class can handle
 * incoming data from the TCP server. This data is
 * always plain text, and thus, your datahandler
 * only needs to be able to handle Strings.
 * @author Jimmy
 */
public interface ServerDataHandler {

    /**
     * Will be called when server has received a string
     * that your datahandler need to take care of.
     * @param data The received string.
     */
    public void handleServerData(String data, String address);

    /**
     * Notifies your datahandler about a clients remote address.
     * Will be called by server when a new socket is opened.
     * @param address The clients remote address (IP number)
     */
    public void setRemoteAddress(String address);
    
    
    /**
     * Notifies your datahandler about a clients dropping connection.
     * Will be called by server when a client timed out.
     * (did not ping/send data/timed out)
     * @param address The clients remote address (IP number)
     */
    public void clientDropped(String address);
}