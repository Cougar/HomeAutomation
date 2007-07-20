/*
 * File created by arune
 * at 2005-jan-09 16:54:44
 */
package Macbeth.Modules.modMailcheck;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;
import Macbeth.Utilities.*;

import java.util.*;
import javax.swing.Timer;
import java.awt.event.*;

/**
 *
 *
 * @author arune
 */
public class modMailcheck extends MbModule {

    //list of tv-channels and their variables
    private HashMap accounts;           //a map of AccountObjects
    private int timerInterval = 0;
    private Timer requestDataTimer = null;
    private String mailMode;
    private String displayText;

    //todo: mer utförliga felmeddelanden

    /**
     * Creates a new instance of modMailcheck.
     */
    public modMailcheck() {
        //construct MbModule
        super();
        //initial values

        accounts = new HashMap(32);
        //we depend on the RemoteControl-module
        addDependency("ARNE");

        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    public String name() {
        return "Mailcheck";
    }

    public String description() {
        return "Module for mail, checks mail and prints a message on display";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
        options.putField("mode", "simple");
        options.putField("checkinterval", "5");
        options.putField("displaytext", "New mail");
//        options.putField("port", "2050");
    }

    public void startup() throws MbStartupException {
        //startup MbModule
        super.startup();

        //get all accounts from the data repository
        DataRepository.DataList list = dataRepository.getList("accounts");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                String server = item.getField("server");
                String accountname = item.getField("accountname");
                String password = item.getField("password");
                int port = Integer.parseInt(item.getField("port"));
                accounts.put(accountname + server, new AccountObject(server, port, accountname, password));
            }
        }

        mailMode = options.getField("mode");
        displayText = options.getField("displaytext");

        timerInterval = Integer.parseInt(options.getField("checkinterval"));
        if (timerInterval > 0) {
            TimerListener lyssnare = new TimerListener();
            requestDataTimer = new Timer(1000*60 * timerInterval, lyssnare);
            requestDataTimer.start();
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

    private void simpleSendToDisplay() {
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self","SmallDisplays"));
        //ARNE packet in XML-format
        p.setContents("  <lcdpacket sender=\"" + name()
                            + "\" type=\"textrow\" prio=\"1\" ttl=\"20\""
                            + " id=\"" + 100559325 + "\""
                            + " data=\"" + displayText + "\""
                            + " />");
        sendPacket(p);
    }

    private void sendToDisplay(int nrOfNewMails) {
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self","SmallDisplays"));
        //ARNE packet in XML-format
        String displayTextReplaced = displayText.replaceAll("#", "" + nrOfNewMails);
        p.setContents("  <lcdpacket sender=\"" + name()
                            + "\" type=\"textrow\" prio=\"1\" ttl=\"20\""
                            + " id=\"" + 100559325 + "\""
                            + " data=\"" + displayTextReplaced + "\""
                            + " />");
        sendPacket(p);
    }



    /**
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {}
        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }
       	//En lyssnare för timern
	private class TimerListener implements ActionListener {

       public void actionPerformed(ActionEvent e) {
           if (!accounts.isEmpty()) {
               Iterator it = accounts.values().iterator();
               boolean newMess = false;
               int nrOfMess = 0;
               while (it.hasNext()) {
                    //_debug.println("timerevent, looping through sensorNodes");
                    AccountObject o = (AccountObject)it.next();
                    //String f = (String) it.next();
                    //String f = (String) sensorNodes.get(key);
                    if (o != null && !newMess) {
                        try {

                            if (mailMode.equals("simple")) {
                                newMess = o.hasNewMessages();
                            } else if (mailMode.equals("verbose")) {
                                nrOfMess = nrOfMess + o.nrOfMessages();
                            }
                           //_info.println("New mail: " + newMess);
                       } catch (Exception e2) {
                           _errors.println(e2);
                       }
                        //sleep(200);

                   }
               }
               if (newMess && mailMode.equals("simple")) {
                   simpleSendToDisplay();
               } else if (nrOfMess > 0 && mailMode.equals("verbose")) {
                   sendToDisplay(nrOfMess);
               }
               //_info.println("nr of mails " + nrOfMess);
           }
		}
	}
}