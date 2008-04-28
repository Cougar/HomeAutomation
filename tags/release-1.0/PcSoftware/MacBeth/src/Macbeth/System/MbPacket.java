/*
 * MbPacket.java
 *
 * Created on den 24 augusti 2003, 12:55
 */
package Macbeth.System;

import Macbeth.XML.XMLDataHandler;
import Macbeth.XML.XMLParser;
import Macbeth.Utilities.MySystem;
import java.util.Iterator;
import java.util.HashMap;
import java.io.IOException;

import org.xml.sax.SAXException;

/**
 * This class represents a Macbeth packet, containing both its destination,
 * its source, and its contents.
 * @author Jimmy
 */
public class MbPacket {

    /**
     * The Macbeth location to which this packet is addressed.
     * If the kernel-field is set to "self", it means that
     * the packet will be delivered locally only (that is,
     * within the same kernel it was sent from). If the field
     * is set to something else, however, this name will be
     * looked up in the list of known kernels, which, in case
     * the kernel was found, also provides the internet address
     * of the specified kernel.
     */
    private MbLocation destination;

    /**
     * The Macbeth location from which this packet was sent.
     * This can be used by receiver module (or kernel)
     * to identify the sender.
     */
    private MbLocation source;

    /**
     * The packet contents (an XML-document).
     */
    private String contents;

    /**
     * The remote address of the client that sent us this packet.
     * If the packet has never been transported via any networks,
     * this value is "self" (that is, source kernel and dest kernel
     * are equal).
     */
    private String originAddress;

    /**
     * Gets the destination of this packet.
     * @return The packet destination.
     */
    public MbLocation getDestination() {
        return destination;
    }
    /**
     * Sets the destination of this packet.
     * @param destination The new destination.
     */
    public void setDestination(MbLocation destination) {
        this.destination = destination;
    }
    /**
     * Sets the kernel field in the destination of this packet.
     * @param kernel The new destination kernel.
     */
    public void setDestinationKernel(String kernel) {
        destination.setKernel(kernel);
    }
    /**
     * Sets the module field in the destination of this packet.
     * @param module The new destination module.
     */
    public void setDestinationModule(String module) {
        destination.setModule(module);
    }

    /**
     * Gets the source of this packet.
     * @return The packet source.
     */
    public MbLocation getSource() {
        return source;
    }
    /**
     * Sets the source of this packet.
     * @param source The new packet source.
     */
    public void setSource(MbLocation source) {
        this.source = source;
    }

    /**
     * Gets the packet contents. This is an XML-string.
     * @return The packet contents.
     */
    public synchronized String getContents() {
        return contents;
    }
    /**
     * Sets the packet contents. This must be valid XML-data.
     * @param contents The new packet contents.
     */
    public synchronized void setContents(String contents) {
        this.contents = contents;
    }
    /**
     * Appends a string to the packet contents. This must be
     * valid XML-data.
     * @param s The string to append.
     */
    public synchronized void appendContents(String s) {
        this.contents += s;
    }

    /**
     * Gets the source remote address of this packet.
     * @return The remote address of the packet source.
     */
    public synchronized String getOriginAddress() {
        return originAddress;
    }

    /**
     * Creates a new instance of MbPacket.
     */
    public MbPacket() {
        destination = new MbLocation();
        source = new MbLocation();
        originAddress = "self";
        contents = "";
    }

    /**
     * Serializes this packet into a string object (XML-formatted)
     * @return the packet as an XML-string.
     */
    public String serializeToString() {
        String s = "<mbpacket ";
        s += "sourcekernel=\"" + source.getKernel() + "\" ";
        s += "sourcemodule=\"" + source.getModule() + "\" ";
        //s += "destkernel=\"" + destination.kernel + "\" ";
        s += "destmodule=\"" + destination.getModule() + "\" ";
        s += ">" + MySystem.lineBreak;
        s += contents;
        s += MySystem.lineBreak + "</mbpacket>";
        return s;
    }

    /**
     * Tries to deserialize an XML-string into a packet.
     * The format of the string must be:
     * <mbpacket destmodule="modulename">
     *     Only valid XML-data in here!
     * </mbpacket>
     * or else this function will not succeed.
     * @param s The XML-string
     * @return true if successful, false otherwise.
     * @throws SAXException if the string contains an invalid XML document (syntax errors).
     */
    synchronized public boolean createFromString(String s) throws SAXException {
        //destination kernel is always set to "self"
        destination = new MbLocation("self",null);
        source = new MbLocation("external","external");
        contents = "";
        XMLParser xmlParser = new XMLParser(new DataHandler());
        try {
            xmlParser.parseString(s);
        } catch (IOException e) {
            System.err.println(e);
            e.printStackTrace();
            return false;
        }
        //TODO: why check this? can it happen?
        if (destination==null) {
            return false;
        }
        return true;
    }

    /**
     * A data handler for XML parser.
     */
    private class DataHandler implements XMLDataHandler {
        //current element name
        private String currentElement = null;
        //current data for the element
        private String currentElementData = null;

        public void XMLstartElement(String element, HashMap attributes) {
            currentElementData = null;
            if (element.equals("mbpacket")) {
                if (attributes.containsKey("sourcekernel")) {
                    source.setKernel((String)attributes.get("sourcekernel"));
                }
                if (attributes.containsKey("sourcemodule")) {
                    source.setModule((String)attributes.get("sourcemodule"));
                }
                if (attributes.containsKey("destmodule")) {
                    destination.setModule((String)attributes.get("destmodule"));
                }
                // NOTE: destination kernel is IGNORED in all incoming packets!
                // (for security reasons, since no relaying is allowed. destkernel is always set to "self")
            } else if (element.equals("remoteaddress") && originAddress.equals("self")) {
                //(it must equal self, or else it can be set more than once, and can thus be faked)
                //remember remote address
                if (attributes.containsKey("value")) {
                    originAddress = (String)attributes.get("value");
                }
            } else {
                //this is user data, so put it into the contents field
                currentElement = element;
                contents += "<" + element;
                Iterator it = attributes.keySet().iterator();
                while (it.hasNext()) {
                    String key = (String)it.next();
                    contents += " " + key + "=\"" + attributes.get(key) + "\"";
                }
                //NEW CODE:
                contents += ">";
            }
        }
        public void XMLelementData(String data) {
            //NEW CODE
            if (currentElement!=null) {
                contents += MySystem.lineBreak + data;
                currentElementData = data;
            }
        }
        public void XMLendElement(String element) {
            if (!element.equals("remoteaddress") && !element.equals("mbpacket")) {
                //NEW CODE:
                //always add full end tag
                //TODO: maybe fix this again. OLD CODE caused some ">" to be missing though!
                contents += "</" + element + ">";
            }
            currentElement = null;
        }
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }
}
