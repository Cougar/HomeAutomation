package utils;

import java.util.*;


/**
 * This class parses arguments from the command line and 
 * make them easely accessable.
 * 
 * @author      Mattias Runge
 * @author      Kristian Holmberg
 * @version     1.1
 */
public class CommandLineParse
{
	private ArrayList<Flag> myFlags = new ArrayList<Flag>();

	/**
	 * Constructor for this class
	 * 
	 * @param args	The arguments specified at the command line
	 * @throws CommandLineParseException	Exception thrown if the input was bad
	 */
	public CommandLineParse(String[] args, ArrayList<String> validFlags) throws CommandLineParseException
	{
		int c = 0;
		
		Flag lastFlag = null;
		
		while (c < args.length)
		{
			if (args[c].substring(0, 1).compareTo("-") == 0)
			{
				lastFlag = new Flag(args[c].substring(1));
				
				if (!validFlags.contains(lastFlag.getName()))
				{
					throw new CommandLineParseException("Invalid flag: " + lastFlag.getName());
				}
				
				//System.out.println("DEBUG: flags.add(" + args[c].substring(1) + ")");
				myFlags.add(lastFlag);
			}
			else if (lastFlag != null)
			{
				lastFlag.addArgument(args[c]);
				//System.out.println("DEBUG: lastFlag.addArgument(" + args[c] + ") - " + lastFlag.getName());
			}
			else
			{
				throw new CommandLineParseException("Unknown flag: " + args[c]);
			}
			
			c++;
		}
	}
	
	/**
	 * Method for checking if a certain flag was set
	 * 
	 * @param flagName	The name of the flag to check
	 * @return Returns true if the flag was set otherwise false
	 */
	public boolean isFlagSet(String flagName)
	{
		//System.out.println("DEBUG: isFlagSet(" + flagName + ")");
		return myFlags.contains(new Flag(flagName));
	}
	
	/**
	 * Method for getting the arguments passed to a certain flag
	 * 
	 * @param flagName	The name of the flag to get the arguments for
	 * @throws CommandLineParseException	Throws an exception if the flag was not found
	 * @return Returns a ArrayList<String> with the arguments
	 */
	public ArrayList<String> getFlagArguments(String flagName) throws CommandLineParseException
	{
		int index = myFlags.indexOf(new Flag(flagName));
		
		if (index == -1)
		{
			throw new CommandLineParseException("Flag name not found");
		}
		
		return myFlags.get(index).getArguments();
	}
	
	/**
	 * Private class a flag
	 * 
	 */
	private class Flag
	{
		private String myName = "";
		private ArrayList<String> myArguments = new ArrayList<String>();
		
		/**
		 * Constructor for the Flag class
		 * 
		 * @param flagName	This is the name of this flag
		 */
		public Flag(String flagName)
		{
			myName = flagName;
		}
		
		/**
		 * Returns the name of this flag
		 * 
		 * @return The name of this flag
		 */
		public String getName()
		{
			return myName;
		}
		
		/**
		 * Method for adding an argument to this flag
		 * 
		 * @param argumentName	The name of the argument to add
		 */
		public void addArgument(String argumentName)
		{
			myArguments.add(argumentName);
		}
		
		/**
		 * Returns the arguments for this flag
		 * 
		 * @return The arguments for this flag
		 */
		public ArrayList<String> getArguments()
		{
			return myArguments;
		}
		
		/**
		 * Method checking if two flags are equal
		 * 
		 * @param flag	Object of the type flag to check against
		 */
		public boolean equals(Object flag)
		{
			String flagName = ((Flag)flag).getName();
			//System.out.println("DEBUG: equals - " + flagName + " == " + name);
			return (myName.compareTo(flagName) == 0);
		}
	}
}
