/*
 * File created by arune
 * at 2004-dec
 */
package Macbeth.Modules.modTVchart;
import java.util.*;

/**
 *
 *
 *
 * @author arune
 */
public class ChannelObject {

    public int tvchannel;
    public int id;
    public String longname;
    public String shortname;
    public ArrayList channelData = new ArrayList();

    /**
     * Creates a new instance of ChannelObject.
     * @param shortname A three character long name of this object.
     * @param longname The node-address at where this object is.
     * @param tvchannel .
     * @param id .
     */
    public ChannelObject(String shortname, String longname, int tvchannel, int id) {
        this.longname = longname;
        this.shortname = shortname;
        this.tvchannel = tvchannel;
        this.id = id;
    }

}
