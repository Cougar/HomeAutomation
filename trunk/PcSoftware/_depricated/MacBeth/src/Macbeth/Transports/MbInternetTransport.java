/**
 * File created by Jimmy
 * at 2003-dec-28 19:56:06
 */
package Macbeth.Transports;

import Macbeth.Net.ServerDataHandler;
import Macbeth.Net.TcpClient;
import Macbeth.Net.TcpServer;
import Macbeth.Utilities.MySystem;
import Macbeth.System.*;
import java.io.IOException;
import java.net.UnknownHostException;

import org.xml.sax.SAXException;

/**
 * An internet packet delivery transport for the Macbeth kernel.
 * This will deliver packets to and from other remotely located
 * Macbeth kernels via the internet.
 * @author Jimmy
 */
public class MbInternetTransport extends MbPacketTransport {

    //A list of known kernels.
    public MbInternetAddressList kernelList;
    //The TCP port on which we're listening for internet packets.
    public int ourPort;
    //Indicates whether or not we're currently receaving a packet as a raw string on the internet server.
    private boolean receivingPacketString;
    //Indicates whether an XML declaration (eg. <?xml version="1.0" encoding="Unicode"?>) has been received
    private boolean xmlDeclarationReceived;
    private String receivedString;
    private String clientRemoteAddress;

    //our tcp server
    private TcpServer server;
    //our tcp client
    private TcpClient client;

    /**
     * Creates a new instance of MbInternetTransport.
     */
    public MbInternetTransport(MbKernel parentKernel) {
        super(parentKernel);
        //create new kernel list
        kernelList = new MbInternetAddressList();
        //initial values
        ourPort = 19000;
        receivingPacketString = false;
        xmlDeclarationReceived = false;
        receivedString = "";
        clientRemoteAddress = "";
        server = null;
        client = null;
    }

    /**
     * Starts up the internet delivery system.
     */
    public void startup() {
        super.startup();
        //start inet server and use our own ServerDataHandler interface to handle data
        server = new TcpServer(ourPort,new MyServerDataHandler());
        server.startServer();
    }

    /**
     * Shuts down the internet delivery system.
     */
    public void shutdown() {
        super.shutdown();
        //shut down internet server
        server.stopServer();
    }

    /**
     * Returns the name of this transport.
     * @return The name of this transport.
     */
    public String getName() {
        return "Internet transport";
    }

    /**
     * Tries to deliver a packet via the internet transport.
     * @param p The packet that needs to be delivered.
     * @return True if the packet has been delivered successfully.
     * False if this packet cannot be delivered via this transport
     * (that is, if it has a destination kernel that is either unknown
     * or local, "self").
     * @throws Macbeth.System.MbPacketTransport.MbPacketDeliveryException if an error occured while trying
     * to deliver the packet (I/O-errors or invalid addresses).
     */
    public boolean deliverPacket(MbPacket p) throws MbPacketTransport.MbPacketDeliveryException {
        MbInternetAddress kernelAddress = kernelList.getAddress(p.getDestination().getKernel());
        //if no such kernel existed in our kernel list...
        if (kernelAddress==null) {
            //then we cannot deliver this packet
            return false;
            //if we do know about the destination kernel
        } else {
            //lets deliver
            try {
                deliverPacketViaInternet(p,kernelAddress);
            } catch (Exception e) {
                throw new MbPacketTransport.MbPacketDeliveryException(e.toString());
            }
        }
        return true;
    }

    /**
     * Delivers a packet via internet.
     * @param p The packet that should be delivered.
     * @param addr The kernel address to which the packet should be delivered.
     * @throws IOException if an I/O-error occurs while trying to send the packet.
     * @throws UnknownHostException if the specified address is invalid.
     */
    protected void deliverPacketViaInternet(MbPacket p, MbInternetAddress addr)
            throws IOException, UnknownHostException
    {
        //connect our TCP client to the specified host and port
        client = new TcpClient(addr.getPort(),addr.getHostname());
        client.openConnection();
        //send the packet via the client
        client.sendString(p.serializeToString());
        //and then disconnect
        client.closeConnection();
        client = null;
        //we've now sent the packet successfully, so increase sent packets-counter
        packetsSent++;
    }


    /**
     * Will take care of incoming server data.
     */
    private class MyServerDataHandler implements ServerDataHandler {
        /**
         * Will be called when server has received a string
         * that your datahandler need to take care of.
         * @param data The received string.
         */
        public void handleServerData(String data, String address) {
            //does the string contain an XML declaration line?
            int xmlDeclarationStart = data.indexOf("<?xml");
            if (xmlDeclarationStart!=-1) {
                //if so, start receiving the (new) XML document (discarding any previously received data)
                receivedString = data.substring(xmlDeclarationStart) + MySystem.lineBreak;
                xmlDeclarationReceived = true;
            }
            //does the string contain the root element of a macbeth packet?
            int mbpacketStart = data.indexOf("<mbpacket");
            if (mbpacketStart != -1) {
                //If so, we're about to begin receiving a packet, but first we must make sure that
                //the XML document contains an XML declaration. If this has not already been received...
                if (!xmlDeclarationReceived) {
                    //begin with a standard declaration (assume ascii!)
                    receivedString = "<?xml version=\"1.0\" encoding=\"ascii\"?>" + MySystem.lineBreak;
                    //and add everything after the root element
                    receivedString += data.substring(mbpacketStart);
                }
                //the XML declaration HAS been received
                else {
                    //just append the data to the existing string
                    receivedString += data;
                }
                receivingPacketString = true;
                //also put the clients remote address into this packet
                receivedString += MySystem.lineBreak + "<remoteaddress value=\"" + clientRemoteAddress + "\"/>" + MySystem.lineBreak;
            }
            //does this line close the root element of a macbeth packet?
            int mbpacketEnd = data.indexOf("</mbpacket>");
            if (mbpacketEnd != -1 && receivingPacketString) {
                receivedString += data.substring(0, mbpacketEnd + "</mbpacket>".length());
                MbPacket p = new MbPacket();
                //if so, try to create a packet from the string we've received
                try {
                    p.createFromString(receivedString);
                    //if successful, handle this packet now
                    packetReceived(p);
                }
                catch (SAXException e) {
                    //if unsuccessful, log this error
                    _info.println("A string was received on the server port, but it could not " +
                                  "be identified as a packet due to syntax errors (" + e + ")");
                    _info.println("The received string was:" + MySystem.lineBreak + receivedString);
                }
                //clear received-string now
                receivedString = new String();
                xmlDeclarationReceived = false;
            }
            //all data inside the packet should just be appended
            if (mbpacketStart==-1 && mbpacketEnd==-1 && xmlDeclarationStart==-1 && receivingPacketString) {
                receivedString += data;
            }
        }

        /**
         * Notifies your datahandler about a clients remote address.
         * Will be called by server when a new socket is opened.
         * @param address The clients remote address (IP number)
         */
        public void setRemoteAddress(String address) {
            //save the remote address for later use
            clientRemoteAddress = address;
        }

        public void clientDropped(String address) { }
    }
}
