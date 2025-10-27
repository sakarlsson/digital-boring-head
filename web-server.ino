void handleRoot()
{
#define BUFFER_SIZE 1024	
  char temp[BUFFER_SIZE];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int day = hr / 24;

  hr = hr % 24;

  snprintf(temp, BUFFER_SIZE - 1,
           "<html>\
<head>\
<meta http-equiv='refresh' content='5'/>\
<title>%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h1>Hello from %s</h1>\
<h3>running WiFiWebServer</h3>\
<h3>on %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
<img src=\"/test.svg\" />\
</body>\
</html>", BOARD_NAME, BOARD_NAME, SHIELD_TYPE, day, hr, min % 60, sec % 60);

  server.send(200, F("text/html"), temp);
}

void handleNotFound()
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, F("text/plain"), message);
}

void setupWebserver()
{
  //out.reserve(ORIGINAL_STR_LEN);

  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print(F("\nStarting AdvancedWebServer on "));
  Serial.print(BOARD_NAME);
  Serial.print(F(" with "));
  Serial.println(SHIELD_TYPE);
  Serial.println(WIFI_WEBSERVER_VERSION);

  Serial.println("wifi nina");
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("WiFi shield not present"));
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println(F("Please upgrade the firmware"));
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    //delay(500);
    Serial.print(F("Connecting to WPA SSID: "));
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  server.on(F("/"), handleRoot);
  /* server.on(F("/test.svg"), drawGraph); */
  server.on(F("/inline"), []()
  {
    server.send(200, F("text/plain"), F("This works as well"));
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("HTTP server started @ "));
  Serial.println(WiFi.localIP());
}
