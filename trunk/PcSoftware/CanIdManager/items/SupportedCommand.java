package items;

import java.util.ArrayList;

public class SupportedCommand
{
	private String name;
	private DirectionType direction;
	private ArrayList<SendingType> sendingTypes;
	private Module module;
	private CanClass canClass;
	
	public SupportedCommand()
	{
		this.sendingTypes = new ArrayList<SendingType>();
	}
	
	public String getName()
	{
		return name;
	}

	public void setName(String name)
	{
		this.name = name;
	}

	public DirectionType getDirection()
	{
		return direction;
	}

	public void setDirection(DirectionType direction)
	{
		this.direction = direction;
	}

	public ArrayList<SendingType> getSendingTypes()
	{
		return sendingTypes;
	}

	public void setSendingTypes(ArrayList<SendingType> sendingTypes)
	{
		this.sendingTypes = sendingTypes;
	}
	
	public void addSendingTypes(SendingType sendingType)
	{
		this.sendingTypes.add(sendingType);
	}
	
	public Module getModule()
	{
		return module;
	}

	public void setModule(Module module)
	{
		this.module = module;
	}

	public CanClass getCanClass()
	{
		return canClass;
	}

	public void setCanClass(CanClass canClass)
	{
		this.canClass = canClass;
	}

	public String getSendingTypesString()
	{
		String result = "";
		
		for (int j = 0; j < this.sendingTypes.size(); j++)
		{
			result += this.sendingTypes.get(j).getName();
			
			if (j+1 < this.sendingTypes.size())
			{
				result += ",";
			}
		}
		
		return result;
	}
	
	public String toString()
	{
		return this.getName() + " - Direction: " + this.getDirection().getName() + ", Sending types(" + this.getSendingTypesString() + ")\n";
	}
}
