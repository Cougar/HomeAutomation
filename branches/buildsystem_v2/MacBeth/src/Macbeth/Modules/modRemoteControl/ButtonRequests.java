/*
 * File created by Jimmy
 * at 2003-sep-07 11:26:29
 */
package Macbeth.Modules.modRemoteControl;

import java.util.LinkedList;
import java.util.Iterator;

/**
 * Keeps track of all modules that have requested a
 * specific button.
 * @author Jimmy
 */
public class ButtonRequests {

    private LinkedList modules;

    /**
     * Creates a new instance of ButtonRequests.
     * @param modulenames A list of modules that have requested
     * this button.
     */
    public ButtonRequests(String[] modulenames) {
        modules = new LinkedList();
        if (modulenames!=null) {
            for (int i=0; i<modulenames.length; i++) {
                addRequest(modulenames[i]);
            }
        }
    }

    /**
     * Creates a new instance of ButtonRequests.
     */
    public ButtonRequests() {
        this(null);
    }

    /**
     * Adds a request for this button.
     * @param module The location of the module that requests this button.
     */
    public void addRequest(String module) {
        //if this module hasn't already requested this button
        if (!modules.contains(module)) {
            //add a request
            modules.add(module);
        }
    }

    /**
     * Returns a list of the names of all modules that have
     * requested this button.
     * @return A String array with all module names.
     */
    public String[] getModuleNames() {
        String[] names = new String[modules.size()];
        Iterator it = modules.iterator();
        int i=0;
        while (it.hasNext()) {
            names[i++] = (String)it.next();
        }
        return names;
    }

}
