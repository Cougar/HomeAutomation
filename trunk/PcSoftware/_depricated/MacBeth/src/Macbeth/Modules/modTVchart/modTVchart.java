/*
 * File created by arune
 * at 2004-dec
 */
package Macbeth.Modules.modTVchart;

import java.util.*;
import java.net.URL;
import java.io.InputStreamReader;
import java.io.BufferedReader;
//import javax.swing.Timer;
import java.util.Timer;
import java.util.TimerTask;

import java.awt.event.*;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;

/**
 *
 *
 * @author arune
 */
public class modTVchart extends MbModule {

    //list of tv-channels and their variables
    private ArrayList channels = new ArrayList();           //a map of channelObjects
    private ArrayList servers = new ArrayList();    //a list of servers
    private int currentServer = -1;     //the serverlist will cycle when connection fails, this var keeps track of current server
    //private HashMap remotebuttons;      //maps remotebutton to tv-channel
    private ArrayList remotebuttons;
    private int currentChannel = 0;     //keeps track of what channel is choosen
    private int lastChannel;            //what number has the last channel (see configfile)
    private int firstChannel;           //what number has the first channel (see configfile)
    private String buttonPplus;         //what button is used for prog+
    private String buttonPminus;        //what button is used for prog-
   
    private Timer requestDataTimer = null;
    private Timer resendMenuTimer = null;
    
    private int timerInterval;

    private String currentSubMenu;
    //main
	//SVT1 (longname)
	//SVT2 (longname)
    
    //ArrayList channelChart;
    
	private String menuMiddleId;
	
	//menusystem specific bool, if this submenu is activated
	private boolean activated;
	
	//to check if valid data exist in channelChart
	boolean tvDataSuccess;
	//a date, when data was successfully fetched last 
	int lastCheck = 0;
	
	int mainListMode = 1;
	
	//private String lastSend = "";
	
	/**
     * Creates a new instance of modTVchart.
     */
    public modTVchart() {
        //construct MbModule
        super();
        //initial values

        //remotebuttons = new HashMap(32);
        remotebuttons = new ArrayList();
        
        //we depend on the RemoteControl-module
        currentSubMenu = "main";
        menuMiddleId = "";
        activated = false;
        //activated = true;
        tvDataSuccess = false;
        timerInterval = 10;
        //activated = true;

        //addDependency("RemoteControl");
        //addDependency("MenuSystem");
        
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
    }

    public String name() {
        return "TVchart";
    }

    public String description() {
        return "Keeps track of current TV channel and outputs information on the TV charts";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
//        options.putField("host", "localhost");
//        options.putField("port", "2050");
    	//options.putField("checkinterval", "2");
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

        //get all remotebuttons from the data repository
        list = dataRepository.getList("remotebtns");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                String remotebutton = item.getField("remotebutton");
                int tvchannel = Integer.parseInt(item.getField("tvchannel"));
                //remotebuttons.put(remotebutton, new Integer(tvchannel));
                remotebuttons.add(new RemoteButton(remotebutton, tvchannel));
            }
        }

        //get all channels from the data repository
        list = dataRepository.getList("channels");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                int tvchannel = Integer.parseInt(item.getField("tvchannel"));
                int id = Integer.parseInt(item.getField("id"));
                String longname = item.getField("longname");
                String shortname = item.getField("shortname");
                channels.add(new ChannelObject(shortname, longname, tvchannel, id));
            }
        }

        if (!remotebuttons.isEmpty()) {
            //get p+ / p- buttons, reside under this if because there must be numeric buttons
            buttonPplus = options.getField("progplusbtn");
            buttonPminus = options.getField("progminusbtn");

            lastChannel = Integer.parseInt(options.getField("lastchannel"));
            firstChannel = Integer.parseInt(options.getField("firstchannel"));
            currentChannel = firstChannel;

            //lets send requests for their remote control buttons
            MbPacket p = new MbPacket();
            p.setDestination(new MbLocation("self", "RemoteControl"));
            //Iterator it = remotebuttons.keySet().iterator();
            for (int i = 0; i < remotebuttons.size(); i++) {
            	String key = ((RemoteButton)remotebuttons.get(i)).remoteButton;
            	p.appendContents("  <buttonrequest button=\"" + key + "\" module=\"" + name() + "\" />");
            }
            	
            /*while (it.hasNext()) {
                String key = (String) it.next();
                p.appendContents("  <buttonrequest button=\"" + key + "\" module=\"" + name() + "\" />");
            }*/
            p.appendContents("  <buttonrequest button=\"" + buttonPplus + "\" module=\"" + name() + "\" />");
            p.appendContents("  <buttonrequest button=\"" + buttonPminus + "\" module=\"" + name() + "\" />");

            sendPacket(p);
        }
        
        //register this module to the displaymodule
       	sendToMenuSys("<registersubmod displayname=\"TV Chart\" displaydesc=\"Whats on TV?\" visible=\"true\" />");

        //timerInterval = Integer.parseInt(options.getField("checkinterval"));
        if (timerInterval > 0) {
            //TimerListener lyssnare = new TimerListener();
        	//TimerTask lyssnare = new TimerTask();
            requestDataTimer = new Timer();
            requestDataTimer.schedule(new DoRequestTask(),1000*60*1,1000*60*timerInterval);
            //requestDataTimer = new Timer(1000*60 * timerInterval, lyssnare);
            //requestDataTimer.start();
        }

        resendMenuTimer = new Timer();
        resendMenuTimer.schedule(new DoResendTask(),1000*60*1);
        
        mainListMode = Integer.parseInt(options.getField("mainlistmode"));
    }

    public void shutdown() {
    	sendToMenuSys("<unregistersubmod />");
    	resendMenuTimer.cancel();
    	requestDataTimer.cancel();
    	super.shutdown();
    }
    
    private void sendActivationReq() {
//    	ska be om tillåtelse att få menyn aktiverad
    	String dataToSend = "<request />";
    	
    	//
    	sendToMenuSys(dataToSend);
    }
    
    private void sendToMenuSys(String dataToSend) {
       	MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "MenuSystem"));
        p.appendContents(dataToSend);
        sendPacket(p);
    }
/*
    private void sendToDisplay(String rowData, int rowNumber) {
        //anropa convertChar här
        rowData = convertText(rowData);
        MbPacket p = new MbPacket();
        //p.setDestination(new MbLocation("self","PacketReceiver"));
        p.setDestination(new MbLocation("self","SmallDisplays"));
        //ARNE packet in XML-format
        p.setContents(" <lcdpacket sender=\"" + name()
                            + "\" type=\"textrow\" prio=\"1\" ttl=\"0\" row=\""
                            + "T+" + rowNumber
                            + "\" id=\"" + 4125882 + rowNumber
                            + "\" data=\"" + convertText(rowData)
                            + "\" />");
      sendPacket(p);

    }*/

    
    private String getMain() {
    	String returnData = "";
    	int timeToNextFetch = -1;
    	fetchData();
    	
    	if (!tvDataSuccess) { 
    		//om det inte finns aktuell data skall vi skicka felmeddelande till displayen
    		
    	} else {
	    	String dataToSend = "";
	    	dataToSend = "<packet menuid=\"MainMenu\"><data>";
	    	dataToSend = dataToSend + "<middleid>" + menuMiddleId + "</middleid>";
	    	dataToSend = dataToSend + "<list id=\"menudata\">";
	        
	        if (channels.size() > 0 && menuMiddleId.equals("")) {
	        	ChannelObject o = (ChannelObject)channels.get(0);
	        	menuMiddleId = "" + o.id;
	        }
	        
	        Iterator it = channels.iterator();
	        //for each channel
	        while (it.hasNext()) {
	            ChannelObject o = (ChannelObject)it.next();
	            
	            //get tv-chart for current channel 
	            ArrayList channelChart = o.channelData; 
	        	String descr = "";
	        	String rightNow = ""; 
	        	//for each show/row on channel
	        	for (int i = 0; i < channelChart.size(); i++) {
	        		//get row
	        		String chartRow = (String)channelChart.get(i);
	        		//split it into tokens
	        		String[] chartTokens = chartRow.split("\t");
	        		int showTime = 0;
	        		
	        		//int currentHour = Integer.parseInt(MyDateTime.now("HH"));
	        		long UTCTime = System.currentTimeMillis();
	        		String date = Long.toString(UTCTime);
	        		int currentTime = Integer.parseInt(date.substring(0, date.length()-3));
	        		if (chartTokens.length >= 3) {
	        			showTime = Integer.parseInt(chartTokens[0]);
	        			//test if showtime has a completely wrong value (trust no one)
	        			if (showTime > currentTime + 24*60*60 || showTime < currentTime - 24*60*60) {
	                        break;
	                    }
	        		}
	                else {
	                    break;
	                }
	        		String prevChartRow = null;
	                String[] prevChartTokens = null;
	                int prevShowTime = -1;
	                //varför denna och exakt vad bryter den??
	                if (i == 0){// && currentHour < 5) {	//varför hade jag detta???
	                    //break;
	                } else if (i != 0) {
	                	//makes sure values for previous row is "fresh"
	                    prevChartRow = (String)channelChart.get(i-1);
	                    prevChartTokens = prevChartRow.split("\t");
	                    if (prevChartTokens.length >= 3) {
	                        //prevTimeTokens = prevChartTokens[0].split(":");
	                        prevShowTime = Integer.parseInt(prevChartTokens[0]);
	                        //test if prevShowTime has a completely wrong value
	                        if (prevShowTime > currentTime + 24*60*60 || prevShowTime < currentTime - 24*60*60) {
	                        	//_debug.println("Break because prevshowtime wrong");
	                            break;
	                        }
	                    }
	                    else {
	                    	//_debug.println("Break because nr of tokens is wrong");
	                        break;
	                    }
	                }
	                if (showTime > currentTime) {
	                    //testa prevShowTime (om i != 0)
	                    //om denna är mindre än currentTime ska den skrivas ut
	                    if (i != 0 && prevShowTime < currentTime) {
	                    	
	                    	int timeToNextShow = showTime-currentTime;
	                    	if (timeToNextFetch == -1 || timeToNextShow < timeToNextFetch) {
	                    		timeToNextFetch = timeToNextShow+2;
	                    	}
	                    	
	                        //_debug.println(prevChartTokens[0] + " " + prevChartTokens[1]);
	                        //skicka med 0 som rowCounter till funktionen som skriver till display
	                        if (mainListMode == 1) {
		                        rightNow = " - " + prevChartTokens[1] + " " + prevChartTokens[2];
	                        } else if (mainListMode == 2) {
		                    	descr = "Just nu: " + prevChartTokens[1] + " " + prevChartTokens[2] + "&#10;"; 
	                        }
	                        if (prevChartTokens.length == 4) {
	                        	descr = descr + prevChartTokens[3]+ "&#10;&#10;";
	                        } else if (mainListMode == 2) {
	                        	descr = descr + "&#10;";
	                        }
	                        descr = descr + "H&#228;rn&#228;st: " + chartTokens[1] + " " + chartTokens[2];
	                        if (chartTokens.length == 4) {
	                        	descr = descr + "&#10;" + chartTokens[3];
	                        }
	                        break;
	                    } else if (i == 0) {
	                    	descr = "H&#228;rn&#228;st: " + chartTokens[1] + " " + chartTokens[2];
	                    }
	                }                    
	        	}
	        	dataToSend = dataToSend + "<item id=\"" + o.id + "\" title=\"" + o.longname + rightNow + "\" desc=\"" + descr + "\" />";
	        }
	    	
	        dataToSend = dataToSend + "</list>";
	        dataToSend = dataToSend + "</data></packet>";
	        //sendToMenuSys(dataToSend);
	        returnData = dataToSend;
    	}
    	if (timeToNextFetch == -1) {
    		timeToNextFetch = 1800;
    	}
    	if (timeToNextFetch < 30) {
    		timeToNextFetch = 30;
    	}
    	if (timeToNextFetch > 3600) {
    		timeToNextFetch = 3600;
    	}
    	//_debug.println("Main: scheduling timer, timeToNextFetch: " + timeToNextFetch);
    	resendMenuTimer.cancel();
    	resendMenuTimer = new Timer();
    	resendMenuTimer.schedule(new DoResendTask(),timeToNextFetch*1000);
    	
    	return returnData;
    }
 
    private String getSub(String id) {
    	String returnData = "";
    	int timeToNextFetch = -1;
    	fetchData();
    	if (!tvDataSuccess) { 
    		//om det inte finns aktuell data skall vi skicka felmeddelande till displayen
    		//returnData = ...
    	} else {
    		
    		//om det finns aktuell data
        	String dataToSend = "";
        	dataToSend = "<packet menuid=\"MainMenu\"><data>";
            	
                //ChannelObject o = (ChannelObject)it.next();
        	ArrayList channelChart = new ArrayList();
            Iterator it = channels.iterator();
            while (it.hasNext()) {
                ChannelObject o = (ChannelObject)it.next();
                if (id.equals("" + o.id)) {
                	channelChart = o.channelData;
                	break;
                }
            }
            
        	if (channelChart.size() > 0) {
    	    	//ArrayList channelChart = getWebPage(id);
    	    	String itemlist = "";
    	    	int current = 0;
    	    	
    	    	for (int i = 0; i < channelChart.size(); i++) {
    	    		String chartRow = (String)channelChart.get(i);
    	    		String[] chartTokens = chartRow.split("\t");
    	    		int showTime = 0;
    	    		
    	    		int currentHour = Integer.parseInt(MyDateTime.now("HH"));
    	    		long UTCTime = System.currentTimeMillis();
    	    		String date = Long.toString(UTCTime);
    	    		int currentTime = Integer.parseInt(date.substring(0, date.length()-3));
    	    		if (chartTokens.length >= 3) {
    	    			showTime = Integer.parseInt(chartTokens[0]);
    	    			//test if showtime has a completely wrong value (trust no one)
    	    			if (showTime > currentTime + 24*60*60 || showTime < currentTime - 24*60*60) {
    	                    break;
    	                }
    	    		}
    	            else {
    	                break;
    	            }
    	    		String prevChartRow = null;
    	            String[] prevChartTokens = null;
    	            int prevShowTime = -1;
    	            if (i == 0 && currentHour < 5) {
    	                //break;
    	            } else if (i != 0) {
    	                prevChartRow = (String)channelChart.get(i-1);
    	                prevChartTokens = prevChartRow.split("\t");
    	                if (prevChartTokens.length >= 3) {
    	                    //prevTimeTokens = prevChartTokens[0].split(":");
    	                    prevShowTime = Integer.parseInt(prevChartTokens[0]);
    	                    //test if prevShowTime has a completely wrong value
    	                    if (prevShowTime > currentTime + 24*60*60 || prevShowTime < currentTime - 24*60*60) {
    	                        break;
    	                    }
    	                }
    	                else {
    	                    break;
    	                }
    	            }
    	            
    	            String descr = "";
    	            if (chartTokens.length == 4) {
    	            	descr = chartTokens[3];
    	            }
    	            
    	            itemlist = itemlist + "<item id=\"" + i + "\" title=\"" + chartTokens[1] + " " + chartTokens[2] + "\" desc=\"" + descr + "\" />";
    	            if (showTime > currentTime) {
    	                //om denna är mindre än currentTime ska den skrivas ut
    	                if (i != 0 && prevShowTime < currentTime) {
    	                	current = i-1;
    	                }

                    	int timeToNextShow = showTime-currentTime;
                    	if (timeToNextFetch == -1 || timeToNextShow < timeToNextFetch) {
                    		timeToNextFetch = timeToNextShow+2;
                    	}
    	            }                    
    	        }
    	    	dataToSend = dataToSend + "<middleid>" + current + "</middleid>";
    	    	dataToSend = dataToSend + "<list id=\"menudata\">";
    	    	dataToSend = dataToSend + itemlist;
    	    	
    	    	dataToSend = dataToSend + "</list>";
    	    	dataToSend = dataToSend + "</data></packet>";
    	        
    	    	returnData = dataToSend;
    	    	//sendToMenuSys(dataToSend);
        	}	
    	}
    	
    	if (timeToNextFetch == -1) {
    		timeToNextFetch = 1800;
    	}
    	if (timeToNextFetch < 30) {
    		timeToNextFetch = 30;
    	}
    	if (timeToNextFetch > 3600) {
    		timeToNextFetch = 3600;
    	}
    	//_debug.println("Sub: scheduling timer, timeToNextFetch: " + timeToNextFetch);
    	resendMenuTimer.cancel();
    	resendMenuTimer = new Timer();
    	resendMenuTimer.schedule(new DoResendTask(),timeToNextFetch*1000);

    	return returnData;
    }

    private String getServerURL() {
        if (currentServer == -1) {
            if (servers.size() == 0) {
                return ""; }
            else {
                currentServer = 0; }
        } else {
        	//currentServer räknas upp i catch:en nedan
        }
        if (currentServer >= servers.size()) {
            currentServer = 0; }
        return (String)servers.get(currentServer);
    }
    
    private boolean fetchData() {
		int currentHour = Integer.parseInt(MyDateTime.now("HH"));
		long UTCTime = System.currentTimeMillis();
		String date = Long.toString(UTCTime);
		int currentTime = Integer.parseInt(date.substring(0, date.length()-3));
		boolean didFetch = false;
		//TODO: Vad gör denna för nytta egentilgen?
		//if (currentTime > lastCheck+12*60*60) {
		//	tvDataSuccess = false;
		//}
		
		if ((currentHour == 6 && currentTime > lastCheck+60*60) || !tvDataSuccess) {
			//incase the server does not respond this var should be false
			tvDataSuccess = false;
			
	        Iterator it = channels.iterator();
	        boolean dataError = false;
	        while (it.hasNext()) {
	            ChannelObject o = (ChannelObject)it.next();
	            o.channelData = getWebPage("" + o.id);
	            if (o.channelData == null) {
	            	dataError = true;
	            	o.channelData = new ArrayList();
	            }
	        }
			
	    	if (!dataError) {
				lastCheck = currentTime;
				tvDataSuccess = true;
				didFetch = true;
	    	} 
	    }
		
		return didFetch;
		
    }
    
    private ArrayList getWebPage(String id) {
        ArrayList rowStrings = null; //new ArrayList();

        String serverURL = getServerURL();

        if (!serverURL.equals("")) {
            serverURL = serverURL + "/index.php?id=" + id;
            try {
                BufferedReader page = new BufferedReader(new InputStreamReader(new URL(serverURL).openStream()));
                
                rowStrings = new ArrayList();
                String curLine = page.readLine();
                while ( curLine != null ) {
                    rowStrings.add(curLine);
                    // get the next line
                    curLine = page.readLine();
                }
            }
            catch ( Exception ex ){
                _errors.println("Comunication problem with tv-chart-server, exception: " + ex.getMessage());
                currentServer++;
            }
        }

        return rowStrings;
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
        		if (currentSubMenu.equals("main")) {
					String dataToSend = getMain();
					sendToMenuSys(dataToSend);
        		} else {
					String dataToSend = getSub(menuMiddleId);
					sendToMenuSys(dataToSend);
        		}
        		
        	} else if (element.equals("refresh")) {
           		//menusystem wants a refreshed menu
        		 if (currentSubMenu.equals("main")) {
 					String dataToSend = getMain();
					sendToMenuSys(dataToSend);
        		} else {
					String dataToSend = getSub(menuMiddleId);
					sendToMenuSys(dataToSend);
        		}
        		 
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
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
			    		if (currentSubMenu.equals("main")) {
			    			activated = false;
			    			//resendMenuTimer.cancel();
			    			sendToMenuSys("<deactivates />");
			    		} else {
			    			currentSubMenu = "main";
							String dataToSend = getMain();
							sendToMenuSys(dataToSend);
			    		}
	        		}
        		}
        	} else if (element.equals("padRight") && attributes.containsKey("state") && attributes.containsKey("level")) {
        		String state = (String)attributes.get("state");
        		int level = Integer.parseInt((String)attributes.get("level"));
        		//user pressed padright on his remote
        		//we should traverse forward one step 
        		if (level >= 1) {
	        		if (state.equals("DOWN")) {
		        		if (currentSubMenu.equals("main")) {
							String dataToSend = getSub(menuMiddleId);
							sendToMenuSys(dataToSend);

		        			currentSubMenu = menuMiddleId;
		        		}
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
		        		if (currentSubMenu.equals("main")) {
		        			int curChan = -1;
		                	for (int i = 0; i < channels.size(); i++) {
		                		if (menuMiddleId.equals("" + ((ChannelObject)channels.get(i)).id)) {
		                			curChan = ((ChannelObject)channels.get(i)).tvchannel;
		                		}
		                	}
		        			if (curChan != -1) {
		                        for (int i = 0; i < remotebuttons.size(); i++) {
		                        	String key = ((RemoteButton)remotebuttons.get(i)).remoteButton;
		                        	int remoteChan = ((RemoteButton)remotebuttons.get(i)).channel;
		                        	
		                        	if (curChan == remoteChan) {
		                        		currentChannel = curChan;
		            	              	MbPacket p = new MbPacket();
		            	                p.setDestination(new MbLocation("self", "GenRoomControl"));
		                                p.appendContents("<remotecontrol button=\"" + key + "\" state=\"DOWN\" />");
		                                sendPacket(p);
		                                try {
		                                	Thread.sleep(120);                                	
		                                } catch (InterruptedException ex) {
		                                }
		            	              	p = new MbPacket();
		            	                p.setDestination(new MbLocation("self", "GenRoomControl"));
		                                p.appendContents("<remotecontrol button=\"" + key + "\" state=\"UP\" />");
		                                sendPacket(p);
		                                
		                                /*
		                                 * 2006-01-30 20:34 Jimmy: det bör inte vara några problem. 
		                                 * modulerna körs ju i egna trådar (om jag inte minns helt fel), 
		                                 * så det går väl i princip göra Thread.Sleep eller vad det nu 
		                                 * kan tänkas heta
		                                 */
		                        	}
		                        }
		        			}
		        		} else if (state.equals("UP")) {
		        			//knappen släpptes efter 120us, se ovan
		        		}
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

        		if (currentSubMenu.equals("main") && userlevel >= 2 && menuId.equals("MainMenu")) {
        			menuMiddleId = middleId;
        		}        		
        		
                /* Ex: <remotecontrol button="NUMERIC4" state="DOWN" />
                 * This is a notification from the remote control module
                 * about one of our requested remote control buttons have
                 * been pressed. */        	
        	} else if (element.equals("remotecontrol") && attributes.containsKey("button") && attributes.containsKey("state")) {
                String state = (String)attributes.get("state");
                String button = (String)attributes.get("button");
                if (state.equals("DOWN")) {
                	boolean update = false;
                    if (buttonPplus.equals(button)) {
                        currentChannel++;
                        update = true;
                        if (currentChannel > lastChannel) {
                            currentChannel = firstChannel;
                        }
                        //updateDisplay();
                    }
                    // P+ och P- ska egentligen hoppa till nästa/föregående kanal 
                    // dessa behöver inte ligga efter varandra
                    else if (buttonPminus.equals(button)) {
                        currentChannel--;
                        update = true;
                        if (currentChannel < firstChannel) {
                            currentChannel = lastChannel;
                        }
                        //updateDisplay();
                    }
                    else {
                        //find channel that is associated with this remote button
                        for (int i = 0; i < remotebuttons.size(); i++) {
                        	String key = ((RemoteButton)remotebuttons.get(i)).remoteButton;
                        	int channel = ((RemoteButton)remotebuttons.get(i)).channel;
                        	if (key.equals(button)) {
                                if (currentChannel != channel) {      //only update on changed channel
                                	currentChannel = channel;
                                	update = true;
                                } // update not only on changed channel, one might want to update the info on the display
                        	}
                        }
                    }
                    if (update) {
                    	for (int i = 0; i < channels.size(); i++) {
                    		if (((ChannelObject)channels.get(i)).tvchannel == currentChannel) {
                    			menuMiddleId = "" + ((ChannelObject)channels.get(i)).id;
                    		}
                    	}
                    	
                    	if (currentSubMenu.equals("main") && activated) {
        					String dataToSend = getMain();
        					sendToMenuSys(dataToSend);
        					//TODO: egentligen ska vi bara skicka gotorow=menuMiddleId
                    		//men flinsh stödjer inte det än
                    	} else {
                    		//TODO: annars ska man väl skicka kommando till menusystem att vi
                    		//vill outa saker på display
                    		sendActivationReq();	//ska be om tillåtelse att få menyn aktiverad
                    	}
                    }
                }
                else if (state.equals("UP")) {
                    //send btn-release-message to RMP (5000)
//                    sendString("5000");
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
     * Timer handles datafetch from tv-server 
     */
	/*private class TimerListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			if (fetchData()) {
				//if tv-chart was downloaded then
				//renew menus

				if (activated) {
					if (currentSubMenu.equals("main")) {
						//sendMain();
						String dataToSend = getMain();
						//if (!lastSend.equals(dataToSend)) {
						sendToMenuSys(dataToSend);
						//}
						//lastSend = dataToSend;
					} else {
						//sendSub(menuMiddleId);
						String dataToSend = getSub(menuMiddleId);
						//if (!lastSend.equals(dataToSend)) {
						sendToMenuSys(dataToSend);
						//} 
						//lastSend = dataToSend;
					}
				}
			}
		}
	}*/
	
    /**
     * Timer handles datafetch from tv-server 
     */
	private class DoRequestTask extends TimerTask {
        public void run() {
        	//_debug.println("DoRequestTask now");
			if (fetchData()) {
	        	//_debug.println("Got new data");
				//if tv-chart was downloaded then
				//renew menus

				if (activated) {
					if (currentSubMenu.equals("main")) {
						//sendMain();
						String dataToSend = getMain();
						sendToMenuSys(dataToSend);
					} else {
						//sendSub(menuMiddleId);
						String dataToSend = getSub(menuMiddleId);
						sendToMenuSys(dataToSend);
					}
				}
			}

        }
	}
	
	private class DoResendTask extends TimerTask {
        public void run() {
        	//_debug.println("DoResendTask now");
        	//resendMenuTimer.cancel();
			//renew menus
        	
			if (activated) {		//we may have left this menu
				if (currentSubMenu.equals("main")) {
					//sendMain();
					String dataToSend = getMain();
					sendToMenuSys(dataToSend);
				} else {
					//sendSub(menuMiddleId);
					String dataToSend = getSub(menuMiddleId);
					sendToMenuSys(dataToSend);
				}
			}
        }
	}
	
    private class RemoteButton {
    	
    	public String remoteButton;
        public int channel;

        /**
         * Creates a new instance of RemoteButton.
         */
        public RemoteButton(String remoteButton, int channel) {
        	this.remoteButton = remoteButton;
        	this.channel = channel;
        }

    }
    

}