package utils;

/**
 * This class is just a simple implementation of our exceptions 
 * 
 * @author      Mattias Runge
 * @author      Kristian Holmberg
 * @version     1.0
 */
public class CommandLineParseException extends Exception
{
	/**
	 * Constructor for this class
	 * 
	 * @param message	The message for this exception
	 */
	public CommandLineParseException(String message)
	{
		super(message);
	}
}
