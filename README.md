# arduino-gsm900
Code to send and receive SMS with the Arduino shield SIM 900.

You need the Shield GSM 900.

This code receives the number,from serial, you want to send the text, between # and $, like #+549314236478$.
Where +54 is the country code, 931 is my city code and 4236478 is the phone number.
Then, you need to send the text between %&, like $Hello world%, after that the text is send to that number.

When you receive, is the same..first the number is send using serial and then the text.

There is a folder with photos, may help you to connect the arduino and the shield. Be carefull with the jumpers. 

Thank you for reading it, any suggestion send it!

