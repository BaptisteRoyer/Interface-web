#include <Wire.h>
#include <Vector.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "AD9834AmplitudeControl.h"

#define WIFI_DELAY_RECONNECT 5000
#define MQTT_BUFFER_SIZE 24


// Change this regarding your WiFi access point 
const char* ssid = "BaptMobile ";
const char* password = "Isischat45";
const char* mqttIP = "192.168.43.58";
const long mqttPort = 1883;

// Acquisition time in microseconds 
unsigned long acqTimeInMicroseconds;

// Variable for acquisition 
boolean rdy = false;
boolean acq = false;

// Store the current result from ADC 
double mmryResult;

// Variable for data 
double curFreq;
double begFreq;
double endFreq;
double stepFreq;

double curAmp;
double begAmp;
double endAmp;
double stepAmp;

double curPosition;
double begPosition;
double endPosition;
double stepPosition;


double storage_array[MQTT_BUFFER_SIZE];
Vector<double> paramsStart(storage_array);

WiFiClient espClient;
PubSubClient client(espClient);

// Interruption routine 
void IRAM_ATTR ISR()
{
  rdy = true;
}


// Sends voltage and current mesured to the Python script via mqtt 
void sendCurrentVoltage (double amplitude, double frequency, double phase)
{
  unsigned long startTime = micros();
  // Calculates three periods to mesure 
  acqTimeInMicroseconds = 3*(1/frequency)*pow(10,6);
  setDDS(amplitude, frequency, phase);
  // Sets up an infinite loop while there are values to evaluate
  while (acq)
  {
    if(rdy)
    {
      rdy = false;
      
      uint8_t dStatus = 0, dData = 0, dCRC = 0;

      double voltage = adcDataRead(&dStatus, &dData, &dCRC) * (REF_ADC / ADC_FULLSCALE);     
      double current = adcDataRead(&dStatus, &dData, &dCRC) * (REF_ADC / ADC_FULLSCALE);     

      Serial.println(voltage);
      Serial.println(current);

      client.publish("voltageCurrent",concatenateValues(voltage,current));
      if (micros() - startTime >= acqTimeInMicroseconds)
      {
        acq = false;
      }
    }
  }
  // When the time is up send "DONE;" to the Python script via mqtt 
  if (!acq)
  {
    client.publish("voltageCurrent","DONE;");
  }
  
}

void callback(char* topic, byte* payload, unsigned int length)
{  
  if((strcmp(topic,"params"))== 0)
  {
    char buf[MQTT_BUFFER_SIZE];
  
    // Form a C-string from the payload 
    memcpy(buf, payload, length);
    buf[length] = '\0';

    double value = strtod(buf, NULL);
    // Push all the values in a vectory for simplicity
    paramsStart.push_back(value);
    if(paramsStart.size() == 9)
    {
      client.publish("readyToStart","");
    }
    //paramsStart.clear();
    
  }

  // When all the values are in the vector, the routine starts
  else if ((strcmp(topic,"readyToStart"))== 0)
  {
    Serial.println("Ready to start !");
    
    // Sets all the frequenciee, amplitude and position variables
    begFreq = paramsStart.at(0);
    Serial.println(begFreq);
    endFreq = paramsStart.at(1);
    Serial.println(endFreq);
    stepFreq = paramsStart.at(2);
    Serial.println(stepFreq);
    
    begAmp = paramsStart.at(3);
    Serial.println(begAmp);
    endAmp = paramsStart.at(4);
    Serial.println(endAmp);
    stepAmp = paramsStart.at(5);
    Serial.println(stepAmp);

    begPosition = paramsStart.at(6);
    Serial.println(begPosition);
    endPosition = paramsStart.at(7);
    Serial.println(endPosition);
    stepPosition = paramsStart.at(8);
    Serial.println(stepPosition);

    curFreq = begFreq;
    curAmp = begAmp;
    curPosition = begPosition;

    client.publish("firstPosition",convertDoubleToString(begPosition));
    client.publish("firstAmp",convertDoubleToString(begAmp));
    client.publish("firstFreq",convertDoubleToString(begFreq));

    paramsStart.clear();
    
  }
    // When python finished it's business, start the routine
  else if((strcmp(topic,"pythonReady"))== 0)
  {
    // Check if all the frequencies have been swiped
    if(curFreq == endFreq)
    {
      curFreq = begFreq;
      // Check if all the amplitudes have been swiped
      if(curAmp == endAmp)
      {
        client.publish("calculateImpedance","")
        curAmp = begAmp;
        // Check if all the positions have been swiped
        if(curPosition == endPosition)
        {
          client.publish("espEndMesure","");
        }
        else
        {
          curPosition += stepPosition;
          // If the current position exceeds the max position, set it to the max position
          if(curPosition > endPosition)
          {
            curPosition = endPosition;
          }
          client.publish("newPosition",convertDoubleToString(curPosition));
        }
      }
      else
      {
        curAmp += stepAmp;
        // If the current amplitude exceeds the max amplitude, set it to the max amplitude 
        if(curAmp > stepAmp)
        {
          curAmp = endAmp;
        }
        client.publish("newAmp",convertDoubleToString(curAmp));
      }
    }
    else
    {
      curFreq += stepFreq;
      // If the current frequency exceeds the max frequency, set it to the max frequency 
      if(curFreq > endFreq)
      {
        curFreq = endFreq;
      }
      client.publish("newFreq",convertDoubleToString(curFreq));
    }
    if (curPosition != endPosition)
    {
      acq = true;
      sendCurrentVoltage(curAmp,curFreq,0);
    }
  }
  // Move down the motor with down arrow key
  else if((strcmp(topic,"motorMoveDown"))== 0)
  {
    Serial.println("DOWN");
    Wire.beginTransmission(0x3C);
    Wire.write(1);
    Wire.write(1);
    Wire.endTransmission();
//      Wire.requestFrom(0x3C,2);
//      while(Wire.available())    // slave may send less than requested
//      {
//        int i = Wire.read();    // receive a byte as character
//        Serial.println(i);         // print the character
//      }
  }
  // Move up the motor with up arrow key
  else if((strcmp(topic,"motorMoveUp"))== 0)
  {
    Serial.println("UP");
    Wire.beginTransmission(0x3C);
    Wire.write(1);
    Wire.write(2);
    Wire.endTransmission();
//      Wire.requestFrom(0x3C,2);
//      while(Wire.available())    // slave may send less than requested
//      {
//        int i = Wire.read();    // receive a byte as character
//        Serial.println(i);         // print the character
//      }
  }
}

void setup()
{
  // Debug purposes
  Serial.begin(115200);
  
  // Initialize for i2c communication
  Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22
  
  // Initialize WiFi connection
  WiFi.begin(ssid,password);
  
  // Reattempt connection to WiFi each time it does not work
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid,password);
    Serial.println("Connecting to WiFi...");
    delay(WIFI_DELAY_RECONNECT);
  }
ZZ
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected !");
  }

  // Initialize mqtt communication
  client.setServer(mqttIP, mqttPort);

  attachInterrupt(DRDY_PIN, ISR, FALLING);

  delay(100);

  SPI_INIT();
  delay(100);

  ADC_INIT();
  delay(100);

  setDDS(0.624, 10, 0);
  delay(100);

}

void loop() 
{
  if (!client.connected()) {
    Serial.println("Client not connected. Connecting...");
    // When connected subscribe to topics
    if (client.connect("clientESPSubscriber")) {
      Serial.println("Client connected !");
      client.subscribe("params");
      client.subscribe("motorMoveUp");
      client.subscribe("motorMoveDown");
      client.subscribe("readyToStart");
      client.setCallback(callback);
    } 
    else 
    {
      // Send error when not connected
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }

  }
  client.loop();
} 
