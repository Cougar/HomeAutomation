/*
 * MbModuleGUI.java
 *
 * Created on den 30 augusti 2003, 19:00
 */
package Macbeth.System;

import java.awt.*;

/**
 * Implement this interface if your macbeth module has
 * a custom GUI. The GUI should be rendered onto the
 * Container given by the kernel. Note that there is no
 * guarantee that the gui will ever be shown at all.
 * This is completely up to the kernel to decide.
 * @author Jimmy
 */
public interface MbModuleGUI {

    /**
     * Will be called when the module needs to render its
     * custom GUI. Note that you should only use Swing-
     * objects when creating the GUI.
     * @param drawArea The Container object on which the GUI objects should be added.
     */
    public void renderGUI(Container drawArea);

}