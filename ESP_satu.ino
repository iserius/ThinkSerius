#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "iSerius_under"
#define WIFI_PASSWORD "silahkanmasuk"

//#define TOKEN "rLRrhmMRqxG48TptoYrr"
#define TOKEN "tz2ISwHkFgxyv7D1WjKu"

// DHT
#define DHTPIN 2
#define DHTTYPE DHT11

char thingsboardServer[] = "inetworks.ddns.net";

WiFiClient wifiClient;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  delay(10);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
//float h=97.33;
//float t=25.22;
float lat = -6.8879;
float Long = 107.6159;
float AQ = 5;
float PU = 10; 
float PO = 15; 

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  /*Serial.print("H: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("T: ");
  Serial.print(t);
  Serial.print(" *C ");*/

  String temperature = String(t);
  String humidity = String(h);
  String latitude = String(lat);
  String longitude = String(Long);
  String AQUA = String(AQ);
  String PUCUK = String(PU);
  String POCARI = String(PO);
  /* Just debug messages
  Serial.print( "Send temp and humid : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );Serial.print( "," ); Serial.print( latitude ); Serial.print( "," );
  Serial.print( longitude );
  Serial.print( "]   -> " );
  */
  // Prepare a JSON payload string
  String payload = "{";  
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity; payload += ",";
  payload += "\"latitude\":"; payload += latitude; payload += ",";
  payload += "\"longitude\":"; payload += longitude;
  payload += "}";

  String payload1 = "{";
  payload1 += "\"AQUA\":"; payload1 += AQUA; payload1 += ",";
  payload1 += "\"PUCUK\":"; payload1 += PUCUK; payload1 += ",";
  payload1 += "\"POCARI\":"; payload1 += POCARI; 
  payload1 += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );

    char attributes1[100];
  payload1.toCharArray( attributes1, 100 );
  client.publish( "v1/devices/me/telemetry", attributes1 );
  Serial.println( attributes1 );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
