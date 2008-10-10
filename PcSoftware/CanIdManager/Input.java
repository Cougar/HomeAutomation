import items.*;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Iterator;


public class Input
{
	public static void addClass(Database db) throws Exception
	{
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		
		System.out.println("You want to create a new class, please answer the questions.");
		
		String nameString = "";
		String descriptionString = "";
		
		while (true)
		{
			System.out.print("Name: ");
			
			nameString = br.readLine().trim();
		
			if (nameString.length() == 0)
			{
				System.out.println("Name can not be empty");
			}
			else
			{
				if (db.getClassByName(nameString) != null)
				{
					System.out.println("A class by that name already exists, enter another name");
					nameString = "";
				}
				else
				{
					break;
				}
			}
		}
		
		System.out.print("Description: ");
		descriptionString = br.readLine().trim();
		
		CanClass canClass = new CanClass();
		canClass.setName(nameString);
		canClass.setDescription(descriptionString);
		canClass.setId(db.getNewClassId(canClass));
		
		System.out.println("Adding new class");
		db.addClass(canClass);
		System.out.println("Successfully added class:");
		System.out.println(canClass.toString());
	}
	
	public static void addCommand(Database db) throws Exception
	{
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		
		System.out.println("You want to create a new command, please answer the questions.");
		
		String nameString = "";
		CommandType commandType;
		String descriptionString = "";
		String moduleString = "";
		
		System.out.println("Avaiable command types:");
		
		Iterator<CommandType> iterator = db.getCommandTypes().iterator();
		
		while (iterator.hasNext())
		{
			System.out.println("* " + iterator.next().getName());
		}
		
		while (true)
		{
			System.out.print("Type: ");
			
			String typeString = br.readLine().trim();
		
			if (typeString.length() == 0)
			{
				System.out.println("Type can not be empty");
			}
			else
			{
				commandType = db.getCommandType(typeString);
				if (commandType == null)
				{
					System.out.println("That type does not exist, choose one from the list above");
				}
				else
				{
					break;
				}
			}
		}
		
		while (true)
		{
			System.out.print("Name: ");
			
			nameString = br.readLine().trim();
		
			if (nameString.length() == 0)
			{
				System.out.println("Name can not be empty");
			}
			else
			{
				Command command = db.getCommandByName(nameString);
				if (command != null)
				{
					if (command.getType().getName().compareTo(commandType.getName()) == 0)
					{
						System.out.println("A command by that name already exists, enter another name");
						nameString = "";
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		
		
		
		if (commandType.getName().compareTo("module") == 0)
		{
			while (true)
			{
				System.out.print("Module name: ");
				
				moduleString = br.readLine().trim();
			
				if (moduleString.length() == 0)
				{
					System.out.println("Module name can not be empty");
				}
				else
				{
					///FIXME: Maybe check against existing modules?
					break;
				}
			}
		}
		
		System.out.print("Description: ");
		descriptionString = br.readLine().trim();
		
		Command command = new Command();
		command.setName(nameString);
		command.setModule(moduleString);
		command.setDescription(descriptionString);
		command.setType(commandType);
		command.setId(db.getNewCommandId(command));
		
		System.out.println("Adding new command");
		db.addCommand(command);
		System.out.println("Successfully added command:");
		System.out.println(command.toString());
	}
	
	public static void addModule(Database db) throws Exception
	{
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		
		System.out.println("You want to create a new module type, please answer the questions.");
		
		String classString = "";
		String nameString = "";
		String descriptionString = "";
		
		while (true)
		{
			System.out.print("Name: ");
			
			nameString = br.readLine().trim();
		
			if (nameString.length() == 0)
			{
				System.out.println("Name can not be empty");
			}
			else
			{
				if (db.getModuleByName(nameString) != null)
				{
					System.out.println("A module type by that name already exists, enter another name");
					nameString = "";
				}
				else
				{
					break;
				}
			}
		}
		
		System.out.println("Avaiable classes:");
		
		Iterator<CanClass> iterator = db.getClasses().iterator();
		
		while (iterator.hasNext())
		{
			System.out.println("* " + iterator.next().getName());
		}
		
		while (true)
		{
			System.out.print("Class: ");
			
			classString = br.readLine().trim();
		
			if (classString.length() == 0)
			{
				System.out.println("Class can not be empty");
			}
			else
			{
				if (db.getClassByName(classString) == null)
				{
					System.out.println("That class does not exist, choose one from the list above");
				}
				else
				{
					break;
				}
			}
		}
		
		System.out.print("Description: ");
		descriptionString = br.readLine().trim();
		
		Module module = new Module();
		module.setName(nameString);
		module.setDescription(descriptionString);
		module.setId(db.getNewModuleId(module));
		module.setCanClass(db.getClassByName(classString));
		
		System.out.println("Adding new module type");
		db.addModule(module);
		System.out.println("Successfully added module type:");
		System.out.println(module.toString());
	}
}
