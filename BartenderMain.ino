#include <AFMotor.h>
#include <Arduino.h>
#include <ezButton.h>
#include <ArduinoJson.h>
#include <string>

AF_Stepper motorY(200, 1);
AF_Stepper motorX(200, 2);

//TODO Update pin numbers
ezButton switchL(5);
ezButton switchT(2);
ezButton switchB(3);
ezButton switchR(4);
int currentX;
int currentY;

void setupCalibration(){
  switchL.setDebounceTime(50);
  switchR.setDebounceTime(50);
  switchT.setDebounceTime(50);
  switchB.setDebounceTime(50);
  home();
}

void home(){

  Serial.println("Calibrating Home...");

  while(!switchL.getState() == LOW && !switchB.getState() == LOW){

    switchL.loop();
    switchB.loop();

    if(switchL.getState() == HIGH){  //May need to change state depending on wiring
    motorX.setSpeed(600);
    motorX.step(1, FORWARD, SINGLE);
    }

    if(switchL.getState() == LOW){ //May need to change state depending on wiring
    currentX = 0;
    motorX.release();
    }

    if(switchB.getState() == HIGH){  //May need to change state depending on wiring
    motorY.setSpeed(600);
    motorY.step(1, FORWARD, SINGLE);
    }

    if(switchB.getState() == LOW){ //May need to change state depending on wiring
    currentY = 0;
    motorY.release();
    }
  }
  Serial.println("Home Calibrated!");
}

void moveToPosition(int targetX, int targetY){
  //Calculate the steps and direction for the x-axis
  int stepsX = abs(targetX - currentX);
  int dirX = (targetX > currentX) ? FORWARD : BACKWARD; //May need to change orientation

  //Calculate the steps and direction for the y-axis
  int stepsY = abs(targetY - currentY);
  int dirY = (targetY > currentY) ? FORWARD : BACKWARD; //May need to change orientation

  while (stepsX > 0 || stepsY > 0) {
    if (stepsX > 0) {
      motorX.step(1, dirX);
      stepsX--;
      currentX += (dirX == FORWARD) ? 1 : -1;  // Update the current x position
    }
    if (stepsY > 0) {
      motorY.step(1, dirY);
      stepsY--;
      currentY += (dirY == FORWARD) ? 1 : -1;  // Update the current y position
    }
  }
  motorX.release();
  motorY.release();
}

void dispense(int amount){
  //call the other arduino to dispense
}

void pour(String ingredientName, int amount){
    // Set x and y positions for each ingredient
  int targetX = 0;
  int targetY = 0;

  // Determine the target x and y positions based on the ingredient
  if (ingredientName == "Vodka") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Gin") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Brandy") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Rum") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Tequila") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Whiskey") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Amaretto") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Triple Sec") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Coke") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Simple Syrup") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Lime Juice") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Club soda") {
    targetX = 0;
    targetY = 0;
  } else if (ingredientName == "Cranberry Juice") {
    targetX = 0;
    targetY = 0;
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
  switchT.loop();
  switchB.loop();
  switchR.loop();

  if(switchR.isPressed()){
    Serial.println("Right switch pressed");
  }  
    if(switchT.isPressed()){
    Serial.println("Top switch pressed");
  }  
    if(switchB.isPressed()){
    Serial.println("Bottom switch pressed");
  }  
}

void setup() {
  Serial.begin(9600);
  setupCalibration();
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
      } else {
        Serial.println("Unknown command");
      }
    } else {
      Serial.println("Missing 'command' field in JSON");
    } 
  }
}



