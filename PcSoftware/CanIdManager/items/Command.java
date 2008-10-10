package items;

public class Command
{
	private int id;
	private String name;
	private CommandType type;
	private String description;
	private String module;
	
	public Command() {}
	
	public Command(int id, String name, CommandType type, String description, String module)
	{
		this.id = id;
		this.name = name;
		this.type = type;
		this.description = description;
		this.module = module;
	}

	public int getId()
	{
		return id;
	}

	public void setId(int id)
	{
		this.id = id;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public CommandType getType()
	{
		return type;
	}

	public void setType(CommandType type)
	{
		this.type = type;
	}

	public String getDescription()
	{
		return description;
	}

	public void setDescription(String description)
	{
		this.description = description;
	}
	
	public String getModule()
	{
		return module;
	}

	public void setModule(String module)
	{
		this.module = module;
	}
	
	public String getCDefineName()
	{
		return "CAN_CMD_MODULE_" + this.type.getCPrefix() + "_" + this.name.toUpperCase();
	}
	
	public String toString()
	{
		String buffer = "";
		buffer += this.getName() + " (" + this.getId() + ")\n";
		buffer += "C define name: " + this.getCDefineName() + "\n";
		buffer += "Type: " + this.getType().getName() + "\n";
		buffer += "Description: " + this.getDescription() + "\n";
		return buffer;
	}
}
