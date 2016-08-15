/*
 flipDot: control of flip-dot sign to display and animate text
 Author: David Montero
 Client: Pusher Sydney
 Created: 12 April 2012
 
 RS-485 communications protocol property of Mobitec Pty Ltd, all rights reserved
 */

//#define sendByte(args) Serial.print("0x");Serial.print(args, HEX);Serial.print(" "); delay(2)
#define sendByte(args) Serial.write(args)
#define readByte(args) Serial.read(args);delay(2)

const byte dotFont = 'w';

const byte address = 0x6; // front sign address
//const byte address = 0xb; // rear sign address

// Length of line in characters
const int lineLength = 23;        

byte data[256];
int dataIdx = 0;
byte currentY = 0x0;
const int stepH = 300; // step time in msecs for horizontal scrolling
const int stepV = 300; // step time in msecs for vertical scrolling
const int delayMulti = 5000; // time in screen for multiline display


const byte width = 0x62; // display width in dots
const byte height = 0x10; // display height in dots

int arraySize(char array[]){
  int count=0;
  while(array[count]){
    count++;
  }
  return count;
}

void copyArray(char *a1, char *a2){
  int i=0;
  while(a2[i]){
    a1[i]=a2[i];
    i++;
  }
  a1[i]='\0';
}

void printLine(byte x, byte y, char font, String buffer){ // Adds a line to the command string, choosing font, start and end position
  data[dataIdx++] = 0xD2; // X position mark
  data[dataIdx++] = x;    // X-coordinate
  data[dataIdx++] = 0xD3; // Y Position mark
  data[dataIdx++] = y;    // Y-coordinate
  data[dataIdx++] = 0xD4; // font mark
  data[dataIdx++] = font; // font type
  for (int i=0; buffer[i]!='\0'; i++){
    encodeChar(buffer[i]);
  }
#ifdef DEBUG  
  Serial.println(buffer); 
#endif
}

int skipBlanks(char *buffer, int idx){
  while (buffer[idx]==' '){
    idx++;
  }
  return idx;
}

void encodeChar(char c){
  // Manually generate special symbols
  switch(c){
  case ',':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0xD3;
    data[dataIdx++]=currentY+1;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0x2c;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD3;
    data[dataIdx++]=currentY;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;
  case ';':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0xD3;
    data[dataIdx++]=currentY+1;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0xD3;
    data[dataIdx++]=currentY;
    data[dataIdx++]=0x39;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;
  case '[':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0x3f;
    data[dataIdx++]=0x31;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;
  case ']':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0x31;
    data[dataIdx++]=0x3f;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;  
  case '_':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0x30;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;
  case '$':
    data[dataIdx++]=0xD4;
    data[dataIdx++]=dotFont;
    data[dataIdx++]=0x32;
    data[dataIdx++]=0x3d;
    data[dataIdx++]=0x37;
    data[dataIdx++]=0x29;
    data[dataIdx++]=0x20;
    data[dataIdx++]=0xD4;
    data[dataIdx++]=font;
    break;
      case '@':
    //  [at] text
//        data[dataIdx++]=0xD4;
//        data[dataIdx++]=dotFont;
//        data[dataIdx++]=0x3f;
//        data[dataIdx++]=0x31;
//        data[dataIdx++]=0x20;
//        data[dataIdx++]=0xD4;
//        data[dataIdx++]=font;
//        data[dataIdx++]='a';
//        data[dataIdx++]='t';
//        data[dataIdx++]=0xD4;
//        data[dataIdx++]=dotFont;
//        data[dataIdx++]=0x31;
//        data[dataIdx++]=0x3f;
//        data[dataIdx++]=0x20;
//        data[dataIdx++]=0xD4;
//        data[dataIdx++]=font;
    //  (at) text
        data[dataIdx++]='(';
        data[dataIdx++]='a';
        data[dataIdx++]='t';
        data[dataIdx++]=')';
    //  5x4 symbol
    //    data[dataIdx++]=0xD4;
    //    data[dataIdx++]=dotFont;
    //    data[dataIdx++]=0x2e;
    //    data[dataIdx++]=0x3d;
    //    data[dataIdx++]=0x33;
    //    data[dataIdx++]=0x3e;
    //    data[dataIdx++]=0x20;
    //    data[dataIdx++]=0xD4;
    //    data[dataIdx++]=font;
        break;
  default:
    // normal character
    data[dataIdx++]=c; // buffer
  }
}

int formatLine(char *buffer, int idx, char *line){
  int lastSpace = 0;
  int line_idx;
  int symbol_idx;
  for (line_idx=0; line_idx < lineLength && buffer[idx]!='\0'; line_idx++, idx++){ // read until end of line or end of buffer
    line[line_idx] = buffer[idx];
    if(buffer[idx]==' '){ // keep track of the last space
      lastSpace=idx;
    }
  }
  if (buffer[idx]=='\0') { // End of buffer
    line[line_idx]='\0';
    idx = -1; // return EOF marker
  } 
  else if (lastSpace < idx && lastSpace !=0 && buffer[idx]!=' ' ){ // Last word to next line
    int last_idx = line_idx-(idx-lastSpace);
    line[last_idx]='\0'; // Terminate line
    idx=lastSpace;
  } 
  else {
    line[line_idx]='\0';
  }  
  return idx;
}

int printScreen(char *buffer, int idx){
  // create first line
  char line1[lineLength]="";
  // Skip blanks at the begginning of line
  idx = skipBlanks(buffer, idx);  
  idx = formatLine(buffer, idx, line1);
  currentY = 0x6;
  printLine(0x00, 0x06, font, line1);
  if (idx != -1){ // There's another line
    char line2[lineLength]="";
    // Skip blanks at the begginning of line
    idx=skipBlanks(buffer, idx);  
    idx = formatLine(buffer, idx, line2);
    currentY = 0xf;
    printLine(0x00, 0x0f, font, line2);  
  }
  sendData();
  return idx;
}

void multiLine(char *buffer){
  int idx = 0;
  // display two lines on the screen every 10s
  while (idx!=-1){
    idx = printScreen(buffer, idx);
    delay(delayMulti);
  }
}

void scrollV(char *buffer){
  int idx=0;
  // create first line
  char line1[lineLength]="";
  char line2[lineLength]="";
  // Skip blanks at the begginning of line
  currentY = height+0x7;
  idx = skipBlanks(buffer, idx);  
  idx = formatLine(buffer, idx, line1);
  for (int i=0; i <= height/2; i++){ //scroll first line until the half of the display
    currentY = height+0x7-i;
    printLine(0x00, height+0x7-i, font, line1);
    sendData();
    delay(stepV);
  }
  while (idx != -1){ 
    currentY = height+0x7;
    idx = skipBlanks(buffer, idx);  
    idx = formatLine(buffer, idx, line2);
    for (int i=height/2; i <= height+1; i++){
      currentY = height+0x7-i;
      printLine(0x00, height+0x7-i, font, line1);
      printLine(0x00, 2*height-i, font, line2);  
      sendData();
      delay(stepV);
    }
    copyArray(line1, line2);
  }
  // scroll to the top of the display
  for (int i=height/2; i <= height+1; i++){
    currentY = height+0x7-i;
    printLine(0x00, height+0x7-i, font, line1);
    sendData();
    delay(stepV);
  }
}

void scrollH(char *buffer){
  String line=buffer;
  for (int i=0,idx=0; i < width; i+=3, idx++){
    printLine(width-i, 0xa, font, line.substring(0, idx));
    sendData();
    delay(stepH);
  }
  for (int i=0; lineLength+i < line.length()+lineLength+5; i++){
    printLine(0x0, 0xa, font, line.substring(0x0+i, lineLength+i+5));
    sendData();
    delay(stepH);  
  }
}

void sendCmd(byte *cmd){
  digitalWrite(EN, HIGH);//Enable data transmit
  delay(1);
  byte sum = address;
  // Begin sending data
  sendByte(0xff);  // Start byte
  sendByte(address); // Sign Address
  int i=0;
  while (cmd[i]){
    sendByte(cmd[i]);
    sum += cmd[i];    // Calculate checksum
    i++;
  }
  if (sum < 255){
    sendByte(sum);
  }
  else{
    sendByte(0xfe);
    sendByte(0x01);
  }
  sendByte(0xff); // Stop byte
  delay(2);
}

void sendData(){
  digitalWrite(EN, HIGH);//Enable data transmit
  delay(1);
  byte sum = address+0xa2;
  // Begin sending data
  sendByte(0xff);  // Start byte
  sendByte(address); // Sign Address
  sendByte(0xa2); // text marker
  for (int i=0; i < dataIdx; i++){
    sendByte(data[i]); // Send Data
    sum += data[i];    // Calculate checksum
  }
  if (sum < 255){
    sendByte(sum);
  }
  else{
    sendByte(0xfe);
    sendByte(0x01);
  }
  sendByte(0xff); // Stop byte
  // restart data buffer
  data[0]='\0';
  dataIdx=0;
}

void receiveData(){
  digitalWrite(EN, LOW);//Disable data transmit
  if (Serial.available()){
    byte firstByte = readByte();
    byte nextByte = readByte();
    if (firstByte == 255 && nextByte < 255){ // start byte, read second byte
      Serial.print("0x");
      Serial.print(firstByte, HEX); // Print start byte
      Serial.print(",");
      while (nextByte < 255){
        Serial.print("0x");
        Serial.print(nextByte, HEX); // print all data bytes
        Serial.print(",");
        nextByte = readByte();
      }
      if (nextByte == 255){ // print stop byte
        Serial.print("0x");
        Serial.println(nextByte, HEX);
      }
    }
  }
}




















