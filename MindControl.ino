//                                                          //
//    Program       : Mindwave with Arduino                 //
//    Interfacing   : HC-05 Bluetooth Module                //
//    Output        : Attention, Meditation and Eye Blink   //
//    Author        : Sonali Gupta                           //
//                  : Extensive use of Pantech Solutions    //  
//                  : Routines for parsing mindwave raw data//

#include <Servo.h>
// define constants
#define   BAUDRATE            57600
#define   EYEBLINK_THRESHOLD  110

// Initialize variables
long payloadDataS[5] = {0};
long payloadDataB[32] = {0};
byte checksum,generatedchecksum;
unsigned int rawData,pLength,poorQuality,attention, meditation = 0;
unsigned int j,n,lastBlinkTime,currentBlinkTime =0;
long temp,avgRaw,tmpAvg;
int blinkCount = 0;
boolean quality = false;

void setup(){              // main setup routing to initialze values 
 Serial.begin(BAUDRATE);  
 setupMechanics();   // call setup from MechanicalControl,initialize 
}

void loop() {         // Main Function
 // Read and parse data from Serial port through bluetooth
 if(ReadOneByte() == 170) { // AA 1 st Sync data
   if(ReadOneByte() == 170){ // AA 2 st Sync data
     pLength = ReadOneByte();
     if(pLength == 4) {    // Small Packet
       smallPacket ();  // possible blink detected
     } else if(pLength == 32) {
       bigPacket ();     // possible attention/meditation
     }
   }
 }
}

void doubleBlinkDetected(){ // Detected Double blink 
     Serial.println("Double Blink Detected");
     showRunclaw();
}

void getAttention(){ // Acting upon attention
  Serial.print("Attention value is: ");
  Serial.println(attention, DEC);
  if ((attention >= 0) && (attention <= 100 ))showAttention(attention);
}

void getMeditation(){ // Acting upon meditation value
  Serial.print("Meditation value is: ");
  Serial.println(meditation, DEC);
  if ((meditation >= 0) && (meditation <= 100 ))showMeditation(meditation);
}

/*=======================================================================
* These routines are parsing raw data from bluetooth serial port
 * and parsing data accorindg to Neuroskys technical manual for 
 * data parsing. Examples from Pantech Solutions used and modified
 *=====================================================================*/
 
void smallPacket () { // possible blink detected
 generatedchecksum = 0;
 for(int i = 0; i < pLength; i++)   { 
   payloadDataS[i]     = ReadOneByte();      //Read payload into memory
   generatedchecksum  += payloadDataS[i] ;
 }
 generatedchecksum = 255 - generatedchecksum;
 checksum  = ReadOneByte();
 if(checksum == generatedchecksum)  {          // Verify Checksum
   if (j<512)     { // in one second 512 bytes of raw deta received
     rawData  = ((payloadDataS[2] <<8)| payloadDataS[3]);
     if(rawData&0xF000)       {
       rawData = (((~rawData)&0xFFF)+1);
     } else       {
       rawData = (rawData&0xFFF);
     }
     temp += rawData;
     j++;
   } else {
     processOneSecRawVal(); // 1 second is over process raw data
   }
 }
}

void bigPacket(){ // possible quality, meditation and attention
 generatedchecksum = 0;
 for(int i = 0; i < pLength; i++)   { 
   payloadDataB[i]     = ReadOneByte();      //Read payload into memory
   generatedchecksum  += payloadDataB[i] ;
 }
 generatedchecksum = 255 - generatedchecksum;
 checksum  = ReadOneByte();
 if(checksum == generatedchecksum)   {          // Varify Checksum
      poorQuality = 200;
      attention,meditation = 0;
      for(int i = 0; i < pLength; i++) { // Parse the payload
        switch (payloadDataB[i]) {
          case 02:
            i++;
            poorQuality = payloadDataB[i];                
           if (poorQuality==0 ){   // if value not zero data is not reliable
              quality = true;
           } else {
              quality = false;
           }             
            break;
          case 04:
            i++;
            attention = payloadDataB[i];
            if (quality) getAttention();
            break;
          case 05:
            i++;
            meditation = payloadDataB[i]; 
            if (quality) getMeditation();                 
            break;
        }                                 // End of switch
      }                      
  Serial.print((String)"Quality = " + poorQuality + ", Blink Enabled  " + quality);
 }
}

void processOneSecRawVal (){
 avgRaw = temp/512;
 Serial.print(", Avg Raw ");
 Serial.println(avgRaw,DEC);
 eyeBlink();
 temp,j=0;
}

void eyeBlink ()
{
 if (quality) {     
   if ((avgRaw > EYEBLINK_THRESHOLD)){
     Serial.println("Blink Detected");
     currentBlinkTime = millis();
     if ((currentBlinkTime - lastBlinkTime) < 2000){        
      doubleBlinkDetected();
     }  
     lastBlinkTime = currentBlinkTime;     
   }
 } 
}

byte ReadOneByte() {             // One Byte Read Function 
 int ByteRead;
 while(!Serial.available());
 ByteRead = Serial.read();
 return ByteRead;
}
