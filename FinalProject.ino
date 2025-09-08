#include <WiFi.h>
#include <Wire.h>
#include <Time.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

short id = 1;

int status = WL_IDLE_STATUS;
WiFiClient client;

#define led 26

int n1=1;
int n2=2;

float prevTemp;
float prevHumid;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  prevTemp = dht.readTemperature();
  prevHumid = dht.readHumidity();

  pinMode(led, OUTPUT);

  if (WiFi.begin("jujjujjij", "0110001111")) {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    while (1) {
      if (client.connect("192.168.237.106", 1234))
      {
        Serial.println("Connected");
        return;
      }
      Serial.println("Failed to connect.");
      delay(3000);
    }
    
  }
  else {
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while(true);
  }

  xTaskCreate(every30Sec, "loop", 1024, (void*)&n1, 1, NULL);
}

void loop()
{
  if (client.connected())
  {
    float currTemp = dht.readTemperature();
    float currHumid = dht.readHumidity();

    if (currTemp - prevTemp >= 1 || prevTemp - currTemp >= 1)
    {
      sendData(currTemp, currHumid);
      xTaskCreate(ledBlink, "led", 1024, (void*)&n2, 1, NULL);
      prevTemp = currTemp;
    }

    if (currHumid - prevHumid >= 5 || prevHumid - currHumid >= 5)
    {
      sendData(currTemp, currHumid);
      xTaskCreate(ledBlink, "led", 1024, (void*)&n2, 1, NULL);
      prevHumid = currHumid;
    }
  }
  else
  {
    Serial.println("Not connected");
  }
  delay(50);
}

void every30Sec(void *parameter)
{
  while (1)
  {
    if (client.connected())
    {
      prevTemp = dht.readTemperature();
      prevHumid = dht.readHumidity();
      sendData(prevTemp, prevHumid);
      xTaskCreate(ledBlink, "led", 1024, (void*)&n2, 1, NULL);
      vTaskDelay(5000/portTICK_PERIOD_MS);
    }
  }
}

void ledBlink(void *parameter)
{
  digitalWrite(led, HIGH);
  vTaskDelay(300/portTICK_PERIOD_MS);
  digitalWrite(led, LOW);
  vTaskDelete(NULL);
}

void sendData(double temp, double humid)
{
  client.println("{\"time\" : \"" + String(millis()) + "\", \"ID\": \"" + id + "\", \"temp\" : \"" + String(temp, 2) + "\", " + "\"humid\" : \"" + String(humid, 2) + "\"}");
}
