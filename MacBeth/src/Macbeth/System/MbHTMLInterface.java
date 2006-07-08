/**
 * File created by Jimmy
 * at 2003-dec-28 13:08:55
 */
package Macbeth.System;

/**
 * Implement this interface if your component has a HTML
 * interface. A HTML interface can be anything from a
 * simple page showing some information about your component
 * to an advanced page with links for controlling
 * functionality in your component. Very basic command-line
 * kernels might never use these HTML capabilities, but
 * more advanced GUI-kernels will.
 */
public interface MbHTMLInterface {

    /**
     * Retreives the full code for your HTML-page. This is
     * formatted as plain text and must be valid HTML-code.
     * @return Full HTML-code for the page.
     */
    abstract public String getHTMLPage();

    /**
     * This method is invoked when a link is pressed on your
     * HTML-page.
     * @param href The href-attribute of the link.
     */
    abstract public void linkPressed(String href);

}