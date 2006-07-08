/*
 * File created by Jimmy
 * at 2003-aug-30 14:31:00
 */

package Macbeth.Modules.modPacketSender;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import Macbeth.System.*;
import Macbeth.Utilities.MySystem;

/**
 * A Macbeth module that allows the user to write and send
 * custom packets from a graphical user interface.
 * @author Jimmy
 */
public class modPacketSender extends MbModule implements MbModuleGUI {
    private GUI gui;

    /**
     * Creates a new instance of modPacketSender.
     */
    public modPacketSender() {
        super();
        gui = new GUI();
    }

    /**
     * Gets the name of the component.
     * @return The name of the component.
     */
    public String name() {
        return "PacketSender";
    }

    /**
     * Gets the description of the component.
     * @return The description of the component.
     */
    public String description() {
        return "Allows you to send custom packets";
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
        super.startup();
    }

    /**
     * Shuts down this module.
     */
    public void shutdown() {
        super.shutdown();
    }

    /**
     * Will be called when a packet needs to be handled.
     * @param p The packet that needs to be handled.
     */
    public void handlePacket(MbPacket p) {
        //we don't care about incoming packets
    }

    /**
     * Will be called when our GUI should be shown.
     */
    public void renderGUI(Container drawArea) {
        gui.render(drawArea);
    }

    /**
     * A class for our GUI.
     */
    private class GUI implements ActionListener {
        private JLabel      lblDestinationKernel;
        private JLabel      lblDestinationModule;
        private JLabel      lblContents;
        private JTextField  txtDestinationKernel;
        private JTextField  txtDestinationModule;
        private JTextArea   txtContents;
        private JScrollPane txtContentsScrolled;
        private JButton     btnSend;

        public void render(Container drawArea) {
            lblDestinationKernel = new JLabel("Destination kernel:");
            lblDestinationModule = new JLabel("Destination module:");
            lblContents = new JLabel("Packet contents (XML):");
            txtDestinationKernel = new JTextField(15);
            txtDestinationKernel.setText("self");
            txtDestinationModule = new JTextField(15);
            txtDestinationModule.setText("PacketReceiver");
            txtContents = new JTextArea(5,15);
            txtContents.setText("<test>" + MySystem.lineBreak + "XML-data only!" + MySystem.lineBreak + "</test>" + MySystem.lineBreak + "<wee attribute=\"value\"/>");
            txtContentsScrolled = new JScrollPane(txtContents);
            btnSend = new JButton("Send");
            btnSend.setActionCommand("send");
            btnSend.addActionListener(this);

            GridBagConstraints c = new GridBagConstraints();
            drawArea.setLayout(new GridBagLayout());
            c.ipadx = 2;
            c.ipady = 2;
            c.weightx = 1;
            c.weighty = 0;
            c.fill = GridBagConstraints.HORIZONTAL;
            c.gridy = 0;
            drawArea.add(lblDestinationKernel,c);
            c.gridy = 1;
            drawArea.add(txtDestinationKernel,c);
            c.gridy = 2;
            drawArea.add(lblDestinationModule,c);
            c.gridy = 3;
            drawArea.add(txtDestinationModule,c);
            c.gridy = 4;
            drawArea.add(lblContents,c);
            c.fill = GridBagConstraints.BOTH;
            c.gridy = 5;
            c.weighty = 1;
            drawArea.add(txtContentsScrolled,c);
            c.fill = GridBagConstraints.HORIZONTAL;
            c.gridy = 6;
            c.weighty = 0;
            drawArea.add(btnSend,c);
        }

        public void actionPerformed(ActionEvent e) {
            if (e.getActionCommand().equals("send")) {
                if (!txtDestinationKernel.getText().equals("") && !txtDestinationModule.getText().equals("") && !txtContents.getText().equals("")) {
                    MbPacket p = new MbPacket();
                    p.setDestination(new MbLocation(txtDestinationKernel.getText(), txtDestinationModule.getText()));
                    p.setContents(txtContents.getText());
                    sendPacket(p);
                }
            }
        }
    }
}
