/*
*  This is code receives and sends SMS using the SHIELD SIM 900.
*  TO send SMS: first we need to receive the destination number, for exaple: #+54291124532$
*  then we need to receive the text:  #+542914317459$%hello&
*  and the sms is automatically send it.
*/
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

const int TEXT_SIZE = 150;
boolean started=false;
char sms_text[TEXT_SIZE];
int NEW_TEXT = 0;
int NEW_NUMBER = 0;
const char START_NUMB = '#';
const char END_NUMB = '$';
const char START_TEXT = '%';
const char END_TEXT = '&';
const int NUMB_SIZE = 20;
char numberToSend[NUMB_SIZE];

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  if (gsm.begin(2400)){
    Serial.println("%status=READY&");
    started=true;  
  }
  else Serial.println("%status=IDLE&");

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
  int jPos = 0;
 
  while(FLAG_END == 0)
  {
    if (Serial.available() > 0) 
    {
       data=Serial.read();
       
       if(data == START_NUMB)//the beggining of the number
        { 
           jPos = 0;
        }
        else 
          if(data == END_NUMB) //the end of the numer 
          {
               FLAG_END = 1;
               NEW_NUMBER = 1; //activate the flag to send a mew SMS
               Serial.flush();
               return numberToSend;
          }
          else
          {
            if(jPos<NUMB_SIZE){
              numberToSend[jPos] = data;
              jPos++;
            }
            else 
            {
               FLAG_END = 1;
               NEW_NUMBER = 1; //activate the flag to send a mew SMS
               Serial.flush();
            }
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
       
       if(data == START_TEXT)//the beggining of the text
        { 
          pos = 0; 
        }
        else 
            if(data == END_TEXT) //the end of the text 
            {
                  FLAG_END = 1;
                  NEW_TEXT = 1;
                  Serial.flush();
            }
            else
            {
              if(pos<TEXT_SIZE){
                sms_text[pos] = data;
                pos++;
              }
            }
      }
  }
}

/**
Sends the sms with the text received from the serial to the number received.
*/
void sendSMS(char *number_str, char *message_str)
{ 
  if (sms.SendSMS(number_str,message_str))
  {
    Serial.println("%SMS OK&");
    NEW_TEXT = 0; //no new sms
    NEW_NUMBER = 0;
  }
}
/**
* Check if there are new SMS. 
* if there is one,  then we read the sms, then we print via serial and we delete it.
*/
void checkSMS(){
   char text[TEXT_SIZE]; 
   char position;
   char numberToReceive[NUMB_SIZE];
   position = sms.IsSMSPresent(SMS_UNREAD);
    if(position)
    {
        sms.GetSMS(position, numberToReceive, text, TEXT_SIZE);
        Serial.print(START_NUMB);
        Serial.print(numberToReceive);
        Serial.println(END_NUMB);
        Serial.print(START_TEXT);
        Serial.print(text);
        Serial.println(END_TEXT);
        sms.DeleteSMS(position);
    } 
}

void loop() 
{
 
 if(started){
    checkSMS(); 
    delay(1000);
    if(NEW_NUMBER == 0 and NEW_TEXT == 0){
      readNumberFromSerial();
    }
    if(NEW_TEXT == 0 and NEW_NUMBER == 1)
    {
      readTextForSMS();
    }
    if(NEW_TEXT == 1 and NEW_NUMBER == 1)
    {
      sendSMS(numberToSend,sms_text);
    }
  }
};

