/*
 * MbInternetAddressList.java
 *
 * Created on den 26 augusti 2003, 11:36
 */
package Macbeth.System;

import java.util.*;

/**
 * Maintains a list of internet addresses, where
 * each address is mapped to a name.
 * @author Jimmy
 */
public class MbInternetAddressList {
    private HashMap addresses;

    /** Creates a new instance of MbInternetAddressList */
    public MbInternetAddressList() {
        addresses = new HashMap(32);
    }

    /**
     * Adds an address to the list and maps it to the given name.
     * @param name The name to which the address will be mapped.
     * @param address The actual address.
     */
    public void addAddress(String name, MbInternetAddress address) {
        addresses.put(name, address);
    }

    /**
     * Gets an address from the list.
     * @param name The name of the address you want to get.
     * @return The address that is mapped to this name. Null if no such name is found in list.
     */
    public MbInternetAddress getAddress(String name) {
        MbInternetAddress address = (MbInternetAddress)addresses.get(name);
        return address;
    }

    /**
     * Removes an address from the list.
     * @param name The name of the address you want to remove.
     */
    public void removeAddress(String name) {
        addresses.remove(name);
    }

    /**
     * Gets an array that contains the names of all addresses.
     * @return An array with all names.
     */
    public String[] getNameList() {
        String[] list = new String[addresses.size()];
        int i=0;
        Iterator it = addresses.keySet().iterator();
        while (it.hasNext()) {
            String name = (String)it.next();
            list[i++] = name;
        }
        return list;
    }
}
