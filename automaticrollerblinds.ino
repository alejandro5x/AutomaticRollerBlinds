#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials
const char* ssid = "ssid"; 
const char* password = "wifipass123"; 

// MQTT Server
const char* mqtt_server = "192.168.x.x";
const char *mqtt_user = "mqtt-user";  
const char *mqtt_pass = "mqttpas123";  

// Stepper Motor Pins
#define STEP_PIN 4  
#define DIR_PIN 5   
#define EN_PIN 0    
#define HALL_PIN A0 

// Microstepping control pins
#define MS1_PIN 14
#define MS2_PIN 12

// Stepper settings
int stepDelay = 800;  
float actual_steps = 0;
volatile int hallValue = 0;
int maximum_steps = 1000;  

// Hall Sensor Thresholds
const int thresholdHigh = 600;
const int thresholdLow  = 300;

// Motor control flags
bool motorRunning = false;
int motorDirection = HIGH;
bool ma = false;

// Wi-Fi and MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// Timer for sensor reading
unsigned long lastHallRead = 0;
const unsigned long hallReadInterval = 100; // Read sensor every 100ms

void setup() {
    Serial.begin(115200);

    // Initialize pins
    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    pinMode(HALL_PIN, INPUT);

    digitalWrite(MS1_PIN, LOW);
    digitalWrite(MS2_PIN, LOW);
    digitalWrite(EN_PIN, LOW);  

    // Read max steps from EEPROM
    EEPROM.begin(10);
    EEPROM.get(0, maximum_steps);
    //agSerial.print("Max Steps: ");
    //agSerial.println(maximum_steps);

    // Connect to Wi-Fi and MQTT
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    //agSerial.println("Setup complete");
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    
    client.loop();  

    // Read Hall sensor in a non-blocking way
    if (millis() - lastHallRead >= hallReadInterval) {
        lastHallRead = millis();
        hallValue = analogRead(HALL_PIN);
    }

    runMotor();  
}

// ** MQTT Functions **
void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    //agSerial.print("Received command: ");
    //agSerial.println(message);

    if (String(topic) == "roller/home") {
        Home();
    } 
    else if (String(topic) == "roller/up") {
        Roll_up();
    } 
    else if (String(topic) == "roller/down") {
        Roll_down();
    } 
    else if (String(topic) == "roller/set_max") {
        Set_max();
    }
    else if (String(topic) == "roller/roll_max") {
        Roll_max();
    }
    else if (String(topic) == "roller/stop") {
        Stop();
    }
}

void setup_wifi() {
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP8266_Roller", mqtt_user, mqtt_pass)) {
            Serial.println("Connected!");
            client.subscribe("roller/home");
            client.subscribe("roller/up");
            client.subscribe("roller/down");
            client.subscribe("roller/stop");
            client.subscribe("roller/roll_max");
            client.subscribe("roller/set_max");
        } else {
            Serial.print("Failed, retrying in 5 seconds...");
            delay(5000);
        }
    }
}

// ** Motor Control Functions **
void Home() {
    //agSerial.println("Homing...");
    motorDirection = HIGH;
    motorRunning = true;
}

void Roll_up() {
    //agSerial.println("Rolling Up...");
    //agSerial.println(hallValue);
    if (hallValue > thresholdLow || hallValue < thresholdHigh) {
        motorDirection = HIGH;
        motorRunning = true;
    } else {
        Serial.println("Already at top.");
    }
}

void Roll_down() {
    //agSerial.println("Rolling Down...");
    if (actual_steps < maximum_steps) {
        motorDirection = LOW;
        motorRunning = true;
        ma = false;
    } else {
        Serial.println("Already at max position.");
    }
}

void Roll_max() {
    //agSerial.println("Rolling Max...");
    motorDirection = LOW;
    motorRunning = true;
    ma = true;
}
void Stop() {
  //agSerial.println("Stop...");
  //agSerial.println(actual_steps);
  motorRunning = false; 
}
void Set_max() {
    //agSerial.println("Rolling Max...");
    motorRunning = false; 
    maximum_steps = actual_steps;
    EEPROM.put(0, maximum_steps);
    EEPROM.commit();

    //agSerial.print("Max Steps Set to: ");
    //agSerial.println(maximum_steps);
}

void runMotor() {
    if (!client.connected()) return;  

    static unsigned long lastStepTime = 0;
    unsigned long now = micros();  

    if (motorRunning && (now - lastStepTime >= stepDelay)) {
        lastStepTime = now;  

        digitalWrite(DIR_PIN, motorDirection);
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(10);  
        digitalWrite(STEP_PIN, LOW);

        if (motorDirection == HIGH) {
            actual_steps--;
            if (hallValue > thresholdHigh || hallValue < thresholdLow) {
                motorRunning = false;
                actual_steps = 0;
            }
        } else {
            actual_steps++;
            if (!ma){
            if (actual_steps >= maximum_steps) {
                motorRunning = false;  
            }}
        }
    }
}
