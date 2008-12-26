
function Interval(callback, timeout)
{
	this.myCallback = callback;
	this.myTimeout = timeout;
	this.myIsRunning = false;
}

Interval.myIntervals = new Array();
Interval.prototype.myIsRunning = null;
Interval.prototype.myId = null;
Interval.prototype.myTimeout = null;
Interval.prototype.myCallback = null;

Interval.triggerIntervalCallback = function(id)
{
	if (Interval.myIntervals[id])
	{
		Interval.myIntervals[id].triggerCallback();
	}
	else
	{
		log("Interval:" + id + "> Encountered a thread that have no javascript Interval object. This is not supposed to be possible, report this.\n");
		
		if (!stopIntervalThread(id))
		{
			log("Interval:" + id + "> Interval thread could not be stopped.\n");
		}
	}
}

Interval.prototype.triggerCallback = function()
{
	this.myCallback();
}

Interval.prototype.setTimeout = function(timeout)
{
	var isRunning = this.myIsRunning;
	if (isRunning)
	{
		this.stop();
	}

	this.myTimeout = timeout;
	
	if (isRunning)
	{
		this.start();
	}
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
		this.myId = startIntervalThread(this.myTimeout)
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
			log("Interval:" + this.myId + "> Interval thread could not be stopped.\n");
		}
		
		delete Interval.myIntervals[this.myId];
		this.myId = null;
	}
}
