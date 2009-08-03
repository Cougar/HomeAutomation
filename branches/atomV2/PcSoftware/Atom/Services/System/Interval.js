
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
		//log("Interval:" + id + "> Triggered!\n");
		Interval.myIntervals[id].triggerCallback();
	}
	else
	{
		log("Interval:" + id + "> Encountered a thread that have no javascript Interval object. This is not supposed to be possible, will try to remove this thread also.\n");
		
		if (!stopIntervalThread(id))
		{
			log("Interval:" + id + "> Interval thread seems to have already been stopped.\n");
		}
	}
}

Interval.prototype.triggerCallback = function()
{
	this.myCallback();
}

Interval.prototype.setTimeout = function(timeout)
{
	if (this.myTimeout != timeout)
	{
		this.myTimeout = timeout;
		setIntervalThreadTimeout(this.myId, timeout);
	}
}

Interval.prototype.reset = function()
{
	setIntervalThreadTimeout(this.myId, this.myTimeout);
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
			log("Interval:" + this.myId + "> Interval thread seems to have already been stopped.\n");
		}
		
		delete Interval.myIntervals[this.myId];
		this.myId = null;
	}
}
