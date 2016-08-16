/*
 twitterClient: XML parser for twitter feeds
 Author: David Montero
 Client: Pusher Sydney
 Created: 12 April 2012
 */

#include <SPI.h>
#include <Ethernet.h>
#include <TextFinder.h>

// Define MAC address
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
// Hard-coded IP addres in case DHCP fails, depends on local network
IPAddress ip(192,168,0,200);

// initialize the network instance:
EthernetClient client;

// initialize parser
TextFinder finder(client);

// twitter URL
const char serverName[] = "pusher.com.au";

// delay between requests, two minutes
const unsigned long requestInterval = 120000;  


boolean requested;                   // whether a request has been made since connecting
unsigned long lastAttemptTime = 0;   // last time connected to the server, in milliseconds

void getTweet(char* tweet){
  int tweetLength;
  if (client.connected()) {
    if (client.available()){
      finder.find("<display_mode><![CDATA[");
      display_mode = finder.getValue();
      finder.find("<interlude_msg><![CDATA[");  // seek to the Results field
      interlude_msg = finder.getValue();
      finder.find("<interlude_time><![CDATA[");  // seek to the Results field
      delayInterlude=finder.getValue();
#ifdef DEBUG
Serial.println();
      Serial.print("Interlude Msg: ");
      Serial.println(interlude_msg);
      Serial.print("Display Mode: ");
      Serial.println(display_mode);
      Serial.print("delayInterlude: ");
      Serial.println(delayInterlude);  
#endif
      tweetLength = finder.getString("<text><![CDATA[","]]></text>",tweet,140);
      if(tweetLength > 0)
      {  
#ifdef DEBUG
        Serial.println(tweet);           
#endif
        tweet[tweetLength]='\0';
      }
      else{
#ifdef DEBUG
        Serial.println("Could not find text field"); 
#endif
      }
      // close the connection to the server:
      client.stop(); 
#ifdef DEBUG
      Serial.println("Connection closed");     
#endif
    }
  }
  else if (millis() - lastAttemptTime > requestInterval) {
    // if not connected, and one minute have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect
#ifdef DEBUG
  Serial.println("connecting to server...");
#endif
  if (client.connect(serverName, 80)) {
#ifdef DEBUG
    Serial.println("making HTTP request...");
#endif
    // make HTTP GET request to twitter:
    client.println("GET /clients/pusher/pushersign/feed.xml HTTP/1.1");
    client.println("HOST: pusher.com.au");
    client.println();
  }
  else{
#ifdef DEBUG
    Serial.print("Couldn't connect to server ");
    Serial.println(serverName);
#endif
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}

void setIP(){
  // attempt a DHCP connection:
#ifdef DEBUG
  Serial.println("Attempting to obtain an IP address using DHCP:");
#endif
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
#ifdef DEBUG
    Serial.println("failed obtaining an IP address using DHCP, trying manually");
#endif
    Ethernet.begin(mac, ip);
  }
#ifdef DEBUG
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
#endif
}





