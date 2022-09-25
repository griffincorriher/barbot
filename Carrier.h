class Carrier{
  public:
  Carrier(int loc){
    _loc = loc;
    }
    

  #define MAXSPEED 1
  #define _weightPin 2
  void begin(){
    // pinMode(_locPin, INPUT);
    // pinMode(_weightPin, INPUT)
  }
  
//  getLoc(){
//    return _loc;
//  }


  void homeCarrier(int homeSwitch, int initialHoming, AccelStepper stepperX){
    while(digitalRead(homeSwitch)){
      Serial.println("Homing...");
 //     stepperX.moveTo(initialHoming);
 //     initialHoming--;
 //     stepperX.run();
 //     delay(5);
    }
//    stepperX.setCurrentPosition(0);
//    stepperX.setMaxSpeed(100.0);
//    stepperX.setAcceleration(100.0);
//    initialHoming=1;   
    while(!digitalRead(homeSwitch)){
//      stepperX.moveTo(initialHoming);  
//      stepperX.run();
//      initialHoming++;
//      delay(5);
    }
//    stepperX.setCurrentPosition(0);
    Serial.println("Homing Completed");
  }
  
  int zeroCarrier(int fsrPin){
    boolean printOnceFlag = false;
    while(analogRead(fsrPin) > 10){
      if(printOnceFlag == false){
        printOnceFlag = true;
        Serial.println("Remove item from carrier");
      }
    }
    int zeroed = analogRead(fsrPin);
    Serial.println("Carrier has been zeroed");
    return zeroed;
  }
  
  boolean isEmpty(int fsrPin, int zeroed){
    // Need to use millis to prevent wifi from cutting out
    if(analogRead(fsrPin) > zeroed*1.05){
          return false;
    }
    return true;
  }

  void moveCarrier(int stepPin, int dirPin){
    digitalWrite(stepPin, HIGH);
    digitalWrite(dirPin, HIGH);
    //dirPin Low = counterclockwise
    //dirPin High = clockwise
    //if else statement based on current location and next liquid location
  }

  void dispenseLiquor(){
    // Dispense liquid
  }

  void dispenseMixer(){
    // Dispense mixer
  }

  void stopCarrier(){
    // stops the carrier and the dispenser
    // flashes red light and prompts user for input to reset machine or replace glass to finish
    // homes and resets itself after 10 seconds
  }

  private:
  int _loc;
};
