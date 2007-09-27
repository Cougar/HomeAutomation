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
        
        //ska g�ras kontroller p� detta nummer, t.ex. ska inte A/B/C/D/*/# komma med, if_numeric ocks� typ
        //f�r att fixa riktnummer separerat fr�n resten (med en dash -)
        //anv�nd http://www.eniro.se/riktnummer/
        //stoppa i ortsnamn i callerOrtsnamn, anv�nds d� inget namn+address dykt upp f�r nummret
        
        //obs, b�rjar nummret med 07 ska de fyra f�rsta antas vara riktnummer 
        callerPhoneNr = "";
    	for (int i = 1; i < data.length; i++) {
    		callerPhoneNr = callerPhoneNr + parseByte(data[i]);
    	}
        
   		//System.out.println("callerPhoneNr.length: " + callerPhoneNr.length());
    	//om telefonnummret �r l�ngre �n 7 och de tv� f�rsta siffrorna inte �r 00 (utrikes)
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
        
    	//om telefonnummret �r l�ngre �n 7 och de tv� f�rsta siffrorna inte �r 00 (utrikes)
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
		//	denna funktion ska h�mta data fr�n www.arune.se/callerid/index.php?id=telenr
		//	meningen �r att det �r l�ttare att skriva om detta php-script om eniro/hitta.se skulle g�ra om
		//	php returnerar data xml-formaterat typ:
		//	<name>Katharina & Ulf Runeson</name>
		//	<address0>F�red Uggletorp 1</address0>
		//	<address1>545 90 T�reboda</address1>

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
    	//HURFAN G�R MAN P� RIKTIGT?? ERS�TT ALLA TECKEN UTOM 0-127 
    	//MED &#char;
    	//finns nog inbyggt i java, finns i php, se rss f�r posten
    	
		text = text.replaceAll("�","&#229;");
		text = text.replaceAll("�","&#228;");
		text = text.replaceAll("�","&#246;");
		text = text.replaceAll("�","&#197;");
		text = text.replaceAll("�","&#196;");
		text = text.replaceAll("�","&#214;");
		text = text.replaceAll("�","&#192;");
		text = text.replaceAll("�","&#193;");
		text = text.replaceAll("�","&#194;");
		text = text.replaceAll("�","&#195;");
		text = text.replaceAll("�","&#198;");
		text = text.replaceAll("�","&#199;");
		text = text.replaceAll("�","&#200;");
		text = text.replaceAll("�","&#201;");
		text = text.replaceAll("�","&#202;");
		text = text.replaceAll("�","&#203;");
		text = text.replaceAll("�","&#204;");
		text = text.replaceAll("�","&#205;");
		text = text.replaceAll("�","&#210;");
		text = text.replaceAll("�","&#211;");
		text = text.replaceAll("�","&#216;");
		text = text.replaceAll("�","&#224;");
		text = text.replaceAll("�","&#225;");
		text = text.replaceAll("�","&#226;");
		text = text.replaceAll("�","&#227;");
		text = text.replaceAll("�","&#230;");
		text = text.replaceAll("�","&#231;");
		text = text.replaceAll("�","&#232;");
		text = text.replaceAll("�","&#233;");
		text = text.replaceAll("�","&#234;");
		text = text.replaceAll("�","&#235;");
		text = text.replaceAll("�","&#236;");
		text = text.replaceAll("�","&#237;");
		text = text.replaceAll("�","&#242;");
		text = text.replaceAll("�","&#243;");
		text = text.replaceAll("�","&#248;");
		text = text.replaceAll("�","&#249;");
		text = text.replaceAll("�","&#250;");
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
        	//currentServer r�knas upp i catch:en nedan
        }
        if (currentServer >= servers.size()) {
            currentServer = 0; }
        return (String)servers.get(currentServer);
    }
    
    
    private void fixPhoneOrt() {
    	//om mobilnummer s� s�tt in en dash
		if (callerPhoneNr.substring(0,2).equals("07")) {
			callerPhoneNr = callerPhoneNr.substring(0,4) + "-" + callerPhoneNr.substring(4,callerPhoneNr.length());
		} else {
			ArrayList riktnrSource = getHTMLSource("http://www.eniro.se/riktnummer/0-9.shtml");
			if (riktnrSource.size() != 0) {
				//ortsnamn sparas mellan varven eftersom ortsnamn �r p� annan rad �n riktnummer
				//String lastOrt = "";
				String lastNumber = "";
				//rota igenom alla rader i html-sourcen
    	    	for (int i = 0; i < riktnrSource.size(); i++) {
    	    		String row = (String)riktnrSource.get(i);
    	    		
    	    		String startstr = "<td><font size=\"2\">";
    	    		String endstr = "</font></td>";
    	    		
    	    		//kolla om raden inneh�ller ortsnamn eller riktnummer
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
    	    				//d� �r det v�l orsnamn:
		    				int riktnummerlength = lastNumber.length();
	    	    			if (lastNumber.equals(callerPhoneNr.substring(0,riktnummerlength))) {
	    	    				//fixa dash
	    	    				callerPhoneNr = callerPhoneNr.substring(0,riktnummerlength) + "-" + callerPhoneNr.substring(riktnummerlength,callerPhoneNr.length());
	    	    				
	    	    				//byt ut ������ i lastOrt mot &#229;&#228;&#252;	&#197;&#196;&#214;
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
