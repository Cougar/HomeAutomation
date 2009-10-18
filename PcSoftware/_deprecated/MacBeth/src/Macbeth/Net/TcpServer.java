/*
 * TcpServer.java
 *
 * Created on den 25 augusti 2003, 22:50
 */
package Macbeth.Net;

import java.net.*;
import java.io.*;
import java.util.*;

/**
 * Multithreaded multi-client TCP Server.
 * @author Jimmy
 */
public class TcpServer extends Thread {
    private ServerSocket serverSocket;
    private boolean listening;
    private int port;
    private LinkedList serverThreads;
    private ServerDataHandler dataHandler;
    private boolean tcpNoDelay;

    /**
     * Creates a new instance of TcpServer.
     * @param port The port on which the server should listen for connections.
     * @param datahandler The data handler that shall be invoked when incoming data needs to be handled.
     */
    public TcpServer(int port, ServerDataHandler datahandler) {
        super("TcpServer");
        serverSocket = null;
        listening = false;
        this.port = port;
        dataHandler = datahandler;
        serverThreads = new LinkedList();
        tcpNoDelay = false;
    }

    /**
     * Starts listening for incoming connections.
     */
    public void startServer() {
        listening = true;
        start();
    }

    /**
     * Stops listening for incoming connections.
     */
    public void stopServer() {
        listening = false;
        //stop all server threads
        while (serverThreads.size()>0) {
            TcpServerThread t = (TcpServerThread)serverThreads.removeFirst();
            t.stopThread();
        }
    }
    
    /**
     * Allows application to turn off TcpNoDelay, http://www.dagblastit.com/java/sockets.html 
     * @param TcpNoDelay If TcpNoDelay should be turned on or off.
     */
    public void setTcpNoDelay(boolean TcpNoDelay) {
    	this.tcpNoDelay = TcpNoDelay;
    }
    
    /**
     * Sends dataToSend to all connected clients
     * @param dataToSend The data to send.
     */
    public void sendToAll(String dataToSend) {
    	TcpServerThread t;
    	for (int i = 0; i < serverThreads.size(); i++) {
        	t = (TcpServerThread)serverThreads.get(i);
        	t.sendData(dataToSend);
    	}
    }
    
    /**
     * Sends dataToSend to all connected clients except those with a certain address
     * @param dataToSend The data to send.
     * @param address The address of the client to exclude from the send.
     */
    public void sendToAllExcept(String dataToSend, String address) {
    	TcpServerThread t;
    	for (int i = 0; i < serverThreads.size(); i++) {
        	t = (TcpServerThread)serverThreads.get(i);
        	if (!address.equals(t.getAddress())){
            	t.sendData(dataToSend);
        	}
    	}
    }
    
    /**
     * Sends dataToSend to all connected clients except those with a certain address
     * @param dataToSend The data to send.
     * @param address The address of the client to send to.
     */
    public void sendToClient(String dataToSend, String address) {
    	TcpServerThread t;
    	for (int i = 0; i < serverThreads.size(); i++) {
        	t = (TcpServerThread)serverThreads.get(i);
        	if (address.equals(t.getAddress())) {
            	t.sendData(dataToSend);
        	}
    	}
    }
   
    /**
     * This is called in a new thread when server starts listening.
     */
    public void run() {
        try {
            serverSocket = new ServerSocket(port);
            serverSocket.setSoTimeout(2000);
            //serverSocket.setReuseAddress(true);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        TcpServerThread t;
        Socket s;
        //listen for incoming connections as long as flag is set
        while (listening) {
            try {
                s = serverSocket.accept();
                //start new server thread for each connection
                t = new TcpServerThread(s, dataHandler, serverThreads, tcpNoDelay);
                serverThreads.add(t);
                t.startThread();
            } catch (SocketTimeoutException e) {
                //don't care...fully normal since we have a timeout set
            	//detta är serversocketsens timeout, inte socketens
            	//System.out.println("Timeout: " + e);
            	//något skapar timeouts från start
            	//det är MbInternetTransport 
            	continue;
            } catch (Exception e) {
                e.printStackTrace();
                break;
            }
        }
        try {
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}