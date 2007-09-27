/*
 * File created by Jimmy
 * at 2004-mar-09 13:32:58
 */
package Macbeth.Utilities;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

/**
 * A repository for string data structures. The repository is built upon named data
 * lists, which can contain list items and/or several levels of sublists. Each list
 * item contains one or more named string data fields.
 * @author Jimmy
 */
public class DataRepository {
    //a root level list, inside which all other lists and items reside
    private DataList rootList;
    //these are used while gradually building up the repository (see methods below)
    private DataList currentList;
    private DataListItem currentListItem;

    /**
     * Creates a new instance of DataRepository.
     */
    public DataRepository() {
        rootList = new DataList(null);
        currentList = rootList;
        currentListItem = null;
    }

    /**
     * Begins building a new sublist inside the current list. You
     * can have any number of sublist. Each list/sublist contains its
     * own list items and/or other sublists.
     * @param name The name of the new list.
     */
    public void advancedBuild_BeginList(String name) {
        //check if another list with the same name already exists inside the current list
        if (currentList.subLists.containsKey(name)) {
            //if so, let's use this as our current list (or else it would be overwritten)
            currentList = (DataList)currentList.subLists.get(name);
        }
        //no conflicts...just add the new list
        else {
            DataList list = new DataList(currentList);
            currentList.subLists.put(name, list);
            currentList = list;
        }
    }

    /**
     * Call this when you have finished adding list items to the
     * current list. This means that control will return to its
     * parent list. You need to call endList() the same number
     * of times as beginList().
     */
    public void advancedBuild_EndList() {
        if (currentList!=rootList) {
            currentList = currentList.parentList;
        }
        else {
            System.err.println("EndList() called too many times!");
        }
    }

    /**
     * Begins building a new list item inside the current list. Each
     * list item contains one or more data fields. Note that all data
     * fields have to be wrapped inside list items, so you cannot add
     * fields directly to a list.
     */
    public void advancedBuild_BeginListItem() {
        DataListItem item = new DataListItem(currentList);
        currentList.items.add(item);
        currentListItem = item;
    }

    /**
     * Call this when you have finished adding data fields to the current
     * list item.
     */
    public void advancedBuild_EndListItem() {
        currentListItem = null;
    }

    /**
     * Adds a data field to the current list item.
     * @param name The name of the data field.
     * @param value The value of the data field.
     */
    public void advancedBuild_PutField(String name, String value) {
        if (currentListItem!=null) {
            currentListItem.putField(name, value);
        }
        else {
            //TODO: throw exception when currentListItem==null
        }
    }

    /**
     * Gets a data list from the root. The root list can be
     * obtained as well by passing null or "root" as name.
     * @param name The name of the list, or null or "root"
     * for the root list.
     * @return The list object.
     */
    public DataList getList(String name) {
        if (name==null || name.equals("root")) {
            return rootList;
        }
        else {
            return (DataList)rootList.subLists.get(name);
        }
    }

    public Iterator lists() {
        return rootList.subLists();
    }

    /**
     * Lists the contents in this data repository in a (multi-line) string.
     * @return A string showing the contents of this data repository.
     */
    public String listContents() {
        return rootList.listContents();
    }



    public class DataList {
        public DataList parentList;
        private LinkedList items;
        private HashMap subLists;

        public DataList(DataList parentList) {
            this.parentList = parentList;
            this.items = new LinkedList();
            this.subLists = new HashMap();
        }

        public Iterator items() {
            return items.iterator();
        }

        public DataListItem firstItem() {
            return (DataListItem)items.getFirst();
        }

        public Iterator subLists() {
            return subLists.keySet().iterator();
        }

        public DataList getSubList(String name) {
            return (DataList) subLists.get(name);
        }

        public String listContents() {
            String tmp = "";
            Iterator it = subLists();
            //list contents in all sublists
            while (it.hasNext()) {
                String sublist = (String) it.next();
                tmp += MySystem.lineBreak + "<LIST: '" + sublist + "'>" + MySystem.lineBreak;
                tmp += getSubList(sublist).listContents();
                tmp += "</LIST>" + MySystem.lineBreak;
            }
            //list all items
            it = items();
            while (it.hasNext()) {
                tmp += "   <ITEM>" + MySystem.lineBreak;
                tmp += ((DataListItem)it.next()).listContents();
                tmp += "   </ITEM>" + MySystem.lineBreak;
            }
            return tmp;
        }
    }


    public class DataListItem {
        public DataList parentList;
        private HashMap fields;

        public DataListItem(DataList parentList) {
            this.parentList = parentList;
            this.fields = new HashMap();
        }

        public boolean containsField(String name) {
            return fields.containsKey(name);
        }

        public String getField(String name) {
            return (String)fields.get(name);
        }

        public void putField(String name, String value) {
            fields.put(name, value);
        }

        public Iterator fields() {
            return fields.keySet().iterator();
        }

        public String listContents() {
            String tmp = "";
            Iterator it = fields();
            while (it.hasNext()) {
                String field = (String) it.next();
                tmp += "      '" + field + "' = '" + getField(field) + "'" + MySystem.lineBreak;
            }
            return tmp;
        }
    }

}
