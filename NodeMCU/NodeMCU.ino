#include <ArduinoJson.h>
#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
HTTPClient HTTP;

//PZEM004T RX TX
PZEM004Tv30 pzem1(D1, D2);//(RX,TX) connection au PZEM 1
PZEM004Tv30 pzem2(D3, D5);//(RX,TX) connection au PZEM 2
PZEM004Tv30 pzem3(D6, D7);//(RX,TX) connection au PZEM 3

//Variables
const char* ssid     = "Le partage";//SSID du Wifi
const char* password = "C'est la réussite";//Mot de passe du Wifi
const String AdrIP   = "192.168.0.0";//Adresse IP de domoticz
const int Port       = 8080;//Port de domoticz

//PZEM004T n°1
#define I1A 5  //IDX PZEM004T n°1 Fréquence (General Custom Sensor Hz) ou NON si pas utilisé
#define I2A 1  //IDX PZEM004T n°1 Tension (General Voltage V) ou NON si pas utilisé
#define I3A 2  //IDX PZEM004T n°1 Intensité (General Current A) ou NON si pas utilisé
#define I4A 6  //IDX PZEM004T n°1 Facteur de puissance (General Custom Sensor Cos Phi) ou NON si pas utilisé
#define I5A 3  //IDX PZEM004T n°1 Puissance Active (Usage Electric Watts) ou NON si pas utilisé
#define I6A 19 //IDX PZEM004T n°1 Puissance Réactive (General Custom Sensor VAR) ou NON si pas utilisé
#define I7A 22 //IDX PZEM004T n°1 Puissance Apparente (General Custom Sensor VA) ou NON si pas utilisé

//PZEM004T n°2
#define I1B 11 //IDX PZEM004T n°2 Fréquence (General Custom Sensor Hz) ou NON si pas utilisé
#define I2B 7  //IDX PZEM004T n°2 Tension (General Voltage V) ou NON si pas utilisé
#define I3B 8  //IDX PZEM004T n°2 Intensité (General Current A) ou NON si pas utilisé
#define I4B 12 //IDX PZEM004T n°2 Facteur de puissance (General Custom Sensor Cos Phi) ou NON si pas utilisé
#define I5B 9  //IDX PZEM004T n°2 Puissance Active (Usage Electric Watts) ou NON si pas utilisé
#define I6B 20 //IDX PZEM004T n°2 Puissance Réactive (General Custom Sensor VAR) ou NON si pas utilisé
#define I7B 23 //IDX PZEM004T n°2 Puissance Apparente (General Custom Sensor VA) ou NON si pas utilisé

//PZEM004T n°3
#define I1C 17 //IDX PZEM004T n°3 Fréquence (General Custom Sensor Hz) ou NON si pas utilisé
#define I2C 13 //IDX PZEM004T n°3 Tension (General Voltage V) ou NON si pas utilisé
#define I3C 14 //IDX PZEM004T n°3 Intensité (General Current A) ou NON si pas utilisé
#define I4C 18 //IDX PZEM004T n°3 Facteur de puissance (General Custom Sensor Cos Phi) ou NON si pas utilisé
#define I5C 15 //IDX PZEM004T n°3 Puissance Active (Usage Electric Watts) ou NON si pas utilisé
#define I6C 21 //IDX PZEM004T n°3 Puissance Réactive (General Custom Sensor VAR) ou NON si pas utilisé
#define I7C 24 //IDX PZEM004T n°3 Puissance Apparente (General Custom Sensor VA) ou NON si pas utilisé

//PZEM004T GENERAL
#define I0D 34 //IDX Temps restant de boucle ou NON si pas utilisé
#define I1D 25 //IDX PZEM004T Puissance Active Générale (Usage Electric Watts) ou NON si pas utilisé
#define I2D 26 //IDX PZEM004T Puissance Réactive Générale (General Custom Sensor VAR) ou NON si pas utilisé
#define I3D 27 //IDX PZEM004T Puissance Apparente Générale (General Custom Sensor VA) ou NON si pas utilisé
#define I4D 35 //IDX PZEM004T Puissance Active Générale Total (compteur incrémenté kWatts/h) ou NON si pas utilisé
#define I5D 36 //IDX PZEM004T Puissance Réactive Générale Total (compteur incrémenté kVAR/h) ou NON si pas utilisé
#define I6D 37 //IDX PZEM004T Puissance Apparente Générale Total (compteur incrémenté kVA/h) ou NON si pas utilisé
#define I7D 31 //IDX Charge L1 ou NON si pas utilisé
#define I8D 32 //IDX Charge L2 ou NON si pas utilisé
#define I9D 33 //IDX Charge L3 ou NON si pas utilisé
#define I10D 38 //IDX Rapport ou NON si pas utilisé

//###################################
//###     Variables Programme     ###
//###################################

//Mesures
String URL; String rapport; String json; float debut; float fin; int i;
float F1; float V1; float A1; float PF1; float P1; float Q1; float S1;   
float F2; float V2; float A2; float PF2; float P2; float Q2; float S2; 
float F3; float V3; float A3; float PF3; float P3; float Q3; float S3;
float PG; float QG; float SG; float PTG; float QTG; float STG; float L1P; float L2P; float L3P;

int IDX[]= {I0D, I1A, I2A, I3A, I4A, I5A, I6A, I7A, I1B, I2B, I3B, I4B, I5B, I6B, I7B, I1C, I2C, I3C, I4C, I5C, I6C, I7C, I1D, I2D, I3D, I4D, I5D, I6D, I7D, I8D, I9D};

void Envoi(){
  HTTP.begin(AdrIP,Port,URL); int httpCode = HTTP.GET();
  for (int i2=0; i2<3; i2++){
    if (httpCode > 0) {
      json = HTTP.getString();
      DynamicJsonDocument doc(96);
      deserializeJson(doc, json);
      const char* statut = doc["status"]; // "OK"
      if (String(statut) != "ERR"){i2 = 3;}
    }
  }
}

//###################################
//###     Programme Principale    ###
//###################################

void setup() {
//Connection
  Serial.begin(74880);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("Compteur Triphasé");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500);}
  pzem1.resetEnergy();pzem2.resetEnergy();pzem3.resetEnergy(); // Reset les PZEM004T 
}

void loop() {
  rapport = "Etat%20:%20";
   
  F1 = pzem1.frequency(); V1 = pzem1.voltage(); A1 = pzem1.current(); PF1 = pzem1.pf(); P1 = pzem1.power(); //Mesure sur PZEM N°1  
  F2 = pzem2.frequency(); V2 = pzem2.voltage(); A2 = pzem2.current(); PF2 = pzem2.pf(); P2 = pzem2.power(); //Mesure sur PZEM N°2 
  F3 = pzem3.frequency(); V3 = pzem3.voltage(); A3 = pzem3.current(); PF3 = pzem3.pf(); P3 = pzem3.power(); //Mesure sur PZEM N°3 
  
  if(isnan(F1) || isnan(V1) || isnan(A1) || isnan(PF1) || isnan(P1))
  {     rapport += "ERREUR%20PZEM%20004T%20N°1%20SUR%20PHASE%20L1,%20";
        F1 = 0; V1 = 0 ; A1 = 0 ; PF1 = 0 ; P1 = 0 ;               }
  else {rapport += "L1%20OK,%20";}
     
  if(isnan(F2) || isnan(V2) || isnan(A2) || isnan(PF2) || isnan(P2))
  {     rapport += "ERREUR%20PZEM%20004T%20N°2%20SUR%20PHASE%20L2,%20";
        F2 = 0; V2 = 0 ; A2 = 0 ; PF2 = 0 ; P2 = 0 ;               }
  else {rapport += "L2%20OK,%20";}

  if(isnan(F3) || isnan(V3) || isnan(A3) || isnan(PF3) || isnan(P3))
  {     rapport += "ERREUR%20PZEM%20004T%20N°3%20SUR%20PHASE%20L3,%20";
        F3 = 0; V3 = 0 ; A3 = 0 ; PF3 = 0 ; P3 = 0 ;               }        
  else {rapport += "L3%20OK,%20";}
    
  Q1 = V1*A1*(sin(acos(PF1))) ; S1 = sqrt((P1*P1)+ (Q1*Q1));
  Q2 = V2*A2*(sin(acos(PF2))) ; S2 = sqrt((P2*P2)+ (Q2*Q2));
  Q3 = V3*A3*(sin(acos(PF3))) ; S3 = sqrt((P3*P3)+ (Q3*Q3));                  
  PG = (P1+P2+P3); QG = (Q1+Q2+Q3); SG = (S1+S2+S3); 
  PTG = PG/3600; QTG = QG/3600; STG = SG/3600;
  
  if (SG != 0){L1P = (S1 / SG)*100; L2P = (S2 / SG)*100; L3P = (S3 / SG)*100;}
  else {L1P = 0; L2P = 0; L3P = 0;}
  
  float Data[]={  i,  F1,  V1,  A1, PF1,  P1,  Q1,  S1,  F2,  V2,  A2, PF2,  P2,  Q2,  S2,  F3,  V3,  A3, PF3,  P3,  Q3,  S3,  PG,  QG,  SG, PTG, QTG, STG, L1P, L2P, L3P}; // Sauvegarde des données
                   
  for (i = 0; i < 31; i++) { 
    if (String(Data[i]) != "NON") {URL = "/json.htm?type=command&param=udevice&idx=" + String(IDX[i]) + "&nvalue=0&svalue=" + String(Data[i]) + ";"; Envoi();}} // Envoi des données
  if ( String(I10D) != "NON") {URL = "/json.htm?type=command&param=udevice&idx=" + String(I10D) + "&nvalue=0&svalue=" + String(rapport) + ";"; Envoi();}; // Envoi du rapport
  
  fin = millis(); i = (1000 - (fin-debut)); if (i < 0){ i = 0; } delay(i); debut = millis(); // Auto correction du temps de boucle
}
