package items;

public class CommandType
{
	private String name;
	private String description;
	private String cPrefix;
	private int minId;
	private int maxId;

	public CommandType() {}
	
	public CommandType(String name, String description, String cPrefix, int minId, int maxId)
	{
		this.name = name;
		this.description = description;
		this.cPrefix = cPrefix;
		this.minId = minId;
		this.maxId = maxId;
	}

	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public String getCPrefix()
	{
		return cPrefix;
	}

	public void setCPrefix(String cPrefix)
	{
		this.cPrefix = cPrefix;
	}

	public String getDescription()
	{
		return description;
	}

	public void setDescription(String description)
	{
		this.description = description;
	}
	
	public int getMinId()
	{
		return this.minId;
	}
	
	public void setMinId(int minId)
	{
		this.minId = minId;
	}
	
	public int getMaxId()
	{
		return this.maxId;
	}

	public void setMaxId(int maxId)
	{
		this.maxId = maxId;
	}
	
	public String toString()
	{
		String buffer = "";
		buffer += this.getName() + " (" + this.getMinId() + " to " + this.getMaxId() + ")\n";
		buffer += "C define prefix: " + this.getCPrefix() + "\n";
		buffer += "Description: " + this.getDescription() + "\n";
		return buffer;
	}
}
