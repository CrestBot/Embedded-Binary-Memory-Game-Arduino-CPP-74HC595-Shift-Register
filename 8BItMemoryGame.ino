//Register Pins
const int SER = 10;
const int RCLK = 9;
const int SERCLK = 8;
const int RESET = 3;
const int OE = 2;

//Button related variables
const int BUTTON = 12;
unsigned long buttonPressedTime = 0;
unsigned long buttonLastPressed = 0;
bool longButtonPress = false;
bool buttonIsPressed = false;
int buttonPressedCounter = 0;
int buttonSTATE = 0;

//player related variables
int playerInputBit = 0;
byte playerByte = 0;
int currentPlayerBit = 7;

//Game related variables
byte dataIN = 0;
int currentBit = 0;
int GREEN_LED = 6;
int YELLOW_LED = 5;
int RED_LED = 4;
bool gameRESET = false;

void setup() {
  //sets up serial terminal for console use
  Serial.begin(9600);
  
  //creates a random number generator using A0 as it's not being used
  randomSeed(analogRead(A0));
  
  //Sets necessary pins as outputs or inputs
  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SERCLK, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(OE, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  //Setting button state as LOW when starting up since it's not being used yet
  int buttonSTATE = 0;

  //Setting OE as LOW since it's an active-low input
  digitalWrite(OE, LOW);

  //Random byte generated as a binary number with 8 bits
  dataIN = random(0, 256);
  Serial.println(dataIN, BIN);

  //Resetting any previous or current values to have a clean slate
  digitalWrite(RESET, LOW);
  digitalWrite(RESET, HIGH);

  //Shiifts generated binary number to the outputs of the register
  for (int i = 7; i >= 0; i--) {
    currentBit = bitRead(dataIN, i);
    digitalWrite(SER, currentBit);

    digitalWrite(SERCLK, HIGH);
    digitalWrite(SERCLK, LOW);
    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);

    delay(500);
  }

  //Flashes 3 times to indicate it will disappear so the player can start inputting
  for (int j = 0; j < 3; j++) {
    digitalWrite(OE, HIGH);
    delay(750);
    digitalWrite(OE, LOW);
    delay(300);
  }

  //once all outputs reset, have to shift the zeros as well
  digitalWrite(RESET, LOW);
  digitalWrite(RESET, HIGH);
  digitalWrite(RCLK, HIGH);
  digitalWrite(RCLK, LOW);
}

// GAME START/LOOP

void loop() {

  // keeps track of button state whether it's pressed or not

  buttonSTATE = digitalRead(BUTTON);

  /* if button is pressed, buttonSTATE goes high and this checks if it is currently being pressed, 
  also checks if boolean buttonIsPressed is currently false, meaning it has already completed a 
  full button cycle previously (pressed and let go) or it hasn't been pressed yet*/

  /* once a button is pressed, it immediately tracks at what time it was pressed and stores it. 
    Also sets boolean ButtonIsPressed to TRUE to indicate a button has been pressed and doesn't allow any other 
    presses to interfere with the following logic; this is because this loop is running many times per second. 
    The loop runs too fast that it cant go through any logic when a button is HIGH; this is why states are needed,
    it keeps track of when a button has been pressed and doesn't allow it to go LOW until certain conditions are met.
    This gives us more control of our game and logic*/

  /* longButtonPress is a variable that tells us when the button has been pressed for longer than 
    one second; it's set to false when a button is initially pressed, as one second hasn't passed yet to activate 
    this variable, only activates when a button is kept HIGH for longer than one second*/

  if (buttonSTATE == HIGH && buttonIsPressed == false) {
    buttonPressedTime = millis();
    buttonIsPressed = true;
    longButtonPress = false;
  }

  /* Below is logic for when it sends a 1 or ON status to our LEDS. If the button has been kept HIGH and its state has 
  been set to TRUE, meaning the button has been pressed. remember, at this point we don't know if the button has been released 
  or is currently still being pressed down. technically we do because the person is interacting 
  with the button, but remember this program is running too fast to go through logic based on real-time input, so in the program 
  we act as if the button is still being pressed and won't "release it" until we need it to be released for logic purposes*/

  /*If the button is being pressed in real-time & boolean buttonIsPressed is TRUE as it has gone through the initial logic before,
  */

  /* At this point we are at the beginning of the button pressed meaning roughly zero time has passed since initial press,
    we check how long its being pressed down for, if its above 1000ms which is one second then we continue with the logic, 
    otherwise it skips this logic and moves on to the next. Again, this program runs over and over many times per second, so
    each time it runs through the program, our previous states are kept the same, as well as our variables. At this point, our time
    variable buttonPressTime has been running and increasing; if it eventually passes one second, it continues with the logic. This
    is why having states is important, as it allows us to keep track of each variable and state even when a loop has been run through 
    thousands of times per second; our variables aren't reset through each loop ran*/

  if (buttonSTATE == HIGH && buttonIsPressed == true && gameRESET == false) {
    if (longButtonPress == false && (millis() - buttonPressedTime > 1000)) {

      /*writes or sets a 1 into our serial input*/
      bitSet(playerByte, currentPlayerBit);
      digitalWrite(SER, HIGH);

      // Shows bit input as well as current bit index
      Serial.println(String("Bit value: ") + bitRead(playerByte, currentPlayerBit) + " | Current Bit Index: " + currentPlayerBit);

      //once the 1 has been input, then it shifts our serial clock and register clock to output the 1 in real-time
      digitalWrite(SERCLK, HIGH);
      digitalWrite(SERCLK, LOW);
      flashForInput();
      digitalWrite(RCLK, HIGH);
      digitalWrite(RCLK, LOW);

      /*Sets long press as TRUE since at this point it has been over one second*/
      longButtonPress = true;

      /*we start at the MSB and shift down to LSB; instead of outputting all bits at once, we shift each bit one by one,
      so we have to start at the MSB since a register shifts bits from right to left, meaning we cant add a bit to the left of another 
      after one has been set, it always adds bits before the one we set. That's how a register works; it shifts from Q0 - Q7 where
      Q7 is the MSB and Q0 is LSB*/
      currentPlayerBit--;
    }
  }

  /*Once a button has been detected to be released (LOW), it checks if a button has been previously pressed, so that 
  way multiple button presses don't interfere with the one we are examining (remember that loops run many times, also 
  helps with debouncing as sometimes a button can trigger more than once even if pressed once). Now that the button has
  been released, we allow the computer to know it's released by setting its pressed state to FALSE. If the longPress boolean
  is FALSE, that means it was a quick press and not a long press (pressed over one second), thus keeping track of when a button 
  was last pressed and increasing its counter as a quick press or multiple presses act as inputting zero*/

  if (buttonSTATE == LOW && buttonIsPressed == true && gameRESET == false) {
    buttonIsPressed = false;
    if (longButtonPress == false) {
      buttonPressedCounter++;
      buttonLastPressed = millis();
    }
  }

  /*checks for how many times a button has been pressed, it's important to note that a long button press doesn't trigger 
  the counter to go up, that's on purpose. We only add to the counter for quick button presses since a counter isn't necessary or valuable to input a 1, thus only neccessary for inputting zero. players can choose to do multiple quick 
  button presses or one quick press to input a zero; this is to make inputs easier to do and to memorize fewer rules to play the game.
  Overall, it makes the game simpler, as a long button press = 1, and multiple quick presses or a quick press = 0*/

  /*after a zero is input it sets the button counter back to zero as at this point in time a button has completed a full cycle, 
  a press and release */

  if (buttonPressedCounter > 0 && (millis() - buttonLastPressed > 1000) && gameRESET == false) {

    //if multiple presses input 0 or if quick press enter 0 ^
    if (buttonPressedCounter >= 1) {
      bitClear(playerByte, currentPlayerBit);
      digitalWrite(SER, LOW);
      Serial.println(String("Bit value: ") + bitRead(playerByte, currentPlayerBit) + " | Current Bit Index: " + currentPlayerBit);

      digitalWrite(SERCLK, HIGH);
      digitalWrite(SERCLK, LOW);
      flashForInput();
      digitalWrite(RCLK, HIGH);
      digitalWrite(RCLK, LOW);

      buttonPressedCounter = 0;
      currentPlayerBit--;
    }
  }

  /*once each bit is filled, it will print both the dataIN byte generated and the player byte,
  then decide who's the winner. Afterwards, the game will be in a reset state where if the 
  player wants to play again they have to press the button once which generates a new random 
  number to memorize*/

  if (currentPlayerBit < 0 && gameRESET == false) {
    currentPlayerBit = 7;
    Serial.print("Player Entered: ");
    printByte(playerByte);
    Serial.print("Game Generated: ");
    printByte(dataIN);
    gameWinner(playerByte, dataIN);
    playerByte = 0;

    digitalWrite(SER, HIGH);

    for (int i = 7; i >= 0; i--) {
      digitalWrite(SERCLK, HIGH);
      digitalWrite(SERCLK, LOW);
    }

    digitalWrite(RCLK, HIGH);
    digitalWrite(RCLK, LOW);
    gameRESET = true;
    delay(500);
  }

  /*Once the game is over, it is now set in a reset state where all bits are 1 or HIGH. You will
  not get out of this state until you press the button, meaning you wish to play again*/

  if (gameRESET == true) {

    /*once button is pressed it creates a new number generation*/
    if (buttonSTATE = HIGH && buttonIsPressed == true) {
      dataIN = random(0, 256);

      //Resetting any previous or current values to have a clean slate
      digitalWrite(RESET, LOW);
      digitalWrite(RESET, HIGH);

      //Shiifts generated binary number to the outputs of the register
      for (int i = 7; i >= 0; i--) {
        currentBit = bitRead(dataIN, i);
        digitalWrite(SER, currentBit);

        digitalWrite(SERCLK, HIGH);
        digitalWrite(SERCLK, LOW);
        digitalWrite(RCLK, HIGH);
        digitalWrite(RCLK, LOW);

        delay(500);
      }

      //Flashes 3 times to indicate it will disappear so the player can start inputting
      for (int j = 0; j < 3; j++) {
        digitalWrite(OE, HIGH);
        delay(750);
        digitalWrite(OE, LOW);
        delay(300);
      }

      //All outputs reset to 0 or LOW
      digitalWrite(RESET, LOW);
      digitalWrite(RESET, HIGH);
      digitalWrite(RCLK, HIGH);
      digitalWrite(RCLK, LOW);

      gameRESET = false;
      buttonIsPressed = false;
    }
  }
}

// HELPER METHODS

//prints out each bit of whatever byte is in parameter
void printByte(byte byteToPrint) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(bitRead(byteToPrint, i));
  }
  Serial.println();
}

//decides game winner
void gameWinner(byte playByte, byte dataByte) {
  if (playByte == dataByte) {
    flashWInner();
  } else {
    flashLoser();
  }
  digitalWrite(RESET, LOW);
  digitalWrite(RESET, HIGH);
  digitalWrite(RCLK, HIGH);
  digitalWrite(RCLK, LOW);
}

//flashes yellow LED to indicate a successful entry
void flashForInput() {
  for (int i = 3; i > 0; i--) {
    digitalWrite(YELLOW_LED, HIGH);
    delay(60);
    digitalWrite(YELLOW_LED, LOW);
    delay(50);
  }
}

//flashing green LED for winner
void flashWInner() {
  for (int i = 10; i > 0; i--) {
    digitalWrite(GREEN_LED, HIGH);
    delay(100);
    digitalWrite(GREEN_LED, LOW);
    delay(75);
  }
}

//flash red LED for game loser
void flashLoser() {
  for (int i = 10; i > 0; i--) {
    digitalWrite(RED_LED, HIGH);
    delay(100);
    digitalWrite(RED_LED, LOW);
    delay(75);
  }
}
