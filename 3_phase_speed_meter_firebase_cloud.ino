int delayTime = 10;
int sensor_pot=A0;
int sensor_value=0;
int start_button = 5;
int speed_Control_port = A0;
int currentphase1_port = A2;
int currentphase2_port = A3;
int currentphase3_port = A4;
int voltagephase1_port = A5;
int voltagephase2_port = A6;
int voltagephase3_port = A7;
int currentphase1_value=0;
int currentphase2_value=0;
int currentphase3_value=0;
#include <WiFi.h> // include the wifi library
const char* ssid     = "your-ssid"; // specify name of accesspoint
const char* password = "your-password"; // specify password of access point

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include <Firebase_ESP_Client.h>
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

 
#include <Adafruit_ILI9341.h>
#define TFT_CS    8      // TFT CS  pin is connected to arduino pin 8
#define TFT_RST   9      // TFT RST pin is connected to arduino pin 9
#define TFT_DC    10     // TFT DC  pin is connected to arduino pin 10

// Insert Firebase project API Key
#define API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"
// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL"
// initialize ILI9341 TFT librartfty
int delayTime = 10;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
void setup() {
  // put your setup code here, to run once:
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(sensor_pot,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  tft.begin();
  // Print a message to the LCD.
  delay(500);
  textToPrint();
}

void loop() {
  // put your main code here, to run repeatedly:
  // 1. Connecting to wifi
  connect_to_wifi_accesspoint();
  //2. sign up to firebase
  signUp_to_firebase();
}
void signUp_to_firebase(){
  /* Assign the api key (required) */
 config.api_key = API_KEY;

 /* Assign the RTDB URL (required) */
 config.database_url = DATABASE_URL;
 /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
void connect_to_wifi_accesspoint(){
  Serial.begin(115200);
  delay(10);
  textToPrint("Connecting to",ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  textToPrint("Connected to", WiFi.localIP());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void without_internet(){
  // IF START BUTTON IS PRESSED CALL THE MOTOR RUN FUNCTION
  if(digitalRead(start_button) == HIGH){
     motor_run();
     read_SensorValues_and_Print();
   }
}
void with_Internet(){
   
}
void Read_Command_From_firebase(){
  if (Firebase.getString(firebaseData, "/Appliances/fan")){
     if (firebaseData.stringData() == "STOP"){
         // STOP THE MOTOR. YOU CAN TRY WRITING A LOW TO THE PINS
         stop_motor();
         // SEND SIGNAL TO CLOUD THAT THE MOTOR IS OFF
         // YOU CAN DISABLE THE CLICK BUTTON TO STOP THE MOTOR ONCE
      }
      if(digitalRead(start_button) == HIGH){
        motor_run();
        read_SensorValues_and_Print();
      }
      
   }
}
void stop_motor(){
  //1 5 6
  digitalWrite(1,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  
  //1,2,6
  digitalWrite(1,LOW);
  digitalWrite(6,LOW);
  digitalWrite(2,LOW);
  
  //1 ,2 ,3
  digitalWrite(1,LOW);
  digitalWrite(3,LOW);
  digitalWrite(2,LOW);
  digitalWrite(6,LOW);
  //2, 3 4
  digitalWrite(4,LOW);
  digitalWrite(3,LOW);
  digitalWrite(2,LOW);
  digitalWrite(1,LOW);
  // 3, 4, 5
  digitalWrite(4,LOW);
  digitalWrite(3,LOW);
  digitalWrite(5,LOW);
  digitalWrite(2,LOW);
  
  // 4 ,5 6
  digitalWrite(4,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(3,LOW);
}
void read_SensorValues_and_Print(){
   currentphase1_value = analogRead(currentphase1_port);
   currentphase2_value = analogRead(currentphase2_port);
   currentphase3_value = analogRead(currentphase1_port);
   float voltagephase1_value = analogRead(voltagephase1_port);
   float voltagephase2_value = analogRead(voltagephase2_port);
   float voltagephase3_value = analogRead(voltagephase3_port);
   textToPrint(currentphase1_value,"A Phase1");
   textToPrint(currentphase2_value,"A Phase2");
   textToPrint(currentphase3_value,"A Phase3");
   textToPrint(voltagephase1_value, "V Phase1");
   textToPrint(voltagephase2_value, "V Phase2");
   textToPrint(voltagephase3_value, "V Phase3");
   float average_voltage = (voltagephase1_value + voltagephase2_value + voltagephase3_value) /3;
   float average_current = (currentphase1_value + currentphase2_value + currentphase3_value)/3
   textToPrint(average_voltage, "V Average voltage");
   textToPrint(average_current, "A Average current");
   textToPrint((average_current * average_voltage), "Power consumed");
}
void motor_run(){
   
  //1 5 6
  digitalWrite(1,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(6,HIGH);
   digitalWrite(4,LOW);
  delay(delayTime);
  //1,2,6
  digitalWrite(1,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(5, LOW);
  delay(delayTime);
  //1 ,2 ,3
  digitalWrite(1,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(6,LOW);
  delay(delayTime);
  //2, 3 4
  digitalWrite(4,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(1,LOW);
  delay(delayTime);
  // 3, 4, 5
  digitalWrite(4,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(2,LOW);
  delay(delayTime);
  // 4 ,5 6
  digitalWrite(4,HIGH);
   digitalWrite(6,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(3,LOW);
  delay(delayTime);
}
unsigned long textToPrint(char* text1, char* text2){
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  tft.println(text1);
  tft.println(text2);
  return micros() - start;
}
