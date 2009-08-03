/*
 * File created by arune
 * at 2004-dec
 */
package Macbeth.Modules.modVagnen;

import java.util.*;
import java.net.URL;
import java.net.URLConnection;
//import java.net.HttpURLConnection;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import javax.swing.Timer;

import java.awt.event.*;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;

/**
 *
 *
 * @author arune
 */
public class modVagnen extends MbModule {

    //list of stops and their variables
    private ArrayList stops = new ArrayList();           //a map of StopObjects
   
    private Timer requestDataTimer = null;
    private int timerInterval = 0;

	private String menuMiddleId;
	
	//menusystem specific bool, if this submenu is activated
	private boolean activated;
	
	/**
     * Creates a new instance of modTVchart.
     */
    public modVagnen() {
        //construct MbModule
        super();
        //initial values

        //remotebuttons = new HashMap(32);
        stops = new ArrayList();
        
        menuMiddleId = "";
        activated = false;
        //TODO: remove next line, for debug
        //activated = true;

        addDependency("MenuSystem");
        
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    public String name() {
        return "Vagnen";
    }

    public String description() {
        return "Checks tram and bus stops";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
//        options.putField("host", "localhost");
//        options.putField("port", "2050");
    	options.putField("checkinterval", "1");
    }

    public void startup() throws MbStartupException {
        //startup MbModule
        super.startup();

        //get all remotebuttons from the data repository
        DataRepository.DataList list = dataRepository.getList("stops");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                String stop = item.getField("stop");
                String displayName = item.getField("displayname");
                String includeLines = item.getField("includelines");
                String excludeLines = item.getField("excludelines");
                String includeDests = item.getField("includedest");
                String excludeDests = item.getField("excludedest");
                //parsa och verifiera includelines, (excludelines), excludedest, (includedest)
                String[] incLines = null;
                String[] excLines = null;
                String[] incDests = null;
                String[] excDests = null;
                if (includeLines != null) {
                	incLines = includeLines.split(",");
                }
                if (excludeLines != null) {
                	excLines = excludeLines.split(",");
                }
                if (includeDests != null) {
                	incDests = includeDests.split(",");
                }
                if (excludeDests != null) {
                	excDests = excludeDests.split(",");
                }
                
                stops.add(new StopObject(stop, displayName, incLines, excLines, incDests, excDests));
            }
        }

        sendToMenuSys("<registersubmod displayname=\"Vagnen\" displaydesc=\"When does the bus arrive?\" visible=\"true\" />");

        timerInterval = Integer.parseInt(options.getField("checkinterval"));
        if (timerInterval > 0) {
            TimerListener lyssnare = new TimerListener();
            requestDataTimer = new Timer(1000*60 * timerInterval, lyssnare);
            requestDataTimer.start();
        }
        
    }

    public void shutdown() {
    	sendToMenuSys("<unregistersubmod />");
    	super.shutdown();
    }
    
    private void sendToMenuSys(String dataToSend) {
       	MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "MenuSystem"));
        p.appendContents(dataToSend);
        sendPacket(p);
    }
    
    public ArrayList appendList(ArrayList appendThis, ArrayList toThis) {
    	ArrayList copyToThis = toThis;
    	
		for (int i = 0; i< appendThis.size(); i++) {
			copyToThis.add(appendThis.get(i));
		}
		
    	return copyToThis;
    }
    
    public ArrayList sortList(ArrayList toSort) {
    	ArrayList sorted = new ArrayList();
    	ArrayList copyToSort = toSort;
    	StopObject.Buss trafikData;
    	
    	int i = 0;
    	while (i < copyToSort.size()) {
    		trafikData = (StopObject.Buss)copyToSort.get(i);
    		if (trafikData.time.equals("Nu")) {
    			sorted.add(copyToSort.get(i));
    			copyToSort.remove(i);
    			
    		} else {
    			i++;
    		}
    	}
    	
    	i = 0;
    	while (copyToSort.size() > 0) {
        	int smallest = 100;
        	int smallestInd = 0;
	    	for (int j = 0; j < copyToSort.size(); j++) {
	    		trafikData = (StopObject.Buss)copyToSort.get(j);
	    		int currentValue = 100;
	    		int spaceInd = trafikData.time.indexOf(" ");
				//_debug.println("|" + trafikData.time+"|");
	    		if (spaceInd == -1) {
	        		try {
	        			currentValue = Integer.parseInt(trafikData.time);
	        		} catch (NumberFormatException ex) {
	        		}
	    		} else {
	    			if (trafikData.time.indexOf(" ", spaceInd+1) > -1) {
	    				spaceInd = trafikData.time.indexOf(" ", spaceInd+1);
	    			}
	        		try {
	        			//_debug.println("|" + trafikData.time.substring(spaceInd, trafikData.time.length())+"|");
	        			currentValue = Integer.parseInt(trafikData.time.substring(spaceInd+1, trafikData.time.length()));
	        		} catch (NumberFormatException ex) {
	        		}
	    		}
	    		
	    		if (currentValue < smallest) {
	    			smallestInd = j;
	    			smallest = currentValue;
	    		}
	    	}
			//trafikData = (StopObject.Buss)copyToSort.get(smallestInd);
			//_debug.println("|" + trafikData.time+"|");
			sorted.add(copyToSort.get(smallestInd));
			copyToSort.remove(smallestInd);
    	}
    	return sorted;
    }
    
    private void sendMain() {
    	ArrayList stopData = new ArrayList();
    	
    	for (int i = 0; i < stops.size(); i++) {
    		StopObject stop = (StopObject)stops.get(i);
    		
    		ArrayList newStopData = stop.getStopData();
    		
    		stopData = appendList(newStopData, stopData);
    		
    	}
    	stopData = sortList(stopData);
    	//TODO: sortering!
    	//specialfall: .equals("Nu") om contains("ca")
    	
    	//TODO: hållplats är inte med, det har jag glömt, det finns ju med i config
    	
    	String dataToSend = "<packet menuid=\"MainMenu\"><data>"; 
    	dataToSend = dataToSend + "<middleid>" + menuMiddleId + "</middleid>";
    	dataToSend = dataToSend + "<list id=\"menudata\">";
    	for (int i = 0; i < stopData.size(); i++) {
    		StopObject.Buss trafikData = (StopObject.Buss)stopData.get(i);
    		String line = "Linje " + trafikData.number;
    		String time = "";
    		if (trafikData.time.equals("Nu")) {
    			time = " avg&#229;r NU";
    		} else if (trafikData.time.equals("1")) {
    			time = " avg&#229;r om 1 minut";
    		} else if (trafikData.time.equals("ca 1")) {
    			time = " avg&#229;r om ca 1 minut";
    		} else {
    			time = " avg&#229;r om " + trafikData.time + " minuter";
    		}
    		String stop = " (" + trafikData.stop + ")";
    		//String stop = " fr&#229;n " + trafikData.stop;
    		String dest = " mot " + trafikData.dest;
    		dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + line + stop + time + dest + "\" desc=\"\" />";
    		
    		//_debug.println(test.stop + " " + test.number + " " + test.time);
    		//System.out.println((String)traficData.get(i));
    		
    	}
        dataToSend = dataToSend + "</list>";
        dataToSend = dataToSend + "</data></packet>";
        sendToMenuSys(dataToSend);
		//_debug.println(dataToSend);

    	//sortera och skapa en lista att skicka flinsh från stopData
    	

    	
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

         		sendMain();
        		
        	} else if (element.equals("refresh")) {
           		//menusystem wants a refreshed menu
       			sendMain();
        		 
        	} else if (element.equals("padLeft") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padleft on his remote
        		//we should traverse back one step or deactivate
        		//if we are in submenu root
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
		    			activated = false;
		    			sendToMenuSys("<deactivates />");
	        		}
        		}
        	} else if (element.equals("padRight") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
	        			//do nothing
	        		}
        		}
        		
        	} else if (element.equals("padUp") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		
        		if (level >= 2) {
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
        		
        		if (level >= 2) {
	        		if (state.equals("DOWN")) {
		                sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>-1</scroll></command></packet>");
	        		} else {
	        			sendToMenuSys("<packet menuid=\"MainMenu\"><command><scroll>0</scroll></command></packet>");
	        		}
        		}
        		
        	} else if (element.equals("padOk") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		//user pressed padOk on his remote 
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        			//state=\"UP\"
        		if (level >= 3) {
	        		if (state.equals("DOWN")) {

	        		} else if (state.equals("UP")) {

	        		}
        		}
        	} else if (element.equals("middleid") && attributes.containsKey("value") && attributes.containsKey("menuid") && attributes.containsKey("userlevel")) {
        		//menuMiddleId = (String)attributes.get("value");
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
    
  	//En lyssnare för timern
    /**
     * Timer handles datafetch from tv-server and also
     * sends data to display periodically
     */
	private class TimerListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			//fetchData();

			//om aktiv: skicka ny data till flinsh (just nu / härnäst kan ha ändrats)
			if (activated) {
				sendMain();
			}
		}
	}
	

}