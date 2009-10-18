/*
 * RepeatedString.java
 *
 * Created on den 23 augusti 2003, 17:37
 */
package Macbeth.Utilities;

/**
 * Repeats a String a given number of times
 * @author Jimmy
 */
public class RepeatedString {
    //The actual String
    private String str;

    /**
     * Creates a new instance of RepeatedString.
     * @param repeatString The String that should be repeated.
     * @param repeatCount How many times the String should be repeated.
     */
    public RepeatedString(String repeatString, int repeatCount) {
        str = new String();
        for (int i=0; i<repeatCount; i++) {
            str += repeatString;
        }
    }

    /**
     * Gets the repeated string.
     * @return The repeated string as a String.
     */
    public String toString() {
        return str;
    }
}
