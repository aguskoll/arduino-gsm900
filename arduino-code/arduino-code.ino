#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

int numdata;
boolean started=false;
char smsbuffer[200];
char receptionNumber[20];
char destinationNumber[20];
char text[200];
int NEW_TEXT = 0;
int NEW_NUMBER = 0;
char START_CHAR = '#';
char END_CHAR = '$';

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    started=true;  
  }
  else Serial.println("\nstatus=IDLE");

};

/*
* reads the number from the serial input
* it's going to be the destination number to send a SMS
* we receive the numer betwen # $
* so for exaple: #+54291124532$
* format of the number: +54 (country code) XXX (city) NNNNNNN (number, 7 digits)
*/
void readNumberFromSerial()
{
  int FLAG_END = 0;
  char data;
  int pos = 0;
  while(FLAG_END == 0)
  {
    if (Serial.available() > 0) 
    {
       data=Serial.read();
       if(data == START_CHAR)//the beggining of the number
        { 
          pos = 0;
        }
        else 
          if(data == END_CHAR) //the end of the numer 
          {
                  FLAG_END = 1;
                  NEW_NUMBER = 1; //activate the flag to send a mew SMS
          }
          else
          {
            destinationNumber[pos] = data;
            pos++;
          }
      }
      else
      {
        FLAG_END = 1;
      }
  }
}

/**
  Reads the text, from the serial communication, to be send in the SMS.
  The text needs to be between # and $ 
  For example: #Hello my friend$ 
*/
void readTextForSMS()
{
   int FLAG_END = 0;
   char data;
   int pos;
  while(FLAG_END == 0)
  {
    if (Serial.available() > 0) 
    {
       data=Serial.read();
       
       if(data == START_CHAR)//the beggining of the text
        { 
          pos = 0; 
        }
        else 
            if(data == END_CHAR) //the end of the text 
            {
                  FLAG_END = 1;
                  NEW_TEXT = 1;
            }
            else
            {
              text[pos] = data;
              pos++;
            }
      }
      else
      {
        FLAG_END = 1;
      }
  }
}

/**
Sends the sms with the text received from the serial to the number received.
*/
void sendSMS()
{
  if (sms.SendSMS(destinationNumber,text)){
    Serial.println("\nSMS sent OK");
    NEW_TEXT = 0; //no new sms
    NEW_NUMBER = 0;
  }


}

void loop() 
{
  if(started){
    if(gsm.readSMS(smsbuffer, 200, receptionNumber, 20))
    {
      Serial.println(receptionNumber);
      Serial.println(smsbuffer);
    }
    delay(1000);
    if(NEW_NUMBER == 0)
      readNumberFromSerial();
    if(NEW_TEXT == 0)
      readTextForSMS();
    if(NEW_TEXT == 1 and NEW_NUMBER == 1)
    {
      Serial.println(destinationNumber);
      Serial.println(text);
      sendSMS();
    }
  }
};
