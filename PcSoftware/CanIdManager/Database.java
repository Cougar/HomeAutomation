import java.io.BufferedReader;
import java.io.FileReader;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;

import items.*;

public class Database {
	private myXML rootXML;
	private String filename;
	private boolean hasChanged;
	private String changeLog;
	
	public Database(String filename) throws Exception
	{
		this.filename = filename;
		
		FileReader fr = new FileReader(this.filename);
		
		BufferedReader bf = new BufferedReader(fr);
		
		this.rootXML = new myXML(bf);
		
		bf.close();
		
		fr.close();
		
		this.hasChanged = false;
		this.changeLog = "";
	}
	
	public boolean hasChanged()
	{
		return this.hasChanged;
	}
	
	public String getChangeLog()
	{
		return this.changeLog;
	}
	
	public void saveChanges() throws Exception
	{
		this.saveChanges(this.filename);
	}
	
	public void saveChanges(String filename) throws Exception
	{
		SimpleDateFormat formatter = new SimpleDateFormat("EEE, dd-MMM-yyyy HH:mm:ss");
		String formattedDate = formatter.format(new Date());
		
		this.rootXML.setComment("Written by CanIdManager on " + formattedDate);
		
		PrintWriter pw = new PrintWriter(filename);
		
		this.rootXML.serialize(pw);
		
		pw.close();
	}
	
	/* =================== */
	/* == Class methods == */
	/* =================== */
	
	public CanClass getClassByName(String name)
	{
		ArrayList<CanClass> classes = this.getClasses();
		
		Iterator<CanClass> iterator = classes.iterator();
		
		while (iterator.hasNext())
		{
			CanClass canClass = iterator.next();
			
			if (canClass.getName().compareTo(name) == 0)
			{
				return canClass;
			}
		}
		
		return null;
	}
	
	public ArrayList<CanClass> getClasses()
	{
		ArrayList<CanClass> classes = new ArrayList<CanClass>();
		
		myXML elementClasses = this.rootXML.findElement("root").findElement("classes");
		
		for (int n = 0; n < elementClasses.size(); n++)
		{
			myXML elementClass = elementClasses.getElement(n);
			
			CanClass canClass = new CanClass();
			canClass.setId(Integer.parseInt(elementClass.Attribute.find("id")));
			canClass.setName(elementClass.Attribute.find("name"));
			canClass.setDescription(elementClass.Attribute.find("description"));
			
			classes.add(canClass);
		}
		
		return classes;
	}
	
	public int getNewClassId(CanClass inputCanClass)
	{
		ArrayList<Integer> takenIds = new ArrayList<Integer>();
		
		Iterator<CanClass> iterator = this.getClasses().iterator();
		
		while (iterator.hasNext())
		{
			takenIds.add(iterator.next().getId());
		}

		int id = 0;
		
		while (takenIds.contains(id))
		{
			id++;
		}
		
		if (id > 32)
		{
			System.err.println("Warning: No more ids are available for this classes");
		}
		
		return id;
	}
	
	public void addClass(CanClass canClass) throws Exception
	{
		myXML elementClasses = this.rootXML.findElement("root").findElement("classes");
		
		myXML elementClass = elementClasses.addElement("class");
		elementClass.Attribute.add("id", canClass.getId());
		elementClass.Attribute.add("name", canClass.getName());
		elementClass.Attribute.add("description", canClass.getDescription());
		
		this.hasChanged = true;
		this.changeLog += "Added new class: " + canClass.getName() + " with id " + canClass.getId() + "\n";
	}
		
	/* ===================== */
	/* == Command methods == */
	/* ===================== */
	
	public Command getCommandByName(String name)
	{
		ArrayList<Command> commands = this.getCommands();
		
		Iterator<Command> iterator = commands.iterator();
		
		while (iterator.hasNext())
		{
			Command command = iterator.next();
			
			if (command.getName().compareTo(name) == 0)
			{
				return command;
			}
		}
		
		return null;
	}
	
	public ArrayList<Command> getCommands()
	{
		ArrayList<Command> commands = new ArrayList<Command>();
		
		myXML elementCommands = this.rootXML.findElement("root").findElement("commands");
		
		for (int n = 0; n < elementCommands.size(); n++)
		{
			myXML elementCommand = elementCommands.getElement(n);
			
			Command command = new Command();
			command.setId(Integer.parseInt(elementCommand.Attribute.find("id")));
			command.setName(elementCommand.Attribute.find("name"));
			command.setDescription(elementCommand.Attribute.find("description"));
			
			String type = elementCommand.Attribute.find("type");
			
			CommandType commandType = this.getCommandType(type);
			
			if (commandType == null)
			{
				System.err.println("Warning: Found command with invalid command type: Name=" + command.getName() + ", Type="+type);
			}
			
			command.setType(commandType);
			
			commands.add(command);
		}
		
		return commands;
	}
	
	public int getNewCommandId(Command inputCommand)
	{
		ArrayList<Integer> takenIds = new ArrayList<Integer>();
		
		Iterator<Command> iterator = this.getCommands().iterator();
		
		while (iterator.hasNext())
		{
			Command command = iterator.next();
			
			// First check if the type is the same, if it is maybe there is an id taken
			if (command.getType().getName().compareTo(inputCommand.getType().getName()) == 0)
			{
				// Then check if type is module, then we must also check module name
				if (inputCommand.getType().getName().compareTo("module") == 0)
				{
					// Check if module name is the same, then we have an id taken
					if (command.getModule().compareTo(inputCommand.getModule()) == 0)
					{
						takenIds.add(command.getId());
					}
				}
				else
				{
					takenIds.add(command.getId());
				}
			}
		}

		int id = inputCommand.getType().getMinId();
		
		while (takenIds.contains(id))
		{
			id++;
		}
		
		if (id > inputCommand.getType().getMaxId())
		{
			System.err.println("Warning: No more ids are available for this type: " + inputCommand.getType().getName());
		}
		
		return id;
	}
	
	public void addCommand(Command command) throws Exception
	{
		myXML elementCommands = this.rootXML.findElement("root").findElement("commands");
		
		myXML elementCommand = elementCommands.addElement("command");
		elementCommand.Attribute.add("id", command.getId());
		elementCommand.Attribute.add("name", command.getName());
		elementCommand.Attribute.add("type", command.getType().getName());
		elementCommand.Attribute.add("module", command.getModule());
		elementCommand.Attribute.add("description", command.getDescription());
		
		this.hasChanged = true;
		this.changeLog += "Added new command: " + command.getName() + " with id " + command.getId() + "\n";
	}
	
	/* ==================== */
	/* == Module methods == */
	/* ==================== */
	
	public Module getModuleByName(String name)
	{
		ArrayList<Module> modules = this.getModules();
		
		Iterator<Module> iterator = modules.iterator();
		
		while (iterator.hasNext())
		{
			Module module = iterator.next();
			
			if (module.getName().compareTo(name) == 0)
			{
				return module;
			}
		}
		
		return null;
	}
	
	public ArrayList<Module> getModules()
	{
		ArrayList<Module> modules = new ArrayList<Module>();
		
		myXML elementClasses = this.rootXML.findElement("root").findElement("classes");
		
		for (int n = 0; n < elementClasses.size(); n++)
		{
			myXML elementClass = elementClasses.getElement(n);
			
			CanClass canClass = new CanClass();
			canClass.setId(Integer.parseInt(elementClass.Attribute.find("id")));
			canClass.setName(elementClass.Attribute.find("name"));
			canClass.setDescription(elementClass.Attribute.find("description"));
			
			myXML elementModules = elementClass.findElement("modules");

			if (elementModules == null)
			{
				continue;
			}

			for (int m = 0; m < elementModules.size(); m++)
			{
				myXML elementModule = elementModules.getElement(m);
				
				Module module = new Module();
				module.setId(Integer.parseInt(elementModule.Attribute.find("id")));
				module.setName(elementModule.Attribute.find("name"));
				module.setDescription(elementModule.Attribute.find("description"));
				module.setCanClass(canClass);

				myXML elementSupportedCommands = elementModule.findElement("supportedCommands");
				
				if (elementSupportedCommands == null)
				{
					continue;
				}

				for (int s = 0; s < elementSupportedCommands.size(); s++)
				{
					myXML elementSupportedCommand = elementSupportedCommands.getElement(s);

					SupportedCommand supportedCommand = new SupportedCommand();
					supportedCommand.setName(elementSupportedCommand.Attribute.find("name"));
					
					DirectionType directionType = this.getDirectionType(elementSupportedCommand.Attribute.find("direction"));
					
					if (directionType == null)
					{
						System.err.println("Warning: Direction type is invalid for supported command: " + supportedCommand.toString());
					}
					
					supportedCommand.setDirection(directionType);
					
					myXML elementSendingTypes = elementSupportedCommand.findElement("sendingTypes");
					
					if (elementSendingTypes == null)
					{
						continue;
					}

					for (int t = 0; t < elementSendingTypes.size(); t++)
					{
						myXML elementSendingType = elementSendingTypes.getElement(t);
						
						SendingType sendingType = this.getSendingType(elementSendingType.Attribute.find("name"));
						
						if (sendingType == null)
						{
							System.err.println("Warning: Sending type is invalid for supported command: " + supportedCommand.toString());
						}

						supportedCommand.addSendingTypes(sendingType);
					}
					
					supportedCommand.setModule(module);
					supportedCommand.setCanClass(canClass);
					
					module.addSupportedCommand(supportedCommand);
				}
					
				modules.add(module);
			}
		}		
		
		return modules;
	}
	
	public int getNewModuleId(Module inputModule)
	{
		ArrayList<Integer> takenIds = new ArrayList<Integer>();
		
		Iterator<Module> iterator = this.getModules().iterator();
		
		while (iterator.hasNext())
		{
			Module module = iterator.next();
			
			// First check if the class is the same, if it is maybe there is an id taken
			if (module.getClass().getName().compareTo(inputModule.getClass().getName()) == 0)
			{
				takenIds.add(module.getId());
			}
		}

		int id = 0;
		
		while (takenIds.contains(id))
		{
			id++;
		}
		
		if (id > 255)
		{
			System.err.println("Warning: No more ids are available for this module type: " + inputModule.getName());
		}
		
		return id;
	}
	
	public void addModule(Module module) throws Exception
	{
		myXML elementClasses = this.rootXML.findElement("root").findElement("classes");

		for (int n = 0; n < elementClasses.size(); n++)
		{
			myXML elementClass = elementClasses.getElement(n);
			
			if (elementClass.Attribute.find("name").compareTo(module.getCanClass().getName()) == 0)
			{
				myXML elementModules = elementClass.findElement("modules");
				if (elementModules == null)
				{
					elementModules = elementClass.addElement("modules");
				}

				myXML elementModule = elementModules.addElement("module");
				elementModule.Attribute.add("id", module.getId());
				elementModule.Attribute.add("name", module.getName());
				elementModule.Attribute.add("description", module.getDescription());

				myXML elementSupportedCommands = elementModule.addElement("supportedCommands");

				ArrayList<SupportedCommand> supportedCommands = module.getSupportedCommands();

				Iterator<SupportedCommand> iterator = supportedCommands.iterator();

				while (iterator.hasNext())
				{
					SupportedCommand supportedCommand = iterator.next();
					
					myXML elementSupportedCommand = elementSupportedCommands.addElement("supportedCommand");
					elementSupportedCommand.Attribute.add("name", supportedCommand.getName());
					elementSupportedCommand.Attribute.add("direction", supportedCommand.getDirection().getName());
					
					myXML elementSendingTypes = elementModule.addElement("sendingTypes");
					
					ArrayList<SendingType> sendingTypes = supportedCommand.getSendingTypes();
					Iterator<SendingType> iterator2 = sendingTypes.iterator();

					while (iterator2.hasNext())
					{
						SendingType sendingType = iterator2.next();
						
						myXML elementSendingtype = elementSendingTypes.addElement("sendingType");
						elementSendingtype.Attribute.add("name", sendingType.getName());
					}	
				}
				
				break;
			}
		}

		this.hasChanged = true;
		this.changeLog += "Added new module: " + module.getName() + " with id " + module.getId() + "\n";
	}
	
	/* ========================== */
	/* == Command type methods == */
	/* ========================== */
	
	public CommandType getCommandType(String name)
	{
		ArrayList<CommandType> commandTypes = this.getCommandTypes();
		
		Iterator<CommandType> iterator = commandTypes.iterator();
		
		while (iterator.hasNext())
		{
			CommandType commandType = iterator.next();
			
			if (commandType.getName().compareTo(name) == 0)
			{
				return commandType;
			}
		}
		
		return null;
	}
	
	public ArrayList<CommandType> getCommandTypes()
	{
		ArrayList<CommandType> commandTypes = new ArrayList<CommandType>();

		commandTypes.add(new CommandType("management", "", "NMT", 0, 255));
		commandTypes.add(new CommandType("global", "", "GLOB", 0, 63));
		commandTypes.add(new CommandType("physical", "", "PHYS", 64, 127));
		commandTypes.add(new CommandType("module", "", "MOD", 128, 191));
		
		return commandTypes;
	}

	/* ========================== */
	/* == Sending type methods == */
	/* ========================== */
	
	public SendingType getSendingType(String name)
	{
		ArrayList<SendingType> sendingTypes = this.getSendingTypes();
		
		Iterator<SendingType> iterator = sendingTypes.iterator();
		
		while (iterator.hasNext())
		{
			SendingType sendingType = iterator.next();
			
			if (sendingType.getName().compareTo(name) == 0)
			{
				return sendingType;
			}
		}
		
		return null;
	}
	
	public ArrayList<SendingType> getSendingTypes()
	{
		ArrayList<SendingType> sendingTypes = new ArrayList<SendingType>();

		sendingTypes.add(new SendingType("request", ""));
		sendingTypes.add(new SendingType("event", ""));
		sendingTypes.add(new SendingType("interval", ""));
		
		return sendingTypes;
	}
	
	/* ============================ */
	/* == Direction type methods == */
	/* ============================ */
	
	public DirectionType getDirectionType(String name)
	{
		ArrayList<DirectionType> directionTypes = this.getDirectionTypes();
		
		Iterator<DirectionType> iterator = directionTypes.iterator();
		
		while (iterator.hasNext())
		{
			DirectionType directionType = iterator.next();
			
			if (directionType.getName().compareTo(name) == 0)
			{
				return directionType;
			}
		}
		
		return null;
	}
	
	public ArrayList<DirectionType> getDirectionTypes()
	{
		ArrayList<DirectionType> directionTypes = new ArrayList<DirectionType>();

		directionTypes.add(new DirectionType("request", ""));
		directionTypes.add(new DirectionType("event", ""));
		directionTypes.add(new DirectionType("interval", ""));
		
		return directionTypes;
	}
}
