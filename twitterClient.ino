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
IPAddress ip(192,168,1,20);

// initialize the network instance:
EthernetClient client;

// initialize parser
TextFinder finder(client);

// twitter URL
const char serverName[] = "api.twitter.com";  

// delay between requests, two minutes
const unsigned long requestInterval = 120000;  


boolean requested;                   // whether a request has been made since connecting
unsigned long lastAttemptTime = 0;   // last time connected to the server, in milliseconds

void getTweet(char* tweet){
  int tweetLength;
  if (client.connected()) {
    if (client.available()) {
      tweetLength = finder.getString("<text>","</text>",tweet,140);
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
    // if not connected, and two minutes have passed since
    // your last connection, then attempt to connect again:
    connectToServer();
  }
}

void connectToServer() {
  // attempt to connect, and wait a millisecond:
#ifdef DEBUG
  Serial.println("connecting to server...");
#endif
  if (client.connect(serverName, 80)) {
#ifdef DEBUG
    Serial.println("making HTTP request...");
#endif
    // make HTTP GET request to twitter:
    client.println("GET /1/statuses/user_timeline.xml?screen_name=monteractive&count=1 HTTP/1.1");
    client.println("HOST: api.twitter.com");
    client.println();
  }
  // note the time of this connect attempt:
  lastAttemptTime = millis();
}

void setIP(){
  // attempt a DHCP connection:
#ifdef DEBUG
  Serial.println("Attempting to get an IP address using DHCP:");
#endif
  if (!Ethernet.begin(mac)) {
    // if DHCP fails, start with a hard-coded address:
#ifdef DEBUG
    Serial.println("failed to get an IP address using DHCP, trying manually");
#endif
    Ethernet.begin(mac, ip);
  }
#ifdef DEBUG
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
#endif
}



