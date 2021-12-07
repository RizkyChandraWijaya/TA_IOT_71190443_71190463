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

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

AfstandsSensor afstandssensor(23, 22);

//globar variable servo
static const int servoPin = 25; //printed G14 on the board
Servo_ESP32 servo1;
int angle = 25;
int angleStep = 1;
int angleMin = 20;
int angleMax = 130;

//globar var data firebase
int jarak = 0;
int delayGate = 0;
String statusGate = "open";

int jarakSensor = 0;


void openGate(){
   servo1.attach(servoPin);
    for(int angle = 130; angle >= angleMin; angle -=angleStep) {
        servo1.write(angle);
//        Serial.println(angle);
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
  //Flash string (PROGMEM and  (FPSTR), String, C/C++ string, const char, char array, string literal are supported
  //in all Firebase and FirebaseJson functions, unless F() macro is not supported.
  delay(20);
  delayGate = 1;
  if (Firebase.ready())
  { 
    if(Firebase.getInt(fbdo, "/gate1/jarak")){
      jarak = fbdo.to<int>();      
      Serial.printf("jarak firebase: %s\n", String(jarak));
    }
    if(Firebase.getString(fbdo, "/gate1/statusGate")){
      statusGate = fbdo.to<String>();
      Serial.printf("StatusGate firebase: %s\n", statusGate);
    }
    if(Firebase.getInt(fbdo, "/gate1/delay")){
      delayGate = fbdo.to<int>();    
      Serial.printf("delay firebase: %s\n", String(delayGate));
    }

    if(jarak<=20 and statusGate=="close"){
      openGate();
//      delay(delayGate*1000);
      Serial.printf("Set string... %s\n", Firebase.setString(fbdo, "/gate1/statusGate", "open") ? "ok" : fbdo.errorReason().c_str());
    }else if(jarak>20 and statusGate=="open"){
      closeGate();
//      delay(delayGate*1000);
      Serial.printf("Set string... %s\n", Firebase.setString(fbdo, "/gate1/statusGate", "close") ? "ok" : fbdo.errorReason().c_str());
    }
    jarakSensor = afstandssensor.afstandCM();
    Serial.printf("jarak sensor: %s cm \n", String(jarakSensor));
    Serial.printf("Set jarak... %s\n", Firebase.setInt(fbdo, "/gate1/jarak", jarakSensor) ? "ok" : fbdo.errorReason().c_str());
    
  }
  delay(delayGate*1000);
}
