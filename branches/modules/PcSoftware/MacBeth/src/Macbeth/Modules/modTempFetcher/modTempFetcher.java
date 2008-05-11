/*
 * File created by arune
 * at 2004-jan-02 18:00:08
 */
package Macbeth.Modules.modTempFetcher;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.HashMap;
import java.util.Iterator;
import java.math.BigDecimal;
import java.io.*;


import Macbeth.System.*;
import Macbeth.Utilities.UByte;
import Macbeth.Utilities.MyDateTime;
import Macbeth.Utilities.DataRepository;
import Macbeth.XML.XMLDataHandler;

/**
 *
 *
 *
 *
 *
 *
 * @author arune
 * @version 1.0
 */

public class modTempFetcher extends MbModule { //implements MbModuleGUI {
    //The sensordata that we receive via XML
    private UByte[] receivedBytes;
    //Interval between the data collecting. default frequency is every 60th second
    private int timerInterval = 60;
    private int nrOfSamples = 0;
    //our data request timer
    private Timer requestDataTimer;

    private HashMap sensors;

    private HashMap sensorNodes;

    private boolean parsingARNEXML;

    //A GUI for our module.
    private GUI gui;
    //Is our GUI currently visible or not?
    private boolean guiVisible = false;

    /**
     * Creates a new instance of modTempFetcher.
     */
    public modTempFetcher() {
        //construct MbModule
        super();
        //todo: ?
        receivedBytes = new UByte[6];
        parsingARNEXML = false;
        gui = new GUI();
        requestDataTimer = null;
        sensors = new HashMap(8);
        sensorNodes = new HashMap(8);
        //we depend on the ARNE-module
        addDependency("ARNE");
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "TempFetcher";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Collects temperature data from ARNE bus";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    protected void initDataFields() {
        options.putField("timerinterval", "60");
        options.putField("samples", "5");
    }

    /**
     * Starts up this module.
     */
    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();
        //get options from data repository
        timerInterval = Integer.parseInt(options.getField("timerinterval"));
        nrOfSamples = Integer.parseInt(options.getField("samples"));

        if (timerInterval > 0) {
            TimerListener lyssnare = new TimerListener();
            requestDataTimer = new Timer(1000 * timerInterval, lyssnare);
            requestDataTimer.start();
        }

        if (nrOfSamples <= 0) {
            nrOfSamples = 1;    //to not cause div by 0
        }

        DataRepository.DataList list = dataRepository.getList("sensors");
        Iterator it = list.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String sensorName = item.getField("name");
            String sensorLogPath = item.getField("logpath");
            UByte sensorIdentByte = UByte.parseUByte(item.getField("identbyte"));
            boolean printToDisplay = Boolean.valueOf(item.getField("printtodisplay")).booleanValue();
            sensors.put(sensorName, new TempSensor(sensorName, sensorIdentByte, sensorLogPath, nrOfSamples, printToDisplay));
        }

        list = dataRepository.getList("sensornodes");
        it = list.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String nodeName = item.getField("name");
            sensorNodes.put(nodeName, nodeName);
        }


    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //stop timer if it's running
        if (requestDataTimer!=null) {
            if (requestDataTimer.isRunning()) {
                requestDataTimer.stop();
            }
        }
        //shut down MbModule
        super.shutdown();
    }



    private void handleSensor(UByte[] bytes, int index) {
        final int D1 = -40; final double D2 = 0.01;
        //start searching on index
        //match first byte (byte index) with any of the sensors identbyte
        if (!sensors.isEmpty()) {
            TempSensor actualSensor = null;
            Iterator it = sensors.keySet().iterator();
            while (it.hasNext()) {
                //_debug.println("handleSensor, looping through sensors");
                String key = (String) it.next();
                TempSensor f = (TempSensor) sensors.get(key);
                if (f != null) {
                    //_debug.println("handleSensor, f:" + f.identByte + " bytes[index]: " + bytes[index]);
                    if (f.identByte.equals(bytes[index])) {
                        //_debug.println("handleSensor, found an actualSensor");
                        actualSensor = f;
                    }
                }
            }

            if (actualSensor != null) {

                int iTemperature = bytes[index+1].shortValue() * 256;
                iTemperature = iTemperature + bytes[index+2].shortValue();
                double dTemp = (iTemperature * D2) + D1;
                //_debug.println(actualSensor.name + " temperature is " + dTemp);
                if (actualSensor.addValue(dTemp)) {     //add value and check if ready to get mean
                    double dMeanTemp = actualSensor.getMean();
                    double dTempRadix2 = round(dMeanTemp, 2);
                    double dTempRadix1 = round(dMeanTemp, 1);

                    //kolla actualSensor.printToDisplay, om true ska paket skickas till display
                    //data = actualSensor.name " " dTempRadix1
                    //obs id, ttl osv!
                    //todo: perhaps change id to something based on actualSensor.identbyte?
                    if (actualSensor.printToDisplay) {
                        MbPacket p = new MbPacket();
                        //p.setDestination(new MbLocation("self","PacketReceiver"));
                        p.setDestination(new MbLocation("self","SmallDisplays"));
                        //ARNE packet in XML-format
                        //todo: byt ut gradertecknet mot &deg; eller nåt
                        p.setContents("  <lcdpacket sender=\"" + name()
                                            + "\" type=\"textrow\" prio=\"1\" ttl=\"0\" id=\"458328\" data=\""
                                            + actualSensor.name + " " + dTempRadix1 + "ÃŸ"
                                            + "\" />");
                        sendPacket(p);
                    }

                    //_debug.println("Printing " + dTempRadix1 + " at " + actualSensor.logPath);
                    printToFile(dTempRadix1, dTempRadix2, actualSensor.logPath);
                }
            }
        }

    }

    private void handleBytes(UByte[] bytes) {
        //_debug.println("handleBytes, bytes.length: " + bytes.length);
        if ((bytes.length % 3 == 0) && (bytes.length > 0)) {
            //_debug.println("handleBytes, mod ok");
            for (int i = 0; i < bytes.length; i = i + 3) {
                //_debug.println("handleBytes, calls handleSensor with i = " + i);
                handleSensor(bytes, i);
            }
        }
    }


    private void printToFile(double dataValue1, double dataValue2, String folderPath) {
        //create log output stream
        PrintStream log;
        try {
            //Date UTCTime = Date.valueOf("2004-01-08");
            long UTCTime = System.currentTimeMillis();
            String date = Long.toString(UTCTime);

            log = new PrintStream(new FileOutputStream(folderPath + "\\datetemphumid"));
            //log.println(MyDateTime.now("yyyy-MM-dd HH:mm:ss"));
            log.println(date.substring(0, date.length()-3));
            log.println(doubleToStringSpecial(dataValue2));
            log.flush();
            log.close();
            log = new PrintStream(new FileOutputStream(folderPath + "\\temperature"));
            log.println(doubleToStringSpecial(dataValue2));
            log.flush();
            log.close();
            log = new PrintStream(new FileOutputStream(folderPath + "\\temperature_rd"));
            log.println(doubleToStringSpecial(dataValue1));
            log.flush();
            log.close();
            log = new PrintStream(new FileOutputStream(folderPath + "\\formated_rd"));
            log.println(" ");
            log.println(doubleToStringSpecial(dataValue1));
            log.flush();
            log.close();
            log = new PrintStream(new FileOutputStream(folderPath + "\\all-log.txt", true));
            log.println(MyDateTime.now("yyyy-MM-dd HH:mm:ss ") +
                                        doubleToStringSpecial(dataValue2));
            log.flush();
            log.close();
            log = new PrintStream(new FileOutputStream(folderPath + "\\datetime"));
            log.println(MyDateTime.now("yyyy-MM-dd HH:mm:ss"));
            log.flush();
            log.close();
        } catch (FileNotFoundException e) {
            _errors.println("Path not found, " + folderPath);
        }
    }

    public static String doubleToStringSpecial(double value) {
        return Double.toString(value).replace('.',',');
    }


    public static double round(double value, int radix) {
        BigDecimal bd;
        bd = new BigDecimal(value);
        bd = bd.setScale(radix,BigDecimal.ROUND_HALF_UP);
        return bd.doubleValue();
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
            //Ex: <arnepacket bytes="6">
            if (element.equals("arnepacket") && attributes.containsKey("bytes")) {
                int bytes = Integer.parseInt((String)attributes.get("bytes"));
                //we should always get six bytes from TempNode ('H' RHh RHl 'T' Th Tl)
               // if (bytes==6) {
                parsingARNEXML = true;
                receivedBytes = new UByte[bytes];
               // } else {
               //     _errors.println("invalid number of bytes in ARNE packet");
               // }
            }
            //Ex: <byte id="1" value="5" />
            else if (element.equals("byte") && attributes.containsKey("id") && attributes.containsKey("value")) {
                if (parsingARNEXML) {
                    short id = Short.parseShort(attributes.get("id").toString());
                    //if id is valid
                    if (id>0 && id<=receivedBytes.length) {
                        //remember byte
                        receivedBytes[id-1] = UByte.parseUByte((String)attributes.get("value"));
                    } else {
                        _errors.println("invalid byte IDs specified");
                        parsingARNEXML = false;
                    }
                } else {
                    _errors.println("byte-attribute found outside arnepacket-attribute");
                }
            }
        }

        /**
         * Called when end of an element was found in the XML-data.
         * Ex: </name> or <test attr="value" />
         * @param element The name of the element.
         */
        public void XMLendElement(String element) {
            //Ex: </arnepacket>
            if (element.equals("arnepacket")) {
                if (parsingARNEXML) {
                    parsingARNEXML = false;
                    //handle our received bytes now
                    handleBytes(receivedBytes);
                }
            }
        }

        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }


    /**
     * Will be called when our GUI should be shown.
     */
    public void renderGUI(Container drawArea) {
        gui.render(drawArea);
    }

    /**
     * A class for our GUI.
     */
    class GUI implements ActionListener {

        public void render(Container renderArea) {
            //renderArea.setPreferredSize(150, 200);
            renderArea.setLayout(new GridLayout(200,1));

            /*btnLampButtons = new JButton[roomObjects.size()];
            Iterator it = roomObjects.keySet().iterator();
            int i=0;
            while (it.hasNext()) {
                RoomObject o = (RoomObject)roomObjects.get((String)it.next());
                if (o!=null) {
                    btnLampButtons[i] = new JButton(o.name);
                    btnLampButtons[i].setActionCommand("roomobject:" + o.name);
                    btnLampButtons[i].addActionListener(this);
                    renderArea.add(btnLampButtons[i]);
                    i++;
                }
            }*/
        }

        public void actionPerformed(ActionEvent e) {
            //was this a room object-button press?
            /*if (e.getActionCommand().startsWith("roomobject:")) {
                //split its actioncommand to find out which lamp it was for
                String[] tokens = e.getActionCommand().split("roomobject:");
                //try to find a room object with this name
                RoomObject o = (RoomObject)roomObjects.get(tokens[1]);
                //if found
                if (o!=null) {
                    //toggle corresponding room object
                    toggleRoomObject(o.byteValue.shortValue());
                }
            }*/
        }
    }

    	//En lyssnare för timern
	private class TimerListener implements ActionListener {

//		public TimerLyssnare()
//		{
//		}

	   public void actionPerformed(ActionEvent e) {
           if (!sensorNodes.isEmpty()) {
               Iterator it = sensorNodes.keySet().iterator();
               while (it.hasNext()) {
                   //_debug.println("timerevent, looping through sensorNodes");
                   String f = (String) it.next();
                   //String f = (String) sensorNodes.get(key);
                   if (f != null) {
                       MbPacket p = new MbPacket();
                       p.setDestination(new MbLocation("self", "ARNE"));
                       p.setContents(  "<arnepacket bytes=\"6\" destnode=\"" + f + "\">" +
                                        "<byte id=\"1\" value=\"65\"/>" +    // send 'A', addressing
                                        "<byte id=\"2\" value=\"64\"/>" +    // send PC-node-address 0x40
                                        "<byte id=\"3\" value=\"0\"/>" +     // send PC-Host 0x00
                                        "<byte id=\"4\" value=\"85\"/>" +    // send PC-Module 0x55
                                        "<byte id=\"5\" value=\"68\"/>" +    // send 'D', data none
                                        "<byte id=\"6\" value=\"80\"/>" +    // send 'P' for poll
                                       "</arnepacket>");
                       sendPacket(p);

                   }
               }
           }
		}
	}
}
