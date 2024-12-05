#include <Arduino.h>
#include "secrets/wifi.h"
#include "secrets/mqtt.h"
#include "wifi_connect.h"
#include <WiFiClientSecure.h>
#include "ca_cert.h"
#include <PubSubClient.h>
#include <Ticker.h>

namespace
{
    const char *ssid = WiFiSecrets::ssid;
    const char *password = WiFiSecrets::pass;
    const char *relay_topic = "esp32/relay";       // Topic điều khiển relay từ Dashboard
    const char *soil_topic = "esp32/soil";        // Topic gửi độ ẩm đất
    const char *status_topic = "esp32/status";    // Topic gửi trạng thái bơm
    const char *mode_topic = "esp32/mode";        // Topic gửi/nhận chế độ điều khiển
    const char *lwt_topic = "esp32/lwt"; 
    const char *lwt_message = "ESP32 disconnected unexpectedly."; 
    const int soilSensorPin = 34;
    const int relayPin = 16;
    const long interval = 5000;
    bool isManualMode = false; // Chế độ mặc định: AUTO
    bool manualPumpState = false; // Trạng thái bơm trong chế độ MANUAL
}

WiFiClientSecure tlsClient;
PubSubClient mqttClient(tlsClient);

Ticker soilPublishTicker;

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

    // Nhận lệnh chế độ AUTO/MANUAL
    if (String(topic) == mode_topic)
    {
        if (message == "MANUAL")
        {
            isManualMode = true;
            Serial.println("Switched to MANUAL mode");
        }
        else if (message == "AUTO")
        {
            isManualMode = false;
            Serial.println("Switched to AUTO mode");
        }
    }

    // Điều khiển relay trong chế độ MANUAL
    if (isManualMode && String(topic) == relay_topic)
    {
        if (message == "true")
        {
            digitalWrite(relayPin, LOW); // Bật bơm
            manualPumpState = true;
            mqttClient.publish(status_topic, "Pump: ON (MANUAL)");
            Serial.println("Pump turned ON manually.");
        }
        else if (message == "false")
        {
            digitalWrite(relayPin, HIGH); // Tắt bơm
            manualPumpState = false;
            mqttClient.publish(status_topic, "Pump: OFF (MANUAL)");
            Serial.println("Pump turned OFF manually.");
        }
    }
}


void publishSoilData()
{
    int soilMoistureValue = analogRead(soilSensorPin);
    float soilPercentage = (100 - (soilMoistureValue / 4095.0) * 100);

    if (!isManualMode) // Chỉ điều khiển bơm tự động khi ở chế độ AUTO
    {
        String pumpState;
        if (soilPercentage < 40.0)
        {
            digitalWrite(relayPin, LOW); // Bật bơm
            pumpState = "ON (AUTO)";
            Serial.println("Relay ON (Soil Moisture < 40%)");
        }
        else
        {
            digitalWrite(relayPin, HIGH); // Tắt bơm
            pumpState = "OFF (AUTO)";
            Serial.println("Relay OFF (Soil Moisture >= 40%)");
        }
        mqttClient.publish(status_topic, pumpState.c_str(), true);
    }

    // Gửi giá trị độ ẩm đất
    String moisture = String(soilPercentage, 2);
    mqttClient.publish(soil_topic, moisture.c_str(), true);
    Serial.printf("Soil Moisture: %s%%\n", moisture.c_str());
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
            Serial.println("MQTT connected");
            mqttClient.subscribe(relay_topic); // Đăng ký topic điều khiển relay
            mqttClient.subscribe(mode_topic);  // Đăng ký topic chuyển đổi chế độ
        }
        else
        {
            Serial.print("MQTT connection failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(". Retrying in 1 second...");
            delay(1000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(soilSensorPin, INPUT);
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH); // Relay OFF by default

    setup_wifi(ssid, password);
    tlsClient.setCACert(ca_cert);

    mqttClient.setCallback(mqttCallback);
    mqttClient.setServer(MQTT::broker, MQTT::port);

    soilPublishTicker.attach_ms(interval, publishSoilData); // Gửi dữ liệu định kỳ
}

void loop()
{
    if (!mqttClient.connected())
    {
        mqttReconnect();
    }
    mqttClient.loop();
}
