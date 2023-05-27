#include <Arduino_JSON.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include<WiFi.h>


#define DHTPIN 4
#define DHTTYPE DHT11
const int sensor_suelo = 36;
int bomba = 2;

DHT dht(DHTPIN,DHTTYPE);

 
#define WIFI_SSID "INFINITUM1E82"
#define WIFI_PASSWORD "Ec2Mm3Rp7t"


#define API_KEY "AIzaSyBSVl0pNFxMiGrxmnzLcGz8yAcE22ue4No"
#define DATABASE_URL "https://sensordht-d8229-default-rtdb.firebaseio.com/"


FirebaseData fbdo;

FirebaseAuth auth; 
FirebaseConfig config;

bool signupOK = false; 
void setup() {
  pinMode(DHTPIN,INPUT);
  pinMode(bomba,OUTPUT);
  dht.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a la red wi-fi");
  analogReadResolution(10); //ajustamos la resolción del sensor

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //asignamos la llave API
  config.api_key = API_KEY;

  //asignamos la URL
  config.database_url = DATABASE_URL;

  //Registro exitoso
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Registro exitoso!");
    signupOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  //asignamos la funcion de callback paraa la generacion de la tarea de token
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
}

void loop() {
 
     float humdht = dht.readHumidity();
     float temdht = dht.readTemperature();

   int hum_suelo = analogRead(sensor_suelo);
  hum_suelo= abs((((hum_suelo)*100)/1023)-100) ;
    //int hum = map(hum_suelo,0,1023,100,0);     
    


     if(Firebase.ready() && signupOK){

        if(Firebase.RTDB.setFloat(&fbdo, "DHT/ humedad",humdht)){
          Serial.print("Humedad: ");
          Serial.println(humdht);
          delay(2000);
        }else {
          Serial.println("Toma no medida");
          Serial.println("Razón: " + fbdo.errorReason());
           delay(2000);
        }

        if(Firebase.RTDB.setFloat(&fbdo, "DHT/Temperatura",temdht)){
          Serial.print("Temperatura: ");
          Serial.println(temdht);
           //delay(2000);
        }else {
          Serial.println("Toma no medida");
          Serial.println("Razón: " + fbdo.errorReason());
           //delay(2000);
        }

        if(Firebase.RTDB.setFloat(&fbdo, "DHT/Humedad en suelo",hum_suelo)){
          Serial.print("Humedad en suelo: ");
          Serial.println(hum_suelo);
           delay(2000);
        }else {
          Serial.println("Toma no medida");
          Serial.println("Razón: " + fbdo.errorReason());
           //delay(2000);
        }
        
        }
              if(hum_suelo < 50){
          digitalWrite(bomba,HIGH);
          delay(1000);
          digitalWrite(bomba,LOW); 
      }else{
        digitalWrite(bomba,LOW); 
      }


      
     Serial.println("--------------------");
     delay(2000);
}
