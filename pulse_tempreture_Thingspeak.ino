#define USE_ARDUINO_INTERRUPTS false
#include <PulseSensorPlayground.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>


const char ssid[] = "nustudent\Gourav.Agrawal19";
const char password[] = "UU49329V@54";
const char* server = "api.thingspeak.com";

#define ONE_WIRE_BUS 4 // Data wire is connected to GPIO 4 i.e. D2 pin of nodemcu
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
 
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor
 

WiFiClient client;

const long CHANNEL = 1946841; //In this field, enter the Channel ID
const char *apiKey = "SIAQ7R0T5LPCOZXZ";// Enter the Write API key 

long prevMillisThingSpeak = 0;
int intervalThingSpeak = 20000; // 15 seconds to send data to the dashboard

const int OUTPUT_TYPE = SERIAL_PLOTTER;
const int PULSE_INPUT = A0;   
const int PULSE_BLINK = 13;    
const int PULSE_FADE = 5;
const int THRESHOLD = 550;   

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;
void setup() 
{
  Serial.begin(115200);
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
  
  if (!pulseSensor.begin()) 
  {
    for(;;)
    {
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, password);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  } 
  {
Serial.begin(9600);
delay(10);
 
Serial.println("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
 
}

}

void loop() 
{
  if (pulseSensor.sawNewSample()) 
  {
    if (--samplesUntilReport == (byte) 0) 
    {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      pulseSensor.outputSample();
      if (pulseSensor.sawStartOfBeat()) 
      {
        pulseSensor.outputBeat();
      }
    }
    int myBPM = pulseSensor.getBeatsPerMinute();

    if (myBPM < 100 && myBPM > 50){
      if (millis() - prevMillisThingSpeak > intervalThingSpeak) 
      {
      ThingSpeak.setField(1, myBPM);
      int x = ThingSpeak.writeFields(CHANNEL, apiKey);
      if (x == 200) 
      {
        Serial.println("Channel update successful.");
      }
      else 
      {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
        prevMillisThingSpeak = millis();
      }        
    }    
  }{
sensors.requestTemperatures();
float tempC = sensors.getTempCByIndex(0);
float tempF = sensors.getTempFByIndex(0);
if ((tempC == -127.00) || (tempF == -196))
{
Serial.println("Failed to read from sensor!");
delay(1000);
}
else
{
Serial.print("Temperature in Celsius: ");
Serial.println(tempC);
Serial.print("Temperature in Fahrenheit: ");
Serial.println(tempF);
delay(1000);
}
 
if (client.connect(server,80)) //184.106.153.149 or api.thingspeak.com
{
String getStr = "GET /update?api_key=SIAQ7R0T5LPCOZXZ";
getStr += apiKey;
getStr +="&field1=";
getStr += String(tempC);
getStr +="&field2=";
getStr += String(tempF);
getStr += "\r\n\r\n";
 
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKapiKey:\n" );
client.print(apiKey);
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(getStr.length());
client.print("\n\n");
client.print(getStr);
Serial.println("Sent data to Thingspeak");
}
client.stop();
Serial.println("Delay of 15 Sec");
// thingspeak needs minimum 15 sec delay between updates
delay(5000);
}
}