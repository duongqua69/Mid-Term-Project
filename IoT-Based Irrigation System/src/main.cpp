#include <Arduino.h>
#include "secrets/wifi.h"
#include "wifi_connect.h"
#include <WiFiClientSecure.h>
#include "ca_cert.h"
#include "secrets/mqtt.h"
#include <PubSubClient.h>
#include <Ticker.h>

namespace
{
    const char *ssid = WiFiSecrets::ssid;
    const char *password = WiFiSecrets::pass;
    const char *relay_topic = "esp32/relay";
    const char *soil_topic = "esp32/soil";
    const char *lwt_topic = "esp32/lwt"; 
    const char *lwt_message = "ESP32 disconnected unexpectedly."; 
    unsigned int publish_count = 0;
    uint16_t keepAlive = 15;    // seconds (default is 15)
    uint16_t socketTimeout = 5; // seconds (default is 15)
    const int soilSensorPin = 34;
    const int relayPin = 26;
    const long interval = 100;
}

WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);

Ticker soilPublishTicker;
unsigned long previousMillis = 0;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.printf("From %s:  ", topic);
    String message = "";
    for (unsigned int i = 0; i < length; i++)
    {
        message += (char)payload[i];
        Serial.print((char)payload[i]);
    }
    Serial.println();

    if (String(topic) == relay_topic)
    {
        if (message == "ON")
        {
            digitalWrite(relayPin, LOW); // Relay ON
        }
        else if (message == "OFF")
        {
            digitalWrite(relayPin, HIGH); // Relay OFF
        }
    }
}

void publishSoilData()
{
    int soilMoistureValue = analogRead(soilSensorPin);
    int maxSoilValue = 4095; // Giá trị tối đa mà cảm biến trả về
    float soilPercentage = (soilMoistureValue / float(maxSoilValue)) * 100; // Chuyển sang phần trăm

    // Kiểm tra độ ẩm và điều khiển relay
    if (soilPercentage > 80.0)
    {
        digitalWrite(relayPin, LOW); // Bật relay khi độ ẩm trên 80%
        Serial.println("Relay ON (Soil Moisture > 80%)");
    }
    else
    {
        digitalWrite(relayPin, HIGH); // Tắt relay khi độ ẩm <= 80%
        Serial.println("Relay OFF (Soil Moisture <= 80%)");
    }

    // Gửi giá trị độ ẩm phần trăm lên MQTT
    String moisture = String(soilPercentage, 2); // Giới hạn 2 chữ số sau dấu phẩy
    mqttClient.publish(soil_topic, moisture.c_str(), false);
    Serial.printf("Published Soil Moisture: %s%%\n", moisture.c_str()); // In ra giá trị
    delay(5000);
}

void mqttReconnect()
{
    while (!mqttClient.connected())
    {
        Serial.println("Attempting MQTT connection...");
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        if (mqttClient.connect(client_id.c_str(), MQTT::username, MQTT::password, lwt_topic, 0, false, lwt_message))
        {
            Serial.print(client_id);
            Serial.println(" connected");
            mqttClient.subscribe(relay_topic);
        }
        else
        {
            Serial.print("MQTT connect failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 1 second");
            delay(1000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    pinMode(soilSensorPin, INPUT);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH); // Relay OFF by default

    setup_wifi(ssid, password);
    tlsClient.setCACert(ca_cert);

    mqttClient.setCallback(mqttCallback);
    mqttClient.setServer(MQTT::broker, MQTT::port);

    soilPublishTicker.attach_ms(interval, publishSoilData); // Set up periodic soil data publishing
}

void loop()
{
    if (!mqttClient.connected())
    {
        mqttReconnect();
    }
    mqttClient.loop();
}