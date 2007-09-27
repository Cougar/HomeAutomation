/**
 * File created by Jimmy
 * at 2003-dec-28 14:48:30
 */
package Macbeth.Utilities;

import java.util.Calendar;
import java.util.TimeZone;

/**
 * Date and time utility functions.
 * @author Jimmy
 */
public class MyDateTime {

    /**
     * Gets current date and time formatted as a string.
     * @param format The date- and time-format you want to use.
     * An example would be "yyyy-MM-dd HH:mm:ss". See
     * @return Current date and time as a string.
     */
    public static String now(String format) {
        Calendar cal = Calendar.getInstance(TimeZone.getDefault());
        String DATE_FORMAT = format;
        java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat(DATE_FORMAT);
        /* on some JDKs, the default TimeZone is wrong
        * we must set the TimeZone manually
        * sdf.setTimeZone(TimeZone.getTimeZone("EST"));
        */
        sdf.setTimeZone(TimeZone.getDefault());
        return sdf.format(cal.getTime());
    }

    /**
     * Gets current date and time formatted as a string using
     * the format "yyyy-MM-dd HH:mm:ss".
     * @return Current date and time as a string.
     */
    public static String now() {
        return now("yyyy-MM-dd HH:mm:ss");
    }

}
