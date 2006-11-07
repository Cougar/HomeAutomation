/*
 * File created by arune
 * at 2004-dec
 */
package Macbeth.Modules.modCallerID;

import java.util.*;
import java.net.*;
import java.io.*;
import java.io.InputStreamReader;
import java.io.BufferedReader;

import Macbeth.Modules.modVagnen.StopObject;
import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;

/**
 *
 *
 * @author arune
 */
public class modCallerID extends MbModule {
    //The callerID bytes that we receive via XML
    private UByte[] receivedBytes;
    
    private ArrayList servers = new ArrayList();    //a list of servers

    private String menuMiddleId;
    //Are we currently parsing valid ARNE remote control XML?
    private boolean parsingARNEXML;	
	//menusystem specific bool, if this submenu is activated
	private boolean activated;
    //list of calls and their variables
    private ArrayList calls = new ArrayList();           //a map of CallObjects
	
    /**
     * Creates a new instance of modTestMenu.
     */
    public modCallerID() {
        //construct MbModule
        super();
        
        calls = new ArrayList();
        //initial values
        receivedBytes = new UByte[15];
        menuMiddleId = "0";
        activated = false;
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
        
        
    }

    public String name() {
        return "CallerID";
    }

    public String description() {
        return "Receives CallerID information from ARNE-network";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
//        options.putField("host", "localhost");
//        options.putField("port", "2050");
    }

    public void startup() throws MbStartupException {
        //startup MbModule
        super.startup();
        
        //get all servers from the data repository
        DataRepository.DataList list = dataRepository.getList("servers");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                String serverlocation = item.getField("serverlocation");
                servers.add(serverlocation);
            }
        }

        sendToMenuSys("<registersubmod displayname=\"CallerID\" displaydesc=\"Shows the last incoming phonecalls\" visible=\"true\" />");
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        sendToMenuSys("<unregistersubmod />");
        //shut down MbModule        
        super.shutdown();
    }
    
    private void sendToMenuSys(String dataToSend) {
       	MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "MenuSystem"));
        p.appendContents(dataToSend);
        sendPacket(p);
    }
    /**
     * Takes care of successfully received bytes from the callerID node.
     * @param bytes The bytes that have been received.
     */
    private void handleBytes(UByte[] bytes) {
    	String callerIDdata = "";
    	for (int i = 0; i < bytes.length; i++) {
    		callerIDdata = callerIDdata + parseByte(bytes[i]) + "";
    	}
		_debug.println("Data " + callerIDdata);
    	
		calls.add(new CallObject(bytes, servers));
		
		if (activated) {
			sendMain();
		} else {
			sendActivationReq();	//ska be om tillåtelse att få menyn aktiverad
		}
    }
    
    private void sendActivationReq() {
//    	ska be om tillåtelse att få menyn aktiverad
    	String dataToSend = "<request />";
    	
    	//dataToSend = dataToSend + "</packet>";
    	sendToMenuSys(dataToSend);
    }
    
    private void sendMain() {
    	String dataToSend = "<packet menuid=\"MainMenu\"><data>"; 
    	dataToSend = dataToSend + "<middleid>" + menuMiddleId + "</middleid>";
    	dataToSend = dataToSend + "<list id=\"menudata\">";
    	for (int i = 0; i < calls.size(); i++) {
    		CallObject call = (CallObject)calls.get(calls.size()-i-1);
    		
    		String datetime = call.getDate();
    		String name = call.getName();
    		String address = call.getAddress();
    		String number = call.getPhoneNr();
    		String ortsnamn = call.getOrtsnamn();
    		
    		if (name.length() != 0) {
    			if (address.length() != 0) {
        			dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + datetime + " - " + name + "\" desc=\"" + number + "&#10;&#10;" + address + "\" />";
    			} else if (ortsnamn.length() != 0) {
    				dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + datetime + " - " + name + "\" desc=\"" + number + "&#10;&#10;" + ortsnamn + "\" />";
    			} else {
    				dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + datetime + " - " + name + "\" desc=\"" + number + "\" />";
    			}
    		} else {
    			dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + datetime + " - " + number + "\" desc=\"" + ortsnamn + "\" />";
    		}
    	}
    	if (calls.size() == 0) {
    		dataToSend = dataToSend + "<item id=\"" + 0 + "\" title=\"" + "Inga inkomna samtal" + "\" desc=\"\" />";
    	}
        dataToSend = dataToSend + "</list>";
        dataToSend = dataToSend + "</data></packet>";
        sendToMenuSys(dataToSend);    	
    }
    
    private String parseByte(UByte dobyte) {
    	byte[] dummyByte = new byte[1]; 
    	dummyByte[0] = dobyte.byteValue();
    	
    	return new String(dummyByte);

    }
    
    /**
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {
            /* Ex: <arnepacket bytes="2">
             * This is the start of an ARNE packet. We receive those from
             * ARNE remote controls when buttons have been pressed on them. */
            if (element.equals("arnepacket") && attributes.containsKey("bytes")) {
                int bytes = Integer.parseInt((String)attributes.get("bytes"));
                receivedBytes = new UByte[bytes];
                //we should always get three bytes from an ARNE remote control (1 status + 2 ident)
                //if (bytes==3) {
                    parsingARNEXML = true;
                    //receivedNrOfBytes = 0;
                //} else {
                //    _errors.println("Invalid number of bytes in ARNE packet");
                //}
            }
            /* Ex: <byte id="1" value="5" />
             * This is a byte inside an ARNE packet. */
            else if (element.equals("byte") && attributes.containsKey("id") && attributes.containsKey("value")) {
                if (parsingARNEXML) {
                    short id = Short.parseShort(attributes.get("id").toString());
                    //remember byte
                    receivedBytes[id-1] = UByte.parseUByte((String)attributes.get("value"));
                } else {
                    _errors.println("Byte-attribute found outside arnepacket-attribute");
                }
            } else if (element.equals("activate")) {
        		//menusystem activates this submenu
        		//we should send a menu to display
        		activated = true;
         		sendMain();

            } else if (element.equals("deactivate")) {
        		//menusystem deactivates this submenu
        		activated = false;
         		
        	} else if (element.equals("refresh")) {
           		//menusystem wants a refreshed menu
       			sendMain();

        	} else if (element.equals("padLeft") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padleft on his remote
        		//we should traverse back one step or deactivate
        		//if we are in submenu root
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 2) {
	        		if (state.equals("DOWN")) {
		        		activated = false;
		        		
		                sendToMenuSys("<deactivates />");
	        		}
        		}
        	} else if (element.equals("padRight") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 2) {
	        		if (state.equals("DOWN")) {
	        		}
        		}

        	} else if (element.equals("padOk") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padOk on his remote
        		// 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 3) {
	        		if (state.equals("DOWN")) {
	        		}
        		}
        		
        	} else if (element.equals("padUp") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
		                sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>1</scroll></command></packet>");
	        		} else {
		                sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>0</scroll></command></packet>");
	        		}
        		}

        	} else if (element.equals("padDown") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
		                sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>-1</scroll></command></packet>");
	        		} else {
		                sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>0</scroll></command></packet>");
	        		}
        		}

        	} else if (element.equals("middleid") && attributes.containsKey("value") && attributes.containsKey("menuid") && attributes.containsKey("userlevel")) {
        		String middleId = (String)attributes.get("value");
        		String menuId = (String)attributes.get("menuid");
        		int userlevel = 0;
        		try {
            		userlevel = Integer.parseInt((String)attributes.get("userlevel"));
        		} catch (NumberFormatException ex) {
        			_errors.println("There was a faulty userlevel sent to me: " + (String)attributes.get("userlevel"));
        		}

        		if (userlevel >= 2 && menuId.equals("MainMenu")) {
        			menuMiddleId = middleId;
        		}        		
        	}

        }

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