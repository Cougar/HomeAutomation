/**
 * File created by Jimmy
 * at 2004-jan-04 19:26:58
 */
package Macbeth.Utilities;

import java.io.PrintStream;
import java.io.PipedOutputStream;

/**
 * This stream will format all text written to it, and then
 * write the formatted text to the underlying PrintStream.
 * The formatting that will be performed can be defined by using
 * methods setPreString, setPostString and setLineBreakString.
 * Per default, no formatting is performed at all.
 * @author Jimmy
 */
public class TextFormatterStream extends PrintStream {
    //the underlying stream
    private PrintStream stream;
    //a pre-string that will be appended _before_ everything that is written to this stream
    private String preString;
    //a post-string that will appended _after_ everything that is written to this stream
    private String postString;
    //any additional line break string (like <br> for instance)
    private String lineBreakString;

    /**
     * Creates a new instance of TextFormatterStream.
     * @param stream The underlying stream to which the
     * formatted text should be written.
     */
    public TextFormatterStream(PrintStream stream) {
        //construct PrintWriter with non-used PipedOutputStream
        super(new PipedOutputStream());
        //save reference to underlying stream
        this.stream = stream;
        //no pre- or post-strings per default
        preString = "";
        postString = "";
        //use system line break per default
        lineBreakString = MySystem.lineBreak;
    }

    /**
     * Sets the pre-string that should be appended <b>before</b>
     * everything else that is written to this stream.
     * @param preString The pre-string. If you don't want any
     * pre-string, set this to null.
     */
    public void setPreString(String preString) {
        this.preString = preString;
    }

    /**
     * Sets the post-string that should be appended <b>after</b>
     * everything else that is written to this stream.
     * @param postString The post-string. If you don't want any
     * post-string, set this to null.
     */
    public void setPostString(String postString) {
        this.postString = postString;
    }

    /**
     * Sets the line break-string. Everytime you write a line
     * to this stream, this line break string will also
     * be written to the underlying stream. Per default this is
     * the system line separator, like '\n' or '\n\r' (depending
     * on the OS).
     * @param lineBreakString The line break-string.
     */
    public void setLineBreakString(String lineBreakString) {
        this.lineBreakString = lineBreakString;
    }

    /* Override all standard print-methods: */

    public void println(char c[]) {
        String str = preString + new String(c,0,c.length) + postString + lineBreakString;
        if(c!=null) stream.print(str);
    }

    public void print(char c[]) {
        String str = preString + new String(c,0,c.length) + postString;
        if(c!=null) stream.print(str);
    }

    public void println(double d) {
        String str = preString + d + postString + lineBreakString;
        stream.print(str);
    }

    public void print(double d) {
        String str = preString + d + postString;
        stream.print(str);
    }

    public void println(float f) {
        String str = preString + f + postString + lineBreakString;
        stream.print(str);
    }

    public void print(float f) {
        String str = preString + f + postString;
        stream.print(str);
    }

    public void println(char c) {
        String str = preString + c + postString + lineBreakString;
        stream.print(str);
    }

    public void print(char c) {
        String str = preString + c + postString;
        stream.print(str);
    }

    public void println(boolean b) {
        String str = preString + b + postString + lineBreakString;
        stream.print(str);
    }

    public void print(boolean b) {
        String str = preString + b + postString;
        stream.print(str);
    }

    public void println(int i) {
        String str = preString + i + postString + lineBreakString;
        stream.print(str);
    }

    public void print(int i) {
        String str = preString + i + postString;
        stream.print(str);
    }

    public void println(String str) {
        String string = preString + (str!=null ? str : "null") + postString + lineBreakString;
        stream.print(string);
    }

    public void print(String str) {
        String string = preString + (str!=null ? str : "null") + postString;
        stream.print(string);
    }

    public void println(Object obj) {
        String str = preString + (obj!=null ? obj.toString() : "null") + postString + lineBreakString;
        stream.print(str);
    }

    public void print(Object obj) {
        String str = preString + (obj!=null ? obj.toString() : "null") + postString;
        stream.print(str);
    }

}
