//#include <StaticThreadController.h>
#include <Thread.h>
//#include <ThreadController.h>
#include <TM1637.h>
#include <IRremote.h>

const int but=53;
const int rele_tempr  =52;
const int tempr = A1;   
const int pm  =A0;
const int RECV_PIN = 2;
const int dd  =49;
const int dd_offer = 48;
const int dd_led =47;
const int tempr_led =50;
const int MQ2= A7 ;
const int MQ_led= 46;
const int grow_sensor = 51;
const int grow_rele = 45;
const int grow_led = 44;

TM1637 tm1637(22,24);

IRrecv irrecv(RECV_PIN);
decode_results results;

Thread tempr_Thread = Thread();
Thread fire_Thread = Thread();
Thread alarm_Thread = Thread();
Thread water_Thread = Thread();
Thread data_Thread = Thread();

float real_tempr, need_tempr=22, start_alarm, start_water;
boolean is_tempr=false,is_dd=false,is_MQ=false, off=false, is_cond=false, is_grow=false, is_nasos=false, is_grow_work=false, is_tempr_work=false, alarm=false;

void     setup() {
       tm1637.init();// инициализация библиотеки «TM1637.h»
       tm1637.set(5);//установка яркости указанная константа равна 2, значение по умолчанию
       analogReference(INTERNAL1V1);
       
       pinMode(but,INPUT);
       pinMode(dd_led,OUTPUT);
       irrecv.enableIRIn(); // Start the receiver
       pinMode(dd,INPUT);
       pinMode(grow_sensor,INPUT);
       pinMode(rele_tempr,OUTPUT); 
       pinMode(grow_rele,OUTPUT); 
       digitalWrite(rele_tempr,HIGH);
       digitalWrite(grow_rele,HIGH);
       pinMode(dd_offer,INPUT);
       pinMode(MQ2, INPUT);
       pinMode(MQ_led,OUTPUT);
       pinMode(tempr_led,OUTPUT);
       Serial1.begin(9600);
       Serial.begin(9600);
        
       tempr_Thread.onRun(tempr_v);
       fire_Thread.onRun(fire_v);
       alarm_Thread.onRun(alarm_v);
       water_Thread.onRun(water_v);
       data_Thread.onRun(data_v);
       
}

void     loop() {

  data_Thread.run();
  if(is_tempr)
    {tempr_Thread.run();}
    else{digitalWrite(tempr_led,LOW); 
         tm1637.clearDisplay();
          if (is_cond==false){digitalWrite(rele_tempr,HIGH);}
          }
  if(is_dd)
    {alarm_Thread.run();}
    else{digitalWrite(dd_led,LOW);}
  if(is_MQ)
    {fire_Thread.run();}
    else{digitalWrite(MQ_led,LOW);}
  if(is_grow)
    {water_Thread.run();}
    else {digitalWrite(grow_led,LOW); digitalWrite(grow_rele,HIGH);}

}
  
void tempr_v()
{
digitalWrite(tempr_led,HIGH);
while (digitalRead(but)==HIGH)
    {
     need_tempr = analogRead(pm)/70 + 15; 
     tm1637.display(2,need_tempr/10);
  tm1637.display(3,(int)need_tempr % 10);
           }
  Serial.print("need t:"); 
  Serial.println(need_tempr); 
    
  real_tempr = analogRead(tempr)/ 10.6;
  Serial.print("real t:"); 
  Serial.println(real_tempr); 
  tm1637.display(2,real_tempr/10);
  tm1637.display(3,(int)real_tempr % 10);
  if(real_tempr > need_tempr)
  {
    digitalWrite(rele_tempr,LOW);
    }
    else if(real_tempr <= need_tempr){digitalWrite(rele_tempr,HIGH);}
}
          
void alarm_v()
{
    digitalWrite(dd_led,HIGH);
if (digitalRead(dd)==HIGH)
  { start_alarm=millis();
    alarm=true;
    Serial.println("HIGH");
    //delay(30000);
    }
if(alarm)
  {if(((millis()-start_alarm)>10000)&&(digitalRead(dd_offer)==LOW))
    {
     Serial.print("A");
     is_dd=false;
     alarm=false;
    }
  }
}
   
    /*if (Serial1.available() > 0) 
   {
    byte incomingByte = Serial1.read();
    Serial.print(incomingByte);
    if (incomingByte==51){is_dd=false; digitalWrite(dd_led,LOW); }
    }
    if(digitalRead(dd_offer)==LOW && is_dd==true){  
  Serial.print("A");
  is_dd=false;
    }
    else if(digitalRead(dd_offer)==HIGH)
    {
      is_dd=false;
      } */
   
   
   

void fire_v()
{ digitalWrite(MQ_led,HIGH);
  Serial.print("smoke: ");
  Serial.println(analogRead(MQ2));
  if(analogRead(MQ2)>450 ||(analogRead(tempr)/10.6)>50)
  {Serial.print("F"); is_MQ=false;Serial1.print("f");
    }
  
  }

/*if(is_nasos==true && is_grow==false)
{ Serial.println("is_nasos");
  digitalWrite(grow_rele,LOW);
delay(10000);
digitalWrite(grow_rele,HIGH);
is_nasos=false;
}else{digitalWrite(grow_rele,HIGH);}*/

void water_v()
{
  digitalWrite(grow_led,HIGH);
  Serial.print("dry:");
  if (digitalRead(grow_sensor)==HIGH){is_grow_work=true; start_water=millis();}
  if(is_grow_work==true)
    { 
    digitalWrite(grow_rele,LOW);
    if ((start_water-millis())>10000)
    {
      digitalWrite(grow_rele,HIGH);
      }
    //delay(10000);
    }
  else{digitalWrite(grow_rele,HIGH);} 
}
 

void data_v()
{
  
  if (Serial1.available() > 0) {
        
        //Считываем пришедший байт
        byte incomingByte = Serial1.read();
        Serial.print(incomingByte);
        if (incomingByte==48){is_tempr=false;}//0
        else if (incomingByte==49){is_tempr=true;}//1
        else if (incomingByte==97){is_dd=false;digitalWrite(dd_led,LOW); } //a
        else if (incomingByte==65){is_dd=true;digitalWrite(dd_led,HIGH);} //A
        else if (incomingByte==102){is_MQ=false;}
        else if (incomingByte==70){is_MQ=true;}
        else if (incomingByte==53){is_grow=false;}
        else if (incomingByte==52){is_grow=true;}
        else if (incomingByte==50){is_cond=true;digitalWrite(rele_tempr,LOW);}
        else if (incomingByte==51){is_cond=false;digitalWrite(rele_tempr,HIGH);}
        else if (incomingByte==55){is_nasos=false;}
        else if (incomingByte==54){is_nasos=true;}
  }
  
  if (irrecv.decode(&results)) {
    switch ( results.value ) {
    case 0x237807F:
       if(is_tempr==false){is_tempr=true;Serial1.print(1);}
       else {is_tempr=false;Serial1.print(0);}
       break;
    case 0x23720DF:
      if(is_dd==false){is_dd=true;Serial1.print("A");}
       else {is_dd=false;Serial1.print("a");}
       break;
    case 0x23702FD:
      if (is_tempr==false)
      {if(is_cond==false)
      {
        digitalWrite(rele_tempr,LOW);is_cond=true;Serial1.print(2);
        }
        else if(is_cond==true)
        {
          digitalWrite(rele_tempr,HIGH);is_cond=false;Serial1.print(3);
        }
      }
        break;
    case 0x237A05F:
        if(is_MQ==true){is_MQ=false;Serial1.print("f");}else{is_MQ=true;Serial1.print("F");}break;
    case 0x23740BF:
        if(is_grow==true){is_grow=false;Serial1.print(5);}else {is_grow=true;is_nasos=false;Serial1.print(4);}break;
    case 0x23758A7:
        if(is_grow==false){is_nasos=true;Serial1.write(6);} break;
        
        }
       

    irrecv.resume(); // Receive the next value
  }
  
  }



