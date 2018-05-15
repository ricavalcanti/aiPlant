#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>

#define pino_sinal_analogico A0
#define pino_bomba D2 

const char* SSID = "LII"; // rede wifi
const char* PASSWORD = "wifiLI2Rn"; // senha da rede wifi

const char* HOST = "192.168.0.107"; //"192.168.0.107"; // ip/host do broker
int PORT =  3001; // porta do broker
SocketIOClient socket;

float getSensor(){
    return ((analogRead(pino_sinal_analogico))/1024.0);
}
void sendData(){
    
    //enviar Json    
    StaticJsonBuffer<200> jsonBuffer;
    char big_json[200];

    JsonObject& sensorLuz = jsonBuffer.createObject();
    sensorLuz["serial"] = "luz";
    sensorLuz["value"] = getSensor(); //vUmd

    /*JsonArray& dadosJson = jsonBuffer.createArray();
    dadosJson.add(sensorUmd);*/ //para incluir em um vetor

    sensorLuz.printTo(big_json);
    Serial.println();
    Serial.print(big_json);


    socket.emit("sendsensor", big_json);

}

void initWiFi() {
  delay(10);
  //Serial.println("Conectando-se em: " + String(SSID));

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
}

  void initWS() {
    if (!socket.connect(HOST, PORT)) {
      Serial.println(F("connection device-server failed"));
      return;
    } else if (socket.connected()) {
        Serial.println(F("\n\n connection device-server established \n\n"));
        delay(10);
      }
    }

void setup() {
    Serial.begin(115200);
    Serial.flush();
    pinMode(pino_sinal_analogico, INPUT);

    Serial.println("INICIO CONEXAO");
    initWiFi();
    initWS();
    delay(100);
}

void loop() {
    if(!socket.connected()){
        initWS();
        socket.emit("getvalue", "bmd"); //pegar valor do estado da bomba
    }
    /*if(timeElapsed > timeSend){
        sendData(); //enviar Json cm serial e valor 
    }*/
    sendData();
    delay(5000); //trocar p interrupção 

    socket.monitor();
}