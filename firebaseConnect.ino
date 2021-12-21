#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Servo_ESP32.h>  
#include <afstandssensor.h>

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID  "Kebayoran"
#define WIFI_PASSWORD  "kebayoran566117"
#define API_KEY  "AIzaSyBwB5H0euEtAPGo_GuI89enyJiFT2MKHto"
#define DATABASE_URL  "https://smarthomeiot-3af33-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "rizdkychandrawijaya@gmail.com"
#define USER_PASSWORD "senopati25"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

int merah = 16;
int kuning = 17;

AfstandsSensor afstandssensor(23, 22);

//globar variable servo
static const int servoPin = 25; //printed G14 on the board
Servo_ESP32 servo1;

int angle = 25;
int angleStep = 1;
int angleMin = 20;
int angleMax = 130;

//globar var data firebase
int tinggiAir = 0;
int delayGate = 0;
String statusGate = "open";

int tinggiSensor = 0;

void openGate(){
   servo1.attach(servoPin);
    for(int angle = 130; angle >= angleMin; angle -=angleStep) {
        servo1.write(angle);
        delay(20);
    }
}

void closeGate(){
   servo1.attach(servoPin);
    for(int angle = 25; angle <= angleMax; angle +=angleStep) {
        servo1.write(angle);
        delay(20);
    }
}

void setup(){
  Serial.begin(115200);
  pinMode(merah, OUTPUT);
  pinMode(kuning, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  //Or use legacy authenticate method
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = "31oWT6iqJOuFDTyDBSgWZu1ekyE3giGE3aZvvPIU";

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
}

void loop(){  
  delay(20);
  delayGate = 1;
  if (Firebase.ready())
  { 
    if(Firebase.getInt(fbdo, "/gate1/tinggiAir")){
      tinggiAir = fbdo.to<int>();      
      Serial.printf("Tinggi Air firebase: %s\n", String(tinggiAir));
    }
    if(Firebase.getString(fbdo, "/gate1/statusGate")){
      statusGate = fbdo.to<String>();
      Serial.printf("StatusGate firebase: %s\n", statusGate);
    }
    if(Firebase.getInt(fbdo, "/gate1/delay")){
      delayGate = fbdo.to<int>();    
      Serial.printf("delay firebase: %s\n", String(delayGate));
    }

    if(tinggiAir<=20 and statusGate=="open"){
      openGate();
      digitalWrite(merah,LOW);
      digitalWrite(kuning,HIGH);
      Serial.printf("Set Status Gate... %s\n", Firebase.setString(fbdo, "/gate1/statusGate", "close") ? "ok" : fbdo.errorReason().c_str());
    }else if(tinggiAir>20 and statusGate=="close"){
      closeGate();
      digitalWrite(merah,HIGH);
      digitalWrite(kuning,LOW);
      Serial.printf("Set Status Gate... %s\n", Firebase.setString(fbdo, "/gate1/statusGate", "open") ? "ok" : fbdo.errorReason().c_str());
    }
    tinggiSensor = afstandssensor.afstandCM();
    Serial.printf("Tinggi Air: %s cm \n", String(tinggiSensor));
    Serial.printf("Set Tinggi Air... %s\n", Firebase.setInt(fbdo, "/gate1/tinggiAir", tinggiSensor) ? "ok" : fbdo.errorReason().c_str());
  }
  delay(delayGate*1000);
  Serial.printf("Set delay time... %s\n", Firebase.setInt(fbdo, "/gate1/delay", 1) ? "ok" : fbdo.errorReason().c_str()); 
}
