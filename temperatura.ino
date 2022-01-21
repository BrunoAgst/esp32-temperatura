//display
#include <Wire.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

//sensor de temperatura e humidade
#include <Adafruit_SHTC3.h>
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

//wifi
#include <ESP8266WiFi.h>
const char* ssid = "";
const char* password = "";
WiFiClient nodemcuClient;

// MQTT
#include <PubSubClient.h>
const char* mqtt_Broker = "192.168.1.130";
PubSubClient client(nodemcuClient);
const char* topicoTemperatura = "esp/temp";
const char* topicoUmidade = "esp/umi";
const char* mqttClientID = "temp/umi";

int temperatura;
int umidade;

void setup() {
  //Serial.begin(115200);
  shtc3.begin();
  configurarDisplay();
  conexaoWifi();
  client.setServer(mqtt_Broker, 1883);
}

void loop() {
  if(!client.connected()) {
    reconectarMQTT();
  }
  medirTemperaturaUmidade();
  publicarTemperaturaUmidade();
  mostrarTemperaturaUmidade();
}

void publicarTemperaturaUmidade(){
  client.publish(topicoTemperatura, String(temperatura).c_str(), true);
  client.publish(topicoUmidade, String(umidade).c_str(), true);

}

void reconectarMQTT(){
  while (!client.connected()) {
    client.connect(mqttClientID);
  }
}

void conexaoWifi(){
  delay(10);
  WiFi.begin(ssid, password);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Conectando");
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
  display.print("Conectado");
  display.display();
  display.clearDisplay();
}


void mostrarTemperaturaUmidade() {
  mostrarMensagemDisplay("Temperatura", (temperatura), " C");
  mostrarMensagemDisplay("Umidade", (umidade), " %");
}

void mostrarMensagemDisplay(const char* texto1, int medicao, const char* texto2) {
  display.setTextSize(1);
  display.setCursor(32,0);
  display.print(texto1);
  display.setTextSize(3);
  display.setCursor(35,8);
  delay(2000);
  display.print(medicao);
  display.setTextSize(2);
  display.print(texto2);
  display.display();
  delay(2000);
  display.clearDisplay();

}

void configurarDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
}

void medirTemperaturaUmidade() {
  sensors_event_t humidity, temp;
  shtc3.getEvent(&humidity, &temp);
  temperatura = temp.temperature;
  umidade = humidity.relative_humidity;
  delay(5000);
}
