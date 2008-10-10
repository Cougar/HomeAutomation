import java.io.BufferedWriter;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.*;

import utils.*;

/**
 * A program for managing the can id database
 * 
 * @author      Mattias Runge
 * @version     0.1
 */
public class CanIdManager
{
	/**
	 * Main for this program
	 * 
	 * @param args	The arguments specified at the command line
	 */
	public static void main(String[] args)
	{
		System.out.println("Can Id Manager, version 0.1");
		System.out.println();
		
		ArrayList<String> validFlags = new ArrayList<String>();
		
		validFlags.add("l");
		validFlags.add("r");
		validFlags.add("a");
		
		String changelogFilename = "../../EmbeddedSoftware/AVR/avr-lib/drivers/can/modules/changes.txt";
		String xmlFilename = "../../EmbeddedSoftware/AVR/avr-lib/drivers/can/modules/canId.xml";
		String cheaderFilename = "../../EmbeddedSoftware/AVR/avr-lib/drivers/can/modules/canId.h";
		
		try
		{
			Runtime runtime = Runtime.getRuntime();
		
			System.out.println("Running svn update on " + xmlFilename);
			
			Process proc = runtime.exec("svn update " + xmlFilename);
		
			proc.waitFor();
			
			System.out.println("svn update complete");
			System.out.println();
		
			CommandLineParse clp = new CommandLineParse(args, validFlags);
			
			// Check for action flags, one must be set
			if (!clp.isFlagSet("l") && !clp.isFlagSet("r") && !clp.isFlagSet("a"))
			{
				System.out.println("Argument error: You must specify one and only one action flag");
				printCommandHelp();
				System.exit(1);
			}
			
			Database db = new Database(xmlFilename);
			
			if (clp.isFlagSet("l"))
			{
				ArrayList<String> arguments = clp.getFlagArguments("l");
			
				ListOutput listOutput = new ListOutput();
				
				if (arguments.size() == 0)
				{
					listOutput.listAll(db);
				}
				else
				{
					if (arguments.get(0).compareTo("commands") == 0)
					{
						listOutput.listCommands(db);
					}
					else if (arguments.get(0).compareTo("classes") == 0)
					{
						listOutput.listClasses(db);
					}
					else if (arguments.get(0).compareTo("modules") == 0)
					{
						listOutput.listModules(db);
					}
					else if (arguments.get(0).compareTo("commandtypes") == 0)
					{
						listOutput.listCommandTypes(db);
					}
					else if (arguments.get(0).compareTo("sendingtypes") == 0)
					{
						listOutput.listSendingTypes(db);
					}
					else if (arguments.get(0).compareTo("directiontypes") == 0)
					{
						listOutput.listDirectionTypes(db);
					}
					else
					{
						System.out.println("Argument error: Unknown listing type specified:" + arguments.get(0));
						printCommandHelp();
						System.exit(1);
					}
				}
			}
			else if (clp.isFlagSet("r"))
			{
				CHeader cHeader = new CHeader(cheaderFilename);
				cHeader.build(db);
			}
			else if (clp.isFlagSet("a"))
			{
				ArrayList<String> arguments = clp.getFlagArguments("a");
				
				if (arguments.size() == 0)
				{
					System.out.println("Argument error: You must specify what you want to add");
					printCommandHelp();
					System.exit(1);
				}
				
				if (arguments.get(0).compareTo("command") == 0)
				{
					Input.addCommand(db);
				}
				else if (arguments.get(0).compareTo("class") == 0)
				{
					Input.addClass(db);
				}
				else if (arguments.get(0).compareTo("module") == 0)
				{
					Input.addModule(db);
				}
			}

			if (db.hasChanged())
			{
				System.out.println("Writing new " + xmlFilename);
				
				db.saveChanges(xmlFilename);
				
				FileWriter fstream = new FileWriter(changelogFilename);
				BufferedWriter out = new BufferedWriter(fstream);
				out.write(db.getChangeLog());
				out.close();
	
				System.out.println("Running svn commit on " + xmlFilename);
				
				proc = runtime.exec("svn --file " + changelogFilename + " commit " + xmlFilename);

				proc.waitFor();
				
				System.out.println("svn commit complete");
				System.out.println();
				
				CHeader cHeader = new CHeader(cheaderFilename);
				cHeader.build(db);
			}
		}
		catch (Exception e)
		{
			System.out.println(e.getMessage());
			System.exit(1);
		}
	}
	
	/**
	 * Prints the help text to standard out
	 *
	 */
	private static void printCommandHelp()
	{
		System.out.println("usage: CanIdManager [-l [commands|classes|modules|commandtypes|sendingtypes|directiontypes]] [-r] [-a [command|class|module]");// [-d library name] [-d [suffix_1, suffix_2, ..., suffix_n]]");
	}
}
