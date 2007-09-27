/*
 * CmdKernel.java
 *
 * Created on den 23 augusti 2003, 13:47
 */
package Macbeth;

import java.io.*;
import java.util.*;
import java.awt.event.WindowListener;
import java.awt.event.WindowEvent;
import javax.swing.*;
import Macbeth.System.*;
//import Macbeth.Utilities.*;
import com.jgoodies.plaf.plastic.Plastic3DLookAndFeel;

/**
 * A custon kernel that provides command prompt, counts
 * number of handled packets, and also keeps
 * track of kernel uptime.
 * @author Jimmy
 */
public class CmdKernel extends MbKernel {
    //Remembers at what time (in milliseconds) the kernel was started (needed for uptime calculation)
    private long startTime;
    //references to all launched module GUIs are stored in this hashmap
    private Map loadedModuleGUIs;

    /**
     * Creates a new instance of CmdKernel.
     * @param configfile The XML configuration file for this kernel.
     */
    public CmdKernel(String configfile) {
        super(configfile);
    }

    /**
     * Gets a short description of the kernel. This should
     * be kept short (1-2 lines) and should be formatted as
     * plain text.
     * @return A short description of the kernel.
     */
    public String description() {
        return "Basic command-line kernel";
    }

    /**
     * Creates a new instance of CmdKernel.
     * Default configuration file name will be used.
     */
    public CmdKernel() {
        super("Config\\Kernel.xml");
    }

    /**
     * Starts up this kernel.
     */
    public void startup() throws MbStartupException {
        //startup MbKernel
        super.startup();
        try {
            _debug.println("Loading Swing look and feel (for module GUIs)");
            //UIManager.setLookAndFeel(new ExtWindowsLookAndFeel());
            UIManager.setLookAndFeel(new Plastic3DLookAndFeel());
        } catch (Exception e) {
        }
        startTime = System.currentTimeMillis();
        /*our hashmap must be synchronized since module GUIs, which are
          JFrames, and thus separate threads, will access it at the same
          time as we do*/
        loadedModuleGUIs = Collections.synchronizedMap(new HashMap(32));
    }

    /**
     * Shuts down this kernel.
     */
    public void shutdown() {
        //must synchronize iterator
        synchronized(loadedModuleGUIs) {
            //kill all opened module GUI windows
            Iterator it = loadedModuleGUIs.values().iterator();
            while (it.hasNext()) {
                ModuleGUIWindow w = (ModuleGUIWindow)it.next();
                w.dispose();
            }
        }
        //shut down MbKernel
        super.shutdown();
    }

    /**
     * Gets the kernel uptime.
     * @return Uptime in seconds
     */
    private int getUptime() {
        //uptime in msec
        long uptime = System.currentTimeMillis()-startTime;
        //in secs
        float uptimeSecs = uptime/1000F;
        return (int)(uptimeSecs+0.5);
    }

    /**
     * Finds out whether or not a module has a graphical user interface.
     * @param moduleName The name of the module class.
     * @return True if this module has a GUI, false otherwise (or if module wasn't found).
     */
    public boolean moduleHasGUI(String moduleName) {
        //get the module
        MbModule m = (MbModule)loadedModules.get(moduleName);
        if (m!=null) {
            //get its interfaces
            Class[] interfaces = m.getClass().getInterfaces();
            //check whether the module implements the MbModuleGUI interface
            for (int i=0; i<interfaces.length; i++) {
                if (interfaces[i].getName().equals("Macbeth.System.MbModuleGUI")) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Loads the GUI for the specified module.
     * @param moduleName The classname of the module whose GUI should be loaded.
     */
    public void loadModuleGUI(String moduleName) {
        //if a GUI is already launched for this module
        if (loadedModuleGUIs.containsKey(moduleName)) {
            //do nothing
            _info.println("This GUI is already loaded!");
            return;
        //else, if this module does have a GUI
        } else if (moduleHasGUI(moduleName)) {
            //TODO: maybe this window needs to be saved in a list so it can be shut down when module is unloaded?
            //create a new window for the GUI
            MbModuleGUI i = (MbModuleGUI)loadedModules.get(moduleName);
            ModuleGUIWindow w = new ModuleGUIWindow(moduleName);
            //and render GUI into this window
            i.renderGUI(w.getContentPane());
            w.setTitle(moduleName);
            w.pack();
            w.setVisible(true);
        } else {
            _info.println("Module '" + moduleName + "' doesn't have a GUI!");
        }
    }

    /**
     * Starts a command prompt, and returns when user types one of the exit commands.
     */
    private void cmdPrompt() {
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
        String cmd = "";
        String args[] = new String[0];
        while (true) {
            System.out.println("[" + name() + ":" + internetTransport.ourPort + "]");
            //try to get command line input
            try {
                cmd = stdin.readLine();
            } catch (IOException ioError) {
                //don't care
            }
            //now, split cmd into args
            args = cmd.split(" ");
            //don't execute the following code if cmd is empty
            if (args.length<=0 || cmd.equals("")) {
                continue;
            }
            if (args[0].equalsIgnoreCase("exit") || args[0].equalsIgnoreCase("quit") || args[0].equalsIgnoreCase("q")) {
                //these commands shut down the kernel
                System.out.println("Quitting...");
                break;
            }
            else if (args[0].equalsIgnoreCase("info")) {
                //this command shows kernel info
                System.out.println("Kernel information:");
                System.out.println("  Name: " + name());
                System.out.println("  Internet server at port: " + internetTransport.ourPort);
                //uptime info
                int uptime = getUptime();
                int seconds = uptime % 60;
                int minutes = (uptime / 60) % 60;
                int hours = (uptime / 3600) % 24;
                int days = uptime / 3600 / 24;
                System.out.println("  Uptime: " + days + " days, " + hours + " hours, " + minutes + " minutes, " + seconds + " secs");
                //transport info
                System.out.println("  Packet transport systems:");
                int packetsSentTotal = 0;
                Iterator it = packetTransports.iterator();
                while (it.hasNext()) {
                    MbPacketTransport pt = (MbPacketTransport)it.next();
                    if (pt!=null) {
                        System.out.println("   - " + pt.getName());
                        System.out.println("     Packets sent: " + pt.packetsSent);
                        System.out.println("     Packets received: " + pt.packetsReceived);
                        packetsSentTotal += pt.packetsSent;
                    }
                }
                //packet info
                System.out.println("  Avarage kernel workload: " + (float)packetsSentTotal/(float)uptime + " packets/sec (a packet every " + (float)uptime/(float)packetsSentTotal + " sec)");
                //queue size
                System.out.println("  Current packet queue size: " + packetQueue.getSize() + " packets");
            }
            else if (args[0].equalsIgnoreCase("kernels")) {
                //this command shows a list of other known kernels
                System.out.println("Listing known kernels (edit your config-file to add more):");
                String[] kernels = internetTransport.kernelList.getNameList();
                for (int i=0; i<kernels.length; i++) {
                    System.out.println("  name: " + kernels[i] + ", hostname: " + internetTransport.kernelList.getAddress(kernels[i]).getHostname() + ", port: " + internetTransport.kernelList.getAddress(kernels[i]).getPort());
                }
            }
            else if (args[0].equalsIgnoreCase("modules")) {
                //this command lists loaded modules
                System.out.println("Listing loaded modules:");
                Iterator it = loadedModules.keySet().iterator();
                while (it.hasNext()) {
                    String moduleName = (String) it.next();
                    MbModule m = (MbModule) loadedModules.get(moduleName);
                    System.out.println("  - '" + moduleName + "'" + (moduleHasGUI(moduleName) ? " [has GUI!]" : ""));
                    System.out.println("      description: " + m.description());
                }
            }
            else if (args[0].equalsIgnoreCase("modulelist")) {
                //this command lists loaded modules
                System.out.println("Complete module list (not all are loaded!):");
                Iterator it = moduleList.keySet().iterator();
                while (it.hasNext()) {
                    String moduleName = (String)it.next();
                    MbModule m = (MbModule)loadedModules.get(moduleName);
                    System.out.println("  - '" + moduleName + "'" + (m!=null ? " [Loaded]" : ""));
                }
            }
            else if (args[0].equalsIgnoreCase("gui")) {
                //this command shows or hides a modules gui
                if (args.length >= 2) {
                    System.out.println("Loading new GUI for '" + args[1] + "'...");
                    MbModule m = (MbModule)loadedModules.get(args[1]);
                    if (m != null) {
                        loadModuleGUI(args[1]);
                    } else {
                        System.out.println("Module '" + args[1] + "' is not loaded!");
                    }
                } else {
                    System.out.println("Error in command line: To few parameters");
                }
            }
            else if (args[0].equalsIgnoreCase("load")) {
                //this command starts up a specified module
                if (args.length >= 2) {
                    try {
                        if (!loadModule(args[1])) {
                            System.out.println("Module '" + args[1] + "' is already loaded!");
                        }
                    } catch (MbModuleNotFoundException e) {
                        System.out.println("Module '" + args[1] + "' could not be found!");
                    } catch (MbModuleLoadException e) {
                        _errors.println("'" + args[1] + "' failed to load! (" + e + ")");
                    }
                } else {
                    System.out.println("Error in command line: To few parameters");
                }
            }
            else if (args[0].equalsIgnoreCase("unload")) {
                //this command shuts down a specified module
                if (args.length >= 2) {
                    if (!unloadModule(args[1])) {
                        System.out.println("Module '" + args[1] + "' is not loaded!");
                    }
                }
                else {
                    System.out.println("Error in command line: To few parameters");
                }
            }
            else if (args[0].equalsIgnoreCase("data")) {
                //this command prints all contents of the data repository
                if (args.length >= 2) {
                    System.out.println("Listing contents of the '" + args[1] + "' data reposotory:");
                    MbModule m = (MbModule) loadedModules.get(args[1]);
                    if (m != null) {
                        System.out.println(m.getDataRepository().listContents());
                    } else {
                        System.out.println("Module '" + args[1] + "' not found!");
                    }
                }
                else {
                    System.out.println("Listing contents of the kernel data repository:");
                    System.out.println(dataRepository.listContents());
                }
            }
            else if (args[0].equalsIgnoreCase("do")) {
                //this command tells the kernel or a module to perform an action
                if (args.length >= 2) {
                    try {
                        String name = args[1];
                        String params = (args.length >= 3 ? args[2] : null);
                        this.performAction(new MbAction(name, params));
                    } catch (MbActionNotPerformedException e) {
                        System.out.println("Action could not be performed! (" + e.getMessage() + ")");
                    }
                } else {
                    System.out.println("Not enough parameters");
                }
            }
            else if (args[0].equalsIgnoreCase("help")) {
                System.out.println("Available Commands:");
                System.out.println("  exit/quit/q - shut down kernel");
                System.out.println("  help - shows this help page");
                System.out.println("  info - shows some info about this kernel");
                System.out.println("  kernels - shows list of other known kernels");
                System.out.println("  modules - shows a list of loaded modules");
                System.out.println("  modulelist - shows a list of installed modules");
                System.out.println("  load <modulename> - loads and starts up a specified module");
                System.out.println("  unload <modulename> - unloads and shuts down a specified module");
                System.out.println("  gui <modulename> - loads the specified module's GUI");
                System.out.println("  data - lists the contents of the kernel data repository");
                System.out.println("  data <modulename> - lists the contents of a modules data repository");
            }
            else {
                System.out.println("Unknown command. Type \"help\" for a list of commands");
            }
        }
    }

    /**
     * Program entry point.
     * @param args The command line arguments.
     */
    public static void main(String[] args) {
        String configFile = "Config\\Kernel.xml";
        //config file can also be specified with command line args
        if (args.length>0) {
            configFile = args[0];
        }
        //create new kernel
        CmdKernel k = new CmdKernel(configFile);
        try {
            //try to start up kernel
            k.startup();
        } catch (MbStartupException e) {
            System.err.println("Failed to start up kernel! (message: '" + e.getMessage() + "')");
            k.shutdown();
            System.exit(1);
        }
        //start kernel command prompt
        k.cmdPrompt();
        //when command prompt returns, it's time to shut down
        k.shutdown();
    }

    /**
     * A standalone window for a module GUI.
     */
    private class ModuleGUIWindow extends JFrame implements WindowListener {
        private String moduleName;

        /**
         * Launches a new GUI window.
         * @param moduleName The name of the module.
         */
        public ModuleGUIWindow(String moduleName) {
            super();
            this.moduleName = moduleName;
            setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            addWindowListener(this);
        }

        public void windowOpened(WindowEvent e) {
            //put a reference to this window into the hashmap
            loadedModuleGUIs.put(moduleName,this);
        }

        public void windowClosed(WindowEvent e) {
            //remove our reference from the hashmap
            loadedModuleGUIs.remove(moduleName);
        }

        public void windowClosing(WindowEvent e) {}
        public void windowIconified(WindowEvent e) {}
        public void windowDeiconified(WindowEvent e) {}
        public void windowActivated(WindowEvent e) {}
        public void windowDeactivated(WindowEvent e) {}
    }
}
