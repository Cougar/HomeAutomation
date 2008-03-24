/*
 * MbInternetAddress.java
 *
 * Created on den 26 augusti 2003, 11:37
 */
package Macbeth.System;

/**
 * Holds an address to a kernel server.
 * This simply consists of a hostname and a port.
 * @author Jimmy
 */
public class MbInternetAddress {
    private String hostname;
    private int port;

    /** Creates a new instance of MbInternetAddress */
    public MbInternetAddress() {
        this.hostname = "localhost";
        this.port = 19000;
    }

    /** Creates a new instance of MbInternetAddress */
    public MbInternetAddress(String hostname, int port) {
        this.hostname = hostname;
        this.port = port;
    }

    /**
     * Gets the hostname. Can be an ip number or a dns name.
     * @return The hostname. There is no guarantee that this is a valid hostname.
     */
    public String getHostname() {
        return hostname;
    }

    /**
     * Sets the hostname. Can be an ip number or a dns name.
     * The hostname will not be checked for validity.
     * @param hostname The hostname.
     */
    public void setHostname(String hostname) {
        this.hostname = hostname;
    }

    /**
     * Gets the port.
     * @return The port number;
     */
    public int getPort() {
        return port;
    }

    /**
     * Sets the port.
     * @param port The port number.
     */
    public void setPort(int port) {
        this.port = port;
    }

    /**
     * Thrown when this address is invalid (i.e. can't be reached)
     */
    public static class InvalidAddressException extends Exception {
        public InvalidAddressException(String s) {
            super(s);
        }
        public InvalidAddressException() {
            super();
        }
    }
}
