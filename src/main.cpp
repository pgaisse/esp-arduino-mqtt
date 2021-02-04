
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
String mensaje= "", strtopic="",strmsg;
const char* ssid = "RU";
const char* password = "1234567890";
//const char* ssid = "pato";
//const char* password = "1234567890";
//const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "192.168.1.83";
const char* usr = "pgaisse";
const char* psw = "patoch";
double x=0;
float msgtofloat;
float val=0.0;
int  sensor=0;
int ns=0,values=0,statuss=0;
char buffern[50];
char bufferv[50];
char buffers[4];
char bufferf[12];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char msg2[50];
int value=0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  strtopic=String((const char*)topic);
  mensaje= String((const char*)payload);
  Serial.println(mensaje.toDouble());
  if(strtopic=="zona1/get/temperature"){
    if(mensaje.toDouble() >=25){
      digitalWrite(D1, HIGH);
    }
    else{
      digitalWrite(D1, LOW);
    }
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String  clientId= "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),usr,psw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe("zona1/get/temperature");
      client.subscribe("zona1/get/humidity");
      client.subscribe("msg1");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     Serial.println(clientId);
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(D1,OUTPUT);

}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  val=  random(0,100);
  //sensor=analogRead(A0);
  
  statuss=digitalRead(D1);
  if (statuss==1){
    msg2[0]='1';
  }
  else
  {
    msg2[0]='0';
  }
  
  long now = millis();
  if (now - lastMsg > 1000) {
    int sensorValue = analogRead(A0); //Lectura del ADC 
    float temp = sensorValue * (3.3*1000 / 1023.0); //escalamos a voltaje 
    lastMsg = now;
    ++value;
    sprintf (buffern, "%d/%3.2f", statuss,temp);
    //sprintf (buffern, "%c", statuss);
    client.publish("msg1", buffern);
    Serial.println(temp);
    }
}