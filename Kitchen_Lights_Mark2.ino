#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define motion01 D5
#define motion02 D6
#define motion03 D7
#define led_strip D1

#define MQTT_AUTH false
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"

int smotion01 = 0;
int smotion02 = 0;
int smotion03 = 0;

int lamp_mode = 0; //0-auto 1-manual



const char* ssid = "<SSID>";
const char* password = "<SSID_PASSWORD>";

const String HOSTNAME  = "d1-kitchen-lights";
const String MQTT_LIGHT_TOPIC = "light/kitchen/led/light";
const String MQTT_LIGHT_STATE_TOPIC = "light/kitchen/led/light/status";
const char* MQTT_SERVER = "<SERVER_IP>";

WiFiClient wclient;
PubSubClient client(MQTT_SERVER,1883,wclient);


void turn_on(){
  digitalWrite(led_strip,HIGH);
  client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"ON");
  delay(30000);
}

//-------------------------------------------

void turn_off(){
  digitalWrite(led_strip,LOW);
  client.publish(MQTT_LIGHT_STATE_TOPIC.c_str(),"OFF");
  delay(100);
}

//-------------------------------------------

void checkSensors(){
  smotion01 = digitalRead(motion01);
  Serial.print("smotion01=");
  Serial.println(smotion01);
  delay(100);
  smotion02 = digitalRead(motion02);
  Serial.print("smotion02=");
  Serial.println(smotion02);
  delay(100);
  smotion03 = digitalRead(motion03);
  Serial.print("smotion03=");
  Serial.println(smotion03);
  delay(100);
 if( (smotion01 == HIGH) || (smotion02 == HIGH) || (smotion03 == HIGH) ){
//  if(  (smotion03 == HIGH)  ){    
    turn_on();
    
  }
  else{
    turn_off();
  }

}

//-------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  for (int i=0; i<length; i++) {
    payloadStr += (char)payload[i];
  }
  Serial.println(payloadStr);
  String topicStr = String(topic);
  if(topicStr.equals(MQTT_LIGHT_TOPIC)){

    if(payloadStr.equals("off")){
      turn_off();
      lamp_mode = 1;
    }
    else if(payloadStr.equals("on")){
      turn_on();
      lamp_mode = 1;      
    }
    if (payloadStr.equals("auto")){
      Serial.println("auto"); 
      lamp_mode = 0;          
    } 
  } 
}

//-------------------------------------------

bool checkMqttConnection(){
  if (!client.connected()) {
    if (MQTT_AUTH ? client.connect(HOSTNAME.c_str(),MQTT_USERNAME, MQTT_PASSWORD) : client.connect(HOSTNAME.c_str())) {
      //SUBSCRIÇÃO DE TOPICOS
      Serial.println("CONNECTED");
      client.subscribe(MQTT_LIGHT_TOPIC.c_str());
    }
  }
  return client.connected();
}
 
//-------------------------------------------


void setup() {
  Serial.begin(115200);
  client.setCallback(callback);
  pinMode(motion01,  INPUT);
  pinMode(motion02,  INPUT);
  pinMode(motion03,  INPUT);
  pinMode(led_strip, OUTPUT);

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println(".");
  // Aguarda até estar ligado ao Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Ligado a ");
  Serial.println(ssid);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

//-------------------------------------------

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    //if (checkMqttConnection()){
      client.loop();
      if( lamp_mode == 0){
        checkSensors();
      }
    //}
  }
}



