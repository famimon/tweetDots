/*
 pushersign: Twitter client displaying latest tweet on a flip-dot sign
 Author: David Montero
 Client: Pusher Sydney
 Created: 12 April 2012
 
 RS-485 communications protocol property of Mobitec Pty Ltd, all rights reserved
 */

//#define DEBUG 

int EN = 2; // Data Transmit enable pin

// Font type
const byte font = '1';
const byte boldFont = '7';

int delayInterlude = 30000; // time in screen for the interlude (in ms)
int display_mode=3; //0=flash screen, 1=vertical scroll, 2=horizontal scroll, default:random
int interlude_msg=3; //0=pusher.com.au, 1=msg me, 2=pusher, default:random

char tweet[140]="";  // string to hold the tweet

void setup() {
  // initialize Transmission pin
  pinMode(EN, OUTPUT);
  // initialize serial:
  Serial.begin(4800);
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
    displayTweet(tweet,display_mode);
  }
  // End of tweet, print interlude
  interlude(interlude_msg);
}

void displayTweet(char *t, int i){
  switch (i){
  case 0: 
    multiLine(t);
    break;
  case 1:
    scrollV(t);
    break;
  case 2:
    scrollH(t);
    break;
  default:
    randomSeed(analogRead(0));
    displayTweet(t, (int)random(0,3));
    break;
  }
}

void interlude(int msg){
  int line1X, line2X;
  byte width=0x62;
  switch(msg){
  case 0:
    line1X=(width-arraySize("pusher.com.au")*5.5)/2;
    printLine(line1X,0xc,boldFont,"pusher.com.au");    
    sendData();
    break;
  case 1:
    {
      // hard-coded interlude including @ sign and two different fonts
      byte x=0x21;
      byte y=0xa;
      byte cmd[] = {
        0xa2,0xd2,0x1,0xd3,0xa,0xd4,font,'m','s','g',' ','m','e',0xD2,x+3,0xD3,y-5,0xD4,0x77,0x30,0x30,0x30,0xD2,x+1,0xD3,y,0xD4,0x77,0x3E,0x21,0x2C,0x32,0x2E,0x31,0x2E,0xD2,x+2,0xD3,y+5,0xD4,0x77,0x21,0x22,0x22,0x22,0x22,0x21,0xd2,0x2a,0xd3,0xc,0xd4,boldFont,'p','u','s','h','e','r','s','i','g','n'                    };
      sendCmd(cmd); 
    }
    break;
  case 2:
    line1X=(width-(arraySize("Pusher")*5.5))/2;
    line2X=(width-(arraySize("Digital Agency")*4))/2;
    printLine(line1X,0x0,boldFont,"Pusher");
    printLine(line2X,0xf,font,"Digital Agency");
    sendData();
    break;
  default:
    randomSeed(analogRead(0));
    interlude((int)random(0,3));
    break;
  }
  delay(delayInterlude);   
}




















