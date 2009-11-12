
function Spotify(callback)
{
	this.myCallback = callback;
	
	if (this.myInterval == null)
	{
		var self = this;
	
		this.myInterval = new Interval(function() { self.timerUpdate() }, 10000);
	}
	
	this.myInterval.start();
}

Spotify.prototype.myArtist = null;
Spotify.prototype.myTrack = null;
Spotify.prototype.myCallback = null;
Spotify.prototype.myInterval = null;

Spotify.prototype.playpause = function()
{
	var output = system("spotify_cmd.exe playpause");
	this.timerUpdate()
}

Spotify.prototype.next = function()
{
	var output = system("spotify_cmd.exe next");
	this.timerUpdate()
}

Spotify.prototype.previous = function()
{
	var output = system("spotify_cmd.exe prev");
	this.timerUpdate()
}

Spotify.prototype.stop = function()
{
	var output = system("spotify_cmd.exe stop");
	this.timerUpdate()
}

Spotify.prototype.getStatus = function()
{
	if (this.myArtist != null)
	{
		return { "status" : "playing", "artist" : this.myArtist, "track" : this.myTrack };
	}
	
	return { "status" : "paused" };
}

Spotify.prototype.timerUpdate = function()
{
	var output = system("spotify_cmd.exe status");
	
	var lines = output.split("\n");
	
	var artist = null;
	var track = null;
	
	if (lines[0] == "OK")
	{
		artist = lines[1];
		track = lines[2];
	}
	
	if (artist != this.myArtist && track != this.myTrack)
	{
		this.myArtist = artist;
		this.myTrack = track;
		
		this.myCallback();
	}
}

