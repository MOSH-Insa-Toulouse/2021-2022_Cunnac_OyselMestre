 #include <stdlib.h>
#include <string.h>
#include <stdio.h>
// ========= Initialisation Sensor ========= //

const int sensorPin = 0; //initialisation du sensorPin sur A0
int Voltage_ADC_map ;
float Voltage_ADC ;

// Valeurs composants circuit 

float R1 = 100000.0;
float R2 = 1000.0;
float R3 = 100000.0;
float R5 = 10000.0;
float Vcc = 5.0;

// ========= Initialisation Bluetooth ========= //
#include <SoftwareSerial.h>
#define rxPin 10  // Broche 10 en tant que RX, à raccorder sur TX du HC-05
#define txPin 11  // Broche 11 en tant que TX, à raccorder sur RX du HC-05
#define baudrate 38400
#define baudrate2 9600
SoftwareSerial bluetooth(rxPin,txPin); // Définition du software serial

volatile int envoi =0; // variable pour gérer l'autorisation d'envoi de données à l'appli
volatile int menu_envoi =0;  // variable pour gérer le menu d'autorisation d'envoi de données à l'appli
volatile int reception_message_appli =0; // si reception message de l'appli alors variable à 1

// ========= Initialisation OLED ========= //

#include <Adafruit_SSD1306.h>

// Initialisation & Paramètrages écran OLED
#define nombreDePixelsEnLargeur   128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur   64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED           -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED       0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)
volatile Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

// Image à afficher
#define largeurDeLimage           128          // Largeur de l'image à afficher, en pixels
#define hauteurDeLimage           64          // Hauteur de l'image à afficher, en pixels

// 'R (1)', 128x64px
const unsigned char imageAafficher [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xbf, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1f, 0xfe, 0x40, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x3c, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x7f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x40, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0x07, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xfe, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3e, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x03, 0xe0, 0x00, 0x0f, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0xe0, 0x00, 0x01, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ========= Initialisation Encodeur Rotatoire ========= //

//Parameters
volatile const int clkPin  = 3;
volatile const int dtPin  = 4;
volatile const int swPin  = 2;

//Variables
volatile int rotVal  = 1;
volatile bool clkState  = LOW;
volatile bool clkLast  = HIGH;
volatile bool swState  = HIGH;
volatile bool swLast  = HIGH;

volatile int choix_affichage=1;
volatile int Switch=0;
volatile int Menu=0;

// *********************************************************//
// *********************************************************//

void setup() {
  Serial.begin(38400);
  //Serial.begin(baudrate);
  Serial.println(F("Initialize System"));
  
  // ========= Setup Bluetooth ========= //
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  bluetooth.begin(baudrate2);
  //Serial.println(F("Setup BT ok"));
  // ========= Setup Encodeur Rotatoire ========= //
  pinMode(clkPin,INPUT);
  pinMode(dtPin,INPUT);
  pinMode(swPin,INPUT_PULLUP);

  pinMode(clkPin, INPUT_PULLUP);  // rajouté
  attachInterrupt(1,readRotary,CHANGE);  // rajouté
  attachInterrupt(0,readRotary,CHANGE);  // rajouté
  //Serial.println(F("Setup Encodeur ok"));
  // ========= Setup OLED ========= //
  //ecranOLED.clearDisplay(); 
  // Initialisation de l'écran OLED
  //Serial.println(F("Setup oled ouais"));
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)){
    //Serial.println(F("Setup oled AIE")); 
    while(1);  }    // Arrêt du programme (boucle infinie) en cas d'échec de l'initialisation
  //Serial.println(F("Setup oled aller"));
  ecranOLED.clearDisplay();   // Effaçage de la mémoire tampon de l'écran OLED
  delay(200);                            
  //Serial.println(F("Setup oled ok"));
  // ========= ANIMATION OLED BEGINNING ========= //
  drawcircle();    // Draw circles (outlines)
  fillcircle();    // Draw circles (filled)
  //Serial.println(F("Animation initialisation ok"));
  // ========= FIN Setup ========= //
  //Serial.println("Fin Setup");
}

// *********************************************************//
// *********************************************************//

void loop() {
  //Serial.print("Menu =");
  //Serial.print(Menu);
  //Serial.print("    Switch = ");
  //Serial.println(Switch);

  // ========= Lecture Sensor ========= //
  Voltage_ADC_map = analogRead(sensorPin) ;
  //Serial.print("Voltage_ADC_map : ");
  //Serial.print(Voltage_ADC_map);
  Voltage_ADC = (float) 5/1023*Voltage_ADC_map ; // transfert de la donnée brute en tension réelle
  //Serial.print("  Voltage_ADC : ");
  //Serial.print(Voltage_ADC);
  float R_sensor = (1+R3/R2)*R1*Vcc/Voltage_ADC - R1 - R5 ; // calcul de la résistance correspondante
  //Serial.print("  Rsensor : ");
  //Serial.println(R_sensor);
  // ========= Envoi donnée Résistance Appli ========= //
//  // ecouter le bluetooth
//  // si reception message
//  // reception_message_appli =1 ;
  int i=0;
  char message[32]={0};       // A CHANGER

  //while (bluetooth.available()){
    
    // Acquisition message
    
    do{
      message[i++]=bluetooth.read();
      delay(3);       
      }while (bluetooth.available() >0) ;
      
      //Serial.print("Message = ");
      //Serial.println(message);
    
    if (strstr(message,"START_ACQUISITION")){ reception_message_appli=1; Serial.print("RéceptionMessage Start_acquisition");}   // A CHANNGEEEER
    if (strstr(message,"STOP_ACQUISITION")){ reception_message_appli=0; }
//}

  if (reception_message_appli==1){
    bluetooth.print("0");
    bluetooth.println((long int)R_sensor);
    //Serial.println((long int))R_sensor);
  }
  /*Serial.print("Voltage_ADC_map : ");
  Serial.print(Voltage_ADC_map);
  Serial.print("  Voltage_ADC : ");
  Serial.print(Voltage_ADC);*/
  Serial.print("  Rsensor : ");
  Serial.println((long int)R_sensor);
  
//  if (reception_message_appli==1){  // si demande de l'appli d'envoi de données
//    
//    if ((menu_envoi==1) && (Switch==0)){  // menu autorisation d'envoi
//      ecranOLED.clearDisplay();
//      ecranOLED.setTextSize(2); 
//      ecranOLED.setCursor(0,1); 
//      ecranOLED.setTextColor(SSD1306_WHITE);  
//      if ((rotVal==1)||(rotVal==3)){
//        ecranOLED.println("Envoyer les données ?");
//        ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
//        ecranOLED.println("Oui");
//        ecranOLED.setTextColor(SSD1306_WHITE);
//        ecranOLED.println("Non");
//        }
//      if ((rotVal==2)||(rotVal==4)){
//        //ecranOLED.setTextColor(SSD1306_WHITE);
//        ecranOLED.println("Envoyer les données ?");
//        ecranOLED.println("Oui");
//        ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
//        ecranOLED.println("Non");          
//      }
//      ecranOLED.display();    
//    }
//    else{
//      Switch=0;
//      menu_envoi=0;
//      reception_message_appli=0;
//      if (envoi==1){  // si autorisation d'envoyer les données
//        // commande écriture canal bluetooth de la valeur de la résistance
//      }
//      // else{ // Rien ? }   
//    }
//  }

  
  
  // ========= Affichage OLED selon Encodeur Rotatoire ========= //
  //Serial.print("AMenu =");
  //Serial.print(Menu);
  //Serial.print("    Switch = ");
  //Serial.println(Switch);
  if ((Menu==1) && (Switch==0)){  // menu
     //Serial.println("Here20");
    // AFFICHAGE OLED MENU 
    // CURSEUR SUR LA 1ERE LIGNE

    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(2); 
    ecranOLED.setCursor(0,1); 
    //ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
    ecranOLED.setTextColor(SSD1306_WHITE);
    
      //Serial.println("Here1")
    if (rotVal==1){;
      ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      ecranOLED.println("Resistance");
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("Zidane");
      ecranOLED.println("INSA");
      ecranOLED.println("Credits"); 
    }
    if (rotVal==2){
      //Serial.println("Here2");
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("Resistance");
      ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      ecranOLED.println("Zidane");
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("INSA");
      ecranOLED.println("Credits");  
    }
    if (rotVal==3){    
      //Serial.println("Here3"); 
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("Resistance");
      ecranOLED.println("Zidane");
      ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      ecranOLED.println("INSA");
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("Credits"); 
    }
    if (rotVal==4){
      //Serial.println("Here4");
      ecranOLED.setTextColor(SSD1306_WHITE);
      ecranOLED.println("Resistance");
      ecranOLED.println("Zidane");
      ecranOLED.println("INSA");
      ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      ecranOLED.println("Credits");   
    }

    ecranOLED.display();    
    //Serial.println("Here30");
  }
  
  else{ // si choix dans le menu effectué alors affichage en conséquence
    
    if (choix_affichage==1){  // AFFICHAGE VALEUR RESISTANCE
      
      affichage_OLED(R_sensor);  // affichage de la valeur sur l'écran OLED
    }
    
    if (choix_affichage==2){  // AFFICHAGE IMAGE ZIDANE
      ecranOLED.clearDisplay(); 
      // Initialisation de l'écran OLED
      if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
        while(1);                               // Arrêt du programme (boucle infinie) en cas d'échec de l'initialisation
        
      // Affichage d'une image au centre de l'écran
      ecranOLED.clearDisplay();                           // Effaçage de la mémoire tampon de l'écran OLED
    
      ecranOLED.drawBitmap(
        (ecranOLED.width()  - largeurDeLimage ) / 2,      // Position de l'extrême "gauche" de l'image (pour centrage écran, ici)
        (ecranOLED.height() - hauteurDeLimage) / 2,       // Position de l'extrême "haute" de l'image (pour centrage écran, ici)
        imageAafficher,
        largeurDeLimage,
        hauteurDeLimage,
        WHITE);                                           // "couleur" de l'image
    
      ecranOLED.display();                                // Transfert de la mémoire tampon à l'écran OLED, pour affichage
      //delay(500);
      }
      
    if (choix_affichage==3){  // AFFICHAGE
          
      } 
      
    if (choix_affichage==4){  // AFFICHAGE Crédits Scrolling txt
      scrolltext();
      }
    //Serial.println("Here14E");

    Switch=0;
    Menu=0;
  }
  

  delay(500);
}

// *********************************************************//
// *********************************************************//

// ========= Fonction Lecture Encodeur Rotatoire ========= //

void readRotary( ) { /* function readRotary */
    //noInterrupts();
    //Serial.println("Here10");
    
    //gestion rotation
    clkState = digitalRead(clkPin);
    if ((clkLast == LOW) && (clkState == HIGH)) {//rotary moving
      //Serial.println("Rotary position ");
      if (digitalRead(dtPin) == HIGH) {
        rotVal = rotVal - 1;
        // MONTER DUNE LIGNE LE CURSEUR DU MENU
        if ( rotVal < 1 ) {
          rotVal = 1;
          // PAS BOUGER
        }
      }
      else {
        rotVal++;
        // DESENDRE DUNE LIGNE LE CURSEUR DU MENU
        if ( rotVal > 4 ) { rotVal = 4; }
      }
      //Serial.print("RotVal = ");
      //Serial.println(rotVal);
      delay(100);
      if(reception_message_appli==1){ menu_envoi=1; } // activation du menu choix affichage oled
      else { Menu=1; /*Serial.println("Menu=1");*/} // activation du menu autorisation transmission données
    
    }
    clkLast = clkState;

    //gestion bouton
    swState = digitalRead(swPin);
    if (swState == LOW && swLast == HIGH) {
      //Serial.println("Rotary pressed");
      if (Menu==1){choix_affichage=rotVal;}
      Switch=1;
      //Menu=1;
      if(menu_envoi==1){ // CHOIX POUR MENU ENVOI 
      if((rotVal==1)&&(rotVal==3)){envoi=1;}                              
      else if((rotVal==2)&&(rotVal==4)){envoi=0;} 
       }
      delay(100);//debounce
    }
    swLast = swState;  
}

// ========= Fonction Affichage valeur Résistance OLED ========= //

void affichage_OLED(float R_sensor){
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(3);
  ecranOLED.setCursor(0,0); 
  ecranOLED.setTextColor(SSD1306_WHITE); //ecranOLED.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
  ecranOLED.print("R=");
  R_sensor = R_sensor/1000000 ;
  ecranOLED.print(R_sensor);
  ecranOLED.print(" Momhs");

  ecranOLED.display();
}

// ========= Fonction Affichage Scroll Text ========= //

void scrolltext(void) {
  ecranOLED.clearDisplay();

  ecranOLED.setTextSize(2); // Draw 2X-scale text
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setCursor(1, 0);
  ecranOLED.println(F("Alex O.M."));
  ecranOLED.println(F("Yohan C."));
  ecranOLED.println(F("4GP INSA"));
  ecranOLED.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  ecranOLED.startscrollright(0x00, 0x0F);
  delay(2000);
  ecranOLED.stopscroll();
  delay(1000);
  ecranOLED.startscrollleft(0x00, 0x0F);
  delay(2000);
  ecranOLED.stopscroll();
  delay(1000);
  ecranOLED.startscrolldiagright(0x00, 0x07);
  delay(2000);
  ecranOLED.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  ecranOLED.stopscroll();
  delay(1000);
}


// ========= Fonctions Affichage Animations ========= //

void drawcircle(void) {
  ecranOLED.clearDisplay();

  for(int16_t i=0; i<max(ecranOLED.width(),ecranOLED.height())/2; i+=2) {
    ecranOLED.drawCircle(ecranOLED.width()/2, ecranOLED.height()/2, i, SSD1306_WHITE);
    ecranOLED.display();
    delay(20);
  }

  delay(1000);
}

void fillcircle(void) {
  ecranOLED.clearDisplay();

  for(int16_t i=max(ecranOLED.width(),ecranOLED.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    ecranOLED.fillCircle(ecranOLED.width() / 2, ecranOLED.height() / 2, i, SSD1306_INVERSE);
    ecranOLED.display(); // Update screen with each newly-drawn circle
    delay(100);
  }

  delay(1500);
}
