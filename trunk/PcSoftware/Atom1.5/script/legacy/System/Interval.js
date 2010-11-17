
function Interval(callback, timeout, single)
{
	if (single == null)
	{
		single = false;
	}
	
	this.myCallback = callback;
	this.myTimeout = timeout;
	this.mySingle = single;
	this.myIsRunning = false;
}

Interval.myIntervals = new Array();
Interval.prototype.myIsRunning = null;
Interval.prototype.myId = null;
Interval.prototype.myTimeout = null;
Interval.prototype.mySingle = null;
Interval.prototype.myCallback = null;

Interval.triggerIntervalCallback = function(id)
{
	if (Interval.myIntervals[id])
	{
		if (Interval.myIntervals[id].myIsRunning)
		{
			//log("Interval:" + id + "> Triggered!\n");
			Interval.myIntervals[id].triggerCallback();
		}
	}
	else
	{
		log("Interval:" + id + "> Encountered a thread that have no javascript Interval object. This is not supposed to be possible...\n");
	}
}

Interval.prototype.triggerCallback = function()
{
	if (this.mySingle)
	{
		this.myIsRunning = false;
	}
	
	this.myCallback();
}

Interval.prototype.setCallback = function(callback)
{
	this.myCallback = callback;
}

Interval.prototype.setTimeout = function(timeout)
{
	this.stop();
	this.myTimeout = timeout;
	this.start();
}

Interval.prototype.reset = function()
{
	this.stop();
	this.start();
}

Interval.prototype.start = function()
{
	if (!this.myIsRunning)
	{
		this.myIsRunning = true;
		
		if (this.mySingle)
		{
			this.myId = SetTimeout(Interval.triggerIntervalCallback, this.myTimeout);
		}
		else
		{
			this.myId = SetInterval(Interval.triggerIntervalCallback, this.myTimeout);
		}
		
		Interval.myIntervals[this.myId] = this;
	}
}

Interval.prototype.stop = function()
{
	if (this.myIsRunning)
	{
		//log("Interval:" + this.myId + "> Interval thread stop.\n");
		
		this.myIsRunning = false;
		
		ClearInterval(this.myId);
		
		delete Interval.myIntervals[this.myId];
		this.myId = null;
	}
}
