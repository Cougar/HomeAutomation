/*
 * File created by arune
 * at 2006-feb
 */
package Macbeth.Modules.modVagnen;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;
import java.util.*;


/**
 *
 *
 *
 * @author arune
 */
public class StopObject {

    public String stop;
    public String displayName;
    public String[] incLines;
    public String[] excLines;
    public String[] excDests;

    /**
     * Creates a new instance of StopObject.
     * @param .
     * @param .
     * @param .
     * @param .
     */
    public StopObject(String stop, String displayName, String[] incLines, String[] excLines, String[] incDests, String[] excDests) {
        this.stop = stop;
        this.displayName = displayName;
        this.incLines = incLines;
        this.excLines = excLines;
        this.excDests = excDests;
    }

    public ArrayList getStopData() {
    	String url = "http://wap.trafikkontoret.goteborg.se/QuestionPage.aspx?__redir=1&hpl=" + stop + "&pg=1";
    	ArrayList traficData = getWebPage(url);
    	
    	//filtrera och lagra som Buss-objekt
    	
    	//StopObject.Buss test = test1.getStopData();
    	ArrayList stopTimeList = new ArrayList();
    	String[] numberStopTime = {"", "", "", ""};
    	int cnt = 0;
    	
    	for (int i = 0; i < traficData.size(); i++) {
    		//_debug.println(((String)traficData.get(i)));
    		//System.out.println((String)traficData.get(i));
    		
    		String row = (String)traficData.get(i);
    		if (row.indexOf("<td><div nowrap>")==0) {
    			if (row.indexOf("<font") == -1) {
    			
	    			int endTag = row.indexOf("</div>"); 
	    			if (endTag > 16) {
	    				String contents = row.substring(16, endTag); 
	        			//System.out.println(contents);
	        			
	        			numberStopTime[cnt] = contents;
	        			
	        			cnt++;
	        			if (cnt == 4) {
	        				cnt = 0;
		        			//specialfall:
		        			/*
		        			 * <td><div nowrap>--</div></td> bussen går inte alls den tiden (därefter-tiden)
		        			 * <td><div nowrap>19,L</div></td> något med komma
		        			 * <td><div nowrap>ca 47</div></td> ca tid (fixas i modVagnen.java)
		        			 */
	        				int delimIndex = numberStopTime[2].indexOf(","); 
	        				if (delimIndex != -1) {
	        					numberStopTime[2] = numberStopTime[2].substring(0, delimIndex);
	        				}
	        				delimIndex = numberStopTime[3].indexOf(","); 
	        				if (delimIndex != -1) {
	        					numberStopTime[3] = numberStopTime[3].substring(0, delimIndex);
	        				}
	        				
	        				boolean addFirst;
	        				boolean addSec;
	        				if (incLines.length > 0) {
        						addFirst = false;
        						addSec = false;
	        					for (int j = 0; j < incLines.length; j++) {
	        						if (incLines[j].equals(numberStopTime[0])) {
	        							addFirst = true;
	        							addSec = true;
	        						}
	        					}
	        				} else if (excLines.length > 0) {
	        					addFirst = true;
	        					addSec = true;
	        					for (int j = 0; j < excLines.length; j++) {
	        						if (excLines[j].equals(numberStopTime[0])) {
	        							addFirst = false;
	        							addSec = false;
	        						}
	        					}
	        					
	        				} else {
	        					addFirst = true;
	        					addSec = true;
	        				}
	        				
	        				if (numberStopTime[2].indexOf("--") > -1) {
	        					addFirst = false;
	        				}
	        				if (numberStopTime[3].indexOf("--") > -1) {
	        					addSec = false;
	        				}
	        				
	        				for (int j = 0; j < excDests.length; j++) {
	        					if (numberStopTime[1].toLowerCase().contains(excDests[j].toLowerCase())) {
	        						addFirst = false;
	        						addSec = false;
	        					}
	        				}
	        				//System.out.println(displayName + " " + numberStopTime[0] + " " + numberStopTime[1] + " " + numberStopTime[2] + "/" + numberStopTime[3]);
	        				//System.out.println(addFirst + "/" + addSec);
	        				
	        				//filtrera obs gemener versaler
	        				//kolla numberStopTime[0] mot inc/excLines
	        				//kolla numberStopTime[1] mot inc/excDests
	        				if (addFirst) {
	        					stopTimeList.add(new Buss(displayName, numberStopTime[0], numberStopTime[2], numberStopTime[1]));
	        				}
	        				if (addSec) {
	        					stopTimeList.add(new Buss(displayName, numberStopTime[0], numberStopTime[3], numberStopTime[1]));
	        				}
	        			}
	        			//då cnt = 3:
	        			//stopTimeList.add()
	        			//kolla om det är en tid/stop som ska vara med
	    			}
    			}

    			
    		}
    	}
    	
    	//return new ArrayList();
    	return stopTimeList;
    }
    
    private ArrayList getWebPage(String url) {
        ArrayList rowStrings = new ArrayList();
        
        try {
        	URL connurl = new URL(url);
        	URLConnection connection = connurl.openConnection();
        	//detta för att man inte ska få en fattig wap-version av sidan
        	connection.setRequestProperty("User-Agent","Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; SV1; .NET CLR 1.1.4322)");
        	connection.connect();
        	
        	//BufferedReader page = new BufferedReader(new InputStreamReader(connurl.openStream()));
        	BufferedReader page = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        	
            String curLine = page.readLine();
            while ( curLine != null ) {
                rowStrings.add(curLine);
                // get the next line
                curLine = page.readLine();
            }
        }
        catch ( Exception ex ){
        	//TODO: borde skriva ut felmeddelande, kanske kasta på anropande funtion?
            //_errors.println("Comunication problem with server (" + url + "), exception: " + ex.getMessage());
        }

        return rowStrings;
    }
    
    public class Buss {
    	
    	public String stop;
        public String number;
        public String time;
        public String dest;
        
        /**
         * Creates a new instance of RemoteButton.
         */
        public Buss(String stop, String number, String time, String dest) {
        	this.stop = stop;
        	this.number = number;
        	this.time = time;
        	this.dest = dest;
        }

    }
}
