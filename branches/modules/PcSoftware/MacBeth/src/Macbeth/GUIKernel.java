/**
 * File created by Jimmy
 * at 2003-dec-23 21:43:41
 */
package Macbeth;

import Macbeth.System.*;
import Macbeth.Utilities.*;
import com.jgoodies.plaf.windows.ExtWindowsLookAndFeel;
import com.jgoodies.plaf.plastic.PlasticXPLookAndFeel;
import com.jgoodies.plaf.plastic.Plastic3DLookAndFeel;
import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.util.HashMap;
import java.util.Iterator;
import java.io.IOException;
import java.net.URL;

/**
 * A full featured kernel with graphical user interface.
 * @author Jimmy
 */
public class GUIKernel extends MbKernel {

    //a list of references to all loaded module GUI windows
    private HashMap loadedModuleGUIs;
    //the module whose info page we're currently viewing
    private MbModule selectedModule;

    /**
     * Creates a new instance of GUIKernel.
     * @param configFile The XML-configuration file to use.
     */
    public GUIKernel(String configFile) {
        super(configFile);
        selectedModule = null;
        //create GUI
        try {
            _debug.println("Loading Swing look and feel");
            //UIManager.setLookAndFeel(new ExtWindowsLookAndFeel());
            UIManager.setLookAndFeel(new Plastic3DLookAndFeel());
        } catch (Exception e) {}
        _debug.println("Creating Swing GUI");
        GUI g = new GUI();
        g.setVisible(true);
        try {
            startup();
        } catch (MbStartupException e) {
            _errors.println("Failed to start up kernel! (message: '" + e.getMessage() + "')");
            shutdown();
        }
        g.updateHTMLBar();
    }

    /**
     * Gets a short description of the kernel.
     * @return A short description of the kernel.
     */
    public String description() {
        return "Advanced kernel with graphical user interface";
    }

    /**
     * Starts up this kernel.
     */
    public void startup() throws MbStartupException {
        //startup MbKernel
        super.startup();
        loadedModuleGUIs = new HashMap(32);
    }

    /**
     * Shuts down this kernel.
     */
    public void shutdown() {
        //shut down MbKernel
        super.shutdown();
    }

    /**
     * Finds out whether or not a module has a graphical user interface.
     * @param moduleName The classname of the module.
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
     * Gets the HTML-page from a specified module.
     * @param moduleName The classname of the module whose HTML-page you want to get.
     * @return Full HTML-code for this modules HTML-page. If the specified module
     * doesn't have a HTML-interface, its description is returned instead.
     */
    public String getHTMLPage(String moduleName) {
        //get the module
        MbModule mod = (MbModule)loadedModules.get(moduleName);
        //get its html-interface
        MbHTMLInterface html = mod.getHTMLInterface();
        //if this module has a HTML-interface
        if (html!=null) {
            //then return the HTML-page
            return html.getHTMLPage();
            //else, if this module doesn't have a html-interface
        } else {
            //return description instead
            return mod.description();
        }
    }

    /**
     * The whole graphical user interface for this kernel.
     */
    private class GUI extends JFrame implements ActionListener,WindowListener {
        //private JSplitPane contentArea;
        private JEditorPane htmlBar;
        private JScrollPane htmlBarScrolled;
        private JTabbedPane tabbedArea;
        private JEditorPane tabInfo;
        private JScrollPane tabInfoScrolled;
        private MDIDesktopPane tabGUI;
        private JScrollPane tabGUIScrolled;
        //private JTextArea console;
        private JEditorPane console;
        //private MbHTMLDocument consoleDocument;
        private JScrollPane consoleScrolled;
        private JMenuBar menuBar;

        public GUI() {
            setTitle("Macbeth GUI kernel");

        //create html bar
            htmlBar = new JEditorPane();
            htmlBar.setEditable(false);
            htmlBar.setBorder(BorderFactory.createEmptyBorder());
            htmlBar.setContentType("text/html");
            htmlBar.addHyperlinkListener(new HyperlinkActionsHTMLBar());
            htmlBarScrolled = new JScrollPane(htmlBar);
            htmlBarScrolled.setMinimumSize(new Dimension(100,100));
            htmlBarScrolled.setBorder(BorderFactory.createEmptyBorder());

        //create info tab
            tabInfo = new JEditorPane();
            tabInfo.setBorder(BorderFactory.createEmptyBorder());
            tabInfo.setContentType("text/html");
            tabInfo.addHyperlinkListener(new HyperlinkActionsInfoPage());
            tabInfo.setText("<html><h1>Macbeth!</h1></html>");
            tabInfo.setEditable(false);
            tabInfoScrolled = new JScrollPane(tabInfo);
            tabInfoScrolled.setBorder(BorderFactory.createEmptyBorder());

        //create gui tab
            tabGUI = new MDIDesktopPane();
            tabGUI.setBorder(BorderFactory.createEmptyBorder());
            tabGUIScrolled = new JScrollPane(tabGUI);
            tabGUIScrolled.setBorder(BorderFactory.createEmptyBorder());

        //create tabbed view
            tabbedArea = new JTabbedPane();
            tabbedArea.addTab("Info Page", null, tabInfoScrolled, "Module information");
            tabbedArea.addTab("GUI Desktop", null, tabGUIScrolled, "Module GUI desktop");
            tabbedArea.setPreferredSize(new Dimension(600,500));
            tabbedArea.setMinimumSize(new Dimension(100,100));
            tabbedArea.setBorder(BorderFactory.createEmptyBorder());

        //create console area
            console = new JEditorPane();
            console.setEditable(false);
            console.setContentType("text/html");
            //console = new JTextArea();
            //console.setMargin(new Insets(5,5,5,5));
            console.setBorder(BorderFactory.createEmptyBorder());
            consoleScrolled = new JScrollPane(console);
            consoleScrolled.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
            consoleScrolled.setPreferredSize(new Dimension(600,80));
            consoleScrolled.setMinimumSize(new Dimension(200,50));
            consoleScrolled.setBorder(BorderFactory.createEmptyBorder());
            consoleScrolled.addComponentListener(
                    new ComponentListener(){
                        //when console is resized, make sure it scrolls down to bottom
                        public void componentResized(ComponentEvent e) {
                            //hmm....ugly hack, but it doesn't seem to work without the 0-position
                            console.setCaretPosition(0);
                            console.setCaretPosition(console.getDocument().getLength());
                        }
                        public void componentMoved(ComponentEvent e) {}
                        public void componentShown(ComponentEvent e) {}
                        public void componentHidden(ComponentEvent e) {}
                    });

            _errors = new TextFormatterStream(new EditorPaneStream(console));
            ((TextFormatterStream)_errors).setPreString("<font size=\"-1\" color=\"red\">");
            ((TextFormatterStream)_errors).setPostString("</font>");
            _info = new TextFormatterStream(new EditorPaneStream(console));
            ((TextFormatterStream)_info).setPreString("<font size=\"-1\" color=\"black\">");
            ((TextFormatterStream)_info).setPostString("</font>");
            _debug = new TextFormatterStream(new EditorPaneStream(console));
            ((TextFormatterStream)_debug).setPreString("<font size=\"-1\" color=\"#777777\">");
            ((TextFormatterStream)_debug).setPostString("</font>");

        //create menu bar
            menuBar = new JMenuBar();
            setJMenuBar(menuBar);
            JMenu menu;
            JMenuItem menuItem;
            menu = new JMenu("System");
            menu.getAccessibleContext().setAccessibleDescription("System commands");
            menuBar.add(menu);
            menuItem = new JMenuItem("Restart modules");
            menuItem.getAccessibleContext().setAccessibleDescription("Restart all modules");
            menu.add(menuItem);
            menuItem = new JMenuItem("Restart kernel");
            menu.add(menuItem);
            menu.addSeparator();
            menuItem = new JMenuItem("Quit");
            menu.add(menuItem);

        //create a split pane that holds the HTML bar to the left and the tabview to the right
            JSplitPane splitHorizontal = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,new TitledControl(htmlBarScrolled,"Macbeth menu"),tabbedArea);
            splitHorizontal.setDividerLocation(130);
            splitHorizontal.setDividerSize(6);
            //tabbedArea will get all extra space when resizing
            splitHorizontal.setResizeWeight(0.0);
            //JWindowPane splitHorizontal = new JWindowPane(JWindowPane.HORIZONTAL,"Macbeth components",htmlBarScrolled,"Content",tabbedArea,null);

        //create a split pane that holds the the content at top and the console at bottom
            JSplitPane splitVertical = new JSplitPane(JSplitPane.VERTICAL_SPLIT,splitHorizontal,new TitledControl(consoleScrolled,"Console"));
            splitVertical.setResizeWeight(0.8);
            //JWindowPane splitVertical = new JWindowPane(JWindowPane.VERTICAL,"top",splitHorizontal,"Console",consoleScrolled,null);

        //add everything to main frame
            Container content = this.getContentPane();
            content.setLayout(new BorderLayout());
            content.add(splitVertical,BorderLayout.CENTER);
            pack();
            setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
            addWindowListener(this);
        }

        /* EVENT HANDLERS... */
        public void actionPerformed(ActionEvent e) {}
        public void windowClosing(WindowEvent e) {
            //when kernel GUI is closed, it's time to shut down
            shutdown();
            setVisible(false);
            _errors = System.err;
            _info = System.out;
            _debug = System.out;
            _debug.println("GUI has been closed");
            _debug.println("Waiting for threads to stop");
            dispose();
        }
        public void windowOpened(WindowEvent e) {}
        public void windowClosed(WindowEvent e) {}
        public void windowIconified(WindowEvent e) {}
        public void windowDeiconified(WindowEvent e) {}
        public void windowActivated(WindowEvent e) {}
        public void windowDeactivated(WindowEvent e) {}

        /**
         * Responds to hyperlink clicks in the left HTML bar.
         */
        class HyperlinkActionsHTMLBar implements HyperlinkListener {
            public void hyperlinkUpdate(HyperlinkEvent e) {
                if (e.getEventType()==HyperlinkEvent.EventType.ACTIVATED) {
                    URL url = e.getURL();
                    String description = e.getDescription();
                    //if a valid URL exists for the link
                    if (url!=null) {
                        //load the page in the main browser
                        try {
                            tabInfo.setPage(url);
                            selectedModule = null;
                        } catch (IOException e1) {
                            e1.printStackTrace();
                        }
                    }
                    //else, use the description to find out what to do
                    else {
                        //interprete the description as an internal macbeth command
                        String[] cmd = description.split(":");
                        if (cmd.length<=0) {
                            return;
                        }
                        //shows information on the specified macbeth component
                        if (cmd[0].equalsIgnoreCase("ShowInfo")) {
                            tabInfo.setText(getHTMLPage(cmd[1]));
                            selectedModule = (MbModule)loadedModules.get(cmd[1]);
                        }
                        //adds a module GUI to the GUI desktop
                        else if (cmd[0].equalsIgnoreCase("ShowGUI")) {
                            loadModuleGUI(cmd[1]);
                        }
                        //unloads a module
                        else if (cmd[0].equalsIgnoreCase("Unload")) {
                            unloadModule(cmd[1]);
                            updateHTMLBar();
                        }
                        //loads an unloaded module
                        else if (cmd[0].equalsIgnoreCase("Load")) {
                            try {
                                loadModule(cmd[1]);
                            } catch (Exception el) {
                                _errors.println("Error loading module: " + el);
                            }
                            updateHTMLBar();
                        }
                    }

                }
            }
        }

        /**
         * Responds to hyperlink clicks on module info page.
         */
        class HyperlinkActionsInfoPage implements HyperlinkListener {
            public void hyperlinkUpdate(HyperlinkEvent e) {
                if (e.getEventType()==HyperlinkEvent.EventType.ACTIVATED) {
                    URL url = e.getURL();
                    String description = e.getDescription();
                    //if a valid URL exists for the link
                    if (url!=null) {
                        //load the page in the main browser
                        try {
                            tabInfo.setPage(url);
                        } catch (IOException e1) {
                            e1.printStackTrace();
                        }
                    }
                    //else, notify the current module about the click
                    else {
                        if (selectedModule!=null) {
                            MbHTMLInterface html = selectedModule.getHTMLInterface();
                            if (html!=null) {
                                html.linkPressed(description);
                            }
                        }
                    }

                }
            }
        }

        public void updateHTMLBar() {
            String html = "<html>" + MySystem.lineBreak;
            html += "<body><b>Loaded modules</b><p>" + MySystem.lineBreak;
            //iterate through loaded modules-list
            Iterator it = loadedModules.keySet().iterator();
            while (it.hasNext()) {
                String modname = (String)it.next();
                html += "<b>" + modname + "</b><br>" + MySystem.lineBreak;
                html += " - [<a href=\"ShowInfo:" + modname + "\">Show Info</a>]<br>" + MySystem.lineBreak;
                if (moduleHasGUI(modname)) {
                    html += " - [<a href=\"ShowGUI:" + modname + "\">Load GUI</a>]<br>" + MySystem.lineBreak;
                }
                html += " - [<a href=\"Unload:" + modname + "\">Unload</a>]<br>" + MySystem.lineBreak;
            }
            html += "</p>" + MySystem.lineBreak;
            html += "<b>Unloaded modules</b><p>";
            //iterate through module list to find unloaded modules
            it = moduleList.keySet().iterator();
            while (it.hasNext()) {
                String modname = (String)it.next();
                //only show unloaded modules
                if (!loadedModules.containsKey(modname)) {
                    html += "<b>" + modname + "</b><br>" + MySystem.lineBreak;
                    html += " - [<a href=\"Load:" + modname + "\">Load</a>]<br>" + MySystem.lineBreak;
                }
            }
            html += "</p>" + MySystem.lineBreak;
            html += "</body></html>";
            htmlBar.setText(html);
        }


        /**
         * Loads the GUI for the specified module.
         * @param moduleName The module whose GUI should be loaded.
         */
        public void loadModuleGUI(String moduleName) {
            //if a GUI is already loaded for this module
            if (loadedModuleGUIs.containsKey(moduleName)) {
                //then bring this GUI to the front
                ModuleGUIWindow w = (ModuleGUIWindow)loadedModuleGUIs.get(moduleName);
                w.moveToFront();
                return;
            }
            //else, if this module does have a GUI
            else if (moduleHasGUI(moduleName)) {
                //create a new window for the GUI
                MbModuleGUI i = (MbModuleGUI)loadedModules.get(moduleName);
                ModuleGUIWindow w = new ModuleGUIWindow(moduleName);
                //and render GUI into this window
                i.renderGUI(w.getContentPane());
                w.setTitle(moduleName);
                //pack the contents to minimum possible size and show frame
                w.pack();
                w.setVisible(true);
                tabGUI.add(w);
            } else {
                //the module doesn't have any GUI
            }
        }

        /**
         * A window for a module GUI. This window will be shown
         * on the GUI desktop located inside the tabview.
         */
        private class ModuleGUIWindow extends JInternalFrame implements InternalFrameListener {
            private String moduleName;

            /**
             * Creates a new GUI window.
             * @param moduleName The name of the module.
             */
            public ModuleGUIWindow(String moduleName) {
                super();
                this.moduleName = moduleName;
                setDefaultCloseOperation(DISPOSE_ON_CLOSE);
                setResizable(true);
                setClosable(true);
                setMaximizable(false);
                addInternalFrameListener(this);
            }

            public void internalFrameOpened(InternalFrameEvent e) {
                //put a reference to this window into the hashmap
                loadedModuleGUIs.put(moduleName,this);
            }

            public void internalFrameClosed(InternalFrameEvent e) {
                //remove our reference from the hashmap
                loadedModuleGUIs.remove(moduleName);
            }

            public void internalFrameClosing(InternalFrameEvent e) {}
            public void internalFrameIconified(InternalFrameEvent e) {}
            public void internalFrameDeiconified(InternalFrameEvent e) {}
            public void internalFrameActivated(InternalFrameEvent e) {}
            public void internalFrameDeactivated(InternalFrameEvent e) {}
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
        new GUIKernel(configFile);
    }

}
