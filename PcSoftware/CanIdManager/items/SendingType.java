package items;

public class SendingType
{
	private String name;
	private String description;
	
	public SendingType(String name, String description)
	{
		this.name = name;
		this.description = description;
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
	
	public String toString()
	{
		String buffer = "";
		buffer += this.getName() + "\n";
		buffer += "Description: " + this.getDescription() + "\n";
		return buffer;
	}
}
