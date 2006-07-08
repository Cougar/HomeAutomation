/**
 * File created by Jimmy
 * at 2003-dec-31 17:44:01
 */
package Macbeth.Utilities;

import javax.swing.*;
import java.awt.*;

/**
 * A title bar.
 * @author Jimmy
 */
public class TitleBar extends JPanel {

    protected JLabel titleLabel;

    public TitleBar(String title) {
        setLayout(new BorderLayout());
        setBackground(UIManager.getColor("InternalFrame.activeTitleBackground"));
        setBorder(BorderFactory.createEmptyBorder());
        setOpaque(true);
        titleLabel = new JLabel(title);
        titleLabel.setBorder(BorderFactory.createEmptyBorder(3,4,3,4));
        add(BorderLayout.CENTER, titleLabel);
    }

    public void setTitle(String title) {
        titleLabel.setText(title);
    }
}
