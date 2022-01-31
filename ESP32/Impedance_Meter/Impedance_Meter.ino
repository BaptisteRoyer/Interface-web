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

void sendCurrentVoltage (double amplitude, double frequency, double phase)
{
  unsigned long startTime = micros();
  acqTimeInMicroseconds = 3*(1/frequency)*pow(10,6);
  setDDS(amplitude, frequency, phase);
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
  if (!acq)
  {
    client.publish("endValues","");
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
    paramsStart.push_back(value);
    if(paramsStart.size() == 9)
    {
      client.publish("readyToStart","");
    }
    //paramsStart.clear();
    
  }

  else if ((strcmp(topic,"readyToStart"))== 0)
  {
    Serial.println("Ready to start !");
    
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

  else if((strcmp(topic,"pythonReady"))== 0)
  {
    if(curFreq == endFreq)
    {
      curFreq = begFreq;
      if(curAmp == endAmp)
      {
        client.publish("calculateImpedance","")
        curAmp = begAmp;
        if(curPosition == endPosition)
        {
          client.publish("espEndMesure","");
        }
        else
        {
          curPosition += stepPosition;
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

  SPI_INIT();
  
  Serial.begin(115200);
  
  Wire.begin (21, 22);   // sda= GPIO_21 /scl= GPIO_22
  
  WiFi.begin(ssid,password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid,password);
    Serial.println("Connecting to WiFi...");
    delay(WIFI_DELAY_RECONNECT);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected !");
  }

  client.setServer("192.168.43.58", 1883);

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
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }

  }
  client.loop();
} 
