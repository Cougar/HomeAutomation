/*
 * File created by Jimmy
 * at 2004-mar-08 00:24:24
 */
package Macbeth.Utilities;

import java.io.PrintStream;
import java.io.PipedOutputStream;
import java.io.PrintWriter;

/**
 * Prefixs everything that is written to the stream with a timestamp.
 * @author Jimmy
 */
public class TimeStampedStream extends PrintStream {
    //the underlying stream
    private PrintStream stream;

    /**
     * Creates a new instance of TextFormatterStream.
     * @param stream The underlying stream to which the
     *               formatted text should be written.
     */
    public TimeStampedStream(PrintStream stream) {
        //construct PrintWriter with non-used PipedOutputStream
        super(new PipedOutputStream());
        //save reference to underlying stream
        this.stream = stream;
    }

    private String timeStamp() {
        return MyDateTime.now("[yyyy-MM-dd HH:mm:ss] ");
    }

    /* Override all standard print-methods: */

    public void println(char c[]) {
        String str = timeStamp() + new String(c, 0, c.length) + MySystem.lineBreak;
        if (c != null) {
            stream.print(str);
        }
    }

    public void print(char c[]) {
        String str = timeStamp() + new String(c, 0, c.length);
        if (c != null) {
            stream.print(str);
        }
    }

    public void println(double d) {
        String str = timeStamp() + d + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(double d) {
        String str = timeStamp() + d;
        stream.print(str);
    }

    public void println(float f) {
        String str = timeStamp() + f + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(float f) {
        String str = timeStamp() + f;
        stream.print(str);
    }

    public void println(char c) {
        String str = timeStamp() + c + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(char c) {
        String str = timeStamp() + c;
        stream.print(str);
    }

    public void println(boolean b) {
        String str = timeStamp() + b + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(boolean b) {
        String str = timeStamp() + b;
        stream.print(str);
    }

    public void println(int i) {
        String str = timeStamp() + i + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(int i) {
        String str = timeStamp() + i;
        stream.print(str);
    }

    public void println(String str) {
        String string = timeStamp() + (str != null ? str : "null") + MySystem.lineBreak;
        stream.print(string);
    }

    public void print(String str) {
        String string = timeStamp() + (str != null ? str : "null");
        stream.print(string);
    }

    public void println(Object obj) {
        String str = timeStamp() + (obj != null ? obj.toString() : "null") + MySystem.lineBreak;
        stream.print(str);
    }

    public void print(Object obj) {
        String str = timeStamp() + (obj != null ? obj.toString() : "null");
        stream.print(str);
    }

}
