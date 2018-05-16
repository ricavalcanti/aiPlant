#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#define pino_sinal_analogico A0
#define pino_bomba D2 

const char* SSID = "Ric"; // rede wifi
const char* PASSWORD = "meh123456"; // senha da rede wifi

const char* HOST = "192.168.43.247"; //"192.168.0.107"; // ip/host do broker
int PORT =  3001; // porta do broker
SocketIOClient socket;

Ticker tSend;

//"sendsensor"; -> enviar dados
//"getvalue"; -> emitir pra receber valor da bomba
//"returnvalue"; -> on receber valor da bomba

//TESTAR EMIT NA CONEXÃO!




bool onPump = false; //estado da bomba

float getSensor(){
    return ((analogRead(pino_sinal_analogico))/1023.0);
}

void sendData(){
    
    //enviar Json    
    StaticJsonBuffer<200> jsonBuffer;
    char big_json[200];

    JsonObject& sensorUmd = jsonBuffer.createObject();
    sensorUmd["serial"] = "umd";
    sensorUmd["value"] = getSensor(); //vUmd

    /*JsonArray& dadosJson = jsonBuffer.createArray();
    dadosJson.add(sensorUmd);*/ //para incluir em um vetor

    sensorUmd.printTo(big_json);
    Serial.println();
    Serial.print(big_json);


    socket.emit("sendsensor", big_json);

}

void activePump(String payload){
    Serial.println("Conteudo: "+ payload);
    onPump = payload.toInt();
    digitalWrite(LED_BUILTIN, !(onPump));
    digitalWrite(pino_bomba, onPump);
    if(onPump){
        tSend.attach(3,sendData);
    }else{
        tSend.attach(15,sendData);
    }
    
     

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
    socket.on("returnvalue",activePump); // isso pode vir dps da conexão?
    if (!socket.connect(HOST, PORT)) {
      Serial.println(F("connection device-server failed"));
      return;
    } else if (socket.connected()) {
        Serial.println(F("\n\n connection device-server established \n\n"));
        delay(10);
            //enviar Json    
    StaticJsonBuffer<200> jsonBuffer;
    char big_json[200];

    JsonObject& sensorUmd = jsonBuffer.createObject();
    sensorUmd["serial"] = "bmd";
    //sensorUmd["value"] = getSensor(); //vUmd

    /*JsonArray& dadosJson = jsonBuffer.createArray();
    dadosJson.add(sensorUmd);*/ //para incluir em um vetor

    sensorUmd.printTo(big_json);
    Serial.println();
    Serial.print(big_json);

        socket.emit("getvalue", big_json);
      }
    }




void setup() {
    Serial.begin(115200);
    Serial.flush();
    pinMode(pino_sinal_analogico, INPUT);
    pinMode(pino_bomba, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println("INICIO CONEXAO");
    initWiFi();
    initWS();
    delay(100);
    //tSend.attach(300,sendData);
    
    
}

void loop() {
    if(!socket.connected()){
        initWS();
        //socket.emit("getvalue", "bmd"); //pegar valor do estado da bomba
    }
    socket.monitor();
}