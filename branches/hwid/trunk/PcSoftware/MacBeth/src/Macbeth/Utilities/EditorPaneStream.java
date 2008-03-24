/**
 * File created by Jimmy
 * at 2004-jan-04 20:22:22
 */
package Macbeth.Utilities;

import javax.swing.*;
import javax.swing.text.Document;
import javax.swing.text.JTextComponent;
import javax.swing.text.BadLocationException;
import javax.swing.text.Element;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLEditorKit;
import javax.swing.text.html.HTML;
import java.io.PrintStream;
import java.io.PipedOutputStream;
import java.io.IOException;

/**
 * A stream that writes the text to a JEditorPane.
 * @author Jimmy
 */
public class EditorPaneStream extends PrintStream {

    //the JEditorPane to which we will write all text
    private JEditorPane editorPane;
    private HTMLEditorKit editorKit;
    private HTMLDocument htmlDocument;

    /**
     * Creates a new instance of EditorPaneStream.
     * @param editorPane The JEditorPane to which all text
     * should be written.
     */
    public EditorPaneStream(JEditorPane editorPane) {
        //construct PrintWriter with non-used PipedOutputStream
        super(new PipedOutputStream());
        //keep reference to our JTextComponent
        this.editorPane = editorPane;
        editorPane.setContentType("text/html");
        //get editor kit
        editorKit = (HTMLEditorKit)editorPane.getEditorKit();
        //get html document
        htmlDocument = (HTMLDocument)editorPane.getDocument();
    }

    private void insertString(String string) {
        try {
            //editorKit.insertHTML(htmlDocument,htmlDocument.getLength(),string,2,2,null);
            //htmlDocument.insertBeforeEnd(htmlDocument.getDefaultRootElement().getElement(0),string);
            editorKit.insertHTML(htmlDocument, htmlDocument.getLength(), string, 0, 0, null);
        } catch (Exception e) {
            e.printStackTrace();
        }
        editorPane.setCaretPosition(htmlDocument.getLength());
    }

    /* Override all standard print-methods: */

    public void println(char c[]) {
        if(c!=null) insertString(new String(c,0,c.length) + MySystem.lineBreak);
    }

    public void print(char c[]) {
        if(c!=null) insertString(new String(c,0,c.length));
    }

    public void println(double d) {
        insertString(d + "" + MySystem.lineBreak);
    }

    public void print(double d) {
        insertString(d + "");
    }

    public void println(float f) {
        insertString(f + "" + MySystem.lineBreak);
    }

    public void print(float f) {
        insertString(f + "");
    }

    public void println(char c) {
        insertString(c + "" + MySystem.lineBreak);
    }

    public void print(char c) {
        insertString(c + "");
    }

    public void println(boolean b) {
        insertString(b + "" + MySystem.lineBreak);
    }

    public void print(boolean b) {
        insertString(b + "");
    }

    public void println(int i) {
        insertString(i + "" + MySystem.lineBreak);
    }

    public void print(int i) {
        insertString(i + "");
    }

    public void println(String str) {
        if(str!=null) insertString(str + MySystem.lineBreak);
        else insertString("null" + MySystem.lineBreak);
    }

    public void print(String str) {
        if(str!=null) insertString(str);
        else insertString("null");
    }

    public void println(Object obj) {
        if(obj!=null) insertString(obj.toString() + MySystem.lineBreak);
        else insertString("null" + MySystem.lineBreak);
    }

    public void print(Object obj) {
        if(obj!=null) insertString(obj.toString());
        else insertString("null");
    }

}
