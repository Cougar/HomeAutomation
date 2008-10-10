package items;

public class CanClass
{
	private int id;
	private String name;
	private String description;
	
	public CanClass() { }
	
	public CanClass(int id, String name, String description)
	{
		this.id = id;
		this.name = name;
		this.description = description;
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

	public String getDescription()
	{
		return description;
	}

	public void setDescription(String description)
	{
		this.description = description;
	}
	
	public String getCDefineName()
	{
		return "CAN_CLASS_MODULE_" +  this.name.toUpperCase();
	}
	
	public String toString()
	{
		String buffer = "";
		buffer += this.getName() + " (" + this.getId() + ")\n";
		buffer += "C define name: " + this.getCDefineName() + "\n";
		buffer += "Description: " + this.getDescription() + "\n";
		return buffer;
	}
}
