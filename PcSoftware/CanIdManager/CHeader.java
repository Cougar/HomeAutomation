import items.*;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;


public class CHeader {
	private String filename;
	
	public CHeader(String filename)
	{
		this.filename = filename;
	}
	
	public void build(Database db)
	{
		System.out.println("Trying to rebuild " + this.filename);
		
		try
		{
			FileWriter fstream = new FileWriter(this.filename);
			BufferedWriter out = new BufferedWriter(fstream);
			
			SimpleDateFormat formatter = new SimpleDateFormat("EEE, dd-MMM-yyyy HH:mm:ss");
			String formattedDate = formatter.format(new Date());
			
			out.write("// Built on " + formattedDate + " by CanIdManager\n");
			out.write("\n");
			
			this.outputClasses(out, db);

			this.outputCommands(out, db);
			
			this.outputModules(out, db);
			
			out.close();
			
			System.out.println("You will now need to recompile all you modules and applications ;)");
	    }
		catch (Exception e)
		{
			System.err.println("Failed to write " + this.filename);
			System.err.println("Error: " + e.getMessage());
			System.exit(1);
		}
	}
	
	private void outputClasses(BufferedWriter out, Database db) throws IOException
	{
		out.write("//------------------ //\n");
		out.write("// Class definitions //\n");
		out.write("//------------------ //\n");

		ArrayList<CanClass> classes = db.getClasses();
		
		Iterator<CanClass> iterator = classes.iterator();
		
		while (iterator.hasNext())
		{
			CanClass canClass = iterator.next();
			out.write("#define " + canClass.getCDefineName() + "\t= " + canClass.getId() + " // " + canClass.getDescription() + "\n");
		}
		
		out.write("\n");
	}
	
	private void outputCommands(BufferedWriter out, Database db) throws IOException
	{
		out.write("//-------------------- //\n");
		out.write("// Command definitions //\n");
		out.write("//-------------------- //\n");

		ArrayList<Command> commands = db.getCommands();
		
		Iterator<Command> iterator = commands.iterator();
		
		while (iterator.hasNext())
		{
			Command command = iterator.next();
			out.write("#define " + command.getCDefineName() + "\t= " + command.getId() + " // " + command.getDescription() + "\n");
		}
		
		out.write("\n");
	}
	
	private void outputModules(BufferedWriter out, Database db) throws IOException
	{
		out.write("//------------------- //\n");
		out.write("// Module definitions //\n");
		out.write("//------------------- //\n");
		
		ArrayList<Module> modules = db.getModules();
		
		Iterator<Module> iterator = modules.iterator();
		
		while (iterator.hasNext())
		{
			Module module = iterator.next();
			out.write("#define " + module.getCDefineName() + "\t= " + module.getId() + " // " + module.getDescription() + "\n");
		}
		
		out.write("\n");
	}
}
