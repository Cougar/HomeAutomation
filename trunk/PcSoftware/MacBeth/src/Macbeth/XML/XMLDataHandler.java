/*
 * XMLDataHandler.java
 *
 * Created on den 27 augusti 2003, 18:05
 */
package Macbeth.XML;

import java.util.*;

/**
 * Implement this interface if your class can take care of
 * XML-data from an XML-parser.
 * @author Jimmy
 */
public interface XMLDataHandler {
    /**
     * Called when start of a new element is found in the XML-data.
     * Ex: <name attr1="value1" attr2="value2">
     * Element name would then be "name" and attribute list
     * would contain "value1" and "value2" mapped to the attribute
     * names "attr1" and "attr2".
     * @param element The name of the element.
     * @param attributes The element attributes.
     */
    public void XMLstartElement(String element, HashMap attributes);

    /**
     * Called when end of an element was found in the XML-data.
     * Ex: </name> or <test attr="value" />
     * @param element The name of the element.
     */
    public void XMLendElement(String element);

    /**
     * Called when data was found between the start of a tag and
     * the end of a tag.
     * Ex: <test>Here is some text</test>
     * The data would then be "Here is some text".
     * @param data The data string.
     */
    public void XMLelementData(String data);

    /**
     * Called when XML-parser starts parsing an XML document.
     */
    public void XMLdocumentStart();

    /**
     * Called when XML-parser stops parsing an XML-document.
     */
    public void XMLdocumentEnd();
}
