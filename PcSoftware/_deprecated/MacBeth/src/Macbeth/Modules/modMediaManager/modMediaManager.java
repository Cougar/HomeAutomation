/*
 * File created by arune
 * at 2004-dec
 */
package Macbeth.Modules.modMediaManager;

import java.util.*;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;

/**
 *
 *
 * @author arune
 */
public class modMediaManager extends MbModule {
	private String menuMiddleId;
	
	//menusystem specific bool, if this submenu is activated
	private boolean activated;
	
    /**
     * Creates a new instance of modMediaManager.
     */
    public modMediaManager() {
        //construct MbModule
        super();
        //initial values
        menuMiddleId = "0";
        activated = false;
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    public String name() {
        return "MediaManager";
    }

    public String description() {
        return "jadajada";
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
        
        sendToMenuSys("<registersubmod displayname=\"Media\" displaydesc=\"Välj film eller musik\" visible=\"true\" />");
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
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {
        	if (element.equals("activate")) {
        		//menusystem activates this submenu
        		//we should send a menu to display
        		activated = true;
        		String dataToSend = "";
        		dataToSend = dataToSend + "<packet menuid=\"MainMenu\"><data>";
                dataToSend = dataToSend + "<middleid>" + menuMiddleId + "</middleid>";
                dataToSend = dataToSend + "<list id=\"menudata\">";
                
                for (int i = 0; i < 200; i++) {
                	dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"Test item #" + i + "\" desc=\"Description test test test, dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla\" />";
                }
                
                dataToSend = dataToSend + "</list>";
                dataToSend = dataToSend + "</data></packet>";
                
                sendToMenuSys(dataToSend);
                
        	} else if (element.equals("refresh")) {
        		//menusystem wants a refreshed menu
        		String dataToSend = "";
        		dataToSend = dataToSend + "<packet menuid=\"MainMenu\"><data>";
                dataToSend = dataToSend + "<middleid>" + menuMiddleId + "</middleid>";
                dataToSend = dataToSend + "<list id=\"menudata\">";
                for (int i = 0; i < 200; i++) {
                	dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"Test item #" + i + "\" desc=\"Description test test test, dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla dubbla\" />";
                }
                dataToSend = dataToSend + "</list>";
                dataToSend = dataToSend + "</data></packet>";
                
                sendToMenuSys(dataToSend);

        	} else if (element.equals("deactivate")) {
        		//menusystem deactivates this submenu
        		activated = false;
        		//resendMenuTimer.cancel();

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

        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }

}