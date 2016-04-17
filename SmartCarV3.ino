//*****************************************************************************
//BT_SmartCar V3.0
//阿布拉機的3D列印與機器人
//http://arbu00.blogspot.tw/
//
//2016/04/14 Writen By Ashing Tsai
//(A):phone controll (B)Avoid mode (C):Trace mode(D)prevent drop mode (Q)Debug mode
//
//******************************************************************************
//#include <Timer.h>  
#include <Servo.h> 
#include <SoftwareSerial.h>

const int M1APin=3;
const int M1BPin=5;
const int M2APin=6;
const int M2BPin=9;


const int sig=2;
const int Eco=4;
const int speaker=12;
const int TracePinL=10;
const int TracePinR=11;
const int ServoPin=13;
Servo servoA;
unsigned long cm;
unsigned long time;
int speed=180;
int angleA=90;
int angle=90;
int angleR=10;
int angleL=165;
int angleM=90;
int Flag=0;
int TraceFlagL=0; 
int TraceFlagR=0; 
int randNumber,RLFlag;
int TraceCount=0;

const int BTPinR=7;
const int BTPinT=8;

char ModeSelect='A';  //4(D)prevent drop mode 3(C):Trace mode, 2(B):auto mode 1(A):phone controll Q:Serial port mode
boolean DirectionFlag=true;  //true=try to turn right first,false=try to turn left first;

SoftwareSerial BT(BTPinR,BTPinT);    
String readString;       
char ch,Fisrtchar,speedchar;      
  
void setup() 
{ 
  BT.begin(9600);      
  Serial.begin(115200);
  Serial.println("press '0'~'9' : setup speed");
  Serial.println("press 'S' : stop");  
  Serial.println("press 'F' :  front");
  Serial.println("press 'B' :  back");   
  Serial.println("press 'R' : turn right");
  Serial.println("press 'L' : turn left"); 

   pinMode(sig,OUTPUT);
   pinMode(Eco,INPUT);
   pinMode(speaker,OUTPUT);
   pinMode(TracePinL,INPUT); 
   pinMode(TracePinR,INPUT); 
   
  servoA.attach(13,620,2620);
  ServoangleA2(angleM);
  delay(200);
  servoA.detach();

 
   digitalWrite(speaker,LOW);

   
   stop();
   Flag=0;
} 
void loop() 
{

//BT+========================
  if(readString.length()>0) {  
    readString="";
  }
  while(BT.available()) {  
        delay(1);   
        ch = BT.read();
        readString+=ch;
  }
    //  Serial.print(readString);
    //  Serial.print('/n');
     // delay(2000);

         if (readString[0]=='A'&&readString[1]=='A'){
             ModeSelect='A'; 
             stop();
         }else if(readString[0]=='B'&&readString[1]=='B'){
             ModeSelect='B';
             stop();
         }else if(readString[0]=='C'&&readString[1]=='C'){
             ModeSelect='C';
             stop();  
         }else if(readString[0]=='D'&&readString[1]=='D'){
             ModeSelect='D';
             stop();
         }
       //  Serial.print(ModeSelect);
       //  Serial.print('/n');
       //  delay(2000);

   if (readString[0]=='P'){
       speedchar=readString[1];
       speed=map(speedchar,'0','9',100,255);
         // Serial.print(readString);
         // Serial.print('/n');
         // delay(500);
    }
//BT-========================

     
 if (ModeSelect=='A'){   //phone mode
  if (readString=="ST") { 
          stop(); 
        } else if (readString=="FW") { 
              ForwardPWM(); 
        } else if (readString=="BW") { 
              backwardPWM(); 
        } else if (readString=="RW") { 
              rightwardPWM();    
        } else if (readString=="LW") { 
              leftwardPWM();    
        }   
        
  }

 if (ModeSelect=='Q'){  //serial port debug mode
  //=============================
   if(Serial.available())
  {
     char key=Serial.read();   
      Serial.print("key=");
      Serial.println(key);
     if(key>='0' && key<='9')
    {
      speed=map(key,'0','9',100,250);
    }  
    if(key=='S'||key=='s')
    {
     stop(); 
    }  
    else if(key=='F'||key=='f')
    {
      stop();
      delay(500);
      ForwardPWM();

    }  
    else if(key=='B'||key=='b')
    {
     stop();
     delay(500);
     backwardPWM(); 
    }  
    else if(key=='R'||key=='r')
    {
     stop();
     rightwardPWM();   
    }  
    else if(key=='L'||key=='l')
    {
     stop();
     leftwardPWM(); 
    } else if(key=='T'||key=='t')
    {
   
     TraceFlagL=digitalRead(TracePinL);
     Serial.println(TraceFlagL);

     TraceFlagR=digitalRead(TracePinR);
     Serial.println(TraceFlagR);

    }          
  }     
 }
   // Checkhit();
//Debug mode-=============================

    //===========================================
    //-Auto mode-----------------------------------------------
    //=========================================== 
   if (ModeSelect=='B'){  //Auto mode
        speed=150;
        ForwardPWM();
        Checkhit();
        
    }//--------------------------------------------------------
    
    //===========================================
    //-Trace mode-----------------------------------------------
    //=========================================== 
   if (ModeSelect=='C'){  //Trace mode
        TraceMode();
    }//-------------------------------------------------------- 


    //-Prevent Drop mode-----------------------------------------------
    //=========================================== 
   if (ModeSelect=='D'){  //PreventDrop mode
        PreventDropMode();
    }//-------------------------------------------------------- 

} 
//Get Supersonic distance  CM
int ping(int sig)
{
   unsigned long cm,duration;
   //===================5us==================
   pinMode(sig,OUTPUT);
   digitalWrite(sig,LOW);
   delayMicroseconds(3);
   digitalWrite(sig,HIGH);
   delayMicroseconds(5);
   digitalWrite(sig,LOW);
   delayMicroseconds(3);
   //=====================================
   pinMode(Eco,INPUT);
   duration=pulseIn(Eco,HIGH);
   cm=duration/29/2;

  // delay(1);
   return cm;
}  
void Checkhit(void)
{
        cm=ping(sig);
        Serial.print(cm);
        Serial.print('\n');

    if ((cm >0)&&(cm <=1500))
 {
   if (cm <=30)
   {
     Flag=Flag+1;
   }
 }
 if (Flag>=3)
    { 
      SafeReturn();
    } 
}

void ForwardPWM() 
{
 
       analogWrite(M1APin,speed);
       analogWrite(M1BPin,0);
       analogWrite(M2APin,speed);
       analogWrite(M2BPin,0);
      
 }

void backwardPWM() 
{
       pinMode(M1APin,OUTPUT);
       pinMode(M1BPin,OUTPUT);
       pinMode(M2APin,OUTPUT);
       pinMode(M2BPin,OUTPUT);

       digitalWrite(M1APin,LOW);
       digitalWrite(M1BPin,HIGH);
       digitalWrite(M2APin,LOW);
       digitalWrite(M2BPin,HIGH);
     //  analogWrite(M1APin,0);
     //  analogWrite(M1BPin,speed);
    //   analogWrite(M2APin,0);
     //  analogWrite(M2BPin,speed);

       
}

void rightwardPWM() 
{
       pinMode(M1APin,OUTPUT);
       pinMode(M1BPin,OUTPUT);
       pinMode(M2APin,OUTPUT);
       pinMode(M2BPin,OUTPUT);
     //  analogWrite(M1APin,speed);
    //   analogWrite(M1BPin,0);
     //  analogWrite(M2APin,0);
     //  analogWrite(M2BPin,speed);
       digitalWrite(M1APin,HIGH);
       digitalWrite(M1BPin,LOW);
       digitalWrite(M2APin,LOW);
       digitalWrite(M2BPin,HIGH);
}

void leftwardPWM() 
{
     //  analogWrite(M1APin,0);
     //  analogWrite(M1BPin,speed);
     //  analogWrite(M2APin,speed);
     //  analogWrite(M2BPin,0);
       pinMode(M1APin,OUTPUT);
       pinMode(M1BPin,OUTPUT);
       pinMode(M2APin,OUTPUT);
       pinMode(M2BPin,OUTPUT);
       digitalWrite(M1APin,LOW);
       digitalWrite(M1BPin,HIGH);
       digitalWrite(M2APin,HIGH);
       digitalWrite(M2BPin,LOW);
}
void stop() 
{
   
  
   pinMode(M1APin,OUTPUT);
   pinMode(M1BPin,OUTPUT);
   pinMode(M2APin,OUTPUT);
   pinMode(M2BPin,OUTPUT);
   digitalWrite(M1APin,LOW);
   digitalWrite(M1BPin,LOW);
   digitalWrite(M2APin,LOW);
   digitalWrite(M2BPin,LOW);
     
}

void Speakersound() 
{

  for(int i=0;i<10;i++)
  {
    tone(speaker,1000);
    delay(25);
    tone(speaker,500);
    delay(25);
  }
  noTone(speaker);
}
void Speakersound2() 
{
  for(int i=0;i<1;i++)
  {
    tone(speaker,1000);
    delay(25);
    tone(speaker,500);
    delay(25);
  }
  noTone(speaker);
}
int ORandom()
{

 // if analog input pin 7 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(7));
  
    // print a random number from 2 to 1
  randNumber = random(0, 2);
  Serial.println(randNumber);

  delay(50);  
  return (randNumber);
  
}
void TraceMode()
 {
     speed=180;
     TraceFlagL=digitalRead(TracePinL);
      Serial.print("TraceFlagL="); 
       Serial.println(TraceFlagL);
     TraceFlagR=digitalRead(TracePinR);
      Serial.print("TraceFlagR="); 
       Serial.println(TracePinR);
   if ((TraceFlagL==1)&&(TraceFlagR==0))   //
  {   DirectionFlag=false;   //try to turn left first;
    
  }else if((TraceFlagL==0)&&(TraceFlagR==1)) {
      DirectionFlag=true; //try to turn right first;
  }

      if ((TraceFlagL==1)||(TraceFlagR==1)){    //for black line. 
  
          TraceCount=0;   
          ForwardPWM();
          delay(100);
          stop();
          delay(200); 
      }else{
           stop();
           Speakersound2();   
           TraceCount=TraceCount+1;

                    if ( DirectionFlag==true){
                      //  if (TraceCount <=30){  
                         backwardPWM();
                         delay(5);
                         rightwardPWM();
                         delay(50);
                     //   }
                    }else{
                      //  if (TraceCount <=30){
                         backwardPWM();
                         delay(5);
                         leftwardPWM();
                         delay(50);   //change to try turn left
                     //   }
                    }
      }
}

void SafeReturn()
{

  int delayF=5;
  int delayS=5;
  int cmR,cmL;
  cmR=0;
  cmL=0;
          Serial.println("SafeReturn"); 
          Flag=0;
          stop();
          Speakersound();   
          delay(200);

//speed down.          
          for(int i=0;i<40;i++)
  {  
          backwardPWM();
          delay(delayF);
          stop();
          delay(delayS);
  }

  servoA.attach(13,620,2620);
  ServoangleA2(angleR);
  delay(500);
  servoA.detach();
  cm=ping(sig);
 if ((cm >0)&&(cm <=1500))
 {  
    for(int i=1;i<=5;i++)
  {
  cmR=(cmR+ping(sig))/i;
 // Serial.print(cmR);
  //Serial.print('\n');
  }
 }
  Serial.print("cmR=");
  Serial.println(cmR);
  servoA.attach(13,620,2620);
  ServoangleA2(angleL);
  delay(500);
  servoA.detach();
  cm=ping(sig);
 if ((cm >0)&&(cm <=1500))
 {  
    for(int i=1;i<=5;i++)
  {
  cmL=(cmL+ping(sig))/i;
 
  }
 }
  Serial.print("cmL=");
  Serial.println(cmL);
  servoA.attach(13,620,2620);
  ServoangleA2(angleM);
  delay(500);
  servoA.detach();
     if (cmR>cmL){
       rightwardPWM();
       delay(500);
    }else
    {
       leftwardPWM();
       delay(500);
    }


      //  RLFlag=ORandom();  //Get random value
          // Serial.println(RLFlag);
      //  if (RLFlag==0x01){    //if 1 then turn right.
       //    rightwardPWM();
       //    delay(200);
      //  }else                 //if 0 turn left
      //  {  leftwardPWM();
      //     delay(200);
      //  }
        
 }
 void PreventDropMode()
 {
 

        TraceFlagL=digitalRead(TracePinL);
         Serial.print("TraceFlagL="); 
         Serial.println(TraceFlagL);
        //delay(2);
        TraceFlagR=digitalRead(TracePinR);
         Serial.print("TraceFlagR="); 
        Serial.println(TraceFlagR);

        speed=120;
      
       ForwardPWM();
       delay(50);
       stop();
       delay(80);

      if ((TraceFlagL==1)||(TraceFlagR==1))
      { 
       SafeReturn();
      }
    }
 
void ServoangleA(int angle)
{

  if  (angleA!=angle){
        while (angleA>angle)
     {
             angleA=angleA-5;
                 servoA.write(angleA);
                 delay(20);  //50ms
      }
      while (angleA<angle)
     {
             angleA=angleA+5;
                 servoA.write(angleA);
                 delay(20);  //50ms
      }
  }
    
}

void ServoangleA2(int angle)
{
                 servoA.write(angle);
                 delay(150);  //200ms
}
