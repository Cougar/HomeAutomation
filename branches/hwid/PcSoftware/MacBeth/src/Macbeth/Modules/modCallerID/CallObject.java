/*
 * File created by arune
 * at 2006-may-21 
 */
package Macbeth.Modules.modCallerID;

import Macbeth.Utilities.UByte;
import java.io.*;
import java.net.URL;
import java.util.*;

import Macbeth.Utilities.MyDateTime;
import Macbeth.System.*;
import Macbeth.Utilities.*;

/**
 * 
 * 
 * 
 * @author arune
 */
public class CallObject {
	
	//private UByte[]	data;
	private String	callerPhoneNr;
	private String	callerName;
	private String	callerAddress;
	private String	callDate;
	private String	callerOrtsnamn;

    private ArrayList servers;    //a list of servers
    private int currentServer = -1;     //the serverlist will cycle when connection fails, this var keeps track of current server

    /**
     * Creates a new instance of CallObject.
     * @param data The byte value of this object.
     */
    public CallObject(UByte[] data, ArrayList servers) {
        //this.data = data;
        callerName = "";
        callerAddress = "";
        callDate = MyDateTime.now();
        callerOrtsnamn = "";
        
        this.servers = servers;
        
        //ska göras kontroller på detta nummer, t.ex. ska inte A/B/C/D/*/# komma med, if_numeric också typ
        //för att fixa riktnummer separerat från resten (med en dash -)
        //använd http://www.eniro.se/riktnummer/
        //stoppa i ortsnamn i callerOrtsnamn, används då inget namn+address dykt upp för nummret
        
        //obs, börjar nummret med 07 ska de fyra första antas vara riktnummer 
        callerPhoneNr = "";
    	for (int i = 1; i < data.length; i++) {
    		callerPhoneNr = callerPhoneNr + parseByte(data[i]);
    	}
        
   		//System.out.println("callerPhoneNr.length: " + callerPhoneNr.length());
    	//om telefonnummret är längre än 7 och de två första siffrorna inte är 00 (utrikes)
    	if (callerPhoneNr.length() > 7 && !callerPhoneNr.substring(0,2).equals("00")) {
    		fixPhoneOrt();
    		getNameAddress();
    		
    	} else if (callerPhoneNr.length() == 2) {
    		//System.out.println("callerPhoneNr: " + callerPhoneNr + " data[0].toString(): " + data[0].toString());
    		if (parseByte(data[0]).equals("B") && callerPhoneNr.equals("00")) {
        		//System.out.println("Hidden");
    			callerPhoneNr = "[Hidden number]";
    		} else if (callerPhoneNr.equals("01")) {
        		//System.out.println("Secret");
    			callerPhoneNr = "[Secret number]";
    		}
    	}
        
    }
    
    public CallObject() {
        this(new UByte[0], new ArrayList());
    }

    public CallObject(String number) {
        callerName = "";
        callerAddress = "";
        callDate = MyDateTime.now();
        callerOrtsnamn = "";
        callerPhoneNr = number;
        
    	//om telefonnummret är längre än 7 och de två första siffrorna inte är 00 (utrikes)
    	if (callerPhoneNr.length() > 7 && !callerPhoneNr.substring(0,2).equals("00")) {
    		fixPhoneOrt();
            //getDataFromEniro
    	}
    }
    
    private String getPart(String data, String startstr, String endstr) {
    	String out = null;
    	int start = data.indexOf(startstr);
    	//System.out.println("start: " + start);
    	if (start != -1) {
    		int end = data.indexOf(endstr,start);
        	//System.out.println("end: " + end);
    		if (end != -1) {
    			out = data.substring(start+startstr.length(),end);
            	//System.out.println("out: " + out);
    		}
    	}
    	
    	return out;
    }
    
    private void getNameAddress() {
		//	denna funktion ska hämta data från www.arune.se/callerid/index.php?id=telenr
		//	meningen är att det är lättare att skriva om detta php-script om eniro/hitta.se skulle göra om
		//	php returnerar data xml-formaterat typ:
		//	<name>Katharina & Ulf Runeson</name>
		//	<address0>Färed Uggletorp 1</address0>
		//	<address1>545 90 Töreboda</address1>

    	ArrayList rows = getWebPage(callerPhoneNr);
    	String name = null;
    	String address0 = null;
    	String address1 = null;
    	
    	for (int i = 0; i < rows.size(); i++) {
    		String row = (String)rows.get(i);
    		if (name == null) {name = getPart(row,"<name>","</name>");}
    		if (address0 == null) {address0 = getPart(row,"<address0>","</address0>");}
    		if (address1 == null) {address1 = getPart(row,"<address1>","</address1>");}
    		//System.out.println(row);
    	}
    	if (name != null) {
    		//name = replaceSweChar(name);
    		callerName = name;
    		//System.out.println("namn: " + name);
    	}
    	if (address0 != null) {
    		//address0 = replaceSweChar(address0);
    		callerAddress = address0;
    		//System.out.println("adress: " + address0);
    	}
    	if (address1 != null) {
    		//address1 = replaceSweChar(address1);
    		if (callerAddress.length() > 0) {
    			callerAddress = callerAddress + "&#10;";
    		}
    		callerAddress = callerAddress + address1;
    	}
    }
    
    private String replaceSweChar(String text) {
    	//HURFAN GÖR MAN PÅ RIKTIGT?? ERSÄTT ALLA TECKEN UTOM 0-127 
    	//MED &#char;
    	//finns nog inbyggt i java, finns i php, se rss för posten
    	
		text = text.replaceAll("å","&#229;");
		text = text.replaceAll("ä","&#228;");
		text = text.replaceAll("ö","&#246;");
		text = text.replaceAll("Å","&#197;");
		text = text.replaceAll("Ä","&#196;");
		text = text.replaceAll("Ö","&#214;");
		text = text.replaceAll("À","&#192;");
		text = text.replaceAll("Á","&#193;");
		text = text.replaceAll("Â","&#194;");
		text = text.replaceAll("Ã","&#195;");
		text = text.replaceAll("Æ","&#198;");
		text = text.replaceAll("Ç","&#199;");
		text = text.replaceAll("È","&#200;");
		text = text.replaceAll("É","&#201;");
		text = text.replaceAll("Ê","&#202;");
		text = text.replaceAll("Ë","&#203;");
		text = text.replaceAll("Ì","&#204;");
		text = text.replaceAll("Í","&#205;");
		text = text.replaceAll("Ò","&#210;");
		text = text.replaceAll("Ó","&#211;");
		text = text.replaceAll("Ø","&#216;");
		text = text.replaceAll("à","&#224;");
		text = text.replaceAll("á","&#225;");
		text = text.replaceAll("â","&#226;");
		text = text.replaceAll("ã","&#227;");
		text = text.replaceAll("æ","&#230;");
		text = text.replaceAll("ç","&#231;");
		text = text.replaceAll("è","&#232;");
		text = text.replaceAll("é","&#233;");
		text = text.replaceAll("ê","&#234;");
		text = text.replaceAll("ë","&#235;");
		text = text.replaceAll("ì","&#236;");
		text = text.replaceAll("í","&#237;");
		text = text.replaceAll("ò","&#242;");
		text = text.replaceAll("ó","&#243;");
		text = text.replaceAll("ø","&#248;");
		text = text.replaceAll("ù","&#249;");
		text = text.replaceAll("ú","&#250;");
		return text;
    }
    
    private ArrayList getWebPage(String id) {
        ArrayList rowStrings = new ArrayList();

        String serverURL = getServerURL();

        if (!serverURL.equals("")) {
            serverURL = serverURL + "/index.php?id=" + id;
            try {
                BufferedReader page = new BufferedReader(new InputStreamReader(new URL(serverURL).openStream()));
                
                String curLine = page.readLine();
                while ( curLine != null ) {
                    rowStrings.add(curLine);
                    // get the next line
                    curLine = page.readLine();
                }
            }
            catch ( Exception ex ){
                //_errors.println("Comunication problem with caller-address-server, exception: " + ex.getMessage());
                currentServer++;
            }
        }

        return rowStrings;
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
    
    
    private void fixPhoneOrt() {
    	//om mobilnummer så sätt in en dash
		if (callerPhoneNr.substring(0,2).equals("07")) {
			callerPhoneNr = callerPhoneNr.substring(0,4) + "-" + callerPhoneNr.substring(4,callerPhoneNr.length());
		} else {
			ArrayList riktnrSource = getHTMLSource("http://www.eniro.se/riktnummer/0-9.shtml");
			if (riktnrSource.size() != 0) {
				//ortsnamn sparas mellan varven eftersom ortsnamn är på annan rad än riktnummer
				//String lastOrt = "";
				String lastNumber = "";
				//rota igenom alla rader i html-sourcen
    	    	for (int i = 0; i < riktnrSource.size(); i++) {
    	    		String row = (String)riktnrSource.get(i);
    	    		
    	    		String startstr = "<td><font size=\"2\">";
    	    		String endstr = "</font></td>";
    	    		
    	    		//kolla om raden innehåller ortsnamn eller riktnummer
    	    		int start = row.indexOf(startstr);
    	    		int end = row.indexOf(endstr);
    	    		if (start > 0 && end > (start + startstr.length())) {
	    				String sNumber = row.substring(start + startstr.length(), end);
	    				String lastOrt = row.substring(start + startstr.length(), end);
	    				int iNumber = 0;
    	    			try {
    	    				iNumber = Integer.parseInt(sNumber);
    	    			} catch (NumberFormatException ex) {
    	    			}
	    				if (iNumber > 0) {
	    					lastNumber = sNumber;
	    				} else {
    	    				//då är det väl orsnamn:
		    				int riktnummerlength = lastNumber.length();
	    	    			if (lastNumber.equals(callerPhoneNr.substring(0,riktnummerlength))) {
	    	    				//fixa dash
	    	    				callerPhoneNr = callerPhoneNr.substring(0,riktnummerlength) + "-" + callerPhoneNr.substring(riktnummerlength,callerPhoneNr.length());
	    	    				
	    	    				//byt ut åäöÅÄÖ i lastOrt mot &#229;&#228;&#252;	&#197;&#196;&#214;
	    	    				lastOrt = replaceSweChar(lastOrt);
	    	    				//spara ortsnamn
	    	    				callerOrtsnamn = lastOrt;
	    	    				
	    	    				break;
	    	    			}
	    				}
    	    		}
    	    	}
			}
		}
    }
    
    private ArrayList getHTMLSource(String url) {
    	ArrayList returndata = new ArrayList();
    	
        try {
            BufferedReader page = new BufferedReader(new InputStreamReader(new URL(url).openStream()));
            
            String curLine = page.readLine();
            while ( curLine != null ) {
            	returndata.add(curLine);
                // get the next line
                curLine = page.readLine();
            }
        }
        catch ( Exception ex ){
            //_errors.println("Comunication problem with tv-chart-server, exception: " + ex.getMessage());
            
        }
    	
    	return returndata;
    }
    
    private String parseByte(UByte dobyte) {
    	byte[] dummyByte = new byte[1]; 
    	dummyByte[0] = dobyte.byteValue();
    	
    	return new String(dummyByte);

    }
    
    public String getPhoneNr() {
    	return callerPhoneNr;
    }
    public String getName() {
    	return callerName;
    }
    public String getAddress() {
    	return callerAddress;
    }
    public String getOrtsnamn() {
    	return callerOrtsnamn;
    }
    public String getDate() {
    	return callDate;
    }
}
