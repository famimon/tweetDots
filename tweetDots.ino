/*
 pushersign: Twitter client displaying latest tweet on a flip-dot sign
 Author: David Montero
 Client: Pusher Sydney
 Created: 12 April 2012
 
 RS-485 communications protocol property of Mobitec Pty Ltd, all rights reserved
 */

#define DEBUG 

int EN = 2; // Data Transmit enable pin

// Font type
const byte font = '1';
const byte boldFont = '7';
const int delayInterlude = 15000; // time in screen for the interlude (in ms)


char tweet[140]="";  // string to hold the tweet

void setup() {
  // initialize Transmission pin
  pinMode(EN, OUTPUT);
  // initialize serial:
  Serial.begin(4800);
  randomSeed(analogRead(0));
  // initialize IP
  setIP();  
  // connect to Twitter:
  connectToServer();
}

void loop()
{
  getTweet(tweet); 
  // print the tweet on the display
  if(tweet[0]!='\0'){
    displayTweet(tweet,(int)random(0,3));
  }
  // End of tweet, print interlude
  interlude();
}

void displayTweet(char *t, int i){
  switch (i){
  case 0: 
    multiLine(t);
    break;
  case 1:
    scrollV(t);
    break;
  default:
    scrollH(t);
    break;
  }
}

void interlude(){
  // hard-coded interlude including @ sign and two different fonts
  byte x=0xa;
  byte y=0xd;
  byte cmd[] = {
    0xa2,0xd2,0xc,0xd3,0x1,0xd4,font,'f','o','l','l','o','w',0xD2,x+3,0xD3,y-5,0xD4,0x77,0x30,0x30,0x30,0xD2,x+1,0xD3,y,0xD4,0x77,0x3E,0x21,0x2C,0x32,0x2E,0x31,0x2E,0xD2,x+2,0xD3,y+5,0xD4,0x77,0x21,0x22,0x22,0x22,0x22,0x21,0xd2,x+9,0xd3,y+2,0xd4,boldFont,'m','o','n','t','e','r','a','c','t','i','v','e','\0'};
  sendCmd(cmd); 
  delay(delayInterlude);   
}





















