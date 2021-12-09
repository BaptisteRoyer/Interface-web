#include <WiFi.h>
#include <PubSubClient.h>

using namespace std;

//00:6C MAC

const char* ssid = "flotte_robotique_2";
const char* password = "passFlotte";


WiFiClient espClient;
PubSubClient client(espClient);

float paramsTable[5];
int k = 0;


void callback(char* topic, byte* payload, unsigned int length) {
  
    Serial.println(strcmp(topic,"mesureParamsa"));
    char buffer[24];

    // Make sure here that `length` is smaller than the above buffer size. 
    // Otherwise, you'd need a bigger buffer
  
    // Form a C-string from the payload
    memcpy(buffer, payload, length);
    buffer[length] = '\0';
  
    // Convert it to float
    char *end = nullptr;
    float value = atof(buffer);
    
  
    // Check for conversion errors
    if (end == buffer || errno == ERANGE)
      ; // Conversion error occurred
    else{
  
      paramsTable[k] = value;
      Serial.print(k);
      Serial.print(" : ");
      Serial.println(paramsTable[k]);
      k++;
      k %= 5;
  
  //    client.publish("sendTable",buffer);

    }
    
  
  
}

void setup() {  
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connecting to WiFi ...");
    delay(500);
  }

  client.setServer("192.168.1.146", 1883);
}

void loop() {
  if (!client.connected()) {
    Serial.println("Client not connected. Connecting...");
    if (client.connect("clientESPSubscriber")) {
      client.subscribe("mesureParams");
      client.setCallback(callback);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  client.loop();
  
  delay(1);
}
