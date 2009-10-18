/*
 * modPacketReceiver.java
 *
 * Created on den 30 augusti 2003, 22:00
 */
package Macbeth.Modules.modPacketReceiver;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import Macbeth.System.*;
import Macbeth.Utilities.MySystem;

/**
 * A Macbeth module that shows all incoming packets
 * in a textbox inside its graphical user interface.
 * @author Jimmy
 */
public class modPacketReceiver extends MbModule implements MbModuleGUI {
    //A GUI for our module.
    private GUI gui;
    //All packet contents delivered to this module.
    private String receivedData = "";

    /**
     * Creates a new instance of modPacketReceiver
     */
    public modPacketReceiver() {
        super();
        gui = new GUI();
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "PacketReceiver";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Shows all incoming packets in a textbox";
    }

    /**
     * This method sets default values on all _required_ data
     * fields in the data repository.
     */
    protected void initDataFields() {
        //this module doesn't require any data fields
    }

    /**
     * Starts up this module.
     */
    public void startup() throws MbStartupException {
        //start up MbModule
        super.startup();
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        //shut down MbModule
        super.shutdown();
    }

    /**
     * Will be called when a packet needs to be handled.
     * @param p The packet that needs to be handled.
     */
    public void handlePacket(MbPacket p) {
        receivedData += p.serializeToString() + MySystem.lineBreak + MySystem.lineBreak;
        gui.updateText();
    }

    /**
     * Will be called when our GUI should be shown.
     * @param drawArea The Container object onto which the
     * GUI objects should be added.
     */
    public void renderGUI(Container drawArea) {
        gui.render(drawArea);
    }

    /**
     * A class for our GUI.
     */
    private class GUI implements ActionListener {
        private JLabel      lblIncomingData;
        private JTextArea   txtIncomingData;
        private JScrollPane txtIncomingDataScrolled;
        private JButton     btnClear;

        public void render(Container renderArea) {
            lblIncomingData = new JLabel("Received data:");
            txtIncomingData = new JTextArea(6,15);
            txtIncomingData.setText(receivedData);
            txtIncomingDataScrolled = new JScrollPane(txtIncomingData);
            btnClear = new JButton("Clear history");
            btnClear.setActionCommand("clear");
            btnClear.addActionListener(this);
            renderArea.setLayout(new BorderLayout());
            renderArea.add(lblIncomingData,BorderLayout.NORTH);
            renderArea.add(txtIncomingDataScrolled,BorderLayout.CENTER);
            renderArea.add(btnClear,BorderLayout.SOUTH);
        }

        public void actionPerformed(ActionEvent e) {
            if (e.getActionCommand().equals("clear")) {
                receivedData = "";
                updateText();
            }
        }

        protected void updateText() {
            if (txtIncomingData!=null) {
                txtIncomingData.setText(receivedData);
                txtIncomingData.setCaretPosition(txtIncomingData.getDocument().getLength());
            }
        }
    }
}
