#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#define pino_bomba D1 

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

void activePump(String payload){
    Serial.println("Conteudo: "+ payload);
    onPump = payload.toInt();
    digitalWrite(LED_BUILTIN, !(onPump));
    digitalWrite(pino_bomba, onPump);

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
      }
    }




void setup() {
    Serial.begin(115200);
    Serial.flush();
    pinMode(pino_bomba, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);

    Serial.println("INICIO CONEXAO");
    initWiFi();
    initWS();
    delay(100);
}

void loop() {
    if(!socket.connected()){
        initWS();
        //socket.emit("getvalue", "bmd"); //pegar valor do estado da bomba
    }
    socket.monitor();
}