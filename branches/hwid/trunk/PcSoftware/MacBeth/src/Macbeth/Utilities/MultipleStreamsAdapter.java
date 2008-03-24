package Macbeth.Utilities;

import java.io.PrintStream;
import java.io.PipedOutputStream;
import java.io.PrintWriter;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

/**
 * An adapter whose output is connected to multiple other streams.
 * @author Jimmy
 */
public class MultipleStreamsAdapter extends PrintStream {
    //a list of underlying PrintStreams
    private List streams;

    /**
     * Creates a new instance of MultipleStreamsAdapter
     *
     * @param streams A list of PrintStreams to which this adapter should forward its data.
     */
    public MultipleStreamsAdapter(List streams) {
        //construct PrintWriter with non-used PipedOutputStream
        super(new PipedOutputStream());
        //save reference to underlying stream
        this.streams = streams;
    }

    /**
     * Creates a new instance of MultipleStreamsAdapter.
     * @param stream1 A stream that immediately should be connected to the output of this adapter.
     * @param stream2 A stream that immediately should be connected to the output of this adapter.
     */
    public MultipleStreamsAdapter(PrintStream stream1, PrintStream stream2) {
        this();
        streams.add(stream1);
        streams.add(stream2);
    }

    /**
     * Creates a new instance of MultipleStreamsAdapter.
     */
    public MultipleStreamsAdapter() {
        this(new ArrayList());
    }


    /**
     * Connects another stream to the output of this adapter.
     * @param stream The stream.
     */
    public void addStream(PrintStream stream) {
        streams.add(stream);
    }


    /**
     * Disconnects a stream from the output of this adapter.
     * @param stream
     */
    public void removeStream(PrintStream stream) {
        streams.remove(stream);
    }


    private void distributeString(String str) {
        Iterator it = streams.iterator();
        while (it.hasNext()) {
            PrintStream p = (PrintStream) it.next();
            p.print(str);
        }
    }


    /* Override all standard print-methods: */

    public void println(char c[]) {
        String str = new String(c, 0, c.length) + MySystem.lineBreak;
        if (c != null) {
            distributeString(str);
        }
    }

    public void print(char c[]) {
        String str = new String(c, 0, c.length);
        if (c != null) {
            distributeString(str);
        }
    }

    public void println(double d) {
        String str = d + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(double d) {
        String str = Double.toString(d);
        distributeString(str);
    }

    public void println(float f) {
        String str = f + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(float f) {
        String str = Float.toString(f);
        distributeString(str);
    }

    public void println(char c) {
        String str = c + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(char c) {
        String str = Character.toString(c);
        distributeString(str);
    }

    public void println(boolean b) {
        String str = b + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(boolean b) {
        String str = Boolean.toString(b);
        distributeString(str);
    }

    public void println(int i) {
        String str = i + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(int i) {
        String str = Integer.toString(i);
        distributeString(str);
    }

    public void println(String str) {
        String string = (str != null ? str : "null") + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(String str) {
        String string = (str != null ? str : "null");
        distributeString(str);
    }

    public void println(Object obj) {
        String str = (obj != null ? obj.toString() : "null") + MySystem.lineBreak;
        distributeString(str);
    }

    public void print(Object obj) {
        String str = (obj != null ? obj.toString() : "null");
        distributeString(str);
    }

}