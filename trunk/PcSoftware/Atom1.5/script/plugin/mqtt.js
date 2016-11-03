function Mqtt_Connect(server, username, password)
{
  return MqttExport_Connect(server, username, password);
}

function Mqtt_Close(resource)
{
  return MqttExport_Close(resource);
}

function Mqtt_SendMessage(resource, topic, message)
{
  return MqttExport_SendMessage(resource, topic, message);
}

