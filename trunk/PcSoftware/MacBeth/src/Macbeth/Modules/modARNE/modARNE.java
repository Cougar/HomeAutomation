/*
 * File created by Jimmy
 * at 2003-sep-02 21:08:53
 */
package Macbeth.Modules.modARNE;

import Macbeth.System.*;
import Macbeth.XML.*;
import Macbeth.Utilities.*;
import javax.comm.*;
import java.util.*;
import java.io.*;

/**
 * Macbeth module that enables macbeth modules to communicate
 * with ARNE nodes via the serial port.
 * @author Jimmy
 * @author arune
 */
public class modARNE extends MbModule {
    //TODO: timeout, if an arne-packet is not completely received during this time, that packet should be flushed

    //problem med att skicka två paket (eller fler) i följd, CTS från avr kommer först när första paketet skickats
    //klart, då har redan paket två placerats i bufferten
    //jag har försökt med handskakning (dvs hårdvaran ska ta hand om CTS och sluta skicka) men det vill inte fungera
    //testat kolla getOutputBufferSize, men den verkar alltid vara 0
    //testat eventen OUTPUT_BUFFER_EMPTY, för att hålla en flagga så man vet när data finns i buffer, men nix

    //This will handle all serial port IO
    private SerialPortIO spr;
    //Which byte are we currently receiving from ARNE?
    private byte currentReceiveByte;
    //A temporary ARNE packet that will be used when receiving from ARNE
    private ARNEPacket apTempARNE;
    //A temporary holder for pc host byte
    private byte pcHost=0;
    //A temporary holder fo pc module byte
    private byte pcModule=0;
    //A temporary ARNE packet that will be used when receiving XML from macbeth
    private ARNEPacket apTempMacbeth;
    //A list of Macbeth module names, where each name is mapped to a byte
    private HashMap moduleMappings;
    //A list of Macbeth kernel names, where each name is mapped to a byte
    private HashMap kernelMappings;
    //A list of ARNE node names, where each name is mapped to a byte
    private HashMap nodeMappings;
    //This flag is set true when an ARNE packet in XML format is currently being parsed
    private boolean parsingARNEXML;

    /**
     * Creates a new instance of modARNE.
     */
    public modARNE() {
        //construct MbModule
        super();
        //initial values...
        currentReceiveByte = 1;
        apTempARNE = null;
        apTempMacbeth = null;
        options.putField("comport", "COM1");
        parsingARNEXML = false;
        //mapping tables
        moduleMappings = new HashMap(64);
        kernelMappings = new HashMap(32);
        nodeMappings = new HashMap(64);
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "ARNE";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Allows for communication between modules and ARNE nodes";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
        options.putField("comport", "COM1");
    }

    /**
     * Starts up this module.
     */
    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();

        //init serial port
        spr = new SerialPortIO(options.getField("comport"));
        try {
            spr.start();
        } catch (IOException e) {
            //we cannot start up if serial port failed to initialize
            spr.stop();
            throw new MbStartupException("The serialport failed to initialize (" + e.getMessage() + ")");
        }

        //get all module mappings from data repository
        DataRepository.DataList modules = dataRepository.getList("modulemappings");
        Iterator it = modules.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String name = item.getField("name");
            UByte b = UByte.parseUByte(item.getField("byte"));
            moduleMappings.put(b, name);
        }

        //get all kernel mappings from data repository
        DataRepository.DataList kernels = dataRepository.getList("kernelmappings");
        it = kernels.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String name = item.getField("name");
            UByte b = UByte.parseUByte(item.getField("byte"));
            kernelMappings.put(b, name);
        }

        //get all kernel mappings from data repository
        DataRepository.DataList nodes = dataRepository.getList("nodemappings");
        it = nodes.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String name = item.getField("name");
            UByte b = UByte.parseUByte(item.getField("byte"));
            nodeMappings.put(name, b);
        }

    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //shut down MbModule
        super.shutdown();
        spr.stop();
    }

    /**
     * Takes care of bytes that we've received from ARNE.
     * @param bytes The received bytes.
     */
    protected void handleIncomingBytes(byte[] bytes) {
        for (int i=0; i<bytes.length; i++) {
            //byte1 is arneheader byte (control bits and data len)
            if (currentReceiveByte==1) {
                //check control bits
                if ((bytes[i] & 0xA0) == 0xA0) {
                    apTempARNE = new ARNEPacket();
                    //if OK, save byte and go on
                    apTempARNE.header[0] = new UByte(bytes[i]);
                    //this byte determines data length,
                    //so allocate memory now
                    int size = bytes[i] & 0x0F;
                    if (size>0 && size<ARNEPacket.MAX_ARNE_DATALEN) {
                        apTempARNE.setDataLen(size);
                        currentReceiveByte++;
                    }
                }
            }
            //byte2 is PC-host byte
            else if (currentReceiveByte==2) {
                pcHost = UByte.fromShort((short)bytes[i]).byteValue();
                currentReceiveByte++;
            }
            //byte3 is PC-module byte
            else if (currentReceiveByte==3) {
                pcModule = UByte.fromShort((short)bytes[i]).byteValue();
                currentReceiveByte++;
            }
            //the rest are arne-data bytes
            else {
                //check if we're still filling data buffer
                if (currentReceiveByte <= apTempARNE.getDataLen()+3) {
                    //if so, just continue to fill
                    apTempARNE.data[currentReceiveByte-4] = new UByte(bytes[i]);
                    //was this the last byte?
                    if (currentReceiveByte==apTempARNE.getDataLen()+3) {
                        //if so, handle all data, and restart
                        handleIncomingARNEPacket(apTempARNE, pcHost, pcModule);
                        currentReceiveByte = 1;
                    } else {
                        //else, go on
                        currentReceiveByte++;
                    }
                }
            }
        }
    }

    /**
     * Takes care of an ARNE packet that've been received from ARNE.
     * @param ap The received ARNE packet.
     * @param pcHost The PC Host byte. This will be translated to a Macbeth Kernel name.
     * @param pcModule The PC Module byte. This will be translated to a Macbeth Module name.
     */
    private void handleIncomingARNEPacket(ARNEPacket ap, byte pcHost, byte pcModule) {
        //_debug.println("handling incoming ARNE packet");
        MbPacket p = new MbPacket();
        p.getDestination().setKernel((String)kernelMappings.get(new UByte(pcHost)));
        p.getDestination().setModule((String)moduleMappings.get(new UByte(pcModule)));
        if (p.getDestination().getKernel()!=null & p.getDestination().getModule()!=null) {
            p.setContents("<arnepacket bytes=\"" + ap.data.length + "\">");
            for (int i=0; i<ap.data.length; i++) {
                p.appendContents("<byte id=\"" + Integer.toString(i+1) + "\" value=\"" + ap.data[i].toString() + "\" />");
            }
            p.appendContents("</arnepacket>");
            sendPacket(p);
        } else {
            _errors.println("Incoming arne packet had unknown host- and/or module-bytes!");
            _errors.println("(host: " + UByte.toString(pcHost) + ", module: " + UByte.toString(pcModule) + ")");
        }
    }

    /**
     * Sends an ARNE packet to the ARNE PC node.
     * @param ap The ARNE packet to send.
     */
    private void sendARNEPacket(ARNEPacket ap) {
        //_debug.println("sending ARNE packet:");
        byte[] bytes = new byte[2+ap.getDataLen()];
        bytes[0] = ap.header[0].byteValue();
        bytes[1] = ap.header[1].byteValue();
        for (int i=0; i<ap.getDataLen(); i++) {
            bytes[2+i] = ap.data[i].byteValue();
            //_debug.println("  byte" + Integer.toString(i) + "=" + ap.data[i].toString());
        }
        try {
            spr.sendBytes(bytes);
        } catch (IOException e) {
            _errors.println("An ARNE packet could not be delivered to the ARNE bus due to I/O-errors! (Exception was '" + e + "')");
        }
    }

    /**
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        /**
         * Called when start of a new element is found in the XML-data.
         * Ex: <name attr1="value1" attr2="value2">
         * Element name would then be "name" and attribute list
         * would contain "value1" and "value2" mapped to the attribute
         * names "attr1" and "attr2".
         * @param element The name of the element.
         * @param attributes The element attributes.
         */
        public void XMLstartElement(String element, HashMap attributes) {
            /**
             * Ex: <arnepacket bytes="5" destnode="25">
             * This is the start of an ARNE packet that some other module
             * is sending to us.
             */
            if (element.equals("arnepacket") && attributes.containsKey("bytes") && attributes.containsKey("destnode")) {
                short bytes = Short.parseShort((String)attributes.get("bytes"));
                String destNode = (String)attributes.get("destnode");
                //check if specified ARNE node is valid
                if (nodeMappings.containsKey(destNode)) {
                    //check if number of bytes is valid
                    if (bytes>0 && bytes<=ARNEPacket.MAX_ARNE_DATALEN) {
                        parsingARNEXML = true;
                        apTempMacbeth = new ARNEPacket();
                        apTempMacbeth.setDataLen(bytes);
                        apTempMacbeth.setNodeAddress(((UByte)nodeMappings.get(destNode)).byteValue());
                    } else {
                        //invalid number of bytes specified (too few or too many)
                        _errors.println("Invalid number of bytes specified in a packet!");
                    }
                } else {
                    //invalid destination ARNE node was specified
                    _errors.println("A packet was addressed to the ARNE node '" + destNode + "', which is unknown!");
                }
            }
            /**
             * Ex: <byte id="1" value="5">
             * This is a byte inside an ARNE packet.
             */
            else if (element.equals("byte") && attributes.containsKey("id") && attributes.containsKey("value")) {
                //only care about byte-tags while parsing valid ARNE packets
                if (parsingARNEXML) {
                    short id = Short.parseShort((String)attributes.get("id"));
                    //if id is valid
                    if (id>0 && id<=apTempMacbeth.getDataLen()) {
                        //set data in arne packet
                        //int intval = Integer.parseInt((String)attributes.get("value"));
                        apTempMacbeth.data[id-1] = UByte.parseUByte((String)attributes.get("value"));
                    } else {
                        _errors.println("Invalid byte IDs specified in a packet");
                        parsingARNEXML = false;
                    }
                } else {
                    //we are not parsing a valid ARNE packet at the moment, so ignore byte-tags
                }
            }
        }

        /**
         * Called when end of an element was found in the XML-data.
         * Ex: </name> or <test attr="value" />
         * @param element The name of the element.
         */
        public void XMLendElement(String element) {
            /**
             * Ex: </arnepacket>
             * End of a received ARNE packet. At this point we should send
             * the whole received packet to the ARNE bus.
             */
            if (element.equals("arnepacket")) {
                if (parsingARNEXML) {
                    //check if all bytes have been parsed
                    boolean allBytesParsed = true;
                    for (int i=0; i<apTempMacbeth.getDataLen(); i++) {
                        if (apTempMacbeth.data[i]==null) {
                            allBytesParsed = false;
                        }
                    }
                    if (allBytesParsed) {
                        //if that is the case, send the packet to the ARNE bus now
                        parsingARNEXML = false;
                        sendARNEPacket(apTempMacbeth);
                        //_debug.println("Ive got packet!");
                    } else {
                        //if not all bytes have been parsed, packet was invalid
                        _errors.println("Invalid packet received! (bytes were missing)");
                    }
                }
            }
        }

        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }


    /**
     * A class for handling all serial port IO.
     */
    public class SerialPortIO implements Runnable, SerialPortEventListener {
        private CommPortIdentifier portId;
        private SerialPort serialPort;
        private boolean isOutputBufferEmpty;
        private InputStream inputStream;
        private OutputStream outputStream;
        private Thread readThread;
        private String portName;
        //a queue for byte sequences that're waiting to be sent
        private LinkedList byteQueue;
        //a thread that makes sure queued byte sequences are sent later
        private ByteQueueHandler byteQueueHandler;

        /**
         * Creates a new instance of SerialPortIO.
         * @param portname The name of the port to use.
         */
        public SerialPortIO(String portname) {
            portName = portname;
            portId = null;
            serialPort = null;
            inputStream = null;
            outputStream = null;
            byteQueue = new LinkedList();
            byteQueueHandler = new ByteQueueHandler(byteQueue);
            isOutputBufferEmpty = true;
        }

        /**
         * Starts listening for data on the port. Also enables
         * outbound data transmission on the port.
         * @throws IOException When, for some reason, the port fails
         * to initialize.
         */
        public void start() throws IOException {
            try {
                portId = CommPortIdentifier.getPortIdentifier(portName);
            } catch (NoSuchPortException e) {
                throw new IOException("The port " + portName + " was not found on this system!");
            }
            try {
                serialPort = (SerialPort) portId.open("modARNE", 2000);
            } catch (PortInUseException e) {
                throw new IOException("The port " + portName + " is already in use! ('" + e + "')");
            }
            try {
                inputStream = serialPort.getInputStream();
                outputStream = serialPort.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                serialPort.addEventListener(this);
            } catch (TooManyListenersException e) {
                e.printStackTrace();
            }
            serialPort.notifyOnDataAvailable(true);
            //serialPort.notifyOnOutputEmpty(true);       //serialEvent for when the output buffer is empty (a new packet can be sent)
            try {
                serialPort.setSerialPortParams(19200,SerialPort.DATABITS_8,SerialPort.STOPBITS_1,SerialPort.PARITY_NONE);
                //serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
                serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
            } catch (UnsupportedCommOperationException e) {
                e.printStackTrace();
            }
            readThread = new Thread(this);
            readThread.start();
        }

        /**
         * Stops listening for data on the port.
         */
        public void stop() {
            byteQueueHandler.stopWorking();
            if (serialPort!=null) {
                serialPort.close();
            }
        }

        /**
         * Will be called from the thread.
         */
        public void run() {
            //don't do shit here. we will be notified about incoming data
            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
            }
        }

        /**
         * Will be called when an event is triggered by the port.
         * @param event The event.
         */
        public void serialEvent(SerialPortEvent event) {
            switch(event.getEventType()) {
                case SerialPortEvent.BI:
                    break;
                case SerialPortEvent.OE:
                    break;
                case SerialPortEvent.FE:
                    break;
                case SerialPortEvent.PE:
                    break;
                case SerialPortEvent.CD:
                    break;
                case SerialPortEvent.CTS:
                    break;
                case SerialPortEvent.DSR:
                    break;
                case SerialPortEvent.RI:
                    break;
                case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
                    //isOutputBufferEmpty = true;
                    break;
                case SerialPortEvent.DATA_AVAILABLE:
                    //there is incoming data available on the port, so read it
                    byte[] readBuffer = new byte[20];

                    try {
                        int numBytes = 0;
                        while (inputStream.available() > 0) {
                            numBytes = inputStream.read(readBuffer);
                        }
                        byte[] readBufferDummy = new byte[numBytes];
                        for (int i = 0; i < numBytes; i++) {
                            readBufferDummy[i] = readBuffer[i];
                        }
                        handleIncomingBytes(readBufferDummy);
                    } catch (IOException e) {
                    }
                    break;
            }
        }

        /**
         * Tries to sends a byte array to the port. If the CTS-flag is not
         * set, the byte sequence will be inserted into a queue and sent
         * later, when the CTS-flag has been set again.
         * @param bytes The bytes to send.
         * @throws IOException if an I/O-error occurs while trying
         * to send the data.
         */
        public void sendBytes(byte[] bytes) throws IOException {
            //check if it is clear to send and no other bytes are in send-queue
//if-case is commented, always put in queue
/*            if (serialPort.isCTS() && byteQueue.isEmpty()) {
                //if everything's OK, send the bytes immediately
                writeBytes(bytes);
            }*/
            
            //we cannot send yet
//            else {
                //so insert the new bytes into the queue instead
                if (byteQueue.size()<100) {
                    byteQueue.addLast(bytes);
                }
                else {
                    _errors.println("There are more than 100 byte sequences in send-queue. The queue will be cleared. Please check your serial port hardware!!");
                    byteQueue.clear();
                }
                //and start the byte queue handler if not already started
                if (!byteQueueHandler.isWorking()) {
                    byteQueueHandler.startWorking();
                }
                //_debug.println("An ARNE packet was inserted in the send-queue! It will be sent as soon as possible.");
//            }
        }

        /**
         * Sends a byte array to the port. No flow control will be used!!
         * You must make sure the CTS-flag is set before calling this!
         * @param bytes The bytes to send.
         * @throws IOException if an I/O-error occurs while trying
         * to send the data.
         */
        private synchronized void writeBytes(byte[] bytes) throws IOException {
            if (outputStream != null) {
                outputStream.write(bytes);
            }
            else {
                _errors.println("Serious error! Cannot write bytes to serial port since the outputstream doesn't exist!");
            }
        }


        //this is a thread that will take care of the byte queue
        private class ByteQueueHandler extends Thread {
            private LinkedList byteQueue;
            private boolean runThread;

            public ByteQueueHandler(LinkedList byteQueue) {
                super("ByteQueueHandler");
                this.byteQueue = byteQueue;
            }

            public void startWorking() {
                runThread = true;
                //start thread (it will call our run-method in a new thread)
                start();
            }

            public void stopWorking() {
                runThread = false;
            }

            public boolean isWorking() {
                return runThread;
            }

            //This is called by the thread when running.
            public void run() {
                //loop while run-flag is set true and queue is not empty
                while (runThread) {
                    //if queue is not empty and CTS-flag is set
                    //if (!byteQueue.isEmpty() && serialPort.isCTS() && isOutputBufferEmpty) {
                    //if (!byteQueue.isEmpty()) {
                    if (!byteQueue.isEmpty() && serialPort.isCTS()) {
                        //let's send the next arne packet in queue
                        try {
                            writeBytes((byte[])byteQueue.removeFirst());
                            //isOutputBufferEmpty = false;
                            //_debug.println("nr of bytes in buffer: " + serialPort.getOutputBufferSize());
                            //is the queue empty now?
                            if (byteQueue.isEmpty()) {
                                //_debug.println("All ARNE-packets in send-queue have now been sent!");
                            }
                        } catch (IOException e) {
                            _errors.println("I/O-error while trying to write bytes to port (Exception was '" + e + "')");
                        }
                        try {
                            sleep(50);
                        } catch (InterruptedException e) {
                            stopWorking();
                        }
                    }
                    //CTS-flag hasn't been set yet, or else the queue is empty
                    else {
                        //so try to get some sleep for a while =)
                        try {
                            sleep(50);
                        } catch (InterruptedException e) {
                            stopWorking();
                        }
                    }
                }
            }
        }
    }

}
