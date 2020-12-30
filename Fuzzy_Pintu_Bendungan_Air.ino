#include <Blynk.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>

#include <BlynkSimpleEsp8266.h>

char auth[] = "X1nuuroGbV0yF8jSp9INMH95Khori4YS";
char ssid[] = "Aial";
char pass[] = "aial1234";

BlynkTimer waktu;

//sesuaikan posisi pin select
int s0 = D3;
int s1 = D2;
int s2 = D1;

//gunakan A0 sebagai input
int analogPin = A0;
int lampu =0;
int modeBak = 0;

float hasilfuzBak1[3], hasilfuzBak2[3];
float valTertutup[3] = {0 , 0 , 0 };
float valSetengah[3] = {0 , 0 , 0};
float valTerbuka[3] = {0 , 0 , 0 };
float valCog[1024];
float arTertutup[1024];
float arSetengah[1024];
float arTerbuka[1024];
float terTertutup;
float terSetengah ;
float terTerbuka ;
float finalVal;
String classification;

float finalClassification(){
  for (int i = 0; i <=255; i++){
    arTertutup[i] = valCog[i];
  }
  for (int i = 256; i <=1023; i++){
    arTertutup[i] = 0;
  }

  for (int i =0 ; i <=178; i++){
    arTertutup[i] = 0;
  } 
  for (int i =179 ; i <=324; i++){
    arSetengah[i] = valCog[i];
  }
  for (int i =325 ; i <=1023; i++){
    arSetengah[i] = 0;
  }

  for (int i = 0; i <=512; i++){
    arTerbuka[i] = 0;
  }
  for (int i = 513; i <=1023; i++){
    arTerbuka[i] = valCog[i];
  }

  if(arTertutup[(int)finalVal] >= arSetengah[(int)finalVal] && arTertutup[(int)finalVal] >= arTerbuka[(int)finalVal]){
    classification = "Pintu Tertutup";
  }else if(arSetengah[(int)finalVal] >= arTertutup[(int)finalVal] && arSetengah[(int)finalVal] >= arTerbuka[(int)finalVal]){
    classification = "Pintu Terbuka Setengah";
  }else if(arTerbuka[(int)finalVal] >= arSetengah[(int)finalVal] && arTerbuka[(int)finalVal] >= arTertutup[(int)finalVal]){
    classification = "Pintu Terbuka";
  }
}

float Bak1Fuzzifikasi(int bak1){
  float hasilfuz[3];
  //sedikit
  if (bak1 <= 255){
    hasilfuz[0] = 1.0;
  }else if (bak1 >= 178 && bak1 <= 256){
    hasilfuz[0] = (float)(256 - bak1)/ (float)20;
  }else{
    hasilfuz[0] = 0;
  }

  //setengah
  if (bak1 >= 178 && bak1 <= 255){
    hasilfuz[1] = (float)(bak1 - 178)/(float)20;
  }else if (bak1 >= 256 && bak1 <= 525){
    hasilfuz[1] = (float)(525 - bak1)/(float)20;
  }
  else{
    hasilfuz[1] = 0;
  }

  //penuh
  if (bak1 >= 255 && bak1 <= 325){
    hasilfuz[2] = (float)(bak1 - 255)/(float)20;
  }else if (bak1 >= 326 ){
    hasilfuz[2] = 1.0 ;
  }else{
    hasilfuz[2] = 0;
  }

  for (int i = 0; i < 3; i++)
  {
    hasilfuzBak1[i] = hasilfuz[i];
//    Serial.println(hasilfuzTemp[i]);
  }
  
}

float Bak2Fuzzifikasi(int bak2){
  float hasilfuz[3];
  //sedikit
  if (bak2 <= 255){
    hasilfuz[0] = 1.0;
  }else if (bak2 >= 178 && bak2 <= 256){
    hasilfuz[0] = (float)(256 - bak2)/ (float)20;
  }else{
    hasilfuz[0] = 0;
  }

  //setengah
  if (bak2 >= 178 && bak2 <= 255){
    hasilfuz[1] = (float)(bak2 - 178)/(float)20;
  }else if (bak2 >= 256 && bak2 <= 525){
    hasilfuz[1] = (float)(525 - bak2)/(float)20;
  }
  else{
    hasilfuz[1] = 0;
  }

  //penuh
  if (bak2 >= 255 && bak2 <= 325){
    hasilfuz[2] = (float)(bak2 - 255)/(float)20;
  }else if (bak2 >= 326 ){
    hasilfuz[2] = 1.0 ;
  }else{
    hasilfuz[2] = 0;
  }

  for (int i = 0; i < 3; i++)
  {
    hasilfuzBak2[i] = hasilfuz[i];
//    Serial.println(hasilfuzTemp[i]);
  }
  
}

float ruleTable(){
  //Tertutup
  if(hasilfuzBak1[2] <= hasilfuzBak2[1]){
    valTertutup[0] = hasilfuzBak1[2];
  }else{
    valTertutup[0] = hasilfuzBak2[1];
  }
  
  if(hasilfuzBak1[1] <= hasilfuzBak2[2]){
    valTertutup[1] = hasilfuzBak1[1];
  }else{
    valTertutup[1] = hasilfuzBak2[2];
  }

  if(hasilfuzBak1[2] <= hasilfuzBak2[2]){
    valTertutup[2] = hasilfuzBak1[2];
  }else{
    valTertutup[2] = hasilfuzBak2[2];
  }

  //Setengah
  if(hasilfuzBak1[2] <= hasilfuzBak2[1]){
    valSetengah[0] = hasilfuzBak1[2];
  }else{
    valSetengah[0] = hasilfuzBak2[1];
  }

  if(hasilfuzBak1[1] <= hasilfuzBak2[1]){
    valSetengah[1] = hasilfuzBak1[1];
  }else{
    valSetengah[1] = hasilfuzBak2[1];
  }

  if(hasilfuzBak1[0] <= hasilfuzBak2[2]){
    valSetengah[1] = hasilfuzBak1[0];
  }else{
    valSetengah[1] = hasilfuzBak2[2];
  }
  
  //Terbuka
  if(hasilfuzBak1[0] <= hasilfuzBak2[0]){
    valTerbuka[0] = hasilfuzBak1[0];
  }else{
    valTerbuka[0] = hasilfuzBak2[0];
  }

  if(hasilfuzBak1[1] <= hasilfuzBak2[0]){
    valTerbuka[0] = hasilfuzBak1[1];
  }else{
    valTerbuka[0] = hasilfuzBak2[0];
  }
  
  if(hasilfuzBak1[0] <= hasilfuzBak2[1]){
    valTerbuka[1] = hasilfuzBak1[0];
  }else{
    valTerbuka[1] = hasilfuzBak2[1];
  }

}

float maxRuleTable(){
   terTertutup = valTertutup[0];
   terSetengah = valSetengah[0];
   terTerbuka = valTerbuka[0];
  
  for (int i = 0; i < 3; i++)
  {
    if (valTertutup[i] >= terTertutup ){
        terTertutup = valTertutup[i];
    }
  }

  for (int i = 0; i < 3; i++)
  {
    if (valSetengah[i] >= terSetengah ){
        terSetengah = valSetengah[i];
    }
  }

  for (int i = 0; i < 3; i++)
  {
    if (valTerbuka[i] >= terTerbuka ){
        terTerbuka = valTerbuka[i];
    }
  }
}

float centerOfGrafity(){
  float atas, bawah, pembanding;
  
  for (int i = 0; i <= 178; i++)
  {
    valCog[i] = terTertutup;
  }

  for (int i = 179; i <= 255; i++)
  {
    pembanding = (float)(255-i)/42;
    if (pembanding >= terTertutup){
      valCog[i] = terTertutup;
    }else if(pembanding < terTertutup){
      valCog[i]= pembanding;
    }
    
    pembanding = (float)(i-179)/42;
    if (pembanding >= terSetengah){
      if (valCog[i] > terSetengah){
        valCog[i] = valCog[i];
      }else{
        valCog[i] = terSetengah;
      }
    }else if(pembanding < terSetengah){
      if (valCog[i] > pembanding){
        valCog[i] = valCog[i];
      }else{
        valCog[i] = pembanding;
      }
    }
  }
  
  for (int i = 256; i <= 512; i++)
  {
    valCog[i] = terSetengah;
  }

  for (int i = 513; i <= 324; i++)
  {
    pembanding = (float)(324-i)/42;
    if (pembanding >= terSetengah){
      valCog[i] = terSetengah;
    }else if(pembanding < terSetengah){
      valCog[i]= pembanding;
    }
    
    pembanding = (float)(i-513)/42;
    if (pembanding >= terTerbuka){
      if (valCog[i] > terTerbuka){
        valCog[i] = valCog[i];
      }else{
        valCog[i] = terTerbuka;
      }
    }else if(pembanding < terTerbuka){
      if (valCog[i] > pembanding){
        valCog[i] = valCog[i];
      }else{
        valCog[i] = pembanding;
      }
    }
  }

  for (int i =325; i <= 1023; i++)
  {
    valCog[i] = terTerbuka;
  }


  for (int i = 0; i < 1024; i++)
  {
    atas += (float)i * (float)valCog[i];
    bawah += (float)valCog[i];
  }
  float hasilAkhir = (float)atas/(float)bawah;
  if (modeBak == 0){
      int out = hasilAkhir * 1024/ 255;
  }
  finalVal = hasilAkhir;

  return hasilAkhir;
}


void setup() {
// put your setup code here, to run once:
Serial.begin(9600);
pinMode(s0, OUTPUT);
pinMode(s1, OUTPUT);
pinMode(s2, OUTPUT);
Blynk.begin(auth, ssid, pass);
//waktu.setInterval(22000L);
delay(1000);
}

void loop(){
  int potensiobak1; //10k
  digitalWrite(s0,LOW);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);
  potensiobak1 = analogRead(analogPin);
//  Serial.print(potensioTemp);
//  Serial.print(" ");
  delay(100);

  int potensiobak2; //50k
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);
  potensiobak2 = analogRead(analogPin);
//  Serial.println(potensioCah);
  delay(100);

  Bak1Fuzzifikasi(potensiobak1);
  Bak2Fuzzifikasi(potensiobak2);
  ruleTable();
  maxRuleTable();

  String stringBak1 = String(potensiobak1);
  String stringBak2 = String(potensiobak2);
  String stringFuzzyVal = String(centerOfGrafity());
  finalClassification();

  Serial.print("Bak1= ");
  Serial.println((char*) stringBak1.c_str());
  Serial.print("Bak2= "); 
  Serial.println((char*) stringBak2.c_str());
  Serial.print("Nilai Fuzzy= ");
  Serial.println((char*) stringFuzzyVal.c_str());
  Serial.print("Classification= ");
  Serial.println((char*) classification.c_str());
  Serial.print("---------------------------------------------------------\n");

  Blynk.virtualWrite(V2,stringFuzzyVal.c_str());
  Blynk.notify((char*) classification.c_str());

  Blynk.run(); 
  waktu.run(); 
  delay(100);
}

