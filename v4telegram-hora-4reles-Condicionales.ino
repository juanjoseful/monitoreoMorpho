
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Las comillas indica que la libraria está en la misma carpeta del sketck
#include <NTPClient.h> //importamos la librería del cliente NTP
#include <WiFiUdp.h> // importamos librería UDP para comunicar con 
                     // NTP
#include "DHT.h"
#define DHTTYPE DHT11 // DHT 11


// Introduzca los datos de configuración del router WIFI
char ssid[] = "sdkjfghkdsf";     // Aquí el SSID (nombre)
char password[] = "wergihng"; // La clave PASSWORD
String hora;
String hora1;

//iniciamos el cliente udp para su uso con el server NTP de la hora
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.south-america.pool.ntp.org",-18000,2000);


// Los datos del Token del Telegram BOT proporcionado por Telegram.
#define BOTtoken "riut5hy69845ugdfdg" 
 
// Su Bot Token (Proporcionado desde Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //tiempo medio entre escaneo de mensajes – 1 segundo).
long Bot_lasttime;   //la última vez que se realizó la exploración de mensajes.
bool Start = false;

// Pines del ESP
const int rele1 = 2; // Pin de configuración del Relé 1 o LED usado
const int rele2 = 4; // Pin de configuración del Relé 2 o LED usado
const int rele3 = 0; // Pin de configuración del Relé 3 o LED usado
const int rele4 = 16; // Pin de configuración del Relé 4 o LED usado

int ledStatus = 0;    // Iniciamos el estado a 0 – apagado.
int ledStatus2 = 0;    // Iniciamos el estado a 0 – apagado.
int ledStatus3 = 0;    // Iniciamos el estado a 0 – apagado.
int ledStatus4 = 0;    // Iniciamos el estado a 0 – apagado.

uint8_t DHTPin = 12; //pin ESP para el sensor DHT
DHT dht(DHTPin, DHTTYPE);


void setup() {
  
  Serial.begin(115200);

  // Establezca WiFi en modo estación y desconéctese de un AP si estaba conectado previamente
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  // Intenta conectarse a la red Wifi:
  Serial.print("Conectando a red Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // El programa se quedará en este bucle hasta que el ESP conecte con el router.
    Serial.print(".");
    delay(500);
    }
  Serial.println("");                            // Aquí continúa una vez conectado
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  client.setInsecure(); // Si su núcleo no puede manejar conexiones seguras
  
  pinMode(rele1, OUTPUT); // Inicializa rele 1 digital como salida.
  delay(10);
  digitalWrite(rele1, HIGH); // Inicializa el rele 1 en OFF

  pinMode(rele2, OUTPUT); // Inicializa rele 2 digital como salida.
  delay(10);
  digitalWrite(rele2, HIGH); // Inicializa el rele2 en OFF
  

  pinMode(rele3, OUTPUT); // Inicializa rele 3 digital como salida.
  delay(10);
  digitalWrite(rele3, HIGH); // Inicializa el rele 3 en OFF

  pinMode(rele4, OUTPUT); // Inicializa rele 3 digital como salida.
  delay(10);
  digitalWrite(rele4, HIGH); // Inicializa el rele 3 en OFF

  pinMode(DHTPin, INPUT);   
  dht.begin();             //Iniciamos el sensor
  
  }
  
void loop() {

  timeClient.update(); //sincronizamos con el server NTP
  hora = timeClient.getFormattedTime(); //esta toma la hora completa


    //int getDay() const;
    //int getMinutes() const;
    //int getSeconds() const;
   
   int solohora = timeClient.getHours();
   Serial.println("imprimiendo solo la hora"); 
   Serial.println(solohora); 

   //aqui se ponen las condiciones de los rele respecto a la hora
   if (solohora <= 21){
      Serial.println("son menos de las 21");
      digitalWrite(rele1, LOW);
      }
   if (solohora >= 23){
      Serial.println("son mas de las 23");
      digitalWrite(rele2, LOW);ledStatus = 1;
      digitalWrite(rele3, LOW);ledStatus = 1;
      digitalWrite(rele4, LOW);ledStatus = 1;
      }
      
  delay(500);
  
if (millis() > Bot_lasttime + Bot_mtbs)  {
int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
while (numNewMessages) {
Serial.println("Obtuvo respuesta");
handleNewMessages(numNewMessages);
numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
Bot_lasttime = millis();
  }
  }

void handleNewMessages(int numNewMessages) {
  Serial.println("Nuevos mensajes"); // Para ver el estado en el puerto serie
  Serial.println(String(numNewMessages));  // Para ver el estado en el puerto serie
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

   float h = dht.readHumidity();
   float t = dht.readTemperature();
   char humedad[6];
   char temperatura[4];

    //los datos de humedad y temperatura debe convertirse
    //a char porque telegram no deja enviar float 
   dtostrf(h, 4, 1, humedad);
   dtostrf(t, 6, 1, temperatura);
    
    if (text == "/hora") {
      bot.sendMessage(chat_id, "Hora en Colombia = ", "");
      bot.sendMessage(chat_id,hora);
      //bot.sendMessage(chat_id, "Tomando solo la parte entera = ", "");
      //bot.sendMessage(chat_id,hora1);
    }

    if (text == "/dht") {
      bot.sendMessage(chat_id, "Humedad = ");
      bot.sendMessage(chat_id,humedad);
      bot.sendMessage(chat_id, "\n");
      bot.sendMessage(chat_id, "Temperatura = ");
      bot.sendMessage(chat_id,temperatura);
    }    
    
    if (text == "/rele1on") {
      digitalWrite(rele1, LOW);   // Activa, enciende el LED – on (HIGH es voltaje nivel alto)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Rele 1 en ON");
    }
    if (text == "/rele1off") {
      ledStatus = 0;
      digitalWrite(rele1, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      bot.sendMessage(chat_id, "Rele 1 en OFF");
    }


    if (text == "/rele2on") {
      digitalWrite(rele2, LOW);   // Activa, enciende el LED – on (HIGH es voltaje nivel alto)
      ledStatus2 = 1;
      bot.sendMessage(chat_id, "Rele 2  en ON");
    }
    if (text == "/rele2off") {
      ledStatus2 = 0;
      digitalWrite(rele2, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      bot.sendMessage(chat_id, "Rele 2 en OFF");
    }

    if (text == "/rele3on") {
      digitalWrite(rele3, LOW);   // Activa, enciende el LED – on (HIGH es voltaje nivel alto)
      ledStatus3 = 1;
      bot.sendMessage(chat_id, "Rele 3  en ON");
    }
    if (text == "/rele3off") {
      ledStatus3 = 0;
      digitalWrite(rele3, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      bot.sendMessage(chat_id, "Rele 3 en OFF");
    }

    if (text == "/rele4on") {
      digitalWrite(rele4, LOW);   // Activa, enciende el LED – on (HIGH es voltaje nivel alto)
      ledStatus4 = 1;
      bot.sendMessage(chat_id, "Rele 4  en ON");
    }
    
    if (text == "/rele4off") {
      ledStatus4 = 0;
      digitalWrite(rele4, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      bot.sendMessage(chat_id, "Rele 4 en OFF");
    }

    if (text == "/allon") {
      ledStatus = 1;
      ledStatus2 = 1;
      ledStatus3 = 1;
      ledStatus4 = 1;
      
      digitalWrite(rele1, LOW);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      digitalWrite(rele2, LOW);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      digitalWrite(rele3, LOW);    // Apaga el LED – off (LOW es voltaje nivel bajo
      digitalWrite(rele4, LOW);    // Apaga el LED – off (LOW es voltaje nivel bajo

      bot.sendMessage(chat_id, "todos los reles en ON");
    }
    
    
    if (text == "/alloff") {
      ledStatus = 0;
      ledStatus2 = 0;
      ledStatus3 = 0;
      ledStatus4 = 0;
      
      digitalWrite(rele1, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      digitalWrite(rele2, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo)
      digitalWrite(rele3, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo
      digitalWrite(rele4, HIGH);    // Apaga el LED – off (LOW es voltaje nivel bajo

      bot.sendMessage(chat_id, "todos los reles en OFF");
    }

    
    if (text == "/estado") {        // Envia el estado del Relé/Led
      if (ledStatus) {
        bot.sendMessage(chat_id, "rele 1 en ON");
      } else {
        bot.sendMessage(chat_id, "rele 1 en OFF");
      }
    

      if (ledStatus2) {
        bot.sendMessage(chat_id, "rele 2 en ON");
      } else {
        bot.sendMessage(chat_id, "rele 2 en OFF");
      }


      if (ledStatus3) {
        bot.sendMessage(chat_id, "rele 3 en ON");
      } else {
        bot.sendMessage(chat_id, "rele 3 en OFF");
      }

      if (ledStatus4) {
        bot.sendMessage(chat_id, "rele 4 en ON");
      } else {
        bot.sendMessage(chat_id, "rele 4 en OFF");
      }

      bot.sendMessage(chat_id, "Hora Colombia = ");
      bot.sendMessage(chat_id,hora);
      bot.sendMessage(chat_id, "Humedad = ");
      bot.sendMessage(chat_id,humedad);
      bot.sendMessage(chat_id, "\n");
      bot.sendMessage(chat_id, "Temperatura = ");
      bot.sendMessage(chat_id,temperatura);

    
    }

    
        if (text == "/start") {      //Es el mensaje que recibimos en el celular cuando conectamos
      String welcome = "Bienvenido al experimento de mariposas, \n";
      welcome += "Puede usar los siguientes comandos:\n\n";
      welcome += "/rele1on : Para encender rele 1 \n";
      welcome += "/rele1off : Para apagar el rele 1 \n\n";
      welcome += "/rele2on : Para encender rele 2 \n";
      welcome += "/rele2off : Para apagar el rele 2 \n\n";
      welcome += "/rele3on : Para encender rele 3 \n";
      welcome += "/rele3off : Para apagar el rele 3 \n\n";
      welcome += "/rele4on : Para encender rele 4 \n";
      welcome += "/rele4off : Para apagar el rele 4 \n\n";
      
      //welcome += "/allon : prender todos los relés \n"; 
      //welcome += "/alloff : apagar todos los relés \n\n"; 
           
      //welcome += "/estado : Estado general del sistema \n";
      //welcome += "/hora : pedir la hora en colombia \n";
      //welcome += "/dht : pedir humedad y temperatura \n";
      
      bot.sendMessage(chat_id, welcome, "");
    }
  }
}
