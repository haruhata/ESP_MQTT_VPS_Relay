
/*
 It connects to an MQTT server then:
  - '0' switches off relay
  - '1' switches on relay

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "pieterse";
const char* password = "yours246";
const char* mqtt_server = "181.214.69.207";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* outTopic = "Sonoff1out";
const char* RelayTopic = "/switch/relayX";

int relay_pin = 5;
bool relayState = LOW;

void setup_wifi() {
  delay(10);
                                      // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
   
    for(int i = 0; i<500; i++){
      
      delay(1);
    }
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
                                               // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(relay_pin, LOW);              // Turn the LED on (Note that LOW is the voltage level
    Serial.println("relay_pin -> LOW");
    relayState = LOW;

  } else if ((char)payload[0] == '1') {
    digitalWrite(relay_pin, HIGH);             // Turn the LED off by making the voltage HIGH
    Serial.println("relay_pin -> HIGH");
    relayState = HIGH;
  
  }
}

void reconnect() {
                                              // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
                                              // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
                                              // Once connected, publish an announcement...
      client.publish(outTopic, "Sonoff1 booted");   
                                              // ... and resubscribe
      client.subscribe(RelayTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
                                             // Wait 5 seconds before retrying
      for(int i = 0; i<5000; i++){
        
        delay(1);
      }
    }
  }
}

void setup() {
              
  pinMode(relay_pin, OUTPUT);               // Initialize the relay pin as an output

  pinMode(5, OUTPUT);
  
  digitalWrite(relay_pin,relayState);
  
  Serial.begin(115200);
  setup_wifi();                             // Connect to wifi
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 }
