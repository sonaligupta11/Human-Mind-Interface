// define constants
#define   LEFT               0
#define   CENTER             90
#define   RIGHT              180
#define   GRAB               45 
#define   RELEASE            0 

// Initialize variables
Servo claw;  // create servo object to control  claw
Servo pour;  // create servo object to control the pour
int clawCP = GRAB;  //current position of claw
int pourCP = CENTER; //current position of pour
int clawPin = 6,pourPin = 7, RED_PIN = 3, GREEN_PIN = 4, BLUE_PIN = 5;

void setupMechanics() {
  claw.attach(clawPin);  // attaches the servo on pin 6 to the servo object
  pour.attach(pourPin);  // attaches the servo on pin 7 to the servo object
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  clawCP = sweep(claw,clawCP,GRAB);   // initialize servo position to grab
  pourCP = sweep(pour,pourCP,CENTER); // initialize pour servo to flat
}

void showMeditation(int color){
  int i=0;
  //mapping meditation range 1-100 to 0-768
  color = map(color,1,100,0,768);
  showRGB(color);
}

void showAttention(int color){
  int i=0;  
  color = map(color,1,100,768,0);
  showRGB(color);
  
}

void showRGB (int color){
  int redIntensity, greenIntensity, blueIntensity;
  if (color <= 255)  {
    greenIntensity = map(color,0,255,128,255); // green from off to on
    blueIntensity = 0;            // blue is always off
    redIntensity = 0;             // green is always off
  } else if (color <= 511) {
    greenIntensity = 0;                     // green  is always off
    blueIntensity = (color - 256); // blue off to on
    redIntensity = 0;        // red is always off
  } else {  
    greenIntensity = 0;         // green is always off
    blueIntensity = 0;                   // blue is always off
    redIntensity = (color - 512);  // red off to on
  }
  // Now that the brightness values have been set, command the LED
  // to those values
  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);  
}


void showRunclaw(){  
  clawCP = sweep(claw,clawCP,GRAB);
  pourCP = sweep(pour,pourCP,LEFT);
  delay(1000);
  pourCP = sweep(pour,pourCP,CENTER);
  clawCP = sweep(claw,clawCP,RELEASE);
  delay(1000);
  clawCP = sweep(claw,clawCP,GRAB);
  pourCP = sweep(pour,pourCP,RIGHT);
  delay(1000);
  pourCP = sweep(pour,pourCP,CENTER);
  delay(1000);
}

int sweep (Servo servo, int curPos, int newPos){ 
  // move servo from Current Positon to New Position
  if (curPos <= newPos){    
     for (int i=curPos; i<= newPos; i++){
        servo.write(i);
        delay(15);
     }
  } else {  
     for (int i=curPos; i >= newPos; i--){
        servo.write(i);
        delay(15);
     }
  }
  return newPos;
}

