/*
 * TcpServerThread.java
 *
 * Created on den 25 augusti 2003, 22:50
 */
package Macbeth.Net;

import java.net.*;
import java.io.*;
import java.util.List;

/**
 * Server thread for the TcpServer.
 * Such a thread will be created for each connected
 * client.
 * @author Jimmy
 */
public class TcpServerThread extends Thread {
    private Socket socket;
    private ServerDataHandler dataHandler;
    private BufferedReader stringInput = null;
    private PrintWriter stringOutput = null;
    boolean run;

    //reference to the list of server threads (this list is maintained in the TcpServer-class
    private List serverThreads;


    /**
     * Creates a new instance of TcpServerThread.
     * @param socket The socket associated with the client we're serving.
     * @param datahandler The data handler that should be invoked when the client is sending us data.
     * @param serverThreads The list of server threads maintained in TcpServer. The server thread needs
     * to be able to delete itself from this list when it has finished running.
     * @param TcpNoDelay Sets if socket use TcpNoDelay
     **/
    public TcpServerThread(Socket socket, ServerDataHandler datahandler, List serverThreads, boolean TcpNoDelay) {
        super("TcpServerThread");
        this.socket = socket;
        this.dataHandler = datahandler;
        this.serverThreads = serverThreads;
        try {
            this.socket.setTcpNoDelay(TcpNoDelay);
        }
        catch (SocketException e) {
        	System.out.println("Socket error: " + e);
        }
    }

    /**
     * Starts up the server thread.
     */
    public void startThread() {
        try {
            //get input stream from socket
            stringInput = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            //create output stream
            stringOutput = new PrintWriter(socket.getOutputStream());
            //timeout creates a pause in readline so that one can test if client is still connected
            socket.setSoTimeout(35000);
            //does this do anything
            //socket.setKeepAlive(true);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        //start new server thread
        run = true;
        super.start();
    }

    /**
     * Stops the server thread.
     */
    public void stopThread() {
        if (run) {
            run = false;
            try {
                if (stringInput!=null) stringInput.close();
                socket.close();
                serverThreads.remove(this);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
	/**
	 * This will send data to this client.
	 * @param dataToSend String line to send to this client.
	 */
    public void sendData(String dataToSend){
    	//borde inte denna generera exception då man skriver till en socket som inte är ansluten?
    	stringOutput.println(dataToSend);
    	stringOutput.flush();
    }
    
    /**
     * Allows application to ask for this clients address.
     * @return The address of this client.
     */
    public String getAddress() {
    	return socket.getRemoteSocketAddress().toString();
    }
    
    /**
     * This will be called from the thread.
     */
    public void run() {
        //tell the datahandler from whom we're receiving data
        dataHandler.setRemoteAddress(socket.getRemoteSocketAddress().toString());
        
        try {
            //do we have a string input stream?
            if (stringInput!=null) {
                //read lines from this stream until connection is closed
                String line;
                while (run) {
                    /*
                    //if input buffer is not empty
                    if (stringInput.ready()) {
                        //read next line
                        line=stringInput.readLine();
                        //and let the data handler handle this new received line
                        dataHandler.handleServerData(line);
                    //else, if input buffer is empty
                    } else {
                        //get out of the loop
                        break;
                    }*/

                    //read next line
                    try {
                    	line = stringInput.readLine();
                    	//gör om denna så den läser tecken för tecken tills ett förinställt termineringstecken dyker upp
                   	
                    } catch (SocketTimeoutException e) {
	                    //don't care...fully normal since we have a timeout set
                    	
	                	//detta är socketens timeout
                    	//här borde man alltså kolla om det verkligen finns någon på andra sidan
	                	//System.out.println("Timeout: " + e);
	                	//e.printStackTrace();
	                	//continue;
	                	break;
	                } catch (SocketException e) {
	                	//koppla ner socketen är väl det enda rätta?
	                	//System.out.println("Disconnect: " + e);
	                	break;
	                }
                    //line = null;
                    if (line!=null) {
                        dataHandler.handleServerData(line, socket.getRemoteSocketAddress().toString());
                   }
                    //until end of stream has been reached
                    else {
                    	dataHandler.clientDropped(socket.getRemoteSocketAddress().toString());
                        break;
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            //stop the thread and close the socket
            stopThread();
        }
    }
}