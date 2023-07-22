#include <AccelStepper.h>
#include <Arduino.h>
#include <ezButton.h>
#include <ArduinoJson.h>
#include <string>
#include <AFMotor.h>

#define motorYRStepPin 5
#define motorYRDirPin 4
#define motorYLStepPin 7 
#define motorYLDirPin 6
#define motorXStepPin 9
#define motorXDirPin 8

AccelStepper motorX(AccelStepper::DRIVER, motorXStepPin, motorXDirPin);
AccelStepper motorYL(AccelStepper::DRIVER, motorYLStepPin, motorYLDirPin);
AccelStepper motorYR(AccelStepper::DRIVER, motorYRStepPin, motorYRDirPin);


//TODO Update pin numbers
ezButton switchL(3);
ezButton switchB(2);
long currentX;
long currentY;

void setupCalibration(){
  switchL.setDebounceTime(50);
  switchB.setDebounceTime(50);
  motorX.setMaxSpeed(800.0);
  motorX.setAcceleration(1000.0);
  motorYL.setMaxSpeed(1000.0);
  motorYL.setAcceleration(1000.0);
  motorYR.setMaxSpeed(1000.0);
  motorYR.setAcceleration(1000.0);
}

void home() {
  Serial.println("Calibrating Home...");

  // Move motorX until switchL is pressed
  if (!switchL.getState() == LOW) { // Check if switchL is not already pressed
    while (!switchL.isPressed()) {
      switchL.loop();
      motorX.setSpeed(800);
      motorX.move(-1);
      motorX.run();
    }
    // Stop motorX when switchL is pressed
    currentX = 0;
    motorX.stop();
  }

  // Move motorYL and motorYR until switchB is pressed
  if (!switchB.getState() == LOW) { // Check if switchB is not already pressed
    while (!switchB.isPressed()) {
      switchB.loop();
      motorYL.setSpeed(800);
      motorYR.setSpeed(800);
      motorYL.move(1);
      motorYR.move(1);
      motorYL.run();
      motorYR.run();
    }
    // Stop motorYL and motorYR when switchB is pressed
    currentY = 0;
    motorYL.stop();
    motorYR.stop();
  }

  Serial.println("Home Calibrated!");
}

void moveToPosition(int targetX, int targetY) {
  targetX *= -1;
  targetY *= -1;
 
  int stepsX = abs(targetX - currentX);
  int dirX = (targetX > currentX) ? 1 : -1; // Set direction based on targetX
  Serial.println("X:");
  Serial.println(stepsX);
  Serial.println(dirX);

  // Calculate the steps and direction for the y-axis
  int stepsY = abs(targetY - currentY);
  int dirY = (targetY > currentY) ? 1 : -1; // Set direction based on targetY
  Serial.println("Y:");
  Serial.println(stepsY);
  Serial.println(dirY);

  // Set target positions for all motors
  motorX.move(stepsX * dirX);
  motorYL.move(stepsY * dirY);
  motorYR.move(stepsY * dirY);

  // Run the motors until they reach their targets
  while (motorX.distanceToGo() != 0) {
  motorX.run();
  }
  // Stop motors after reaching their targets
  motorX.stop();
  while (motorYL.distanceToGo() != 0 || motorYR.distanceToGo() != 0){
    motorYL.run();
    motorYR.run();
  }

  // Stop motors after reaching their targets
  motorYL.stop();
  motorYR.stop();

  // Update current positions after the motors have reached their targets
  currentX = targetX;
  currentY = targetY;
}

void dispense(int amount){
  // Send the command to Arduino2
  Serial.print("DISPENSE:");
  Serial.println(amount);

  // Wait for Arduino2 to respond 
  // Here, we'll wait for a confirmation message from Arduino2
  while (!Serial.available()) {
    // Wait for data from Arduino2
  }

  // Read the response from Arduino2
  String response = Serial.readStringUntil('\n');
}

void pour(String ingredientName, int amount) {
    // Set x and y positions for each ingredient
    int targetX = 0;
    int targetY = 0;

    // Determine the target x and y positions based on the ingredient
    if (ingredientName == "Vodka") { //0,0
        targetX = 0;
        targetY = 0;
    } else if (ingredientName == "Gin") { //1,0
        targetX = 2600;
        targetY = 0;
    } else if (ingredientName == "Brandy") { //2,0
        targetX = 5400;
        targetY = 0;
    } else if (ingredientName == "Rum") { //3,0
        targetX = 8300;
        targetY = 0;
    } else if (ingredientName == "Tequila") { //4,0
        targetX = 11100;
        targetY = 0;
    } else if (ingredientName == "Whiskey") { //5,0
        targetX = 13800;
        targetY = 0;
    } else if (ingredientName == "Amaretto") { //0,1
        targetX = 2200;
        targetY = 17000;
    } else if (ingredientName == "Triple Sec") { //1,1
        targetX = 6800;
        targetY = 17000;
    } else if (ingredientName == "Coke") { //2,1
        targetX = 11400;
        targetY = 17000;
    } else if (ingredientName == "Simple Syrup") { //0,2
        targetX = 0;
        targetY = 18200;
    } else if (ingredientName == "Lime Juice") { //1,2
        targetX = 4700;
        targetY = 18200;
    } else if (ingredientName == "Club soda") { //2,2
        targetX = 9400;
        targetY = 18200;
    } else if (ingredientName == "Cranberry Juice") { //3,2
        targetX = 13800;
        targetY = 18200;
    } else {
        Serial.println("Unknown ingredient");
        return; // Exit the function if the ingredient is unknown
    }

    // Move to the target position
    moveToPosition(targetX, targetY);

    // Dispense the specified amount
    dispense(amount);
}

void testSwitch(){  

  //switchL.loop(); // MUST call the loop() function first
  switchL.loop();
  switchB.loop();

  if(switchL.isPressed()){
    Serial.println("Left switch pressed");
  }  
    if(switchB.isPressed()){
    Serial.println("Bottom switch pressed");
  }   
}

void setup() {
  Serial.begin(9600);
  setupCalibration();
  home(); //re-enable after testing
  moveToPosition(13800,18200);
  moveToPosition(5400,0);
  moveToPosition(6800,17000);
  moveToPosition(0,0);
}

void loop() {

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    StaticJsonDocument<512> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, command);

    if (error) {
      Serial.print("Deserialization error: ");
      Serial.println(error.c_str());
      return;
    }
    if (jsonDocument.containsKey("command")) {
      String command = jsonDocument["command"].as<String>();

      if (command == "home") {
        home();
      } else if (command == "pour") {
        for (JsonPair kv : jsonDocument.as<JsonObject>()) {
          if (kv.key() != "command" && kv.key() != "name") {
            String ingredientName = kv.key().c_str();
            int amount = kv.value().as<int>();
            pour(ingredientName, amount); 
          }
        }
        home();
        Serial.println("Drink complete! Enjoy!");
      } else {
        Serial.println("Unknown command");
      }
    } else {
      Serial.println("Missing 'command' field in JSON");
    } 
  }
}



