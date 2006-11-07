/*
 * TcpClient.java
 *
 * Created on den 25 augusti 2003, 22:50
 */
package Macbeth.Net;

import java.net.*;
import java.io.*;

/**
 * A TCP client that sends data to a TCP Server.
 * @author Jimmy
 */
public class TcpClient {
    private int port;
    private String hostname;
    private Socket socket;
    private BufferedWriter bw;
    private BufferedReader br;

    /**
     * Creates a new instance of TcpClient.
     * @param port The port that the socket connection should be opened through use.
     * @param hostname The hostname that the socket should connect to.
     */
    public TcpClient(int port, String hostname) {
        this.port = port;
        this.hostname = hostname;
        socket = null;
        bw = null;
        br = null;
    }

    /**
     * Finds out whether or not the client has successfully
     * connected to the server.
     * @return true if connected, false otherwise.
     */
    public boolean isConnected() {
        if (socket!=null && bw!=null) {
            return socket.isConnected();
        } else {
            return false;
        }
    }

    /**
     * Opens the connection to the server.
     * @throws UnknownHostException if the hostname is invalid.
     * @throws IOException if an I/O-error occurs while trying to connect.
     */
    public void openConnection() throws UnknownHostException,IOException {
        if (hostname==null || hostname.equals("")) return;
        socket = new Socket(hostname, port);
        bw = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
        br = new BufferedReader(new InputStreamReader(socket.getInputStream()) );
    }

    /**
     * Closes the connection to the server.
     * @throws IOException if an I/O-error occurs while trying to close connection.
     */
    public void closeConnection() throws IOException {
        if (bw!=null) {
            bw.close();
        }
        if (br!=null) {
            br.close();
        }
        socket.close();
    }

    /**
     * Sends a string to the server through the socket.
     * @param s The string. It will be sent immediately (output stream is flushed).
     * @throws IOException If an I/O-error occurs while trying to send the string.
     */
    public void sendString(String s) throws IOException {
        if (isConnected()) {
            bw.write(s);
            bw.newLine();
            bw.flush();
        } else {
            throw new IOException("Not connected to server!");
        }
    }


    /**
     * Gets a row of data from the server through the socket.
     * @throws IOException If an I/O-error occurs while trying to receive a row.
     */
    public String getRow() throws IOException {
        if (isConnected()) {
            return br.readLine();
        } else {
            throw new IOException("Not connected to server!");
        }
    }
}
