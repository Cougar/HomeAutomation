/*
 * XMLParser.java
 *
 * Created on den 27 augusti 2003, 17:18
 */
package Macbeth.XML;

import java.io.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.SAXParser;
import java.util.*;

/**
 * A helper class for parsing XML files.
 * @author Jimmy
 */
public class XMLParser extends DefaultHandler {

    private XMLDataHandler dataHandler;
    private SAXParserFactory factory;
    private SAXParser saxParser;

    /**
     * Creates a new instance of XMLParser.
     * @param dataHandler The XMLDataHandler that should be
     * invoked when XML data is being parsed.
     */
    public XMLParser(XMLDataHandler dataHandler) {
        this.dataHandler = dataHandler;
        try {
            //use the default (non-validating) parser
            factory = SAXParserFactory.newInstance();
            
            //create a parser
            saxParser = factory.newSAXParser();
            
            XMLReader parser = saxParser.getXMLReader(); 
            try {
            	parser.setFeature("http://xml.org/sax/features/validation", false);
            	//parser.setFeature("http://xml.org/sax/features/external-general-entities", false);
            	//parser.setFeature("http://xml.org/sax/features/external-parameter-entities", false);
            	} catch (SAXNotRecognizedException e) {
            	  System.err.println("Unknown feature specified: " + e.getMessage());
            	} catch (SAXNotSupportedException e) {
            	  System.err.println("Unsupported feature specified: " + e.getMessage());
            	} catch (SAXException e) {
            	  System.err.println("Error in setting feature: " + e.getMessage());
            	}
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Parses an XML-file and invokes the XML data handler for each tag.
     * @param xmlfile The XML-file to parse.
     * @throws FileNotFoundException If the file isn't found.
     * @throws IOException If an I/O-error occurs while reading file.
     * @throws SAXException If a parse error occurs. Probably
     * because of bad XML-syntax in file.
     */
    public void parseFile(String xmlfile) throws FileNotFoundException,IOException,SAXException {
        saxParser.parse(new File(xmlfile), this);
    }

    /**
     * Parses an XML-string and invokes the XML data handler for each tag.
     * @param xmlstring The XML-string to parse.
     * @throws IOException If an IO error occured while reading the string.
     * @throws SAXException If a parse error occured. Probably because of bad syntax in file.
     */
    public void parseString(String xmlstring) throws IOException, SAXException {
        ByteArrayInputStream xmlStream = new ByteArrayInputStream(xmlstring.getBytes());
        InputSource xmlSource = new InputSource(xmlStream);
        saxParser.parse(xmlSource, this);
    }

    /**
     * Called by SAX parser when start of XML-document has been reached.
     */
    public void startDocument() {
        dataHandler.XMLdocumentStart();
    }

    /**
     * Called by SAX parser when end of XML-document has been reached.
     */
    public void endDocument() {
        dataHandler.XMLdocumentEnd();
    }

    /**
     * Called by SAX parser when start of an XML-element was found.
     * TODO: describe these parameters
     * @param namespaceURI
     * @param lName
     * @param qName
     * @param attrs
     */
    public void startElement(String namespaceURI, String lName, String qName, Attributes attrs) {
        //get element name
        String elementName = lName;
        if (elementName.equals("")) {
            elementName = qName; // namespaceAware = false
        }
        //get attributes
        HashMap attributes = new HashMap(256);
        if (attrs != null) {
            for (int i=0; i<attrs.getLength(); i++) {
                String attributeName = attrs.getLocalName(i); // Attr name
                if (attributeName.equals("")) attributeName = attrs.getQName(i);
                String attributeValue = attrs.getValue(i);
                attributes.put(attributeName, attributeValue);
            }
        }
        dataHandler.XMLstartElement(elementName, attributes);
    }

    /**
     * Called by SAX parser when end of an XML-element was found.
     * TODO: describe these parameters
     * @param namespaceURI
     * @param sName
     * @param qName
     * @throws SAXException
     */
    public void endElement(String namespaceURI, String sName, String qName) throws SAXException {
        String name;
        if (sName.equals("")) {
            name = qName;
        } else {
            name = sName;
        }
        dataHandler.XMLendElement(name);
    }

    /**
     * Called by SAX parser when characters were found inside an element.
     * TODO: describe these parameters
     * @param buf
     * @param offset
     * @param len
     * @throws SAXException
     */
    public void characters(char buf[], int offset, int len) throws SAXException {
        String s = new String(buf, offset, len);
        dataHandler.XMLelementData(s);
    }

}
