/**
 * File created by Jimmy
 * at 2003-dec-25 16:15:54
 */
package Macbeth.System;

import Macbeth.Utilities.*;
import Macbeth.XML.XMLDataHandler;
import Macbeth.XML.XMLParser;
import Macbeth.Transports.MbLocalTransport;
import Macbeth.Transports.MbInternetTransport;

import java.io.*;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.HashMap;
import java.util.List;

import org.xml.sax.SAXException;

/**
 * The Macbeth kernel. This is the core component that does all the
 * work. You need to extend this class in order to create command-line
 * kernels, GUI-kernels or whatever else.
 * @author Jimmy
 */
public abstract class MbKernel extends MbComponent {

    //list of all existing modules (module names mapped to module class names)
    protected HashMap moduleList;
    //list of loaded modules (module names mapped to module objects)
    protected HashMap loadedModules;
    //our debug-output-stream (write meaningless debug-info here)
    protected PrintStream _debug;
    //our info-output-stream (write important info here)
    protected PrintStream _info;
    //our error-output-stream (write error messages here)
    protected PrintStream _errors;
    //our logfile-stream
    protected PrintStream _log;
    //a null-stream that ignores any data written to it
    protected PrintStream _null;
    //list of packet transports
    protected List packetTransports;
    //default transports
    protected MbLocalTransport localTransport;
    protected MbInternetTransport internetTransport;
    //packet dumper
    protected PacketDumper packetDumper;
    //our kernel configuration XML-file
    protected String configFile;
    //our data repository. stores options and other data from the XML-file
    protected DataRepository dataRepository;
    //this points to the options data entry inside the data repository
    protected DataRepository.DataListItem options;
    //in this map, all used script events are mapped to lists filled with MbActions
    protected HashMap scriptEventActions;

    /**
     * Creates a new instance of MbKernel.
     * @param configFile The XML-configuration file to use for this kernel.
     */
    public MbKernel(String configFile) {
        //construct MbComponent
        super();

        //remember configuration file
        this.configFile = configFile;

        //create data repository
        dataRepository = new DataRepository();
        dataRepository.advancedBuild_BeginList("options");
        dataRepository.advancedBuild_BeginListItem();
        dataRepository.advancedBuild_EndListItem();
        dataRepository.advancedBuild_EndList();
        options = (DataRepository.DataListItem)dataRepository.getList("options").firstItem();

        //set default values on all our options
        options.putField("kernelname", "Macbeth@" + System.getProperty("user.name").toString());
        options.putField("moduledirectory", System.getProperty("user.dir") + "\\bin\\");
        options.putField("debugstream.logged", "false");
        options.putField("debugstream.timestamped", "false");
        options.putField("debugstream.visible", "false");
        options.putField("debugstream.prefix", "D ");
        options.putField("infostream.logged", "false");
        options.putField("infostream.timestamped", "true");
        options.putField("infostream.visible", "true");
        options.putField("infostream.prefix", "I ");
        options.putField("errorstream.logged", "false");
        options.putField("errorstream.timestamped", "true");
        options.putField("errorstream.visible", "true");
        options.putField("errorstream.prefix", "E ");

        //create output streams
        _debug = System.out;
        _info = System.out;
        _errors = System.err;

        //log stream will be created during startup
        _log = null;

        //create null stream
        _null = new PrintStream(new PipedOutputStream());

        //create module lists
        moduleList = new HashMap(32);
        loadedModules = new HashMap(32);

        //create packet transport list
        packetTransports = new LinkedList();
        //TODO: search dynamically for installed transports at runtime instead.
        //add local delivery transport
        localTransport = new MbLocalTransport(this);
        installTransport(localTransport);
        //add internet delivery transport
        internetTransport = new MbInternetTransport(this);
        installTransport(internetTransport);

        //create packet dumper
        packetDumper = new PacketDumper();

        //create the script map
        scriptEventActions = new HashMap();
    }

    /**
     * Installs a packet delivery transport into this kernel.
     * @param t The transport.
     */
    protected void installTransport(MbPacketTransport t) {
        packetTransports.add(t);
    }

    /**
     * Gets the name of this kernel. Per default, this is
     * "Macbeth@username" where username is your computer
     * username.
     * @return The kernel name.
     */
    public String name() {
        return (String)options.getField("kernelname");
    }

    /**
     * Gets a short description of the kernel. This should
     * be kept short (1-2 lines) and should be formatted as
     * plain text.
     * @return A short description of the kernel.
     */
    public abstract String description();


    /**
     * Call this when you want to trigger events from the kernel. Note that
     * the event name you provide will automatically be prefixed by the string
     * "Kernel.On" so that the final event string takes the form
     * 'Kernel.OnXXX" where XXX is the event name you provide.
     *
     * @param eventName The name of the event. Please read above.
     */
    private final void triggerEvent(String eventName) {
        triggerMacbethEvent("Kernel.On" + eventName);
    }


    /**
     * Triggers a Macbeth event. The kernel will perform all Macbeth
     * actions that are associated with this event.
     *
     * @param eventName The name of the Macbeth event that is being triggered.
     */
    public void triggerMacbethEvent(String eventName) {
        //get all actions associated with this event
        List l = (List) scriptEventActions.get(eventName);
        if (l!=null) {
            Iterator it = l.iterator();
            while (it.hasNext()) {
                MbAction a = (MbAction) it.next();
                try {
                    performAction(a);
                } catch (MbActionNotPerformedException e) {
                    _errors.println("The scripted action '" + a.getActionString() + " " + a.getParameters() + "' could not be performed! (" + e.getMessage() + ")");
                }
            }
        }
    }


    /**
     * Performs an action (called by scripts or by the user).
     * @param action The Macbeth action that should be performed.
     * @throws MbActionNotPerformedException If, for some reason, this action could not be performed.
     */
    protected void performAction(MbAction action) throws MbActionNotPerformedException {
        //split the action string into owner and action name
        String[] tokens = action.getActionString().split("\\.");
        if (tokens.length!=2) {
            throw new MbActionNotPerformedException("Invalid action string (format should be OWNER.ACTION)");
        }
        //extract owner
        String owner = tokens[0];
        String actionName = tokens[1];

        //if the action was owned by us (that is by the kernel), let's perform it
        if (owner.equalsIgnoreCase("Kernel")) {
            if (actionName.equalsIgnoreCase("LoadModule")) {
                try {
                    loadModule(action.getParameters());
                } catch (MbModuleNotFoundException e) {
                    throw new MbActionNotPerformedException("Module not found!");
                } catch (MbModuleLoadException e) {
                    throw new MbActionNotPerformedException("Module found, but couldn't be loaded!");
                }
            }
            else if (actionName.equalsIgnoreCase("UnloadModule")) {
                if (!unloadModule(action.getParameters())) {
                    throw new MbActionNotPerformedException("Module not loaded!");
                }
            }
            //TODO: action that can clean up the packet queue with some filtering abilities
            //TODO: action that reloads all modules
            else {
                throw new MbActionNotPerformedException("Unknown kernel action!");
            }
        }
        //the action isn't owned by the kernel, so it must be owned by a module
        else {
            //try to get the owner module
            MbModule m = (MbModule) loadedModules.get(owner);
            //check if it existed
            if (m==null) {
                throw new MbActionNotPerformedException("The module '" + owner + "' isn't loaded!");
            }
            //handle over the action to the module
            m.performAction(action);
        }
    }

    /**
     * Starts up this kernel.
     */
    public void startup() throws MbStartupException {
        _debug.println("Macbeth kernel is starting up");
        //start up MbComponent
        super.startup();

        //create XML parser and parse configuration file
        _debug.println("Parsing kernel configuration file: " + configFile);
        XMLParser xmlParser = new XMLParser(new ConfigDataHandler());
        try {
            xmlParser.parseFile(configFile);
        } catch (FileNotFoundException e) {
            _errors.println("Configuration file '" + configFile + "' not found. Default settings will be used!");
        } catch (IOException e) {
            _errors.println("I/O error while reading '" + configFile + "'. Default settings will be used!");
        } catch (SAXException e) {
            _errors.println("XML Parsing error while reading '" + configFile + "'. Settings might have been lost. Please check syntax!");
        }
        _debug.println("Finished parsing kernel configuration file");

        //start up packet transport systems
        _debug.println("Starting up packet transport systems:");
        Iterator it = packetTransports.iterator();
        while (it.hasNext()) {
            MbPacketTransport pt = (MbPacketTransport)it.next();
            if (pt!=null) {
                _debug.println("  - " + pt.getName());
                pt.startup();
            }
        }
        _debug.println("OK, " + packetTransports.size() + " transports installed");

        //create a module list
        _debug.println("Enumerating modules...");
        LinkedList classList = new LinkedList();
        try {
            //try to load all classes that inherit from Macbeth.Systen.MbModule, since these are all modules
            DynamicClassLoader.locateClasses(options.getField("moduledirectory"), "", new String[]{"Macbeth.System.MbModule"}, null, classList);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        //iterate through the class list to find out the module names
        it = classList.iterator();
        while (it.hasNext()) {
            String classname = (String)it.next();
            try {
                Class c = DynamicClassLoader.loadClass(options.getField("moduledirectory"), classname);
                //construct a new module (so we can get its name)
                MbModule mod = (MbModule)c.newInstance();
                //save the module name mapped to the full classname in module list
                moduleList.put(mod.name(),classname);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        _debug.println("OK, " + Integer.toString(classList.size()) + " modules found");

        _debug.println("Setting up streams (visibility, timestamping, logging)...");

        //create logstreams (that might be needed if logging is enabled)
        String logfile = "Macbeth log " + MyDateTime.now("yyyy-MM-dd HH_mm_ss") + ".log";

        //if any logging is enabled
        if (options.getField("debugstream.logged").equalsIgnoreCase("true") ||
            options.getField("infostream.logged").equalsIgnoreCase("true") ||
            options.getField("errorstream.logged").equalsIgnoreCase("true")) {
            //create log stream
            try {
                _log = new PrintStream(new FileOutputStream(logfile));
            } catch (FileNotFoundException e) {
                //the log stream has to be valid in order to proceed
                throw new MbStartupException("Error while trying to create the log file '" + logfile + "'!");
            }
        }

        //if visibility is turned off for any of the streams, redirect them to null stream
        if (options.getField("debugstream.visible").equalsIgnoreCase("false")) {
            _debug = _null;
        }
        if (options.getField("infostream.visible").equalsIgnoreCase("false")) {
            _info = _null;
        }
        if (options.getField("errorstream.visible").equalsIgnoreCase("false")) {
            _errors = _null;
        }

        //add logging to the streams
        if (options.getField("debugstream.logged").equalsIgnoreCase("true")) {
            _debug = new MultipleStreamsAdapter(_debug, _log);
        }
        if (options.getField("infostream.logged").equalsIgnoreCase("true")) {
            _info = new MultipleStreamsAdapter(_info, _log);
        }
        if (options.getField("errorstream.logged").equalsIgnoreCase("true")) {
            _errors = new MultipleStreamsAdapter(_errors, _log);
        }

        //add prefixes to streams
        _debug = new TextFormatterStream(_debug);
        ((TextFormatterStream) _debug).setPreString(options.getField("debugstream.prefix"));
        _info = new TextFormatterStream(_info);
        ((TextFormatterStream) _info).setPreString(options.getField("infostream.prefix"));
        _errors = new TextFormatterStream(_errors);
        ((TextFormatterStream) _errors).setPreString(options.getField("errorstream.prefix"));

        //add timestamps to streams
        if (options.getField("debugstream.timestamped").equalsIgnoreCase("true")) {
            _debug = new TimeStampedStream(_debug);
        }
        if (options.getField("infostream.timestamped").equalsIgnoreCase("true")) {
            _info = new TimeStampedStream(_info);
        }
        if (options.getField("errorstream.timestamped").equalsIgnoreCase("true")) {
            _errors = new TimeStampedStream(_errors);
        }

        //trigger the Startup-event now so any scripted post-processing can take place
        triggerEvent("Startup");

        //everything is started up now
        _info.println("Macbeth kernel started!");
    }

    /**
     * Loads and starts up a specific module. Before any modules
     * can be loaded, the module list must have been created. The
     * kernel will do this during startup, so normally this is not
     * a problem.
     * @param moduleName The name of the module that should be loaded.
     * @return true if this module was loaded and started successfully,
     * false if not (because the module was already started!).
     * @throws MbModuleNotFoundException if the specified module isn't found.
     * @throws MbModuleLoadException if the module cannot be loaded and started.
     */
    protected boolean loadModule(String moduleName) throws MbModuleNotFoundException, MbModuleLoadException {
        //if module is already loaded
        if (loadedModules.containsKey(moduleName)) {
            //get out of here
            return false;
        }
        //get classname from our module list
        String classname = (String)moduleList.get(moduleName);
        //if it was in the list
        if (classname!=null) {
            Class c;
            MbModule mod;
            try {
                //try to load class
                c = DynamicClassLoader.loadClass(options.getField("moduledirectory"),classname);
                //try to create new instance (and thus, constuct the module)
                mod = (MbModule)c.newInstance();
            } catch (Exception e) {
                //module couldn't be loaded
                throw new MbModuleLoadException(e.getMessage());
            }
            //work out dependencies for this module
            List dependencies = mod.getDependencies();
            //if there are dependencies
            if (!dependencies.isEmpty()) {
                //for each dependency
                Iterator it = dependencies.iterator();
                while (it.hasNext()) {
                    String dpmod = (String)it.next();
                    if (!loadedModules.containsKey(dpmod)) {
                        _info.println("'" + mod.name() + "' depends on '" + dpmod + "'. This module will now be loaded as well!");
                        loadModule(dpmod);
                    }
                }
            }
            //try to start up module now
            mod.setParentKernel(this);
            try {
                mod.startup();
                //put module in our loaded modules-list
                loadedModules.put(mod.name(), mod);
            }
            //if module failed to start up
            catch (MbStartupException e) {
                _errors.println("Module '" + mod.name() + "' failed to load! (" + e.getMessage() + ")");
                //shut down the parts of the module that were started up
                mod.shutdown();
                //and throw exception
                throw new MbModuleLoadException(e.getMessage());
            }
        }
        //module wasn't found
        else {
            //so throw exception
            throw new MbModuleNotFoundException("Module not found in module list!");
        }
        return true;
    }

    /**
     * Shuts down and unloads a specific module.
     * @param moduleName The name of the module that should be unloaded.
     * @return true if the module was shut down, false if not (because
     * it wasn't even started in the first place!).
     */
    protected boolean unloadModule(String moduleName) {
        //get the module from the enumeration list
        MbModule mod = (MbModule)loadedModules.get(moduleName);
        //if module was found (and thus, loaded)
        if (mod != null) {
            //shut it down
            mod.shutdown();
            loadedModules.remove(moduleName);
        }
        //module wasn't started
        else {
            return false;
        }
        return true;
    }

    /**
     * Returns a given macbeth module. The module must be loaded
     * in this local kernel, or else an exception will be thrown.
     * @param moduleName Classname of the module.
     * @return The module. Null if it isn't found (that is, not loaded).
     * be found within this kernel.
     */
    public MbModule getModule(String moduleName) {
        return (MbModule)loadedModules.get(moduleName);
    }

    /**
     * Shuts down this kernel. Modules will be shut
     * down and removed from module collection.
     */
    public void shutdown() {
        _debug.println("Macbeth kernel is about to shut down");

        //trigger shutdown-event first so any scripted pre-processing can take place
        triggerEvent("Shutdown");

        //shut down any loaded modules
        if (!loadedModules.isEmpty()) {
            _debug.println("Shutting down started modules:");
            //iterate through all loaded modules
            Iterator it = loadedModules.keySet().iterator();
            while (it.hasNext()) {
                //shut down each loaded module
                String modname = (String)it.next();
                unloadModule(modname);
                //we have removed a module from the map, so iterator needs to be re-initialized!
                it = loadedModules.keySet().iterator();
            }
        }

        //shut down packet transport systems
        if (!packetTransports.isEmpty()) {
            _debug.println("Shutting down packet transport systems");
            Iterator it = packetTransports.iterator();
            while (it.hasNext()) {
                MbPacketTransport t = (MbPacketTransport)it.next();
                if (t!=null) {
                    t.shutdown();
                }
            }
        }

        _info.println("Macbeth kernel successfully shut down");

        //shut down stream logging
        if (_log!=null) {
            _log.close();
        }

        //shutdown MbComponent
        super.shutdown();
    }

    /**
     * Will be called when a packet needs to be delivered to
     * its specified destination module.
     * @param p The packet that needs to be handled.
     */
    public void handlePacket(MbPacket p) {
        //iterate through all registered packet transports
        Iterator it = packetTransports.iterator();
        boolean deliverySuccessful = false;
        while (it.hasNext()) {
            //try to deliver with each transport
            MbPacketTransport t = (MbPacketTransport)it.next();
            if (t!=null) {
                try {
                    deliverySuccessful = t.deliverPacket(p);
                } catch (MbPacketTransport.MbPacketDeliveryException e) {
                    //the transport attempted to deliver the packet, but failed
                    _errors.println("Packet delivery error: " + e.getMessage());
                    //dump the packet
                    packetDumper.dumpPacket(p,t.getName() + " attempted to deliver the packet, but failed with the message \"" + e.getMessage() + "\"");
                    //and get out of here
                    return;
                }
                //if delivery was successful
                if (deliverySuccessful) {
                    //get out of here
                    return;
                }
            }
        }
        //no transport could deliver the packet, so dump it
        packetDumper.dumpPacket(p,"None of the transports could deliver this packet!");
    }


    /**
     * A data handler for the XML-configuration file parser.
     */
    public class ConfigDataHandler implements XMLDataHandler {
        private String currentEvent = null;

        public void XMLstartElement(String element, HashMap attributes) {
            if (element.equals("list")) {
                if (attributes.containsKey("name")) {
                    dataRepository.advancedBuild_BeginList((String)attributes.get("name"));
                }
                else {
                    _errors.println("Syntax error in '" + configFile + "': 'name'-attribute missing in 'list'-tag!");
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
                    _errors.println("Syntax error in '" + configFile + "': 'name' and/or 'value'-attributes missing in 'datafield'-tag!");
                }
            }
            else if (element.equalsIgnoreCase("option")) {
                if (attributes.containsKey("name") && attributes.containsKey("value")) {
                    options.putField((String)attributes.get("name"), (String)attributes.get("value"));
                }
                else {
                    _errors.println("Syntax error in '" + configFile + "': 'name'- and/or 'value'-attributes are missing in an 'option'-tag!");
                }
            }
            else if (element.equalsIgnoreCase("event")) {
                if (attributes.containsKey("name")) {
                    currentEvent = (String) attributes.get("name");
                }
                else {
                    _errors.println("Syntax error in '" + configFile + "': 'name'-attribute missing in an 'event'-tag!");
                }
            }
            else if (element.equalsIgnoreCase("do")) {
                if (attributes.containsKey("action")) {
                    String action = (String) attributes.get("action");
                    String params = (String) attributes.get("params");
                    //get the list of actions that are associated with this event so far
                    List actionList = (List) scriptEventActions.get(currentEvent);
                    //is this the first action associated with this event?
                    if (actionList==null) {
                        //if so, a new list needs to be created
                        actionList = new LinkedList();
                        //..and put into the events-actions-map
                        scriptEventActions.put(currentEvent,actionList);
                    }
                    //now, add the new action to the list
                    actionList.add(new MbAction(action, params));
                }
                else {
                    _errors.println("Syntax error in '" + configFile + "': 'action'-attribute missing in a 'do'-tag!");
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
            else if (element.equals("event")) {
                currentEvent = null;
            }
        }
        public void XMLelementData(String data) {}
        public void XMLdocumentStart() {}
        public void XMLdocumentEnd() {}
    }


    /**
     * A packet dumper. This is invoked to kill packets that cannot be
     * delivered. All packet dumps will be logged to a file,
     */
    protected class PacketDumper {
        protected PrintStream log;
        protected String startupTime;
        protected String startupTime2;
        protected int packetsDumped;

        public PacketDumper() {
            //remember the startup time, because when packets are dumped for the first time
            //we will need to create a file whose name contains our startup time.
            startupTime = MyDateTime.now();
            startupTime2 = MyDateTime.now("yyyy-MM-dd HH_mm_ss");
            packetsDumped = 0;
        }

        protected void finalize() throws Throwable {
            if (log != null) {
                //close log stream
                log.flush();
                log.close();
            }
            super.finalize();
        }

        /**
         * Dumps a packet. Call this when a packet is invalid or the packet destination
         * cannot be found. A log event will be created and the packet will be killed.
         * @param p The packet that is invalid somehow, and thus needs to be dumped.
         * @param reason The reason why the packet needs to be dumped.
         */
        public void dumpPacket(MbPacket p, String reason) {
            if (log == null) {
                //create log output stream
                try {
                    log = new PrintStream(new FileOutputStream("dumped packets " + startupTime2 + ".log"));
                    log.println("Macbeth session started at " + startupTime + MySystem.lineBreak);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
            }
            String now = MyDateTime.now();
            log.println("--Packet dump log event-----------------------------------------------");
            log.println("[" + now + "]");
            log.println("Dump reason:");
            log.println(reason);
            log.println("Packet was:");
            log.println(p.serializeToString());
            log.println("----------------------------------------------------------------------");
            log.println();
            _info.println("A packet was dumped! (Reason: " + reason + ")");
            p = null;
            packetsDumped++;
        }
    }

}
