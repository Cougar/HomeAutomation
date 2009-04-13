
loadScript("Logic/MenuItem.js");

function Display(shortName)
{
	this.shortName = shortName;
}

/* Declaration of instance variables, for static variables remove prototype */
/* The currently displayed menuitem */
Display.prototype.currentMenuItem = null;

/* format: minutes since 00:00 (480/60 = 8:00) */
Display.prototype.bookFromTime = 480;
Display.prototype.bookToTime = 510;

/* the shortname for this exchangeobject */
Display.prototype.shortName = "";	


