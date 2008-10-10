package items;

import java.util.ArrayList;

public class Module
{
	private int id;
	private String name;
	private CanClass canClass;
	private String description;
	private ArrayList<SupportedCommand> supportedCommands;
	
	public Module()
	{
		this.supportedCommands = new ArrayList<SupportedCommand>();
	}
	
	public Module(int id, String name, CanClass canClass, String description)
	{
		this.id = id;
		this.name = name;
		this.canClass = canClass;
		this.description = description;
		this.supportedCommands = new ArrayList<SupportedCommand>();
	}

	public int getId()
	{
		return this.id;
	}

	public void setId(int id)
	{
		this.id = id;
	}

	public String getName()
	{
		return this.name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public CanClass getCanClass()
	{
		return this.canClass;
	}

	public void setCanClass(CanClass canClass)
	{
		this.canClass = canClass;
	}

	public String getDescription()
	{
		return this.description;
	}

	public void setDescription(String description)
	{
		this.description = description;
	}
	
	public ArrayList<SupportedCommand> getSupportedCommands()
	{
		return this.supportedCommands;
	}

	public void setSupportedCommands(ArrayList<SupportedCommand> supportedCommands)
	{
		this.supportedCommands = supportedCommands;
	}
	
	public void addSupportedCommand(SupportedCommand supportedCommand)
	{
		this.supportedCommands.add(supportedCommand);
	}

	public String getCDefineName()
	{
		return "CAN_TYPE_MODULE_" + this.name;
	}
	
	public String getSupportedCommandsString()
	{
		String result = "";
		
		for (int n = 0; n < this.supportedCommands.size(); n++)
		{
			result += this.supportedCommands.get(n).toString();
		}
		
		return result;
	}
	
	public String toString()
	{
		String buffer = "";
		buffer += this.getName() + " (" + this.getId() + ")\n";
		buffer += "C define name: " + this.getCDefineName() + "\n";
		buffer += "Class: " + this.getCanClass().getName() + "\n";
		buffer += "Supported commands:\n";
		buffer += this.getSupportedCommandsString();
		buffer += "Description: " + this.getDescription() + "\n";
		return buffer;
	}
}
