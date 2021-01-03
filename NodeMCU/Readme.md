# Version Pour NodeMCU

Pour l'instant ce programme a été essayé sur un NodeMCU lolin V3 composé d'un module ESP8266-12F :
* Wifi 2.4G avec WPA/WPA2
* 802.11b/g/n
* Antenne PCB
* PWM/I2C/SPI/UART

## Installation sur IDE Arduino
* Installez l'[IDE Arduino](https://www.arduino.cc/en/software)
* Démarrez Arduino et ouvrez la fenêtre Préférences dans l'onglet `Fichier > Préférences`.
* inserer dans le champ `URL de gestionnaire de cartes supplémentaires`:
```https://arduino.esp8266.com/stable/package_esp8266com_index.json```     
(Vous pouvez ajouter plusieurs URL, en les séparant par des virgules.)
* Ouvrez le Gestionnaire de cartes dans l'onglet `Outils > Type de cartes > Gestionnaire de carte`
* Installez la plate-forme `esp8266` by ESP8266 Community. 
* Sélectionner votre carte ESP8266 dans l'onglet `Outils > Type de cartes >` après l'installation.

## Connection au WIFI:

Dans un premier temps intégrer la librairie:
```c++
#include <ESP8266WiFi.h>
```
Il vous faudra définir le nom de votre de réseau et son mot de passe :
```c++
const char* ssid     = "Nom"; //SSID du Wifi
const char* password = "MDP"; //Mot de passe du Wifi
```
Puis utiliser ce programme :
```c++
void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Nom"); //Nom de votre appareil
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
}
```
Ou ce programme qui permet d'avoir un retour dans le moniteur :
```c++
void setup() {
//Connection
  Serial.begin(74880); // Vitesse de mon NodeMCU (peut-être remplacé par exemple par 9600 bauds)
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Nom"); //Nom de votre appareil
  WiFi.begin(ssid, password);
  Serial.println("Connection en cours");
  while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print("."); delay(250); }
  Serial.println("");
  Serial.println("Connecté au Réseau");
  Serial.print("Address IP: ");
  Serial.println(WiFi.localIP());
}
```
## Requêtes HTTP
Sur le Web, les clients, comme votre navigateur, communiquent avec les serveurs Web en utilisant le protocole HTTP. Ce protocole contrôle la façon dont le client formule ses demandes et la façon dont le serveur y répond. Le protocole HTTP connaît différentes méthodes de requête.
La méthode GET est celle qui nous intéresse, elle est l’ « ancêtre » parmi requêtes HTTP. Cette méthode de requête existe depuis le début du Web. Elle est utilisée pour demander une ressource, par exemple un fichier HTML, au serveur Web.

Si vous entrez l’URL http://192.168.0.0:8080/json.htm?type=command&param=getversion dans votre navigateur, celui-ci se connecte au serveur Web et lui envoie la requête GET :
```
GET /json.htm?type=command&param=getversion
```
#### Pour envoyer :
Dans un premier temps intégrer la librairie:
```c++
#include <ESP8266HTTPClient.h>
HTTPClient http;
```
Il vous faudra définir l'adresse ip de votre de serveur et son port :
```c++
const String AdrIP   = "192.168.0.0"; //Adresse IP de domoticz
const int Port       = 8080;          //Port de domoticz
String URL = "/json.htm?type=command&param=getversion" // Dans cet exemple URL cela demande à domotiticz sa version 
```
Puis utiliser cette fonction :
```c++
void Envoi(){
  HTTP.begin(AdrIP,Port,URL); 
  HTTP.GET();
}
```
Dans notre cas ce programme permet de vérifier que Domoticz à bien actualisé et dans le cas contraire fera 3 tentatives, il y a besoin d'une librairie suplémentaire:
```c++
#include <ArduinoJson.h>
```
```c++
void Envoi(){
  HTTP.begin(AdrIP,Port,URL); int httpCode = HTTP.GET();
  for (int i2=0; i2<3; i2++){
    if (httpCode > 0) {
      json = HTTP.getString();
      DynamicJsonDocument doc(96);
      deserializeJson(doc, json);
      const char* statut = doc["status"]; // "OK"
      if (String(statut) != "ERR"){i2 = 3;
      }
    }
  }
}
```
