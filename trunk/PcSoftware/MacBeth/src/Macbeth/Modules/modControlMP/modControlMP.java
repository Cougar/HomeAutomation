/*
 * File created by Jimmy
 * at 2004-jan-12 14:17:44
 */
package Macbeth.Modules.modControlMP;

import Macbeth.System.MbModule;
import Macbeth.System.MbPacket;
import Macbeth.System.MbLocation;
import Macbeth.System.MbStartupException;
import Macbeth.Net.TcpClient;
import Macbeth.XML.XMLDataHandler;
import Macbeth.Utilities.*;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Macbeth module that controls external mediaplayers
 * (only arune's RemoteMedia at this point).
 * @author Jimmy
 */
public class modControlMP extends MbModule {

    //used for communication with RemoteMedia
    private TcpClient tcpClient;
    //list of used RemoteMedia functions
    private HashMap functions;

    /**
     * Creates a new instance of modControlMP.
     */
    public modControlMP() {
        //construct MbModule
        super();
        //initial values
        options.putField("host", "localhost");
        options.putField("port", "2050");
        tcpClient = new TcpClient(Integer.parseInt(options.getField("port")), options.getField("host"));
        functions = new HashMap(32);
        //we depend on the RemoteControl-module
        addDependency("RemoteControl");
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    public String name() {
        return "ControlMP";
    }

    public String description() {
        return "Controls external media players";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
        options.putField("host", "localhost");
        options.putField("port", "2050");
    }

    public void startup() throws MbStartupException {
        //startup MbModule
        super.startup();

        //get all RMP function bindings from the data repository
        DataRepository.DataList list = dataRepository.getList("RMPfunctions");
        Iterator it = list.items();
        while (it.hasNext()) {
            DataRepository.DataListItem item = (DataRepository.DataListItem) it.next();
            String function = item.getField("id");
            String remotebtn = item.getField("remotebutton");
            functions.put(function, new RemoteMediaFunction(function, remotebtn));
        }

        //if any RemoteMedia functions are used
        if (!functions.isEmpty()) {
            //lets send requests for their remote control buttons
            MbPacket p = new MbPacket();
            p.setDestination(new MbLocation("self", "RemoteControl"));
            it = functions.keySet().iterator();
            while (it.hasNext()) {
                String key = (String) it.next();
                RemoteMediaFunction f = (RemoteMediaFunction) functions.get(key);
                if (f != null) {
                    p.appendContents("  <buttonrequest button=\"" + f.remoteButton + "\" module=\"" + name() + "\" />");
                }
            }
            sendPacket(p);
        }
        //create the TCP client
        tcpClient = new TcpClient(Integer.parseInt(options.getField("port")), options.getField("host"));
    }

    //sends a string to the Remote Media Player socket server
    private void sendString(String s) {
        //try to open connection to the server and send the string
        try {
            tcpClient.openConnection();
            tcpClient.sendString(s);
            tcpClient.closeConnection();
        } catch (Exception e) {
            _info.println("Cannot connect to Remote Media Player at '" + options.getField("host") + ":" + options.getField("port") + "' ! " +
                    "Exception was: '" + e + "'");
        }
    }

    /**
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {
            /* Ex: <remotecontrol button="NUMERIC4" state="DOWN" />
             * This is a notification from the remote control module
             * about one of our requested remote control buttons have
             * been pressed. */
            if (element.equals("remotecontrol") && attributes.containsKey("button") && attributes.containsKey("state")) {
                String state = (String)attributes.get("state");
                String button = (String)attributes.get("button");
                if (state.equals("DOWN")) {
                    //find all RemoteMedia functions that are associated with this remote button
                    Iterator it = functions.values().iterator();
                    while (it.hasNext()) {
                        RemoteMediaFunction f = (RemoteMediaFunction)it.next();
                        //if this RemoteMedia function uses the remote button,
                        if (f.remoteButton.equals(button)) {
                            //try to send the ID string of this function to RemoteMedia
                            sendString(f.id);
                        }
                    }
                }
                else if (state.equals("UP")) {
                    //send btn-release-message to RMP (5000)
                    sendString("5000");
                }
            }
        }

        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }

}