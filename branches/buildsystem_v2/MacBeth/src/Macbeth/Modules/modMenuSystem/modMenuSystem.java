/*
 * File created by arune
 * at 2005-jan-09 16:54:44
 */
package Macbeth.Modules.modMenuSystem;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;
import Macbeth.XML.XMLParser;
import Macbeth.Utilities.*;
import Macbeth.Modules.modGenRoomControl.GenRoomObject;
import Macbeth.Net.*;

import java.util.*;

import javax.swing.*;
import javax.swing.Timer;

import org.xml.sax.SAXException;

import java.awt.event.*;
import java.io.IOException;

/**
 * This is the core of the menusystem, this module should register
 * the Pad-, Ok- and Cancel/back/menu-buttons from remotecontrol.
 * Other submenumodules can register to this module.
 * Submenumodules get (if they need) a row in the root menu. 
 * The user navigates the rootmenu with the pad-buttons, when selecting one
 * row with pad-right or ok then this choosen submenu should send its root-menu 
 * to menusystem and it sends it to be displayed, menusystem should also 
 * send the pad-buttons to this submenu.
 * 
 * for now...
 * 
 * @author arune
 */
public class modMenuSystem extends MbModule implements ServerDataHandler{

private XMLDataHandler flinshDataHandler;
//private XMLParser packetXMLParser;

private TcpServer flinshServer;
//private Timer requestDataTimer = null;
//private int timerInterval = 0;
private final char EOF = (char) 0x00;
//A table containing all registered modules (submenus and vlc)
private ArrayList subMenus = new ArrayList(); 
//what module that should receive all remotebutton changes
private String curButtonReceiver;
//what module that is visible on all flashclients
//often same as curButtonReceiver, but not when curButtonReceiver is vlc-module 
private String curSubMenu;

private String currentAddress = "";
private int currentLevel = 0;

private ArrayList userLogin = new ArrayList(); 
private ArrayList clientsConnected = new ArrayList(); 

//holds track of what item is marked in the root_menu (menu system)
private String rootMenuMiddleId;

private boolean guestCanView;

String padUp;
String padDown;
String padLeft;
String padRight;
String padOk;


    /**
     * Creates a new instance of .
     */
    public modMenuSystem() {
        //construct MbModule
        super();
        //initial values

        addDependency("RemoteControl");
        
        //from start the root-menu should be visible
        curButtonReceiver = name(); 
        curSubMenu = name(); 
        rootMenuMiddleId = "0";
        
        //subMenuLastMenu = "";
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
        //packetXMLParser = new XMLParser(new PacketDataHandler());
        flinshDataHandler = new FlinshDataHandler();
   }

    public String name() {
        return "MenuSystem";
    }

    public String description() {
        return "bla";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    public void initDataFields() {
        //set default values on all REQUIRED option fields before starting up subsystems
        //options.putField("mode", "simple");
        //options.putField("checkinterval", "5");
        //options.putField("displaytext", "New mail");
//        options.putField("port", "2050");
    }

    public void handleServerData(String data, String address) {
     	
   		//nulltecken finns, tar bort
   		data = data.replaceAll("" + EOF, "");

    	if (data.length() > 0) {
        	if (data.equals("<ping />")) {
            	//gör inget om det är ping
        	} else {
        		//_debug.println("got data: " + data);
        		
               	//kan vara en säkerhetsrisk att skicka med det såhär, klienten kan ju
               	//konstruera en sån här sträng och skicka till oss
        		//data = "<clientaddress value=\"" + address + "\">" + data;
        		//fet säkerhetsrisk ja
        		//private String currentAddress = "";
        		currentLevel = 0;
        		//check user level
       			currentAddress = address;
               	for (int i=0; i<clientsConnected.size(); i++) {
               		String adr = ((ClientObj)clientsConnected.get(i)).address;
               		if (adr.equals(address)) {
               			currentLevel = ((ClientObj)clientsConnected.get(i)).level;
               			break;
               		}
            	}
        		
        		
        		//<flinshpacket id="MainMenu" middleitem="1" />
                if (flinshDataHandler!=null) {
                    //create XML parser and try to parse configuration file
                    XMLParser xmlParser = new XMLParser(flinshDataHandler);
                    try {
                        xmlParser.parseString(data);
	                } catch (SAXException e) {
	                    _errors.println("The data send from flinsh contains errors:");
	                    _errors.println(e);
	                    e.printStackTrace();
	                } catch (IOException e) {
	                    //I/O-exception. that's worse, so lets warn
	                    _errors.println("I/O-error while trying to read data from flinsh:");
	                    _errors.println(e);
	                    e.printStackTrace();
	                }
                }
                //allt parsas nu av xml-parsern
        	}
    	}
    	
    	
    }
    
    /**
     * When a client dropps out, delete it from the client list
     * 
     */
    public void clientDropped(String address) {
       	for (int i=0; i<clientsConnected.size(); i++) {
       		String adr = ((ClientObj)clientsConnected.get(i)).address;
       		if (adr.equals(address)) {
       			clientsConnected.remove(i);
       			_debug.println("client dropped: " + address);
       			break;

       		}
    	}
    }
    
    /**
     * When flins client connects it should get the latest menu. If its the root menu  
     * sendRootMenu get called. If its a submenu a refresh is sent to that submenu
     * and the submenu should send out the latest menu to menusystem.
     */
    public void setRemoteAddress(String address) {
    	//_debug.println("got connection: " + address);
    	
    	/*
    	 * check if the newly connected client has a level specified in config
    	 */
    	int level = 0;
    	_debug.println("client connected: " + address);
    	//rota igenom listan över de möjliga logins som finns
    	for (int i=0; i<userLogin.size(); i++) {
    		if (((UserLoginObj)userLogin.get(i)).user.equals("ip")) {
    			String userLoginIp = ((UserLoginObj)userLogin.get(i)).password;
    			String[] ipPort = address.split(":");
    			       
    			if (ipPort[0].equals("/" + userLoginIp)) {
    				if (((UserLoginObj)userLogin.get(i)).level > level) {
    					level = ((UserLoginObj)userLogin.get(i)).level;
    					_debug.println("client got level by ip: " + level);
    				}
    			}
    		}
    	}
    	//lägg till klienten i listan över klienter:
    	clientsConnected.add(new ClientObj(address, level));
    	
    	if (curSubMenu.equals(name())) {
    		sendRootMenu(address);
    	} else {
    		sendMacbeth(curSubMenu, "<refresh />");
    	}
    }
    
    
    /**
     * 
     * @param address Specifies flinsh-client to send data to, send to all clients if null
     */
    private void sendRootMenu(String address) {
		//nu ska vi visa rootmenyn (lista alla submodulers displayname)
    	String dataToSend = "<?xml version=\"1.0\"?><packet menuid=\"MainMenu\"><data>";
    	
    	dataToSend = dataToSend + "<middleid>" + rootMenuMiddleId + "</middleid><list id=\"menudata\">";
		for (int i = 0; i < subMenus.size(); i++) {
			//generera e
			dataToSend = dataToSend + "<item id=\"" + i + "\" title=\"" + ((SubMenu)subMenus.get(i)).getDisplayName() + "\" desc=\"" + ((SubMenu)subMenus.get(i)).getDisplayDesc() + "\" />";
			/*
			dataToSend = dataToSend + "<item>";
			dataToSend = dataToSend + "<id>" + i + "</id>";
			dataToSend = dataToSend + "<title>" + ((SubMenu)subMenus.get(i)).getDisplayName() + "</title>";
			dataToSend = dataToSend + "<description>" + ((SubMenu)subMenus.get(i)).getDisplayDesc() + "</description>";
			dataToSend = dataToSend + "</item>";
			*/
		}
		dataToSend = dataToSend + "</list></data></packet>" + EOF;
		
		if (address != null) {
			//skicka till en klient
			if (guestCanView) {
				flinshServer.sendToClient(dataToSend, address);
			} else {
				//skicka till alla inloggade med adressen address (level > 0)
		       	for (int i=0; i<clientsConnected.size(); i++) {
		       		String adr = ((ClientObj)clientsConnected.get(i)).address;
		       		int level = ((ClientObj)clientsConnected.get(i)).level;
		       		if (adr.equals(address) && level > 0) {
		       			flinshServer.sendToClient(dataToSend, adr);
		       		}
		    	}
			}
				
		} else {
			//skicka till alla
			if (guestCanView) {
				flinshServer.sendToAll(dataToSend);
			} else {
				//skicka till alla inloggade (level > 0)
		       	for (int i=0; i<clientsConnected.size(); i++) {
		       		String adr = ((ClientObj)clientsConnected.get(i)).address;
		       		int level = ((ClientObj)clientsConnected.get(i)).level;
		       		if (level > 0) {
		       			flinshServer.sendToClient(dataToSend, adr);
		       		}
		    	}
			}
    	}
    }
    
    private void sendData(String address, String data) {
    	String dataToSend = "<?xml version=\"1.0\"?>";
    	dataToSend = dataToSend + data;    	
    	dataToSend = dataToSend + EOF;
		if (address != null) {
			//skicka till en klient
			flinshServer.sendToClient(dataToSend, address);
		} else {
			//skicka till alla
			if (guestCanView) {
				flinshServer.sendToAll(dataToSend);
			} else {
				//skicka till alla inloggade (level > 0)
		       	for (int i=0; i<clientsConnected.size(); i++) {
		       		String adr = ((ClientObj)clientsConnected.get(i)).address;
		       		int level = ((ClientObj)clientsConnected.get(i)).level;
		       		if (level > 0) {
		       			flinshServer.sendToClient(dataToSend, adr);
		       		}
		    	}
			}
		}    	
    }
    
    private void sendCommand(String address, String command) {
    	String dataToSend = "<?xml version=\"1.0\"?><packet menuid=\"MainMenu\"><command>";
    	dataToSend = dataToSend + command;
		dataToSend = dataToSend + "</command></packet>" + EOF;
        //_debug.println("skickar " + dataToSend);
		if (address != null) {
			//skicka till en klient
			flinshServer.sendToClient(dataToSend, address);
		} else {
			//skicka till alla
			if (guestCanView) {
				flinshServer.sendToAll(dataToSend);
			} else {
				//skicka till alla inloggade (level > 0)
		       	for (int i=0; i<clientsConnected.size(); i++) {
		       		String adr = ((ClientObj)clientsConnected.get(i)).address;
		       		int level = ((ClientObj)clientsConnected.get(i)).level;
		       		if (level > 0) {
		       			flinshServer.sendToClient(dataToSend, adr);
		       		}
		    	}
			}
    	}
    }
    
    public void startup() throws MbStartupException {
        //startup MbModule
        super.startup();

        //get all login users/ip objects from the data repository
        DataRepository.DataList list = dataRepository.getList("login");
        if (list!=null) {
            Iterator it = list.items();
            while (it.hasNext()) {
                DataRepository.DataListItem item = (DataRepository.DataListItem)it.next();
                String name = (String)item.getField("user");
                String passIp = (String)item.getField("pass");
                int level = 0;
                try {
                	level = Integer.parseInt((String)item.getField("level"));
                } catch (NumberFormatException ex) {
                	_errors.println("Config error: level not a number: " + (String)item.getField("level"));
                }
                
                userLogin.add(new UserLoginObj(name, passIp, level));
            }
        }
        
        guestCanView = ((String)options.getField("guestcanview")).equals("true");
        
        int flinshServerPort = Integer.parseInt(options.getField("port"));
        //konfigurerbar port via vanlig config
        if (flinshServerPort >= 1024) {
        	flinshServer = new TcpServer(flinshServerPort, this);
        	flinshServer.startServer();
        	flinshServer.setTcpNoDelay(true);
        } else {
        	_errors.println("Config error: Port number must be greater than or equal to 1024");
        }
        
        padUp = options.getField("padup");
        padDown = options.getField("paddown");
        padLeft = options.getField("padleft");
        padRight = options.getField("padright");
        padOk = options.getField("padok");
        //lets send requests for their remote control buttons
        MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", "RemoteControl"));
        p.appendContents("  <buttonrequest button=\"" + padUp + "\" module=\"" + name() + "\" />");
        p.appendContents("  <buttonrequest button=\"" + padDown + "\" module=\"" + name() + "\" />");
        p.appendContents("  <buttonrequest button=\"" + padLeft + "\" module=\"" + name() + "\" />");
        p.appendContents("  <buttonrequest button=\"" + padRight + "\" module=\"" + name() + "\" />");
        p.appendContents("  <buttonrequest button=\"" + padOk + "\" module=\"" + name() + "\" />");

        sendPacket(p);

    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //stop timer if it's running
        /*if (requestDataTimer!=null) {
            if (requestDataTimer.isRunning()) {
                requestDataTimer.stop();
            }
        }*/
        //shut down MbModule
    	flinshServer.stopServer();
    	super.shutdown();
    }

    private void sendMacbeth(String receiver, String data) {
    	MbPacket p = new MbPacket();
        p.setDestination(new MbLocation("self", receiver));
        p.appendContents(data);
        sendPacket(p);	            	

    }
    
    private void parseKeys(String button, String state, int level) {
	    //_debug.println("knapp " + button + " i state " + state);
	    if (state.equals("DOWN")) {
	    	if (padUp.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	if (level >= 1) {
	            		sendCommand(null, "<scroll>1</scroll>");
	            	}
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padUp state=\"DOWN\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padDown.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	if (level >= 1) {
	            		sendCommand(null, "<scroll>-1</scroll>");
	            	}
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padDown state=\"DOWN\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padRight.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	if (level >= 2) {	
	            		curButtonReceiver = ((SubMenu)subMenus.get(Integer.parseInt(rootMenuMiddleId))).getModuleName();
	            		curSubMenu = curButtonReceiver;
	            		sendMacbeth(curSubMenu, "<activate />");
	            	}
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padRight state=\"DOWN\" level=\"" + level + "\" />");
	            }
	        } else if (padLeft.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	//do nothing
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padLeft state=\"DOWN\" level=\"" + level + "\" />");
	            }
	        }

	        else if (padOk.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padOk state=\"DOWN\" level=\"" + level + "\" />");
	            }
	        }
	    }
	    else if (state.equals("UP")) {
	        if (padUp.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	if (level >= 1) {
	            		sendCommand(null, "<scroll>0</scroll>");
	            	}
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padUp state=\"UP\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padDown.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	if (level >= 1) {
	            		sendCommand(null, "<scroll>0</scroll>");
	            	}
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padDown state=\"UP\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padOk.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padOk state=\"UP\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padLeft.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padLeft state=\"UP\" level=\"" + level + "\" />");
	            }
	        }
	        else if (padRight.equals(button)) {
	        	//if current button-receiver is the menu system module
	            if (curButtonReceiver.equals(name())) {
	            	
	            } else {
	            	sendMacbeth(curButtonReceiver, "<padRight state=\"UP\" level=\"" + level + "\" />");
	            }
	        }

	    }
    }

    
    /**
     * Takes care of XML-data found in incoming macbeth-packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {
        	if (element.equals("packet")) {
        		if (currentPacket.getSource().getModule().equals(curSubMenu)) {
        			//data arrive from active submenu
        			sendData(null, currentPacket.getContents());
        			
        		} else {
        			//data arrives from submenu that is not active
        			//what happens then?
        		}
        		
            } else if (element.equals("deactivates") ) {
            	String module = currentPacket.getSource().getModule();
            	if (curSubMenu.equals(module)) {
            		curSubMenu = name();
            		sendRootMenu(null);
            		sendMacbeth(curSubMenu, "<deactivate />");
            	}
            	if (curButtonReceiver.equals(module)) {
            		curButtonReceiver = name();
            		sendMacbeth(curButtonReceiver, "<deactivate />");
            	}

            } else if (element.equals("request") ) {
            	//om en undermeny vill ha uppmärksamhet skickar den "request"
            	//vad är kraven på att en undermeny ska få visas på begäran?
            	//en timeout när vald meny inte skickat något?
            	//eller att en krävd visning kan klickas bort med cancel/back på fjärren
            	// och att man då återgår till den meny som visades innan
            	
            	String module = currentPacket.getSource().getModule();
            	if (!curSubMenu.equals(module)) {
            		//kontroller här?
            		curSubMenu = module;
                	sendMacbeth(curSubMenu, "<activate />");
            	}                        
            	if (!curButtonReceiver.equals(module)) {
            		//kontroller här?
            		curButtonReceiver = module;
            	}

       		/*
        	 * a submenu should be able to register itself to the menu system
        	 * it provides a name and description to display, but also a true/false value
        	 * indicating if the submenu should be displayed in the root_menu list at all
        	 */
        	} else if (element.equals("registersubmod") && attributes.containsKey("displayname") && attributes.containsKey("visible") && attributes.containsKey("displaydesc")) {
                String displayname = (String)attributes.get("displayname");
                String displaydesc = (String)attributes.get("displaydesc");
                _debug.println(displayname + " registrerar sig");
                //_debug.println(displaydesc);
                String module = currentPacket.getSource().getModule();
                boolean visible = false;
                if (attributes.get("visible").toString().equals("true")) {
                	visible = true;
                }

                //has this module been reged before?
                boolean exists = false;
                for (int i = 0; i < subMenus.size(); i++) {
                	if (((SubMenu)subMenus.get(i)).getModuleName().equals(module)) {
                		exists = true;
                		break;
                	}
                }
                if (!exists) {
                    //so create a new submenu object
                    SubMenu sm = new SubMenu(module, displayname, displaydesc, visible);
                    subMenus.add(sm);
                    //refresh flinsh-client menu (if it is root menu)
                	if (curSubMenu.equals(name())) {
                		sendRootMenu(null);
                	}                        
                }

            //en submeny ska kunna avregistrera sig också, om den avslutas
            } else if (element.equals("unregistersubmod") ) {
                String module = currentPacket.getSource().getModule();

                //has this module been registered?
                for (int i = 0; i < subMenus.size(); i++) {
                	if (((SubMenu)subMenus.get(i)).getModuleName().equals(module)) {
                        subMenus.remove(i);
                        
//                      TODO: avskaffa parseint på data som kommer utifrån
                        if (Integer.parseInt(rootMenuMiddleId) >= subMenus.size() & Integer.parseInt(rootMenuMiddleId) > 0) {
                        	rootMenuMiddleId = "" + (Integer.parseInt(rootMenuMiddleId) - 1);
                        }
                        
                    	if (curSubMenu.equals(module)) {
                    		curSubMenu = name();
                        	//refresha klientens meny (om den är root)
                    		sendRootMenu(null);
                    	}                        
                    	if (curButtonReceiver.equals(module)) {
                    		curButtonReceiver = name();
                    	}                        

                    	break;
                	}
                }
            } 
            
            /* Ex: <remotecontrol button="NUMERIC4" state="DOWN" />
             * This is a notification from the remote control module
             * about one of our requested remote control buttons have
             * been pressed. */
            else if (element.equals("remotecontrol") && attributes.containsKey("button") && attributes.containsKey("state")) {
                String state = (String)attributes.get("state");
                String button = (String)attributes.get("button");
                parseKeys(button, state, 3);
            }
        	
        }
        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }
    
    /**
     * Takes care of XML-data found in incoming flinsh-packets.
     */
    private class FlinshDataHandler implements XMLDataHandler {
        public void XMLstartElement(String element, HashMap attributes) {
        	if (element.equals("flinshpacket") && attributes.containsKey("button") && attributes.containsKey("state")) {
        		String button = (String)attributes.get("button");
        		String state = (String)attributes.get("state");
        		String newButton = "";
        		
        		if (button.equals("padUp")) {
        			newButton = padUp;
        		} else if (button.equals("padDown")) {
        			newButton = padDown;
        		} else if (button.equals("padLeft")) {
        			newButton = padLeft;
        		} else if (button.equals("padRight")) {
        			newButton = padRight;
        		} else if (button.equals("padOk")) {
        			newButton = padOk;
        		}
        		
        		if (!newButton.equals("") && (state.equals("UP") || state.equals("DOWN"))) {
        			parseKeys(newButton, state, currentLevel);
        		}
        	}
        	else if (element.equals("flinshpacket") && attributes.containsKey("menuid") && attributes.containsKey("middleid")) {
                String id = (String)attributes.get("middleid");
                String menuid = (String)attributes.get("menuid");
                //int flinshMiddleItem = Integer.parseInt(id);
                
                //om root:
                if (curButtonReceiver.equals(name())) {
                	if (currentLevel >= 2) {
                		rootMenuMiddleId = id;
                	}
                } else {
                	sendMacbeth(curSubMenu, "<middleid value=\"" + id + "\" menuid=\"" + menuid + "\" userlevel=\"" + currentLevel + "\" />");
                }
        		//_debug.println("data parsad, id: " + menuid + " middleitem: " + id);
        	}
        	else if (element.equals("flinshpacket") && attributes.containsKey("login") && attributes.containsKey("pass")) {
        		String user = (String)attributes.get("login");
        		String pass = (String)attributes.get("pass");
        		login(currentAddress, user, pass);

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
     * @param user
     * @param pass
     */
    private void login(String address, String user, String pass) {
		//ip är inte ett giltigt användarnamn
		//eftersom ip används för att få autologin för ipadresser
		if (!user.equals("ip")) {
	    	int level = 0;
	    	//rota igenom listan över de möjliga logins som finns
	    	for (int i=0; i<userLogin.size(); i++) {
	    		String username = ((UserLoginObj)userLogin.get(i)).user;
    			String userLoginIp = ((UserLoginObj)userLogin.get(i)).password;
    			if (username.equals(user) && userLoginIp.equals(pass)) {
    				level = ((UserLoginObj)userLogin.get(i)).level;
    				//rota igenom alla inloggade klienter
                  	for (int j=0; j<clientsConnected.size(); j++) {
                   		String adr = ((ClientObj)clientsConnected.get(j)).address;
                   		if (adr.equals(address)) {
                   			int oldlevel = ((ClientObj)clientsConnected.get(j)).level;
                   			if (oldlevel < level) {
                   				_debug.println("client logged in with new level: " + level);
                   				((ClientObj)clientsConnected.get(j)).level = level;
                   				if (oldlevel == 0 && !guestCanView) {
                   					//skicka meny till klienten
                   			    	if (curSubMenu.equals(name())) {
                   			    		sendRootMenu(address);
                   			    	} else {
                   			    		sendMacbeth(curSubMenu, "<refresh />");
                   			    	}
                   				}
                   			}
                   			break;
                   		}
                	}
                  	break;
    			}
	    	}	
		}
    }
    /*private class TimerListener implements ActionListener {

	       public void actionPerformed(ActionEvent e) {
	    	   _debug.println("timer ping");
	    	   //först skicka testdata här bara
	    	   //börja med <scrollup />
	    	   testserver.sendToAll("<?xml version=\"1.0\"?><contentdata><middleitem>0</middleitem><contentname>MainMenu</contentname><list><item><title>08:00 Clifford, den lilla r&#246;da valpen</title><description>Tecknad serie. Del 13 av 17.</description></item><item><title>08:35 Lantmusen och stadsmusen</title><description>Animerad serie. M&#246;ssen Alexander och Emily reser jorden runt. Del 9 av 13.</description></item><item><title>15:00 Perkele</title><description>Test bla bla</description></item></list></contentdata>" + EOF);
	    	   
	    	   //koppla upp sig mot http://www.arune.se/tv/tv_rss_now.php?id0=21&id1=22&id2=26&id3=27&id4=10&id5=42&id6=14&id7=89
	    	   //forma om den xml till det format flinsh vill ha skiten
	    	   //skicka till flinsh
	       }
		}*/

    /**
     * Objekt som håller reda på vilka möjliga inloggningar klienten kan göra
     */
    private class UserLoginObj {
    	public String user;
        public String password;
        public int level;
        /**
         * Creates a new instance of UserLoginObj.
         */
        public UserLoginObj(String user, String password, int level) {
        	this.user = user;
        	this.password = password;
        	this.level = level;
        }
    }
    
    /**
     * Objekt som håller reda på alla inloggade klienter
     * @author arune
     */
    private class ClientObj {
    	public String address;
        public int level;
        /**
         * Creates a new instance of ClientObj.
         */
        public ClientObj(String address, int level) {
        	this.address = address;
        	this.level = level;
        }
    }
}