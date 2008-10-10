import items.*;

import java.util.ArrayList;
import java.util.Iterator;


public class ListOutput {
	public void listAll(Database db)
	{
		this.listCommandTypes(db);
		this.listSendingTypes(db);
		this.listDirectionTypes(db);
		this.listCommands(db);
		this.listClasses(db);
		this.listModules(db);
	}
	
	public void listCommands(Database db)
	{
		ArrayList<Command> commands = db.getCommands();
		
		System.out.println("Found " + commands.size() + " commands.");
		System.out.println("===============================");
		
		Iterator<Command> iterator = commands.iterator();
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
	
	public void listClasses(Database db)
	{
		ArrayList<CanClass> classes = db.getClasses();
		
		System.out.println("Found " + classes.size() + " classes.");
		System.out.println("===============================");
		
		Iterator<CanClass> iterator = classes.iterator();
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
	
	public void listModules(Database db)
	{
		ArrayList<Module> modules = db.getModules();
		
		System.out.println("Found " + modules.size() + " module types.");
		System.out.println("===============================");
		
		Iterator<Module> iterator = modules.iterator();
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
	
	public void listCommandTypes(Database db)
	{
		ArrayList<CommandType> commandTypes = db.getCommandTypes();
		
		Iterator<CommandType> iterator = commandTypes.iterator();
		
		System.out.println("Found " + commandTypes.size() + " command types.");
		System.out.println("===============================");
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
	
	public void listSendingTypes(Database db)
	{
		ArrayList<SendingType> sendingTypes = db.getSendingTypes();
		
		Iterator<SendingType> iterator = sendingTypes.iterator();
		
		System.out.println("Found " + sendingTypes.size() + " sending types.");
		System.out.println("===============================");
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
	
	public void listDirectionTypes(Database db)
	{
		ArrayList<DirectionType> directionTypes = db.getDirectionTypes();
		
		Iterator<DirectionType> iterator = directionTypes.iterator();
		
		System.out.println("Found " + directionTypes.size() + " direction types.");
		System.out.println("===============================");
		
		while (iterator.hasNext())
		{
			System.out.println(iterator.next().toString());
		}
	}
}
