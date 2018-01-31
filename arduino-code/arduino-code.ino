/*
*  This is code receives and sends SMS using the SHIELD SIM 900.
*  TO send SMS: first we need to receive the destination number, for exaple: #+54291124532$
*  then we need to receive the text: #hello$
*  and the sms is automatically send it.
*/
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;

const int TEXT_SIZE = 160;
int numdata;
boolean started=false;
char sms_text[TEXT_SIZE];
char receptionNumber[20];
char destinationNumber[20];
int NEW_TEXT = 0;
int NEW_NUMBER = 0;
const char START_NUMB = '#';
const char END_NUMB = '$';
const char START_TEXT = '%';
const char END_TEXT = '&';

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  if (gsm.begin(2400)){
    Serial.println("#status=READY$");
    started=true;  
  }
  else Serial.println("#status=IDLE$");

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
  for(i = 0; i <TEXT_SIZE;i++ )
    sms_text[i] = ' ';   
}
/**
Sends the sms with the text received from the serial to the number received.
*/
void sendSMS()
{
  if (sms.SendSMS(destinationNumber,sms_text)){
    Serial.println("\n#SMS OK$");
    NEW_TEXT = 0; //no new sms
    NEW_NUMBER = 0;
  }


}

void loop() 
{
  char position;
  if(started){
    position = sms.IsSMSPresent(SMS_UNREAD);
    if(position)
      //if(gsm.readSMS(smsbuffer, 200, receptionNumber, 20))
      {
        sms.GetSMS(position, receptionNumber, sms_text, TEXT_SIZE);
        String number =START_TEXT+receptionNumber+END_TEXT; 
        Serial.println(number);
        String texto = START_TEXT+sms_text+END_TEXT;
        Serial.println(texto);
      }
    delay(1000);
    if(NEW_NUMBER == 0)
      readNumberFromSerial();
    if(NEW_TEXT == 0 && NEW_NUMBER == 1)
      readTextForSMS();
    if(NEW_TEXT == 1 and NEW_NUMBER == 1)
    {
      sendSMS();
    }
  }
};
