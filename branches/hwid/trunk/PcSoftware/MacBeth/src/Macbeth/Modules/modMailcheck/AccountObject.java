/*
 * File created by arune
 * at 2005-jan-09
 */
package Macbeth.Modules.modMailcheck;

import Macbeth.Utilities.UByte;
import Macbeth.Net.TcpClient;

import java.io.*;

/**
 *
 *
 *
 * @author arune
 */
public class AccountObject {

    public String server;
    public int port;
    public String accountname;
    public String password;

    private String lastMessageID = null;


    /**
     * Creates a new instance of AccountObject.
     * @param server The servername for this account.
     * @param port .
     * @param accountname .
     * @param password .
     */
    public AccountObject(String server, int  port, String accountname, String password) {
        this.server = server;
        this.port = port;
        this.accountname = accountname;
        this.password = password;
    }

     /**
     *
     * @throws Exception whenever something goes wrong
     */
    public boolean hasNewMessages() throws Exception {
        TcpClient tcpClient = new TcpClient(port, server);
        String reply;
        try {
            tcpClient.openConnection();
        } catch (Exception e) {
            throw (new Exception("POP3: Cannot connect to POP3-server at '"  + server + ":" + port + "' ! " +
                    "Exception was: '" + e + "'"));
        }

        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(connect): " + reply));
        }

        tcpClient.sendString("USER " + accountname);
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(user): " + reply));
        }

        tcpClient.sendString("PASS " + password);
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(pass): " + reply));
        }

        tcpClient.sendString("STAT");
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(stat): " + reply));
        }
        String[] replyTokens = reply.split(" ");
        int nrOfMess;
        if (replyTokens.length == 3) {
            try {
                nrOfMess = Integer.parseInt(replyTokens[1]);
            } catch (Exception e) {
                throw (new Exception("POP3(stat): " + reply));
            }
        }
        else {
            throw (new Exception("POP3(stat): " + reply));
        }
        //kolla om antalet är 0
        if (nrOfMess == 0) {
            lastMessageID = "";
            return false;
        }

        tcpClient.sendString("UIDL " + nrOfMess);
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(uidl): " + reply));
        }
        replyTokens = reply.split(" ");
        String messID;
        if (replyTokens.length == 3) {
            messID = replyTokens[2];
        }
        else {
            throw (new Exception("POP3(uidl): " + reply));
        }

        boolean returnmess = false;
        if (lastMessageID == null) {
            lastMessageID = messID;
            returnmess = false;
        } else if (lastMessageID.equals("")) {
            //there is new messages on server BUT there were non last check
            lastMessageID = messID;
            returnmess = true;
        } else if (!lastMessageID.equals(messID)) {
            //sista meddelandet är inte samma längre
            lastMessageID = messID;
            returnmess = true;
        }
        tcpClient.sendString("QUIT");
        //System.out.println(tcpClient.getRow());
        tcpClient.closeConnection();
        return returnmess;

    }
    /**
    *
    * @throws Exception whenever something goes wrong
    */
    public int nrOfMessages() throws Exception {
        TcpClient tcpClient = new TcpClient(port, server);
        String reply;
        try {
            tcpClient.openConnection();
        } catch (Exception e) {
            throw (new Exception("POP3: Cannot connect to POP3-server at '"  + server + ":" + port + "' ! " +
                    "Exception was: '" + e + "'"));
        }
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(connect): " + reply));
        }

        tcpClient.sendString("USER " + accountname);
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(user): " + reply));
        }

        tcpClient.sendString("PASS " + password);
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(pass): " + reply));
        }

        tcpClient.sendString("STAT");
        reply = tcpClient.getRow();
        //System.out.println(reply);
        //System.out.println("'" + reply.substring(0, 3) + "'");
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(stat1, " + accountname + " " + server + "): " + reply));
        }
        String[] replyTokens = reply.split(" ");
        int nrOfMess;
        if (replyTokens.length == 3) {
            try {
                nrOfMess = Integer.parseInt(replyTokens[1]);
            } catch (Exception e) {
                throw (new Exception("POP3(stat2, " + accountname + " " + server + "): " + reply));
            }
        }
        else {
            throw (new Exception("POP3(stat3, " + accountname + " " + server + "): " + reply));
        }
        //kolla om antalet är 0
        if (nrOfMess == 0) {
            lastMessageID = "";
            return 0;
        }

        tcpClient.sendString("UIDL");
        reply = tcpClient.getRow();
        //System.out.println(reply);
        if (!reply.substring(0, 3).equalsIgnoreCase("+OK")) {
            throw (new Exception("POP3(uidl): " + reply));
        }

        int nrOfNewMess = 0;
        for (int i = 1; i <= nrOfMess; i++) {
            reply = tcpClient.getRow();

            replyTokens = reply.split(" ");
            String messID;
            if (replyTokens.length == 2) {
                messID = replyTokens[1];
            }
            else {
                throw (new Exception("POP3(uidl): " + reply));
            }

            if (lastMessageID == null) {
                if (i == nrOfMess) {
                    lastMessageID = messID;
                    nrOfNewMess = 0;
                }
            } else if (lastMessageID.equals("")) {
                //there is new messages on server BUT there were non last check
                if (i == nrOfMess) {
                    lastMessageID = messID;
                    nrOfNewMess = nrOfMess;
                }
            } else if (lastMessageID.equals(messID)) {
                //found the old message
                nrOfNewMess = nrOfMess - i;
            } else if (i == nrOfMess) {
                //if all messages have been searched
                lastMessageID = messID;
                if (nrOfNewMess == 0) {
                    //and the old mess was not found
                    nrOfNewMess = nrOfMess;
                }
            }
        }
        tcpClient.sendString("QUIT");
        //System.out.println(tcpClient.getRow());
        tcpClient.closeConnection();
        return nrOfNewMess;
    }

}
