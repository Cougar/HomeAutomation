/*
 * File created by arune
 * at 2004-jan-06 13:30:00
 */
package Macbeth.Modules.modSmallDisplays;

import Macbeth.System.MbPacket;

import java.util.Vector;


/**
 * A representation of one of the displays on the arne-network,
 * it also handles everything that can be done with it.
 * @author arune
 */
public class DisplayObject {

    //todo: vore trevligt ifall displayen scrollade text om den var längre än displayens bredd (men max 40 tecken)
    //ska detta göras i macbeth? helst inte, se om man kan få scroll i avren
    //om det går i avren så får man i displaymodulen fixa så man inte klipper ner rader till displayens bredd
    private final int ARNEMAXPACKETSIZE = 15;
    // place what type of display this is, location ...
    private String sInfo;
    // arne name for this display
    private String sTarget;
    // The number of columns of the display
    private int iWidth;
    // The number of rows of the display
    private int iHeight;
    // Holds the to-be contents of the display, sNewContents[0] is top-row, sNewContents[iHeight-1] is bottom-row
    private String[] sNewContents;
    // Holds the CURRENT contents of the display
    private String[] sCurrContents;

    private static final int LCDSETADDRESS = 128;
    private static final int LCDTEXTWRITE = 129;
    private static final int LCDNULL = 160;
    private static final int LCDCLEAR = 130;
    private static final int LCDSETBL = 164;
    private static final int LCDFADEBL = 165;
    private static final int LCDFADESPEED = 166;
    private static final int LCDCGRAMWRITE = 166;

    //queue-vectors
    private Vector rowPrio0 = new Vector();
    private Vector rowPrio1 = new Vector();
    private Vector rowPrio2 = new Vector();
    //queue-objects gets moved into this vector when placed on display
    private Vector rowsCurrent = new Vector();

    private Vector freeRows = new Vector();
    //private Vector takenRows = new Vector();

    //stores an addresslayout of the display
    private Vector rowAddresses = new Vector();
    //stores a list of what modules that sends data to this display
    private Vector moduleUsers = new Vector();

    /**
     * Creates a new instance of DisplayObject.
     * @param target arne name for this display.
     * @param info The name of this object, what type of display this is, location, and so on.
     * @param width The number of columns of the display.
     * @param height The number of rows of the display.
     * @param rows A list of first-position-addresses.
     * @param users A list of modules that sends data to this display.
     */
    public DisplayObject(String target, String info, int width, int height, Vector rows, Vector users) {
        sInfo = info;
        sTarget = target;
        iWidth = width;
        iHeight = height;
        rowAddresses = rows;
        moduleUsers = users;

        //initiate our contentsholders
        if (iHeight > 0) {
            sNewContents = new String[iHeight];
            sCurrContents = new String[iHeight];
            for (int i = 0; i < iHeight; i++) {
                //kor rowFixer for att fixa radernas längd direkt
                sNewContents[i] = rowFixer("");
                sCurrContents[i] = rowFixer("............................................");

            }
        }
        //todo: initiera rowsCurrent

        for (int i = 0; i < iHeight; i++) {
            RowObject dummyRowObject = new RowObject();
            dummyRowObject.rowNumber = i;
            dummyRowObject.TTL = 0;
            //System.out.println("init rowelement: " + i);
            if (i == 0) {
                dummyRowObject.row = "-Macbeth-";
                writeRow(dummyRowObject.rowNumber, dummyRowObject.row);
            } else if (i==1) {
                dummyRowObject.row = "Booting up";
                writeRow(dummyRowObject.rowNumber, dummyRowObject.row);
            }
            rowsCurrent.add(dummyRowObject);
            //... ???


        }

    }

    /**
     *
     * @return the number of rows on this display.
     */
    public int getRows() {
        return iHeight;
    }
    /**
     * For testing if this display should show data from modulename.
     * @param modName A modulename to test.
     * @return true if modulename exists in this displays lists of modulenames.
     */
    public boolean displayModuleData(String modName) {
        for (int j = 0; j < moduleUsers.size(); j++) {     //sok igenom alla modulnamn
            if (((String)moduleUsers.elementAt(j)).equals(modName)) {
                return true;
            }
        }
        return false;
    }
    /**
     *
     * @return the number of columns on this display.
     */
    public int getCols() {
        return iWidth;
    }

    /**
     * Write text at specified pos, row=0 and col=0 is topleftmost char on display.
     * @param row Specifies row to write.
     * @param col Specifies what column to start text.
     * @param newText The text to write.
     */
    public void write(int row, int col, String newText) {
        if ((row < iWidth) && (col < iHeight)) {
            sNewContents[row] = rowFixer(sNewContents[row].substring(0,col) + newText);
        }
    }

    /**
     * Write a new line.
     * @param row What row to write.
     * @param newText What text to write.
     */
    public void writeRow(int row, String newText) {
        sNewContents[row] = rowFixer(newText);

    }
    /**
     * Write an empty line / clear a line.
     * @param row What row to clear.
     */
    public void clearRow(int row) {
        this.writeRow(row, "");
    }

    /**
     * Write a new line at the bottom.
     * @param newText What text to write.
     */
    public void writeLastRow(String newText) {
        this.writeRow(iHeight-1, newText);
    }

    /**
     * Write a new line at the top.
     * @param newText What text to write.
     */
    public void writeFirstRow(String newText) {
        this.writeRow(0, newText);
    }


    /**
     * Move text on row rowToMove left nrOfCols steps.
     * @param rowToMove What row to move.
     * @param nrOfCols How many columns to move text on row.
     */
    public void shiftLeft(int rowToMove, int nrOfCols) {
        if (nrOfCols > 0 && nrOfCols < iWidth) {
            sNewContents[rowToMove] = rowFixer(sNewContents[rowToMove].substring(nrOfCols));
        }
    }

    /**
     * Move text on row rowToMove left, place newText there (move current text so newText fits).
     * @param rowToMove What row to move.
     * @param newText What text to write.
     */
    public void shiftLeft(int rowToMove, String newText) {
        if (newText.length() > 0 && newText.length() < iWidth) {
            sNewContents[rowToMove] = rowFixer(sNewContents[rowToMove].substring(newText.length()) + newText);
        }
    }

    /**
     * Move text on row rowToMove right nrOfCols steps.
     * @param rowToMove What row to move.
     * @param nrOfCols How many columns to move text on row.
     */
    public void shiftRight(int rowToMove, int nrOfCols) {
        if (nrOfCols > 0 && nrOfCols < iWidth) {
            String sDummy = "";
            for (int i = 0; i < nrOfCols; i++) {
                sDummy = sDummy + " ";
            }
            sNewContents[rowToMove] = rowFixer(sDummy + sNewContents[rowToMove]);
        }
    }

    /**
     * Move text on row rowToMove right, place newText there (move current text so newText fits).
     * @param rowToMove What row to move.
     * @param newText What text to write.
     */
    public void shiftRight(int rowToMove, String newText) {
        if (rowToMove < iHeight) {
            sNewContents[rowToMove] = rowFixer(newText + sNewContents[rowToMove]);
        }
    }

    /**
     * Move all rows up, nrOfRows step.
     * @param nrOfRows The number of rows to scroll.
     */
    public void scrollUp(int nrOfRows) {
        if (nrOfRows > 0 && iHeight > nrOfRows) {
            for (int i = 0; i < iHeight; i++) {
                sNewContents[i] = sNewContents[i+nrOfRows];
            }
            for (int i = iHeight - 1; i >= iHeight - nrOfRows; i--) {
                sNewContents[i] = rowFixer("");
            }
        } else if (nrOfRows > 0) {
            sNewContents[iHeight - 1] = rowFixer("");
        }
    }

    /**
     * Just scroll one row, and specify the new row.
     * @param newRow What text to write on new row.
     */
    public void scrollUp(String newRow) {
        if (iHeight > 1) {
            for (int i = 0; i < iHeight; i++) {
                sNewContents[i] = sNewContents[i+1];
            }
            sNewContents[iHeight - 1] = rowFixer(newRow);
        }
    }

    /**
     * Move all rows down, nrOfRows step.
     * @param nrOfRows The number of rows to scroll.
     */
    public void scrollDown(int nrOfRows) {

    }

    /**
     * Just scroll one row, and specify the new row.
     * @param newRow What text to write on new row.
     */
    public void scrollDown(String newRow) {
        if (iHeight > 1) {
            for (int i = iHeight - 1; i > 0; i--) {
                sNewContents[i] = sNewContents[i-1];
            }
            sNewContents[0] = rowFixer(newRow);
        }
    }

    /**
     * This method compares the current data on the display and the
     * "virtual" display, it makes packages of the data, just to be
     * transmitted to display. It is efficient and doesn't send text
     * if its already on display.
     * //todo: effektivisera genom att kolla om addresseringen (och ev null) far plats i aktuellt paket.
     * @return a vector with MBPackets, ready to be transmitted.
     */
    public Vector getDataToSend() {
        //first place data in strings as packages
        Vector sVector = new Vector();
        //current package, gets stored away in sVector when filled
        String sCurrentPacket = "";

        //newPacket just says that address&CO should be written if true
        boolean newPacket = true;
        char cTemp = 0;
        //for each row on display
        for (int row = 0; row < iHeight; row++) {
            String sNewDum = sNewContents[row];
            String sCurDum = sCurrContents[row];
            String delayedChars = "";
            newPacket = true;           //for each row, address should be written to packet

            //for each column on display
            for (int col = 0; col < iWidth; col++) {
                if (sNewDum.charAt(col) == sCurDum.charAt(col)) {
                    if (!newPacket) {
                        delayedChars = delayedChars + sNewDum.charAt(col);
                    }
                } else {                //if chars at column differed
                    if (!newPacket) {       //if packet already started
                        if (delayedChars.length() <= 4) {
                            if (sCurrentPacket.length() > ARNEMAXPACKETSIZE - 1 - delayedChars.length()) {
                                //if all delayedChars does not fit, create a new packet, without delayedChars
                                String dummyString = sCurrentPacket;
                                sVector.addElement(dummyString);
                                sCurrentPacket = "";
                                //write setadress
                                cTemp = LCDSETADDRESS;
                                sCurrentPacket = sCurrentPacket + cTemp;
                                //write the address   (rowAddresses.elementAt(row) + col  ???)
                                cTemp = (char)(Integer.parseInt((String)rowAddresses.elementAt(row)) + col);
                                sCurrentPacket = sCurrentPacket + cTemp;
                                //write settext
                                cTemp = LCDTEXTWRITE;
                                sCurrentPacket = sCurrentPacket + cTemp;
                            } else {    //if delayedchars and a new char fits in packet then
                                //add delayedchars to packet
                                sCurrentPacket = sCurrentPacket + delayedChars;
                            }
                            //write char
                            sCurrentPacket = sCurrentPacket + sNewDum.charAt(col);

                            delayedChars = "";
                        } else {        //if more than 4 delayedChars
                            delayedChars = "";
                            //check if there is space in packet, if not, create a new packet and dont write null to it
                            if (sCurrentPacket.length() > ARNEMAXPACKETSIZE - 5) {
                                String dummyString = sCurrentPacket;
                                sVector.addElement(dummyString);
                                sCurrentPacket = "";
                            } else {
                                //write null
                                cTemp = LCDNULL;    //(null = 0xA0 = 160)
                                sCurrentPacket = sCurrentPacket + cTemp;
                            }

                            //write setadress
                            cTemp = LCDSETADDRESS;
                            sCurrentPacket = sCurrentPacket + cTemp;
                            //write the address   (rowAddresses.elementAt(row) + col  ???)
                            cTemp = (char)(Integer.parseInt((String)rowAddresses.elementAt(row)) + col);
                            sCurrentPacket = sCurrentPacket + cTemp;
                            //write settext
                            cTemp = LCDTEXTWRITE;
                            sCurrentPacket = sCurrentPacket + cTemp;

                            //write character
                            sCurrentPacket = sCurrentPacket + sNewDum.charAt(col);

                        }

                    } else {                //if new packet (or just request to write address)
                        //check if it fits (yes it could be almost full if we just changed display row)
                        if (sCurrentPacket.length() > ARNEMAXPACKETSIZE - 4) {
                            String dummyString = sCurrentPacket;
                            sVector.addElement(dummyString);
                            sCurrentPacket = "";
                        }

                        if (sCurrentPacket.length() > 0) {
                            //new line, then send null
                            cTemp = LCDNULL;
                            sCurrentPacket = sCurrentPacket + cTemp;
                        }
                        //write setadress   //(dont destroy contents of sCurrentPacket, if we just changed display
                                            // row we just want to address at the end of existing packet)
                        cTemp = LCDSETADDRESS;
                        sCurrentPacket = sCurrentPacket + cTemp;
                        //write the address
                        cTemp = (char)(Integer.parseInt((String)rowAddresses.elementAt(row)) + col);
                        sCurrentPacket = sCurrentPacket + cTemp;
                        //write settext
                        cTemp = LCDTEXTWRITE;
                        sCurrentPacket = sCurrentPacket + cTemp;
                        //write character
                        sCurrentPacket = sCurrentPacket + sNewDum.charAt(col);

                        newPacket = false;
                    }
                }
            }
        }
        if (sCurrentPacket.length() > 0) {
            sVector.addElement(sCurrentPacket);
        }
        for (int i = 0; i < iHeight; i++) {
            //sCurrContents[i] = "....................";//sNewContents[i];
            sCurrContents[i] = sNewContents[i];
        }

        //sVector is now a vector with each packet as strings

        Vector pVector = new Vector();
        MbPacket p;

        //repack to MbPackets and return them
        for (int i = 0; i < sVector.size(); i++){
            p = new MbPacket();
            sCurrentPacket = (String)sVector.elementAt(i);
            sCurrentPacket = lcdDisplayReplace(sCurrentPacket);
            //System.out.println("apan: " + sCurrentPacket + " " + i);
            if (sCurrentPacket.length() > 0) {
                //ARNE packet in XML-format
                String contents = "<arnepacket bytes=\"" + sCurrentPacket.length() + "\""
                                   + " destnode=\"" + sTarget + "\">";
                for (int col = 0; col < sCurrentPacket.length(); col++) {
                    contents = contents + "<byte id=\"" + (col +1) + "\" value=\""
                                        + (int)sCurrentPacket.charAt(col) + "\"/>";
                }
                contents = contents + "</arnepacket>";
                //System.out.println("****start****");
                //System.out.println("sCurrentPacket=" + sCurrentPacket);
                //System.out.println("****contents:****");
                //System.out.println(contents);
                //System.out.println("****end****");

                p.setContents(contents);
                pVector.addElement(p);
            }
        }

        return pVector;

    }


    private String lcdDisplayReplace(String stringToReplace) {
        char dispChar;
        stringToReplace = stringToReplace.replace('å','a');
        dispChar = 225;
        stringToReplace = stringToReplace.replace('ä', dispChar);
        dispChar = 239;
        stringToReplace = stringToReplace.replace('ö',dispChar);
        stringToReplace = stringToReplace.replace('Å','A');
        dispChar = 225;
        stringToReplace = stringToReplace.replace('Ä', dispChar);
        dispChar = 239;
        stringToReplace = stringToReplace.replace('Ö',dispChar);
        dispChar = 245;
        stringToReplace = stringToReplace.replace('ü',dispChar);
        dispChar = 245;
        stringToReplace = stringToReplace.replace('Ü',dispChar);
        stringToReplace = stringToReplace.replace('é','e');
        stringToReplace = stringToReplace.replace('É','E');
        stringToReplace = stringToReplace.replace('\\','/');

        return stringToReplace;
    }

    /**
     * User can add a row to display-queue.
     * @param rowText Textrow to be added to queue.
     * @param prio The priority of the message; 0, 1 or 2.
     * @param timeToLive Set the minimum time the message should be displayed.
     * @param isSpecial Specifies if rowText isn't text but a command.
     * @param id
     * @param rowPos is "" if not used, "T+x" for Top+x, "B-x" for Bottom-x or "M+x"/"M-x" for Middle+/-x.
     */
    public void addRowToQueue(String rowText, int prio, int timeToLive, boolean isSpecial, int id, String rowPos) {
        RowObject dummyRow = new RowObject();
        dummyRow.row = rowText;
        dummyRow.TTL = timeToLive;
        dummyRow.isSpecial = isSpecial;
        dummyRow.id = id;
        boolean useRow = true;

        if (prio > 2 || prio < 0) {
            prio = 2;
        }

        //dummyRow.rowNumber
        if (rowPos.length() > 0) {
            int iDummy = -1;
            boolean bUseRowNum = false;

            if (rowPos.length() == 1) {
                iDummy = 0;
                bUseRowNum = true;
            } else if (rowPos.length() == 3) {
                //todo: storre an 3 da? en display med mer an 9 rader...?
                try {
                    if (rowPos.substring(1,2).compareTo("+") == 0) {
                        iDummy = Integer.parseInt(rowPos.substring(2,3));
                    } else if (rowPos.substring(1,2).compareTo("-") == 0) {
                        iDummy = Integer.parseInt(rowPos.substring(1,3));
                    }
                    bUseRowNum = true;
                } catch (NumberFormatException e) {
                System.out.println("exception with rownumber!");
                }
            }
            //System.out.println("iDummy: " + iDummy);

            if (bUseRowNum == true) {
                if (rowPos.charAt(0) == 'T') {
                    //iDummy = iDummy;
                } else if (rowPos.charAt(0) == 'B') {
                    iDummy = iDummy + iHeight-1;
                } else if (rowPos.charAt(0) == 'M') {
                    iDummy = iDummy + (int)((iHeight-1)/2);
                } else {
                    //dummyRow.rowNumber = null;
                }

                if (iDummy < 0 || iDummy >= iHeight) {
                    useRow = false;
                }

            }
            dummyRow.rowNumber = iDummy;        //set even if not used
            if (iDummy >= 0) {
                //System.out.println("rownumber: " + iDummy);
                dummyRow.useRowNum = true;
            }

        }

        //put message in priority-queues
        if (dummyRow.row.length() > 0 && useRow) {  //useRow is false if rownumber if out of range
            //System.out.println("put in queue");
            boolean checkNext = true;
            //check here if id > 0, if so then check if message with same id is in currentRows
            //if not in currentRows then check queue (prio accordning to object.prio), replace if found
            //if no message with the same id is found then place message last in queue
            if (dummyRow.id > 0) {
                //System.out.println("id > 0");
                for (int i = 0; i < rowsCurrent.size() && checkNext; i++) {
                    RowObject dummyCurrentRow = (RowObject)rowsCurrent.elementAt(i);
                    if (dummyCurrentRow.id == dummyRow.id){
                        //System.out.println("replacing on display");
                        rowsCurrent.remove(i);                                   //remove old
                        dummyRow.rowNumber = dummyCurrentRow.rowNumber;          //if not set then set to the row to be placed
                        rowsCurrent.addElement(dummyRow);                        //place new stuff
                        if (dummyRow.TTL < 0) {                                  //om raden redan ska betraktas som ledig
                            if (!freeRows.contains(dummyCurrentRow)) {           //och tidigare rad inte redan var ledig
                                freeRows.addElement(dummyRow);
                            }
                        } else {
                            if (freeRows.contains(dummyCurrentRow)) {           //och tidigare rad inte redan var ledig
                                freeRows.remove(dummyCurrentRow);
                            }
                        }
                        writeRow(dummyRow.rowNumber, dummyRow.row);
                        checkNext = false;

                    }
                }

                for (int j = 0; j < rowPrio0.size() && checkNext; j++) {
                    //System.out.println("replacing in queue");
                    RowObject queueDummyRow = (RowObject)rowPrio0.elementAt(j);
                    if (queueDummyRow.id == dummyRow.id) {
                        rowPrio0.remove(j);
                        rowPrio0.insertElementAt(dummyRow, j);
                        checkNext = false;
                    }
                }
                for (int j = 0; j < rowPrio1.size() && checkNext; j++) {
                    RowObject queueDummyRow = (RowObject)rowPrio1.elementAt(j);
                    if (queueDummyRow.id == dummyRow.id) {
                        rowPrio1.remove(j);
                        rowPrio1.insertElementAt(dummyRow, j);
                        checkNext = false;
                    }
                }
                for (int j = 0; j < rowPrio2.size() && checkNext; j++) {
                    RowObject queueDummyRow = (RowObject)rowPrio2.elementAt(j);
                    if (queueDummyRow.id == dummyRow.id) {
                        rowPrio2.remove(j);
                        rowPrio2.insertElementAt(dummyRow, j);
                        checkNext = false;
                    }
                }
            }

            if (checkNext) {
                if (prio == 0) {
                    //System.out.println("adding to prio0");
                    rowPrio0.addElement(dummyRow);
                } else if (prio == 1) {
                    //System.out.println("adding to prio1");
                    rowPrio1.addElement(dummyRow);
                } else if (prio == 2) {
                    //System.out.println("adding to prio2");
                    rowPrio2.addElement(dummyRow);
                }
            }

        }

        manageQueues();     //do this for enabling data to be sent more then 1 time per sec
    }


    /**
     * This method should be run once every second, to update queue and
     * ttl for messages.
     */
    public void manageQueues() {
        RowObject dummyRow;

        for (int i = 0; i < rowsCurrent.size(); i++) {
            dummyRow = (RowObject)rowsCurrent.elementAt(i);
            if (dummyRow.TTL >= 0) {
                //System.out.println("Counting down ttl for: " + dummyRow.rowNumber + ", text: " + dummyRow.row);
                if (dummyRow.TTL == 0) {
                    freeRows.addElement(dummyRow);                  //added at the end (rows free longest at the beginning)
                    //System.out.println("rowelement added to freerows: " + dummyRow.rowNumber + ", text: " + dummyRow.row);
                }
                dummyRow.TTL = dummyRow.TTL - 1;
            }
        }

        //glom inte initiera vektorn, alla rader ska vara med i den

        //gor en koll av TTL nar raden flyttas till rowsCurrent, if ttl<0 then add to freerows


        for (int i = 0; i < freeRows.size(); i++) {
            boolean checkNextQueue = true;

            for (int j = 0; j < rowPrio0.size() && checkNextQueue; j++) {
                //System.out.println("Prio0queueelement: " + j + ", freerow: " + i);
                RowObject queueDummyRow = (RowObject)rowPrio0.elementAt(j);
                dummyRow = (RowObject)freeRows.elementAt(i);
                if (dummyRow.rowNumber == queueDummyRow.rowNumber || !queueDummyRow.useRowNum) {
                    //System.out.println("Place row, prio0element" + ", text: " + queueDummyRow.row);
                    freeRows.remove(i);                                     //not free anymore
                    rowPrio0.remove(j);                                     //remove queue-item
                    queueDummyRow.rowNumber = dummyRow.rowNumber;           //if not set then set to the row to be placed
                    rowsCurrent.remove(dummyRow);                           //remove old stuff
                    rowsCurrent.addElement(queueDummyRow);                  //place new stuff
                    if (queueDummyRow.TTL < 0) {                            //om raden redan ska betraktas som ledig
                        freeRows.addElement(queueDummyRow);
                    }
                    writeRow(queueDummyRow.rowNumber, queueDummyRow.row);
                    i = i - 1;
                    checkNextQueue = false;
                }
            }
            for (int j = 0; j < rowPrio1.size() && checkNextQueue; j++) {
                //System.out.println("Prio1queueelement: " + j + ", freerow: " + i);
                RowObject queueDummyRow = (RowObject)rowPrio1.elementAt(j);
                dummyRow = (RowObject)freeRows.elementAt(i);
                if (dummyRow.rowNumber == queueDummyRow.rowNumber || !queueDummyRow.useRowNum) {
                    //System.out.println("Place row, prio1element" + ", text: " + queueDummyRow.row);
                    freeRows.remove(i);                                     //not free anymore
                    rowPrio1.remove(j);                                     //remove queue-item
                    queueDummyRow.rowNumber = dummyRow.rowNumber;           //if not set then set to the row to be placed
                    rowsCurrent.remove(dummyRow);                           //remove old stuff
                    rowsCurrent.addElement(queueDummyRow);                  //place new stuff
                    if (queueDummyRow.TTL < 0) {                            //om raden redan ska betraktas som ledig
                        freeRows.addElement(queueDummyRow);
                    }
                    writeRow(queueDummyRow.rowNumber, queueDummyRow.row);
                    i = i - 1;
                    checkNextQueue = false;
                }
            }
            for (int j = 0; j < rowPrio2.size() && checkNextQueue; j++) {
                RowObject queueDummyRow = (RowObject)rowPrio2.elementAt(j);
                dummyRow = (RowObject)freeRows.elementAt(i);
                if (dummyRow.rowNumber == queueDummyRow.rowNumber || !queueDummyRow.useRowNum) {
                    freeRows.remove(i);                                     //not free anymore
                    rowPrio2.remove(j);                                     //remove queue-item
                    queueDummyRow.rowNumber = dummyRow.rowNumber;           //if not set then set to the row to be placed
                    rowsCurrent.remove(dummyRow);                           //remove old stuff
                    rowsCurrent.addElement(queueDummyRow);                  //place new stuff
                    if (queueDummyRow.TTL < 0) {                            //om raden redan ska betraktas som ledig
                        freeRows.addElement(queueDummyRow);
                    }
                    writeRow(queueDummyRow.rowNumber, queueDummyRow.row);
                    i = i - 1;
                    checkNextQueue = false;
                }
            }

            //System.out.println("apan: ja");

        }

    }

    /**
     * Keeps the rows the same length as display-width
     * Cut or fill with spaces.
     * @param sRow String to fix.
     * @return a string, sRow, but with the same length as the display-width.
     */
    private String rowFixer(String sRow) {
        if (sRow.length() > iWidth) {
            return sRow.substring(0, iWidth);
        } else if (sRow.length() < iWidth) {
            String sDummy = "";
            for (int i = 0; i < iWidth - sRow.length(); i++) {
                sDummy = sDummy + " ";
            }
            return sRow + sDummy;
        } else {
            return sRow;
        }
    }

    /**
     * A queue-object
     */
    private class RowObject {
        protected int TTL;
        protected String row;
        //protected boolean isDisplaying;
        protected boolean isSpecial;    //row definierar en speciell egenskap som t.ex. cleardisplay osv om true
                                //fixa in detta senare

        protected int rowNumber=0;
        protected boolean useRowNum = false;
        protected int id;
    }
}
