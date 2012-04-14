
function MySql_Connect(server, username, password)
{
  return MySqlExport_Connect(server, username, password);
}

function MySql_Close(resource)
{
  return MySqlExport_Close(resource);
}

function MySql_Query(resource, query)
{
  return MySqlExport_Query(resource, query);
}

function MySql_SelectDb(resource, database)
{
  return MySqlExport_SelectDb(resource, database);
}

function MySql_AffectedRows(resource)
{
  return MySqlExport_AffectedRows(resource);
}
