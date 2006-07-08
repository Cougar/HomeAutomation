/*
 * MbModule.java
 *
 * Created on den 25 augusti 2003, 01:23
 */
package Macbeth.System;

import org.xml.sax.SAXException;

import java.io.PrintStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.util.LinkedList;
import java.util.List;
import java.util.HashMap;

import Macbeth.XML.XMLDataHandler;
import Macbeth.XML.XMLParser;
import Macbeth.Utilities.TextFormatterStream;
import Macbeth.Utilities.DataRepository;

/**
 * The Macbeth Module class. Extend this when creating
 * your own modules.
 * @author Jimmy
 */
abstract public class MbModule extends MbComponent implements MbPacketHandler {
    //our packet XML parser
    private XMLParser packetXMLParser;
    //our configuration data handler (if any)
    private XMLDataHandler configDataHandler;
    //our packet data handler (if any)
    private XMLDataHandler packetDataHandler;
    //our html interface (if any)
    private MbHTMLInterface htmlInterface;
    //a list of other modules that we depend on
    private List dependencies;
    //our configuration file
    protected String configFile;
    //our data repository. stores options and other data from the XML-file
    protected DataRepository dataRepository;
    //this points to the options data entry inside the data repository
    protected DataRepository.DataListItem options;

    /**
     * Our debug output stream. Write debug-info to this
     * stream. You can assume that information written
     * to this stream is not necessarily visible to the
     * user (only if he/she has turned it on).
     */
    protected PrintStream _debug;

    /**
     * Our information output stream. Write non-critical
     * information to this stream. This information will
     * be visible to the user most of the time (unless
     * he/she has turned it off).
     */
    protected PrintStream _info;

    /**
     * Our error output stream. Write all error messages
     * to this stream. This information will always be
     * visible to the user.
     */
    protected PrintStream _errors;

    /**
     * The parent kernel that loaded this module.
     */
    protected MbKernel parentKernel;

    /**
     * This is always a reference to the packet that
     * is currently being parsed, so if you need access
     * to the packet during XML parsing, you can use this.
     */
    protected MbPacket currentPacket;

    /**
     * Creates a new instance of MbModule.
     */
    public MbModule() {
        //construct MbComponent
        super();
        parentKernel = null;
        configDataHandler = null;
        packetDataHandler = null;
        currentPacket = null;
        packetXMLParser = null;
        htmlInterface = null;
        configFile = "";
        //create dependencies-list
        dependencies = new LinkedList();
        //default streams
        _debug = System.out;
        _info = System.out;
        _errors = System.err;
        //create data repository
        dataRepository = new DataRepository();
        dataRepository.advancedBuild_BeginList("options");
        dataRepository.advancedBuild_BeginListItem();
        dataRepository.advancedBuild_EndListItem();
        dataRepository.advancedBuild_EndList();
        options = (DataRepository.DataListItem)dataRepository.getList("options").firstItem();
        //create configuration data handler
        configDataHandler = new ConfigDataHandler();
    }

    /**
     * Gets the name of the module.
     * @return The name of the module.
     */
    public abstract String name();

    /**
     * Gets a short description of the module. This should
     * be kept short (1-2 lines) and should be formatted as
     * plain text.
     * @return A short description of the component.
     */
    public abstract String description();

    /**
     * Selects which XMLDataHandler should take care of the XML-data
     * found when parsing the configuration file for this module.
     * @param configDataHandler The XMLDataHandler.
     * @deprecated Due to new configuration system. Get config data
     * from data repository instead!
     */
    protected void setConfigDataHandler(XMLDataHandler configDataHandler) {
        //this.configDataHandler = configDataHandler;
    }

    /**
     * Selects which XMLDataHandler should take care of the XML-data
     * found in incoming packet.
     * @param packetDataHandler The XMLDataHandler.
     */
    protected void setPacketDataHandler(XMLDataHandler packetDataHandler) {
        this.packetDataHandler = packetDataHandler;
    }

    /**
     * Sets our HTML-interface.
     * @param htmlInterface The html-interface.
     */
    protected void setHTMLInterface(MbHTMLInterface htmlInterface) {
        this.htmlInterface = htmlInterface;
    }

    /**
     * Retreives this modules HTML-interface.
     */
    public MbHTMLInterface getHTMLInterface() {
        return htmlInterface;
    }

    /**
     * Adds a module dependency. If your module depends on another
     * modules you should call this with the module names in your
     * constructor. The kernel will then make sure the dependency
     * modules are started up before your module.
     * @param moduleName
     */
    protected final void addDependency(String moduleName) {
        if (!dependencies.contains(moduleName)) {
            dependencies.add(moduleName);
        }
    }

    /**
     * Returns this modules dependencies. This is a list containing
     * the names of all modules that this module depend on.
     * @return The dependencies-list.
     */
    public List getDependencies() {
        return dependencies;
    }

    /**
     * Returns this modules data repository, which contains all the
     * module options and its data lists.
     * @return this modules data repository.
     */
    public DataRepository getDataRepository() {
        return dataRepository;
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    abstract protected void initDataFields();

    /**
     * Will be called when this module should start up. Most
     * initialization should be done here (rather than in the
     * constructor).
     * @throws MbStartupException if this module cannot
     * start up for some reason.
     */
    public void startup() throws MbStartupException {
        //this method will set default-values on all required data repository fields
        initDataFields();
        //if there is a packet data handler registered
        if (packetDataHandler!=null) {
            //create packet xml parser
            packetXMLParser = new XMLParser(packetDataHandler);
        }
        //if there is a configuration data handler registered
        if (configDataHandler!=null) {
            //create XML parser and try to parse configuration file
            XMLParser xmlParser = new XMLParser(configDataHandler);
            configFile = "Config\\mod" + name() + ".xml";
            try {
                xmlParser.parseFile(configFile);
            } catch (FileNotFoundException e) {
                //no config file existed. that's OK, but warn user
                // (a config data handler was registered after all)
                _info.println("WARNING: a configuration data handler exists, but no configuration file was found!");
            } catch (IOException e) {
                //I/O-exception. that's worse, so lets warn
                _errors.println("I/O-error while trying to read '" + configFile + "':");
                _errors.println(e);
                e.printStackTrace();
            } catch (SAXException e) {
                _errors.println("The configuration file '" + configFile + "' contains syntax errors:");
                _errors.println(e);
                e.printStackTrace();
            }
        }
        //now that config file is parsed, we can let the packet system start up
        super.startup();
        _debug.println("I was started up!");
    }

    /**
     * Will be called when this module should shut down itself.
     * You cannot send any packets to other modules here, they
     * might have been shut down already!
     */
    public void shutdown() {
        super.shutdown();
        _debug.println("I was shut down!");
    }

    /**
     * Sets this module's parent kernel.
     * @param parentKernel The parent kernel that loaded this module.
     */
    public void setParentKernel(MbKernel parentKernel) {
        this.parentKernel = parentKernel;
        _debug = new TextFormatterStream(parentKernel._debug);
        ((TextFormatterStream)_debug).setPreString(name() + ": ");
        _info = new TextFormatterStream(parentKernel._info);
        ((TextFormatterStream)_info).setPreString(name() + ": ");
        _errors = new TextFormatterStream(parentKernel._errors);
        ((TextFormatterStream)_errors).setPreString(name() + ": ");
    }

    /**
     * Sends a packet to the parent kernel.
     * @param p The packet that should be sent.
     */
    final public void sendPacket(MbPacket p) {
        //put our kernel- and module name into the packet source field
        p.setSource(new MbLocation(parentKernel.name(),this.name()));
        //tell kernel to receive the packet now
        parentKernel.packetReceived(p);
    }

    /**
     * Will be called when a packet needs to be handled.
     * Per default, this method invokes the XML-parser
     * to parse the contents of the packet.
     * @param p The packet that needs to be handled.
     */
    public void handlePacket(MbPacket p) {
        //if we have an XML parser for packets
        if (packetXMLParser!=null) {
            //set current packet reference
            currentPacket = p;
            //try to parse this packets contents
            try {
                packetXMLParser.parseString(p.serializeToString());
            } catch (SAXException e) {
                _errors.println("Error while parsing packet. Probably bad syntax!");
                _errors.println("Error description was: " + e.getMessage());
                _errors.println("The packet was:");
                _errors.println(p.serializeToString());
                //probably not valid xml data
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Performs an action. This is where yor module can expose functionality
     * for scripts and for the user.
     *
     * @param action The Macbeth action that should be performed.
     * @throws MbActionNotPerformedException If the action ID is invalid.
     */
    public void performAction(MbAction action) throws MbActionNotPerformedException {
        throw new MbActionNotPerformedException("Unknown action command!");
    }


    /**
     * Call this when you want to trigger events from your module. Note that
     * the event name you provide will automatically be prefixed by the module
     * name and the string ".On" so that the final event string takes the form
     * 'modulename.OnXXX" where XXX is the event name you provide.
     *
     * @param eventName The name of the event. Please read above.
     */
    private final void triggerEvent(String eventName) {
        parentKernel.triggerMacbethEvent(name() + ".On" + eventName);
    }


    /**
     * Takes care of XML-data found in configuration files.
     */
    private class ConfigDataHandler implements XMLDataHandler {
        /**
         * Called when start of a new element is found in the XML-data.
         * @param element    The name of the element.
         * @param attributes The element attributes.
         */
        public void XMLstartElement(String element, HashMap attributes) {
            if (element.equals("list")) {
                if (attributes.containsKey("name")) {
                    dataRepository.advancedBuild_BeginList((String)attributes.get("name"));
                }
                else {
                    _errors.println("Syntax error in configuration file: 'name'-attribute missing in 'list'-tag!");
                }
            }
            else if (element.equals("item")) {
                dataRepository.advancedBuild_BeginListItem();
            }
            else if (element.equals("field")) {
                if (attributes.containsKey("name") && attributes.containsKey("value")) {
                    dataRepository.advancedBuild_PutField((String)attributes.get("name"), (String)attributes.get("value"));
                }
                else {
                    _errors.println("Syntax error in configuration file: 'name' and/or 'value'-attributes missing in 'datafield'-tag!");
                }
            }
            else if (element.equalsIgnoreCase("option")) {
                if (attributes.containsKey("name") && attributes.containsKey("value")) {
                    options.putField((String)attributes.get("name"), (String)attributes.get("value"));
                }
                else {
                    _errors.println("Syntax error in '" + configFile + "' - 'name'- and/or 'value'-attributes are missing in an 'option'-tag!");
                }
            }
        }

        public void XMLendElement(String element) {
            if (element.equals("list")) {
                dataRepository.advancedBuild_EndList();
            }
            else if (element.equals("item")) {
                dataRepository.advancedBuild_EndListItem();
            }
        }

        public void XMLelementData(String data) {
        }

        public void XMLdocumentStart() {
        }

        public void XMLdocumentEnd() {
        }
    }

}
