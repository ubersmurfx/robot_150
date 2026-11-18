#include <PS2X_lib.h>  // library for support PS2 controller and esp32 https://github.com/MyArduinoLib/Arduino-PS2X-ESP32.git

#define PS2_DAT        19  //MISO  19
#define PS2_CMD        23  //MOSI  23
#define PS2_SEL         5  //SS     5
#define PS2_CLK        18  //SLK   18

#define R_UP           32  // Driver
#define R_GND          33  // Pins
#define L_UP           26  //
#define L_GND          25  //

#define pressures   true  // select mode: analog reading of push-butttons
#define rumble      false  // select mode: motor rumbling

PS2X ps2x;  // create PS2 Controller Class

uint8_t speedR, speedL;  // Speed for motors
uint8_t valueR, valueL;  // Values on buttons
bool navR, navL;  // Value for choosing the direction of movement
byte vibrate = 0;
byte type = 0;


void setup(){

 // 115200
  Serial.begin(115200);

  //added delay to give wireless ps2 module some time to startup, before configuring it

  while (error != 0) {
    delay(1000);// 1 second wait
    //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.print("#try config ");
    Serial.println(tryNum);
    tryNum ++;
  }

  Serial.println(ps2x.Analog(1), HEX);
  
  type = ps2x.readType();
  switch(type) {
    case 0:
      Serial.println(" Unknown Controller type found ");
      break;
    case 1:
      Serial.println(" DualShock Controller found ");
      break;
    case 2:
      Serial.println(" GuitarHero Controller found ");
      break;
   case 3:
      Serial.println(" Wireless Sony DualShock Controller found ");
      break;
   }
}

void loop() {
  if(type == 1){ //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    //read values from sticks
    valueL = ps2x.Analog(PSS_LY);  
    valueR = ps2x.Analog(PSS_RY); 
    // normalization of values for correct PWM signal generation
    if (valueL > 127){ // forvard left motor
      speedL = abs(128 - valueL)*2+1;
      navL = 0;
    }
    else { // backward left motor
      speedL = (127 - valueL)*2+1;
      navL = 1;
    }
    if (valueR > 127){ //forward right motor
      speedR = abs(128 - valueR)*2+1;
      navR = 0;
    }
    else { //backward rigth motor
      speedR = (127 - valueR)*2+1;
      navR = 1;
    }
    //will be TRUE as long as button is pressed
    //to enable control via the controller's pad
    if(ps2x.Button(PSB_PAD_UP)) { //move forward
      speedL = ps2x.Analog(PSAB_PAD_UP);
      speedR = ps2x.Analog(PSAB_PAD_UP); 
      navR = 1;
      navL = 1;
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){ //turn right
      speedL = ps2x.Analog(PSAB_PAD_RIGHT);
      speedR = ps2x.Analog(PSAB_PAD_RIGHT); 
      navR = 0;
      navL = 1;
    }
    if(ps2x.Button(PSB_PAD_LEFT)){ //turn left
      speedL = ps2x.Analog(PSAB_PAD_LEFT);
      speedR = ps2x.Analog(PSAB_PAD_LEFT); 
      navR = 1;
      navL = 0;
    }
    if(ps2x.Button(PSB_PAD_DOWN)){ //move backward
      speedL = ps2x.Analog(PSAB_PAD_DOWN);
      speedR = ps2x.Analog(PSAB_PAD_DOWN); 
      navR = 0;
      navL = 0;
    }
    // set the generated signal to the driver
    if (navL == 1){
      analogWrite(L_UP, speedL);
      analogWrite(L_GND, 0);
    }
    else{
      analogWrite(L_GND, speedL);
      analogWrite(L_UP, 0);
    }
    if (navR == 1){
      analogWrite(R_UP, speedR);
      analogWrite(R_GND, 0);
    }
    else{
      analogWrite(R_GND, speedR);
      analogWrite(R_UP, 0);
    }
  }
  delay(50); // delay for correct work 
}