/*
 * File created by arune
 * at 2004-jan-02 18:00:08
 */
package Macbeth.Modules.modSmallDisplays;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.HashMap;
import java.util.Vector;
import java.util.Iterator;

import Macbeth.Utilities.*;
import Macbeth.System.*;
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
public class modSmallDisplays extends MbModule { //implements MbModuleGUI {
    //The sensordata that we receive via XML
    //private UByte[] receivedBytes;

    //Interval between refreshing display / queue
    //private int timerInterval = 0;
    //our refresh-display-timer
    private Timer refreshDisplayTimer;
    private boolean parsingARNEXML;
    //A GUI for our module.
    private GUI gui;
    //Is our GUI currently visible or not?
    private boolean guiVisible = false;
    //A list of known display objects.
    private Vector displayObjects = new Vector();

    private boolean parsingConfXML;
    private DisplayObject currentDisplay;
    /*private String target;
    private String info;
    private int width;
    //private int[] rowAddresses;
    private Vector rowAddresses = new Vector();
    private Vector moduleUsers = new Vector();*/

    /**
     * Creates a new instance of modRoomControl.
     */
    public modSmallDisplays() {
        //construct MbModule
        super();
        //receivedBytes = new UByte[6];
        //displayObjects = new HashMap(32);
        parsingARNEXML = false;
        gui = new GUI();
        //we depend on the ARNE-module
        addDependency("ARNE");
        //register our configuration data handler
        //setConfigDataHandler(new ConfigDataHandler());
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "SmallDisplays";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Handles small LCD-displays on ARNE bus";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    protected void initDataFields() {
        //this module doesn't require any data fields
    }

    /**
     * Starts up this module.
     */
    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();
        TimerListener lyssnare = new TimerListener();
        refreshDisplayTimer = new Timer(1000, lyssnare);
        refreshDisplayTimer.start();

        //get the display list from the data repository
        DataRepository.DataList displays = dataRepository.getList("displays");
        //for each defined display...
        Iterator it = displays.items();
        while (it.hasNext()) {
            Vector rowAddresses = new Vector();
            Vector moduleUsers = new Vector();

            //get its properties
            DataRepository.DataListItem display = (DataRepository.DataListItem) it.next();
            String target = display.getField("target");
            String info = display.getField("info");
            int width = Integer.parseInt(display.getField("width"));

            //get the row addresses for this display
            String[] addresses = display.getField("rowaddresses").split(",");
            //for each specified address
            for (int i=0; i<addresses.length; i++) {
                //check if valid...
                if (Integer.parseInt(addresses[i]) >= 0 && Integer.parseInt(addresses[i]) < 256) {
                    //and add to rowAddresses-list
                    rowAddresses.addElement(addresses[i]);
                } else {
                    _errors.println("Invalid display address specified in '" + configFile + "' !");
                }
            }

            //get the users of this display
            String[] users = display.getField("users").split(",");
            //add each user to the moduleUsers-list
            for (int i = 0; i < users.length; i++) {
                moduleUsers.addElement(users[i]);
            }

            //if no important properties were missing for this display,
            if (rowAddresses.size() > 0) {
                //add the display to the real display-list
                int rows = rowAddresses.size();
                displayObjects.addElement(new DisplayObject(target, info, width, rows, rowAddresses, moduleUsers));
            }
        }
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        if (refreshDisplayTimer!=null) {
            if (refreshDisplayTimer.isRunning()) {
                refreshDisplayTimer.stop();
            }
        }
        //shut down MbModule
        super.shutdown();
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
            // saker som lcd-backligth ska kunna stallas in genom att ange en nyckel i paketet
            // sånna saker ska inte skickas till DisplayObject? låt sendData ta hand om det?
            //dä ska visst till displayobject, så den kan halla reda på vad som är installt
            //http://deep.csbnet.se/arne/doc.arne/doc.arne.nod.lcd.html

            //bygg om, behover inte innehalla ttl, prio, id osv, satt till default annars

            if (element.equals("lcdpacket") && attributes.containsKey("sender")
                && attributes.containsKey("type") && attributes.containsKey("data")) {
                String sSender = (String)attributes.get("sender");
                String sType = (String)attributes.get("type");
                String sData = (String)attributes.get("data");

                //_debug.println("got package");

                int iPrio = 1;
                if (attributes.containsKey("prio")) {
                    iPrio = Integer.parseInt((String)attributes.get("prio"));
                }

                int timeToLive = 0;
                if (attributes.containsKey("ttl")) {
                    timeToLive = Integer.parseInt((String)attributes.get("ttl"));
                }

                int id = 0;
                if (attributes.containsKey("id")) {
                    id = Integer.parseInt((String)attributes.get("id"));
                }

                String rowDefine = "";
                if (attributes.containsKey("row")) {
                    rowDefine = (String)attributes.get("row");
                }


                //sok igenom varje displayobject och fraga om de vill ha data fran sSender
                for (int i = 0; i < displayObjects.size(); i++) {
                    DisplayObject dispObjDummy = (DisplayObject)displayObjects.elementAt(i);

                    if (dispObjDummy.displayModuleData(sSender)) {  //om datan skall skickas till denna display
                        //_debug.println("add row to queue: " + sData + " " + iPrio + " " + timeToLive + " " + id + " " + rowDefine);
                        dispObjDummy.addRowToQueue(sData, iPrio, timeToLive, false, id, rowDefine);
                    }
                }
            }
        }

        /**
         * Called when end of an element was found in the XML-data.
         * Ex: </name> or <test attr="value" />
         * @param element The name of the element.
         */
        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }


    //TODO: da modulen startas upp ska displayen rensas


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
        public void actionPerformed(ActionEvent e) {
            //loopa igenom alla displayObjects och kor manageQueues, getDataToSend and then send data, if any
            for (int i = 0; i < displayObjects.size(); i++) {
                DisplayObject dispDummy = (DisplayObject)displayObjects.elementAt(i);
                dispDummy.manageQueues();
                //_debug.println("managing queues");
                Vector vDataToSend = dispDummy.getDataToSend();
                //MbPacket p;
                for (int j = 0; j < vDataToSend.size(); j++) {
                    MbPacket p = (MbPacket)vDataToSend.elementAt(j);
                    p.setDestination(new MbLocation("self","ARNE"));
                    sendPacket(p);
                    //_debug.println("apan bepa");
                    //_debug.println(p.getContents());

                }
            }
        }
	}
}
