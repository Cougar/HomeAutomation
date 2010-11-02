
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
		//log("Interval:" + id + "> Triggered!\n");
		Interval.myIntervals[id].triggerCallback();
	}
	else
	{
		log("Interval:" + id + "> Encountered a thread that have no javascript Interval object. This is not supposed to be possible...\n");
	}
}

Interval.prototype.triggerCallback = function()
{
	this.myCallback();
}

Interval.prototype.setCallback = function(callback)
{
	this.myCallback = callback;
}

Interval.prototype.start = function()
{
	if (!this.myIsRunning)
	{
		this.myIsRunning = true;
		this.myId = startIntervalThread(this.myTimeout, this.mySingle)
		Interval.myIntervals[this.myId] = this;
	}
}

Interval.prototype.stop = function()
{
	if (this.myIsRunning)
	{
		this.myIsRunning = false;
		
		if (!stopIntervalThread(this.myId))
		{
			log("Interval:" + this.myId + "> Interval thread seems to have already been stopped.\n");
		}
		
		delete Interval.myIntervals[this.myId];
		this.myId = null;
	}
}
