/**
 * File created by Jimmy
 * at 2003-dec-31 17:48:26
 */
package Macbeth.Utilities;

import javax.swing.*;
import java.awt.*;

/**
 * Puts a title onto another control.
 * @author Jimmy
 */
public class TitledControl extends JPanel {

    private Component component;
    private TitleBar titleBar;

    /**
     * Creates a new instance of TitledControl.
     * @param component The component that you want to put a title on.
     * @param title The title.
     */
    public TitledControl(Component component, String title) {
        this.component = component;
        titleBar = new TitleBar(title);
        setLayout(new BorderLayout());
        add(titleBar,BorderLayout.NORTH);
        add(component,BorderLayout.CENTER);
    }
}
