//INVOLT ARDUINO SKETCH
/*
  AUTO PINMODE
  Involt by default automatically adds pinMode to received
  pin data so you don't need to add pinMode output in setup for
  basic interactions. This mode is not recommended when
  mixing digital inputs and outputs.
*/
boolean autoPinMode = true;

/*
  DIRECT MODE
  Direct mode sends data from app to digital pins without storing
  it in chromeDigital array and without additional code in void
  loop. It can be used for testing interaction with Arduino
  inside App. In direct mode digitalWrite/analogWrite is
  automatic only for values received from Involt.
*/
boolean directMode = false;

/*
  Array for digital pins.
  IMPORTANT: You need to declare the total number of variables.
  Default is Arduino UNO total pin number +1.
*/
int chromeDigital[14] = {};
String fname;
String V = "V";
////////////////////////////INVOLD ENDS HERE///////////////////////////////////

////////////////////////////////////////////////////////////////////////////
byte outPins[8] = {38, 40, 42, 44, 46, 48, 50, 52};
byte inPins[8] = {39, 41, 43, 45, 47, 49, 51, 53};
//int ledPins[2] = {2, 3};


char files[9] = "hgfedcba";
char ranks[9] = "12345678";

String squares[64];

byte initialStatus[64];
byte finalStatus[64];

//flags for gameplay logic
boolean moveInProgress = false;
boolean moveEnded = false;
boolean part1 = false;
boolean printMove = false;

boolean whitesMove = true;

//string for stoding the move data
String initialSquare;
//String originSq2;
//String trueOriginSq;
String finalSquare;
String moveStr = "x0x0";
String prevMoveStr = "x0x0";

// variables for buetooth communication

int incomingByte;
int stringLength;
char data[10];
boolean connectFlag;
char modeIs[12];
char computerIs[12];
String GameType;

String moveBackup;


////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 8; i++) {
    pinMode(outPins[i], OUTPUT);
    pinMode(inPins[i], INPUT_PULLUP);
  }
  
  for (int j = 0; j < 64; j++) {
    initialStatus[j] = 1;
    finalStatus[j] = 1;
  }

  for (int k = 0; k < 8; k++) {
    for (int l = 0; l < 8; l++) {
      squares[k * 8 + l] = String(files[k]) + String(ranks[l]);
    }
  }
  Serial.begin(115200);
  //delay(100);
  moveBackup="";
  chromeReceive();
  chromeSend(0, "ready");
}
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  MAIN CODE
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void loop() {
  
  // put your main code here, to run repeatedly:
  moveBackup="";
  boardScan1();
  printInitialBlock();
  delay(500);
  boardScan2();
  byte detectedSquare = arrayCompare();

  while (detectedSquare == 64) {
    delay(1);
    boardScan2();
    detectedSquare = arrayCompare();
  }
  
  //printFinalBlock();
  decodeMove(detectedSquare);  
  fname = "";
}
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void boardScan1() {
  //Serial.println("Board Scan 1");
  for (int i = 0; i < 8; i++) { // for each column...
    digitalWrite(outPins[i], HIGH); // write a high signal
    for (int j = 0; j < 8; j++) { // for each row...
      initialStatus[(i * 8) + j] = digitalRead(inPins[j]);   // read signal and set square status array value = signal
    }
    digitalWrite(outPins[i], LOW);     // maintain a low signal
  }
}

void boardScan2() {
  //Serial.println("Board Scan 2");
  for (int i = 0; i < 8; i++) {
    digitalWrite(outPins[i], HIGH);
    for (int j = 0; j < 8; j++) {
      finalStatus[(i * 8) + j] = digitalRead(inPins[j]);
    }
    digitalWrite(outPins[i], LOW);
  }
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

int arrayCompare() {
  //Serial.println("Array Compare");
  boolean difference = false;
  byte blockNo = 64;

  for (int i = 0; i < 64; i++) {
    if (initialStatus[i] != finalStatus[i]) {
      difference = true;
      blockNo = i;
      //Serial.print("Change detected @ Block no: ");
      //Serial.println(blockNo);
    }
  }

  if (difference) {
    if (finalStatus[blockNo] == 1) {
      moveInProgress = true;
      //Serial.print(" moveInProgress is: ");
      //Serial.println(moveInProgress);
    } else {
      moveInProgress = false;
      moveEnded = true;

      //Serial.print(" moveInProgress is: ");
      //Serial.println(moveInProgress);

      //Serial.print(" moveEnded is: ");
      //Serial.println(moveEnded);
    }
  }

  return blockNo;

}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
void decodeMove(byte block) {
  
  if (moveInProgress && !moveEnded) {   
    //part1 = true;
    initialSquare = squares[block];
    //Serial.println("Initial Square copied as: "  + initialSquare);
    
  } else if (!moveInProgress && moveEnded) {
    
    //part1 = false;
    finalSquare = squares[block];
    printMove = true;
    //Serial.println("Final Square copied as: "  + finalSquare);
    
  } 
  
  if (printMove) {
    
    printMove = false;
    moveEnded = false;
     
    prevMoveStr = moveStr;
    moveStr = initialSquare +'-'+ finalSquare;
    //Serial.println(moveStr);
    if (moveStr.length() == 5 && moveStr != prevMoveStr && initialSquare != finalSquare) {
        
      chromeSend(0, moveStr);
 
      if (whitesMove) {
        //ledBlack();
        whitesMove = false;
      } else {
        //ledWhite();
        whitesMove = true;
      }
    } 
  }
}
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  TEST FEATURES
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void printFinalBlock() {

  Serial.println("Printing Final Block");
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      Serial.print(finalStatus[((63-(j*8))-i)]);
      //[(i * 8) + j] // orignal formula
    }
    Serial.println();
  }
  //delay(1000);
  Serial.println("END");
}

void printInitialBlock() {

  Serial.println("Printing Initial Block");
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      
      Serial.print(initialStatus[((63-(j*8))-i)] );
    }
    Serial.println();
  }
  //delay(1000);
  Serial.println("END");
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

void chromeSend(int pinNumber, String sendValue) {
  String A = "A";
  String E = "E";
  //Serial.print("********");
  Serial.println(A + pinNumber + V + sendValue + E);
  //Serial.println("********");
}

void chromeReceive() {
  String chrome;
  String pwm = "P";
  String dig = "D";
  String fn = "FN";
  int pin;
  int val;

  if (Serial.available() > 0) {
    String chrome = Serial.readStringUntil('\n');
    int chromeLen = chrome.length();

    if (chrome.indexOf(fn) == 0) {
      fname = chrome.substring(2, chromeLen);
    }
    else if (chrome.indexOf(dig) == 0 || chrome.indexOf(pwm) == 0 ){
      pin = chrome.substring(1, chrome.indexOf(V)).toInt();
      String valRaw = chrome.substring(chrome.indexOf(V) + 1, chromeLen);
      val = valRaw.toInt();

      if (autoPinMode) {
        pinMode(pin, OUTPUT);
      };

      if (directMode) {
        if (chrome.indexOf(dig) == 0) {
          digitalWrite(pin, val);
        }
        else if (chrome.indexOf(pwm) == 0 ) {
          analogWrite(pin, val);
        };
      }
      else {
        chromeDigital[pin] = val;
      };

    };
    
  };
};
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

/* // Valid Dummy Moves
  Serial1.println("xya2a4n");
  Serial.println("xya2a4n");
  moveBackup="xya2a4n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xya7a5n");
  Serial.println("xya7a5n");
  moveBackup="xya7a5n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyb2b4n");
  Serial.println("xyb2b4n");
  moveBackup="xyb2b4n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyb7b5n");
  Serial.println("xyb7b5n");
  moveBackup="xyb7b5n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyc2c4n");
  Serial.println("xyc2c4n");
  moveBackup="xyc2c4n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyc7c5n");
  Serial.println("xyc7c5n");
  moveBackup="xyc7c5n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyd2d4n");
  Serial.println("xyd2d4n");
  moveBackup="xyd2d4n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xyd7d5n");
  Serial.println("xyd7d5n");
  moveBackup="xyd7d5n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xye2e4n");
  Serial.println("xye2e4n");
  moveBackup="xye2e4n";
  stringParsingValidation();
  delay(2000);
  
  Serial1.println("xye7e5n");
  Serial.println("xye7e5n");
  moveBackup="xye7e5n";
  stringParsingValidation();
  delay(2000);
  */
