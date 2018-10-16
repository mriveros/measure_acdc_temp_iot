#include <SoftwareSerial.h>
#define RX 10
#define TX 11
#include "DHT.h"
#define dht_apin A0 // Analog Pin sensor is connected to
String AP = "FLIA RIVEROS BENITEZ";       // AP
String PASS = "XXXXXXXXXXXXXXXXXX"; // OCULTANDING PASSWORD
String API = "GH3R83DTYWUEOK46";   // OCULTANDING API
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
boolean found = false; 
SoftwareSerial esp8266(RX,TX); 
dht DHT; 
int countTrueCommand;
int countTimeCommand; 
int valSensor = 1;
int _max = 0;
int _min = 1023;
int _peak = 0;
int adc = 0;
float v_sensor = 0;
float voltaje_rms = 0;
float corriente = 0;
float potencia =0;
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() {
Serial.print("La potencia es:");
Serial.print(medicion());
Serial.println("Watts");
delay(1000);

 valSensor = getSensorData();
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}
int getSensorData(){
  
   DHT.read11(dht_apin);  
    Serial.print("Current humidity = ");
    Serial.print(DHT.humidity);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(DHT.temperature); 
    Serial.println("C  ");
    return DHT.temperature;
}
float medicion(void){
  for (int i = 0; i < 300; i++){
    adc = analogRead(A0);
    if (adc <= _min) _min = adc;
    if (adc >= _max) _max = adc;
    }
    _peak = _max - _min;
    v_sensor = (float)_peak * 5.0 /1024.0;
    voltaje_rms = v_sensor * (RAIZ_2 / 2);
    corriente = voltaje_rms * 10;
    potencia = corriente * 220.0;
    _max = 0;
    _min = 1023;
    return potencia;
 }

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
