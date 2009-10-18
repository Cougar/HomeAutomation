

/**
* All objects in Flinsh that can get data through communcation object 
* should implement this interface.
*/
interface CommunicatorIF {
	
	
	/**
	* 
	* 
	*/
  public function setNewData(data:XMLNode):Void;
	
	/**
	* The communication object will ask this object of the name
	* to know who to set new data.
	* @return The name of object.
	*/
	public function getName():String;
	
	public function setCommunicator(commObj:CommunicationObj):Void;
	
}

