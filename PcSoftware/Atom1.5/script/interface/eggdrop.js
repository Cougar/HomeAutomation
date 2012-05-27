function Eggdrop_Post(host, port, username, password, channel, text)
{
  var socket_id = Socket_Connect(host, port, function(socket_id, data)
  {
    /* When connected the host will first ask us for the username */
    if (data.indexOf("Please enter your nickname") > -1)
    {
      Socket_Send(socket_id, username + "\n");
    }
    /* Then the host will ask us for the password */
    if (data.indexOf("Enter your password") > -1)
    {
      Socket_Send(socket_id, password + "\n");
    }
    /* If credentials is correct we are connected */
    if (data.indexOf("joined the party line") > -1)
    {
      Socket_Send(socket_id, ".say " + channel + " " + text + "\n");
      Socket_Send(socket_id, ".quit\n");
    }
  }, function(socket_id, state)
  {
	  /* No need to do anything on connection state change */
  });
    
  return socket_id;
}
