/*
 * File created by Jimmy
 * at 2003-sep-06 18:24:50
 */
package Macbeth.Utilities;

/**
 * An unsigned byte representation. It uses a signed byte
 * for the actual storage, but when converted to short and
 * string, the data will be shown as unsigned, resulting
 * in the output range [0,255] instead of [-128,127]
 * @author Jimmy
 */
public class UByte {
    //The actual byte value (signed)
    private byte byteVal;

    /**
     * Creates a new instance of UByte using the specified
     * (signed) initial value.
     * @param b The initial byte value (signed).
     */
    public UByte(byte b) {
        byteVal = b;
    }

    /**
     * Creates a new instance of UByte.
     */
    public UByte() {
        this((byte)0);
    }

    /**
     * Sets the signed byte value.
     * @param b The signed byte value.
     */
    public void setByteValue(byte b) {
        byteVal = b;
    }

    /**
     * Creates an unsigned byte from a short value.
     * @param s The short value. Must be within the 0-255 range, or else a NumberFormatException will be thrown.
     * @return The unsigned byte.
     */
    public static UByte fromShort(short s) {
		//If s>255 then return the short - 256 (cast to a byte).  Else
		//just return s (cast to a byte).
		return ((s>255) ? (new UByte(((byte)(s-256)))) : (new UByte((byte)s)));
	}

    /**
     * Parses an unsigned byte from a string.
     * @param str The string to parse from. If this isn't in range 0-255, a NumberFormatException will be thrown.
     * @return The UByte.
     * @throws NumberFormatException When string couldn't be parsed as a UByte within the range 0-255.
     */
    public static UByte parseUByte(String str) throws NumberFormatException {
        short s = Short.parseShort(str);
        return fromShort(s);
    }

    /**
     * Gets the unsigned short representation of this byte.
     * @return The unsigned short value. It will be within the 0-255 range.
     */
    public short shortValue() {
        //If b<0 (ie negative) then return b+256 (cast to a short).  Else
        //b is positive, so just return b (cast to a short).
        return ((byteVal<0) ? (short)(byteVal+256) : (short)byteVal);
	}

    /**
     * Gets the signed byte value.
     * @return The signed byte value.
     */
    public byte byteValue() {
        return byteVal;
    }

    /**
     * Gets a string showing the unsigned short representation of this byte.
     * @return The unsigned short representation as a String.
     */
    public String toString() {
        short s = shortValue();
        return Short.toString(s);
    }

    /**
     * Gets a string showing the unsigned short representation of a byte.
     * @param b The byte.
     * @return The unsigned short representation as a String.
     */
    public static String toString(byte b) {
        return Short.toString((new UByte(b)).shortValue());
    }

    /**
     * Checks whether an object is equal to this UByte.
     * @param o The Object to compare with.
     * @return True if equal, otherwise false.
     */
    public boolean equals(Object o)
    {
        //must be instance of this class, and must not be null
        if (o==null || !(o instanceof UByte)) {
          return false;
        }
        // Now compare data fields...
        return ((UByte)o).byteValue()==this.byteValue();
    }

    /**
     * Returns this object's hash code.
     * @return The hash code.
     */
    public int hashCode() {
        return (int)byteValue();
    }
}