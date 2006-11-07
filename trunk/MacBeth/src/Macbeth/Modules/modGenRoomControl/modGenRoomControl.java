/*
 * File created by Jimmy
 * at 2003-sep-06 14:38:08
 */
package Macbeth.Modules.modGenRoomControl;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Iterator;
import java.util.HashMap;

import Macbeth.System.*;
import Macbeth.Utilities.UByte;
import Macbeth.Utilities.MySystem;
import Macbeth.Utilities.DataRepository;
import Macbeth.XML.XMLDataHandler;

/**
 * A Macbeth module that allows the user to turn on/off
 * lamps connected to the RoomControl ARNE node, which
 * is assumed to be connected to the ARNE bus. The ARNE
 * Macbeth module is required as well of course, or else
 * this module won't be able to communicate with the
 * RoomControl node.
 * @author Jimmy, arune
 * TODO: ska man testa om node-firendlyname existerar nar man laddar in dem?
 */
public class modGenRoomControl extends MbModule implements MbModuleGUI {

    //A GUI for our module.
    private GUI gui;
    //Is our GUI currently visible or not?
    private boolean guiVisible = false;
    //A list of known room objects.
    private HashMap roomObjects;

    /**
     * Creates a new instance of modRoomControl.
     */
    public modGenRoomControl() {
        //construct MbModule
        super();
        roomObjects = new HashMap(32);
        gui = new GUI();
        //we depend on the RemoteControl-module
        addDependency("RemoteControl");
        //we depend on the ARNE-module
        addDependency("ARNE");
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
        //we have a html-interface, so lets register it
        setHTMLInterface(new HTMLInterface());
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "GenRoomControl";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Controls any on/off-thing via the ARNE bus";
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

        //get all room objects from the data repository
        DataRepository.DataList list = dataRepository.getList("roomobjects");
        if (list!=null) {
            Iterator items = list.items();
            while (items.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)items.next();
                String name = (String)item.getField("name");
                String address = (String)item.getField("nodename");
                
                String bytes = (String)item.getField("byte");
                String[] bytearray = bytes.split(",");
                UByte[] b = new UByte[bytearray.length];
                for (int i = 0; i < bytearray.length; i++) {
                	b[i] = UByte.parseUByte(bytearray[i]);
                }
                //UByte b = UByte.parseUByte((String)item.getField("byte"));
                
                boolean pub = ((String)item.getField("public")).equals("true");
                String remotebtn = item.containsField("remotebutton") ? (String)item.getField("remotebutton") : "";
                roomObjects.put(name, new GenRoomObject(name, address, b, remotebtn, pub));
            }
        }
        //no room objects found in data repository
        else {
            _info.println("roomobjects-list is missing. no room objects defined!");
        }

        //if we have any room objects
        if (!roomObjects.isEmpty()) {
            //lets send requests for their remote control buttons
            MbPacket p = new MbPacket();
            p.setDestination(new MbLocation("self", "RemoteControl"));
            Iterator it = roomObjects.keySet().iterator();
            while (it.hasNext()) {
                String key = (String) it.next();
                GenRoomObject o = (GenRoomObject) roomObjects.get(key);
                if (o != null) {
                    p.appendContents("  <buttonrequest button=\"" + o.remoteButton + "\" module=\"" + name() + "\" />");
                }
            }
            sendPacket(p);
        }
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //shut down MbModule
        super.shutdown();
    }

    /**
     * A HTML-interface for this module.
     */
    private class HTMLInterface implements MbHTMLInterface {
        /**
         * Retreives the full code for your HTML-page. This is
         * formatted as plain text and must be valid HTML-code.
         * @return Full HTML-code for the page.
         * TODO: remove? redo? ta bort toggle? bor inte funkar alls om name anvamns
         */
        public String getHTMLPage() {
            String s = "<html><body>" + MySystem.lineBreak;
            s += "<h1>Room Control module</h1>" + MySystem.lineBreak;
            s += "<p>This module controls any on/off-node connected to ARNE bus.</p>" + MySystem.lineBreak;
            s += "<h2>Controls:</h2>";
            s += "<center><p><table border=\"1\" cellpadding=\"3\" cellspacing=\"0\">";
            Iterator it = roomObjects.keySet().iterator();
            while (it.hasNext()) {
                GenRoomObject o = (GenRoomObject)roomObjects.get(it.next());
                if (o!=null) {
                    if (o.displHTML) {      //if display this object (configured in modGenRoomControl.xml)
                        s += "<tr><td>" + o.name + "</td><td>[<a href=\"toggle:" + o.name + "\">toggle</a>]</td><td>[<a href=\"on:" + o.name + "\">ON</a>]</td><td>[<a href=\"off:" + o.name + "\">OFF</a>]</td></tr></tr>";
                    }
                }
            }
            s += "</table></center>";
            s += "</body></html>";
            return s;
        }

        /**
         * This method is invoked when a link is pressed on your
         * HTML-page.
         * @param href The href-attribute of the link.
         */
        public void linkPressed(String href) {
            String[] s = href.split(":");
            if (s.length >= 2) {
                /*  Ex: "toggle:Window lamp"
                    this means we should toggle the Window lamp. */
                if (s[0].equals("toggle")) {
                    GenRoomObject o = (GenRoomObject) roomObjects.get(s[1]);
                    if (o != null) {
                        toggleRoomObject(o.address, o.byteValue);
                    }
                }
                /*  Ex: "on:Window lamp"
                    this means we should turn ON the Window lamp. */
                else if (s[0].equals("on")) {
                    GenRoomObject o = (GenRoomObject) roomObjects.get(s[1]);
                    if (o != null) {
                        setRoomObject(o.address, o.byteValue ,true);
                    }
                }
                /*  Ex: "off:Window lamp"
                    this means we should turn OFF the Window lamp. */
                else if (s[0].equals("off")) {
                    GenRoomObject o = (GenRoomObject) roomObjects.get(s[1]);
                    if (o != null) {
                        setRoomObject(o.address, o.byteValue, false);
                    }
                }
            }
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
             * Ex: <remotecontrol button="NUMERIC4" state="DOWN" />
             * This is a notification from the remote control module
             * about one of our requested remote control buttons have
             * been pressed.
             */
            if (element.equals("remotecontrol") && attributes.containsKey("button") && attributes.containsKey("state")) {
                String state = (String)attributes.get("state");
                String button = (String)attributes.get("button");

                //find all room objects that are associated with this remote button
                Iterator it = roomObjects.values().iterator();
                while (it.hasNext()) {
                    GenRoomObject o = (GenRoomObject)it.next();
                    if (o.remoteButton.equals(button)) {
                        //toggle all such room objects
                        sendRoomObject(o.address, o.byteValue, state);
                    }
                }
            }
        }

        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }

    /**
     *
     * @param address
     * @param value
     * @param state
     */
    private void sendRoomObject(String address, UByte[] value, String state) {
        int iState = 0;

        if (state.equals("DOWN")) {
            iState = 240;
        }
        else if (state.equals("UP")) {
            iState = 0;
        }
        else if (state.equals("ON")) {
            iState = 1;
        }
        else if (state.equals("OFF")) {
            iState = 2;
        }

        //create a packet and address it to the ARNE module
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "ARNE"));
        //ARNE packet in XML-format
        String contentValueData = "";
        for (int i = 0; i < value.length; i++) 
        {
        	contentValueData = contentValueData + "<byte id=\"" + (i+2) + "\" value=\"" + Integer.toString(value[i].shortValue()) + "\"/>";
        }
        int arneBytes = value.length+1;
        p.setContents("<arnepacket bytes=\"" + arneBytes + "\" destnode=\"" + address + "\">" + //destnode -  address
                "<byte id=\"1\" value=\"" + Integer.toString(iState) + "\"/>" +
                contentValueData +
                "</arnepacket>");
        sendPacket(p);

    }

    /**
     * Toggles a room object (=lamp) by telling the RoomControl node
     * to toggle a specific output.
     * @param address
     * @param value The object to toggle (0 is first one).
     */
    private void toggleRoomObject(String address, UByte[] value) {
        //create a packet and address it to the ARNE module
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "ARNE"));
        //ARNE packet in XML-format
        String contentValueData = "";
        for (int i = 0; i < value.length; i++) 
        {
        	contentValueData = contentValueData + "<byte id=\"" + (i+2) + "\" value=\"" + Integer.toString(value[i].shortValue()) + "\"/>";
        }       
        int arneBytes = value.length+1;
        p.setContents("<arnepacket bytes=\"" + arneBytes + "\" destnode=\"" + address +"\">" +
                "<byte id=\"1\" value=\"240\"/>" +
                contentValueData +
                "</arnepacket>");
        sendPacket(p);
    }

    /**
     * Sets the state of a room object (=lamp) by telling the RoomControl
     * node either to turn on or to turn off the specified output.
     * @param address
     * @param value object whose state you want to set (0 is first one).
     * @param state The new state of the object. ON=true, OFF=false.
     */
    private void setRoomObject(String address, UByte[] value, boolean state) {
        //create a packet and address it to the ARNE module
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "ARNE"));
        //ARNE packet in XML-format
        String contentValueData = "";
        for (int i = 0; i < value.length; i++) 
        {
        	contentValueData = contentValueData + "<byte id=\"" + (i+2) + "\" value=\"" + Integer.toString(value[i].shortValue()) + "\"/>";
        }       
        int arneBytes = value.length+1;
        p.setContents("<arnepacket bytes=\"" + arneBytes + "\" destnode=\"" + address + "\">" +
                "<byte id=\"1\" value=\"" + (state==true ? "01" : "02") + "\"/>" +
                contentValueData +
                "</arnepacket>");
        sendPacket(p);
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
    private class GUI implements ActionListener {
        private JButton[]   btnLampButtons;

        public void render(Container renderArea) {
            Iterator it = roomObjects.keySet().iterator();
            int i=0;

            while (it.hasNext()) {
                GenRoomObject o = (GenRoomObject)roomObjects.get(it.next());
                if (o!=null) {
                    if (o.displHTML) {
                        i++;
                    }
                }
            }
            if (i > 0) {
                renderArea.setLayout(new GridLayout(i, 1));
                btnLampButtons = new JButton[i];
                it = roomObjects.keySet().iterator();
                i=0;
                while (it.hasNext()) {
                    GenRoomObject o = (GenRoomObject)roomObjects.get(it.next());
                    if (o!=null) {
                        if (o.displHTML) {
                            btnLampButtons[i] = new JButton(o.name);
                            btnLampButtons[i].setActionCommand("roomobject:" + o.name);
                            btnLampButtons[i].addActionListener(this);
                            renderArea.add(btnLampButtons[i]);
                            i++;
                        }
                    }
                }
            }
        }

        public void actionPerformed(ActionEvent e) {
            //was this a room object-button press?
            if (e.getActionCommand().startsWith("roomobject:")) {
                //split its actioncommand to find out which lamp it was for
                String[] tokens = e.getActionCommand().split("roomobject:");
                //try to find a room object with this name
                GenRoomObject o = (GenRoomObject)roomObjects.get(tokens[1]);
                //if found
                if (o!=null) {
                    //toggle corresponding room object
                    toggleRoomObject(o.address, o.byteValue);
                }
            }
        }
    }
}
