/*
*  This is code receives and sends SMS using the SHIELD SIM 900.
*  TO send SMS: first we need to receive the destination number, for exaple: #+54291124532$
*  then we need to receive the text: %hello&
*  and the sms is automatically send it.
*/
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

const int TEXT_SIZE = 160;
boolean started=false;
char sms_text[160];
int NEW_TEXT = 0;
int NEW_NUMBER = 0;
const char START_NUMB = '#';
const char END_NUMB = '$';
const char START_TEXT = '%';
const char END_TEXT = '&';
char numberToSend[20];
char numberToReceive[20];

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
  int pos = 0;
  while(FLAG_END == 0)
  {
    if (Serial.available() > 0) 
    {
       data=Serial.read();
       
       if(data == START_NUMB)//the beggining of the number
        { 
           pos = 0;
        }
        else 
          if(data == END_NUMB) //the end of the numer 
          {
               FLAG_END = 1;
               NEW_NUMBER = 1; //activate the flag to send a mew SMS
          }
          else
          {
            numberToSend[pos] = data;
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
       
       if(data == START_TEXT)//the beggining of the text
        { 
          pos = 0; 
          cleanBuffer();
        }
        else 
            if(data == END_TEXT) //the end of the text 
            {
                  FLAG_END = 1;
                  NEW_TEXT = 1;
            }
            else
            {
              sms_text[pos] = data;
              pos++;
            }
      }
  }
}

void cleanBuffer(){
  int i;
  for(i = 0; i <160;i++ )
    sms_text[i] = ' ';   
}
/**
Sends the sms with the text received from the serial to the number received.
*/
void sendSMS()
{
  Serial.print("numero ");
  Serial.println(numberToSend);
  Serial.print(" a ese fue ");
  
  if (sms.SendSMS(numberToSend,sms_text))
  {
    Serial.println("%SMS OK&");
    NEW_TEXT = 0; //no new sms
    NEW_NUMBER = 0;
  }

}

void loop() 
{
  char position;
  if(started){
     position = sms.IsSMSPresent(SMS_UNREAD);
   // if(gsm.readSMS(sms_text, TEXT_SIZE, numberToReceive, 20))
    if(position)
    {
        sms.GetSMS(position, numberToReceive, sms_text, TEXT_SIZE);
        Serial.print(START_NUMB);
        Serial.print(numberToReceive);
        Serial.println(END_NUMB);
        Serial.print(START_TEXT);
        Serial.print(sms_text);
        Serial.println(END_TEXT);
    }
    delay(1000);
    if(NEW_NUMBER == 0 and NEW_TEXT == 0)
      readNumberFromSerial();
    if(NEW_TEXT == 0 and NEW_NUMBER == 1)
      readTextForSMS();
    if(NEW_TEXT == 1 and NEW_NUMBER == 1)
    {
      sendSMS();
    }
  }
};
