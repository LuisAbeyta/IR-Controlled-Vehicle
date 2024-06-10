/*
Luis Abeyta and Brian Turner
CSCI 4287
Description: This code is made with the intention of controling two dc motors for a vehicle. The vehicle is controlled by the user through and IR receiver and remote.
It also has a mode to control the direction through the use of photocells and a light source. Finally it contains an interrupt to avoid colliding with anything.

*/
#include <IRremote.h>

#include <HCSR04.h>

#include<avr/io.h>
#include<avr/interrupt.h>

//motors pins
int enablepin = 5;
int dirpin1 = 3;
int dirpin2 = 4;

int enablepin2 = 10;
int dir3 = 9;
int dir4 = 8;

//photocell pins
int Mphotocell = A1;
int Lphotocell = A2;
int Rphotocell = A0;

//IR remote setup
String remoteInput = " ";
int IRreceiver = 6;



//ultrasonic sensor
HCSR04 hc(12, 11);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //motor setup
  pinMode(enablepin, OUTPUT);
  pinMode(dirpin1, OUTPUT);
  pinMode(dirpin2, OUTPUT);

  pinMode(enablepin2, OUTPUT);
  pinMode(dir3, OUTPUT);
  pinMode(dir4, OUTPUT);

  //IR receiver enable input
  IrReceiver.begin(IRreceiver, ENABLE_LED_FEEDBACK); //start receiver

  //ultrasonic sensor interrupt setup timer1
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = (1<< OCIE1A);
  //comparsion value
  OCR1A = 3000;
  TCCR1B |= (1<<WGM12);
  //prescaler 1024
  TCCR1B |= (1<<CS10);
  TCCR1B |= (1<<CS12);

  sei();
  
}

ISR (TIMER1_COMPA_vect){
  float distance = hc.dist();
  Serial.println(distance);
  //check distance
  if(distance <= 15 && distance != 0)
  {
    //turn off interrupts if too close to wall
    TIMSK1 = (0<< OCIE1A);
    //stop vehicle
    analogWrite(enablepin, 0);
    analogWrite(enablepin2, 0);
    //Then back up
    delay(1000);

    analogWrite(enablepin, 255);

    digitalWrite(dirpin1, HIGH);
    digitalWrite(dirpin2, LOW);
    
    analogWrite(enablepin2, 255); 

    digitalWrite(dir3, LOW);
    digitalWrite(dir4, HIGH);

    delay(2000);
    //then stop
    analogWrite(enablepin, 0);
    analogWrite(enablepin2, 0);

    //activate interrupt
    TIMSK1 = (1<< OCIE1A);
  }
}

//motor values only worked from 230 to 255
void loop() {
  // put your main code here, to run repeatedly:

  if(IrReceiver.decode()){
    
    IrReceiver.printIRResultShort(&Serial);
    //press 2
    if (IrReceiver.decodedIRData.command == 0x18)
    {
      //forward
      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, LOW);
      digitalWrite(dirpin2, HIGH);

      analogWrite(enablepin2, 255);

      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, LOW);
    }
    else if(IrReceiver.decodedIRData.command == 0x1c)
    {
      //press 5 stop    
      analogWrite(enablepin, 0);
      analogWrite(enablepin2, 0); 
    }
     else if(IrReceiver.decodedIRData.command == 0x52)
    {
      //reverse
      //press 8
      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, HIGH);
      digitalWrite(dirpin2, LOW);
      
      analogWrite(enablepin2, 255); 

      digitalWrite(dir3, LOW);
      digitalWrite(dir4, HIGH);
    }
     else if(IrReceiver.decodedIRData.command == 0x5A)
    {
      
      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, HIGH);
      digitalWrite(dirpin2, LOW);

      analogWrite(enablepin2, 255); 

      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, LOW);
    }
     else if(IrReceiver.decodedIRData.command == 0x8)
    {
      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, LOW);
      digitalWrite(dirpin2, HIGH);

      analogWrite(enablepin2, 255); 

      digitalWrite(dir3, LOW);
      digitalWrite(dir4, HIGH);
    }
    else if(IrReceiver.decodedIRData.command == 0xC)
    {
      lightNav();
    }

    IrReceiver.resume();
  }

  

}




int lightNav()
{
  analogWrite(enablepin, 0);
  analogWrite(enablepin2, 0);

  IrReceiver.resume();

  bool cont = 1;

  
  while(cont){
    int reading = analogRead(Mphotocell);
    int reading2 = analogRead(Lphotocell);
    int reading3 = analogRead(Rphotocell);
    Serial.print("Middle:");
    Serial.println(reading);
    Serial.print("Left:");
    Serial.println(reading2);
    Serial.print("Right:");
    Serial.println(reading3);

    if(reading >= 50 && (reading > reading2 && reading > reading3))
    {
      //middle photocell getting most light
      //move foward
      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, LOW);
      digitalWrite(dirpin2, HIGH);

      analogWrite(enablepin2, 255);

      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, LOW);

    }
    else if(reading2 >= 50 && (reading2 > reading && reading2 > reading3))
    {
      //left photocell getiing the most light
      //move left so middle gains most light and moves foward

      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, LOW);
      digitalWrite(dirpin2, HIGH);

      analogWrite(enablepin2, 255); 

      digitalWrite(dir3, LOW);
      digitalWrite(dir4, HIGH);
    }
    else if(reading3 >= 50 && (reading3 > reading && reading3 > reading2))
    {
      //right photocell getiing the most light
      //move right so middle gains most light and moves foward

      analogWrite(enablepin, 255);

      digitalWrite(dirpin1, HIGH);
      digitalWrite(dirpin2, LOW);

      analogWrite(enablepin2, 255); 

      digitalWrite(dir3, HIGH);
      digitalWrite(dir4, LOW);
    }
    else
    {
      analogWrite(enablepin, 0);
      analogWrite(enablepin2, 0);
    }

    if(IrReceiver.decode()){
        if (IrReceiver.decodedIRData.command == 0x42)
        {
          cont = 0;
        }
        else
        {
          IrReceiver.resume();
        }
    }
  

  }

  return 0;
  
}





