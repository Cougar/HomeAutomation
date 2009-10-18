/*
 * modLoopback.java
 *
 * Created on den 23 augusti 2003, 14:10
 */
package Macbeth.Modules.modLoopback;

import java.util.*;
import Macbeth.System.*;
import Macbeth.Utilities.MySystem;
import Macbeth.XML.XMLDataHandler;

/**
 * Macbeth module for performance loopback tests.
 * @author Jimmy
 */
public class modLoopback extends MbModule {

    private int nrPacketBounces = 0;

    /**
     * Creates a new instance of modLoopback.
     */
    public modLoopback() {
        //construct MbModule
        super();
        //register our packet data handler
        setPacketDataHandler(new PacketDataHandler());
        //we have a html-interface, so lets register it
        setHTMLInterface(new HTMLInterface());
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "Loopback";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Simple loopback test module";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    protected void initDataFields(){
        //this module doesn't require any data fields
    }

    /**
     * Starts up this module.
     */
    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();
        MbPacket p = new MbPacket();
        p.setContents("<returntosource/>");
        p.setDestination(new MbLocation("self",name()));
        for (int i=0; i<1000; i++) {
            sendPacket(p);
        }
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //shut down MbModule
        super.shutdown();
    }

    /**
     * A HTML-interface for this module.
     */
    private class HTMLInterface implements MbHTMLInterface {
        /**
         * Retreives the full code for your HTML-page. This is
         * formatted as plain text and must be valid HTML-code.
         * @return Full HTML-code for the page.
         */
        public String getHTMLPage() {
            String s = "<html><body>" + MySystem.lineBreak;
            s += "<h1>Loopback module</h1>" + MySystem.lineBreak;
            s += "<p>This module sends packets to itself in order to put kernel on heavy workload</p>" + MySystem.lineBreak;
            s += "<p>So far, a total of <b>" + Integer.toString(nrPacketBounces) + "</b> packets have been bounced back and forth to kernel";
            s += "</p></body></html>";
            return s;
        }

        /**
         * This method is invoked when a link is pressed on your
         * HTML-page.
         * @param href The href-attribute of the link.
         */
        public void linkPressed(String href) {
            _info.println("Link pressed: " + href);
        }
    }

    /**
     * Takes care of XML-data found in incoming packets.
     */
    private class PacketDataHandler implements XMLDataHandler {
        /**
         * Called when start of a new element is found in the XML-data.
         * Ex: <name attr1="value1" attr2="value2">
         * Element name would then be "name" and attribute list
         * would contain "value1" and "value2" mapped to the attribute
         * names "attr1" and "attr2".
         * @param element The name of the element.
         * @param attributes The element attributes.
         */
        public void XMLstartElement(String element, HashMap attributes) {
            /**
             * Ex: <returntosource/>
             * Tells us to return this packet to where it came from.
             */
            if (element.equals("returntosource")) {
                MbPacket p = currentPacket;
                p.setDestinationModule(p.getSource().getModule());
                p.setDestinationKernel("self");
                sendPacket(p);
                nrPacketBounces++;
            }
        }

        public void XMLendElement(String element) {}
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }
}
