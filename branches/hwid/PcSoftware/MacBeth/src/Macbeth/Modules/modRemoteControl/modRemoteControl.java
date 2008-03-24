/*
 * File created by Jimmy
 * at 2003-sep-06 15:09:02
 */
package Macbeth.Modules.modRemoteControl;

import java.util.*;
import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;

/**
 * A Macbeth module that receives remote control data from
 * different sources. It will then try to interprete this
 * data in order to find out which button was pressed
 * (buttons are mapped to byte combinations in its XML-
 * configuration file). If the button was known, then this
 * module will also send a notification to all other modules
 * that have requested to be notified about this specific
 * button.
 * @author Jimmy
 */
public class modRemoteControl extends MbModule {
    //The remote control bytes that we receive via XML
    private UByte[] receivedBytes;
    //Are we currently parsing valid ARNE remote control XML?
    private boolean parsingARNEXML;
    //A table containing the name of every known button, each with its corresponding bytes as key
    private HashMap remoteControlButtons;
    //A table containing all button requests made by other modules
    private HashMap buttonRequests;

    /**
     * Creates a new instance of modRemoteControl.
     */
    public modRemoteControl() {
        //construct MbModule
        super();
        receivedBytes = new UByte[3];
        parsingARNEXML = false;
        //mapping tables
        remoteControlButtons = new HashMap(256);
        buttonRequests = new HashMap(256);
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
        //register our HTML interface
        setHTMLInterface(new HTMLInterface());
    }

    public String name() {
        return "RemoteControl";
    }

    public String description() {
        return "Sends remote control notifications to other modules";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    protected void initDataFields() {
        //this module doesn't require any data fields
    }

    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();

        //get all remote control definitions from the data repository
        DataRepository.DataList list = dataRepository.getList("remotecontrolbuttons");
        if (list != null) {
            Iterator items = list.items();
            while (items.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem) items.next();
                String buttonname = (String) item.getField("buttonname");
                //read the unsigned bytes
                UByte b1 = UByte.parseUByte((String) item.getField("byte1"));
                UByte b2 = UByte.parseUByte((String) item.getField("byte2"));
                //create a key string from the two bytes
                String keyString = b1.toString() + ":" + b2.toString();
                //store all key definitions in a list
                remoteControlButtons.put(keyString, buttonname);
            }
        }
        //no remote control definitions found in data repository
        else {
            _info.println("remotecontrolbuttons-list is missing. no remote control buttons defined!");
        }
    }

    public void shutdown() {
        //shut down MbModule
        super.shutdown();
    }

    /**
     * Takes care of successfully received bytes from any remote control.
     * @param bytes The bytes that have been received.
     */
    private void handleBytes(UByte[] bytes) {
        //first byte is status
        UByte status = bytes[0];
        String state = (status.byteValue() & 0xF0)==0xF0 ? "DOWN" : "UP";
        //next two bytes are button ID
        UByte[] buttonID = new UByte[2];
        buttonID[0] = bytes[1];
        buttonID[1] = bytes[2];
        String keyString = buttonID[0].toString() + ":" + buttonID[1].toString();
        //do we know of this button?
        if (remoteControlButtons.containsKey(keyString)) {
            String buttonName = (String)remoteControlButtons.get(keyString);
            sendButtonNotification(buttonName,state);
        } else {
            _debug.println("Unknown remote control button pressed, bytes were:");
            _debug.println("  status: " + status.toString());
            _debug.println("  ID byte1: " + buttonID[0].toString());
            _debug.println("  ID byte2: " + buttonID[1].toString());
        }
    }

    /**
     * Call this when the state changes for a remote control button.
     * Notifications will be sent to the modules that have requested
     * notifications about this button.
     * @param buttonName The button name.
     * @param buttonState The button state ("UP" or "DOWN").
     */
    private void sendButtonNotification(String buttonName, String buttonState) {
        //has any module requested notifications about this button?
        if (buttonRequests.containsKey(buttonName)) {
            //find out which modules have requested notifications about this button
            ButtonRequests br = (ButtonRequests) buttonRequests.get(buttonName);
            String[] modules = br.getModuleNames();
            //and notify each of them
            for (int i = 0; i < modules.length; i++) {
                MbPacket p = new MbPacket();
                p.setContents("<remotecontrol button=\"" + buttonName + "\" state=\"" + buttonState + "\" />");
                p.setDestination(new MbLocation("self", modules[i]));
                sendPacket(p);
            }
        }
    }

    /**
     * A HTML-interface for this module.
     */
    private class HTMLInterface implements MbHTMLInterface {
        /**
         * Retreives the full code for your HTML-page. This is
         * formatted as plain text and must be valid HTML-code.
         * @return Full HTML-code for the page.
         */
        public String getHTMLPage() {
            String s = "<html><body>" + MySystem.lineBreak;
            s += "<h1>Remote Control module</h1>" + MySystem.lineBreak;
            s += "<p>This module receives data from Macbeth remote controls and notifies other modules about button events on such remote controls.</p>" + MySystem.lineBreak;
            s += "<p>The table below shows all known remote control buttons and which modules have requested to use them. You can also press the button names to emulate a button press/button release sequence for that button.</p>";
            s += "<center><p><table border=\"1\" cellpadding=\"3\" cellspacing=\"0\"><tr><td><b>Remote control button</b></td><td><b>Modules that use the button</b></td></tr>";
            //iterate through the known button names
            Iterator it = remoteControlButtons.values().iterator();
            while (it.hasNext()) {
                String button = (String)it.next();
                if (button != null) {
                    s += "<tr><td><a href=\"button:" + button + "\">" + button + "</a></td><td>";
                    //check if there are any requests for this button
                    String modulelist = "";
                    ButtonRequests requests = (ButtonRequests)buttonRequests.get(button);
                    if (requests != null) {
                        String[] modules = requests.getModuleNames();
                        for (int i = 0; i < modules.length; i++) {
                            modulelist += modules[i];
                            if (i + 1 < modules.length) modulelist += "<br>";
                        }
                    }
                    s += modulelist + "</td></tr>";
                }
            }
            s += "</table></p></center>";
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
            if (s.length>=2) {
                /*  Ex: "button:PLAY"
                    this means we should emulate the PLAY button. */
                if (s[0].equals("button")) {
                    sendButtonNotification(s[1], "DOWN");
                    sendButtonNotification(s[1], "UP");
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
            /* Ex: <arnepacket bytes="2">
             * This is the start of an ARNE packet. We receive those from
             * ARNE remote controls when buttons have been pressed on them. */
            if (element.equals("arnepacket") && attributes.containsKey("bytes")) {
                int bytes = Integer.parseInt((String)attributes.get("bytes"));
                //we should always get three bytes from an ARNE remote control (1 status + 2 ident)
                if (bytes==3) {
                    parsingARNEXML = true;
                } else {
                    _errors.println("Invalid number of bytes in ARNE packet");
                }
            }
            /* Ex: <byte id="1" value="5" />
             * This is a byte inside an ARNE packet. */
            else if (element.equals("byte") && attributes.containsKey("id") && attributes.containsKey("value")) {
                if (parsingARNEXML) {
                    short id = Short.parseShort(attributes.get("id").toString());
                    //if id is valid
                    if (id>0 && id<=3) {
                        //remember byte
                        receivedBytes[id-1] = UByte.parseUByte((String)attributes.get("value"));
                    } else {
                        _errors.println("Invalid byte IDs specified");
                        parsingARNEXML = false;
                    }
                } else {
                    _errors.println("Byte-attribute found outside arnepacket-attribute");
                }
            }
            /* Ex: <buttonrequest button="NUMERIC4" module="RoomControl" />
             * This is a request from another macbeth module about receiving
             * notifications about one of our remote control buttons. We need
             * to save all button requests in a list so we can notify these
             * modules when the requested buttons are pressed. */
            else if (element.equals("buttonrequest") && attributes.containsKey("button") && attributes.containsKey("module")) {
                String button = (String)attributes.get("button");
                String module = (String)attributes.get("module");
                //traverse all remote control buttons to find out whether this button is known
                boolean buttonKnown = false;
                Iterator it = remoteControlButtons.values().iterator();
                while (it.hasNext()) {
                    String buttonName = (String)it.next();
                    if (buttonName!=null){
                        if (buttonName.equals(button)) {
                            buttonKnown = true;
                            break;
                        }
                    }
                }
                //if the button is known, lets remember this button request
                if (buttonKnown) {
                    //has this button been requested before?
                    if (buttonRequests.containsKey(button)) {
                        //if so, get its previous requests
                        ButtonRequests br = (ButtonRequests)buttonRequests.get(button);
                        //and att this request as well
                        br.addRequest(module);
                        buttonRequests.put(button,br);
                        //else, this is the first request for this button
                    } else {
                        //so create a new request object
                        ButtonRequests br = new ButtonRequests();
                        br.addRequest(module);
                        buttonRequests.put(button,br);
                    }
                }
                //else, if button was unknown
                else {
                    _errors.println("The button '" + button + "' was requested by '" + module + "', but this " +
                            "button is unknown! The request is denied");
                }
            }
        }

        /**
         * Called when end of an element was found in the XML-data.
         * Ex: </name> or <test attr="value" />
         * @param element The name of the element.
         */
        public void XMLendElement(String element) {
            /* Ex: </arnepacket>
             * This is the end of an ARNE packet. This means we have now
             * (probably) received an ARNE remote control button press. */
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
}
