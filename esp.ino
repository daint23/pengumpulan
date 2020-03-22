
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "MQ7.h"
#include "MQ135.h"

MQ7 mq7(A0, 5.0);

const int ledPin = 0;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

unsigned long delayTime;

const char* ssid = "RaspiTA";
const char* wifi_password = "raspberry";

const char* mqtt_server = "192.168.23.1";
const char* mqtt_topic = "suhu";
const char* mqtt_username = "";
const char* mqtt_password = "";

const char* clientID = "ESP8266_1";

const int sensorPin= 0;     // deklarasi pin sensor
float air_quality5;
float air_quality6;

float temp;
float pressu;
float altitu;
float humi;

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);


long lastMsg = 0;
long lastMsg2 = 0;
long lastMsg3 = 0;
long lastMsg4 = 0;
long lastMsg5 = 0;
long lastMsg6 = 0;
char msg[50];
char msg2[50];
char msg3[50];
char msg4[50];
char msg5[50];
char msg6[50];
float value = 0;
float value2 = 0;
float value3 = 0;
float value4 = 0;
float value5 = 0;
float value6 = 0;


void setup_wifi() {

  delay(10);
  // Koneksi dengan WIFI
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifi_password);

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
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void reconnect() {
  // looping untuk koneksi dengan MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Membuat clientID secara Acak
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Publish setelah tersambung dengan MQTT
      client.publish(mqtt_topic, "suhu");
      // dan Subscribe
      client.subscribe("suhu");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Apabila gagal terkoneksi, mencoba lagi dalam 5 detik
      delay(5000);
    }
  }
}



void setup() {
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883); //Inisiasi server MQTT
  client.setCallback(callback); //memanggil fungsi callback diatas
  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 3000;

  Serial.println();
}

void loop() {
// fungsi looping untuk mendapatkan data dari sensor MQ135
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  temp = bme.readTemperature();
  pressu = bme.readPressure() / 100.0F;
  altitu = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humi = bme.readHumidity();

  air_quality5 = mq7.getPPM();
  
  MQ135 gasSensor = MQ135(A0);  
  air_quality6 = gasSensor.getPPM();

long now = millis();
  if (now - lastMsg > 2000) {
    {
    lastMsg = now;
    value=temp;
    snprintf (msg, 75, "Suhu: %f *C", value); 
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("suhu", msg); 
    }
    {
    lastMsg2 = now;
    value2=pressu;
    snprintf (msg2, 75, "Tekanan: %f hPa", value2); 
    Serial.print("Publish message: ");
    Serial.println(msg2);
    client.publish("tekanan", msg2); 
    }
    {
    lastMsg3 = now;
    value3=altitu;
    snprintf (msg3, 75, "Ketinggian: %f m", value3); 
    Serial.print("Publish message: ");
    Serial.println(msg3);
    client.publish("tinggi", msg3); 
    }
    {
    lastMsg4 = now;
    value4=humi;
    snprintf (msg4, 75, "Kelembaban: %f %", value4); 
    Serial.print("Publish message: ");
    Serial.println(msg4);
    client.publish("lembab", msg4); 
    }
    {
    lastMsg5 = now;
    value5=air_quality5;
    snprintf (msg5, 75, "CO Quality: %f", value5);
    Serial.print("Publish message: ");
    Serial.println(msg5);
    client.publish("kualitas_udara", msg5); 
    }
    {
    lastMsg6 = now;
    value6=air_quality6;
    snprintf (msg6, 75, "Air Quality: %f", value6); 
    Serial.print("Publish message: ");
    Serial.println(msg6);
    client.publish("kualitas_udara2", msg6); 
    }
    delay(delayTime);
  }
}