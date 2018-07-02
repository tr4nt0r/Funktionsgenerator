
#include "LiquidCrystal_I2C.h"
#include "AD9833.h"
#include "Rotary.h"
#include "math.h"

//Uncomment the line below if you want to change the Phase instead of the FREQ register
//#define usePhase

AD9833 sigGen(10, 24000000); // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD Initialise
Rotary encoder(2, 3);// Initialise the encoder on pins 2 and 3 (interrupt pins)

// Variables used to input data and walk through menu
unsigned long encValue;        // Value used by encoder
unsigned long lastButtonPress; // Value used by button debounce
unsigned char lastButtonState;
unsigned char settingsPos[] = {0, 14, 20, 29};
unsigned char button;
volatile unsigned char cursorPos = 0;
unsigned char lastCursorPos = 0;
unsigned char menuState = 0;
const int buttonPin = 1;
int digitPos = 0;
const unsigned long maxFrequency = 14000000;
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register
unsigned long newFrequency = 1000;
volatile bool updateDisplay = false;
unsigned long depressionTime;
int freqRegister = 0; // Default FREQ register is 0
// LCD constants
const String powerState[] = {" ON", "OFF"};
const String mode[] = {"SIN", "TRI", "CLK"};
// Variables used to store phase, frequency, mode and power
unsigned char currentMode = 0;
unsigned long frequency0 = 1000;
unsigned long frequency1 = 1000;
unsigned long frequency = frequency0;
unsigned long currFrequency; // Current frequency used, either 0 or 1
unsigned long phase = 0; // Only used if you enable PHASE setting instead of FREQ register
unsigned char currentPowerState = 0;
// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
uint8_t phi[8] = {0b01110, 0b00100, 0b01110, 0b10101,
                  0b10101, 0b01110, 0b00100, 0b01110
                 };

void setup() {
  // Initialise the LCD, start the backlight and print a "bootup" message for two seconds
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, phi); // Custom PHI char for LCD
  lcd.home();
  lcd.print("AllAboutCircuits");
  lcd.setCursor(0, 1);
  lcd.print("Signal Generator");
  delay(2000);
  // Display initial set values for freq, phase, mode and power
  lcd.clear();
  displayFrequency();
  displayMode();
#ifdef usePhase
  displayPhase();
#endif
  displayPower();
#ifndef usePhase
  displayFreqRegister();
#endif
  // Initialise the AD9833 with 1KHz sine output, no phase shift for both
  // registers and remain on the FREQ0 register
  // sigGen.lcdDebugInit(&lcd);
  sigGen.reset(1);
  sigGen.setFreq(frequency0);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(1);
  sigGen.setFreq(frequency1);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(0);
  sigGen.mode(currentMode);
  sigGen.reset(0);

  // Set pins A and B from encoder as interrupts
  attachInterrupt(digitalPinToInterrupt(2), encChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), encChange, CHANGE);
  // Initialise pin as input with pull-up enabled and debounce variable for
  // encoder button
  pinMode(1, INPUT_PULLUP);
  lastButtonPress = millis();
  lastButtonState = 1;
  button = 1;
  // Set Cursor to initial possition
  lcd.setCursor(0, 0);
}

void loop() {
  // Check to see if the button has been pressed
  checkButton();
  // Update display if needed
  if (updateDisplay == true) {
    displayFrequency();
#ifdef usePhase
    displayPhase();
#endif
    displayPower();
#ifndef usePhase
    displayFreqRegister();
#endif
    displayMode();
    updateDisplay = false;
  }
  // We are using the variable menuState to know where we are in the menu and
  // what to do in case we press the button or increment/drecrement via the
  // encoder
  // Enter setting mode if the button has been pressed and display blinking
  // cursor over options (menuState 0)
  // Pick a setting (menuState 1)
  // Change that particular setting and save settings (menuState 2-5)

  switch (menuState) {
    // Default state
    case 0: {
        lcd.noBlink();
        if (button == 0) {
          button = 1;
          lcd.setCursor(0, 0);
          lcd.blink();
          menuState = 1;
          cursorPos = 0;
        }
      } break;
    // Settings mode
    case 1: {
        if (button == 0) {
          button = 1;
          // If the setting in Power just toggle between on and off
          if (cursorPos == 1) {
            currentPowerState = abs(1 - currentPowerState);
            updateDisplay = true;
            menuState = 0;
            if (currentPowerState == 1)
              sigGen.sleep(3); // Both DAC and clock turned OFF
            else
              sigGen.sleep(0); // DAC and clock are turned ON
          }
          // If usePhase has not been set
#ifndef usePhase
          else if (cursorPos == 2) {
            updateDisplay = true;
            menuState = 0; // return to "main menu"
            if (freqRegister == 0) {
              freqRegister = 1;
              sigGen.setFPRegister(1);
              frequency = frequency1;
            } else {
              freqRegister = 0;
              sigGen.setFPRegister(0);
              frequency = frequency0;
            }
          }
#endif
          // Otherwise just set a new state
          else
            menuState = cursorPos + 2;
        }
        // Move the cursor position in case it changed
        if (lastCursorPos != cursorPos) {
          unsigned char realPosR = 0;
          unsigned char realPosC;
          if (settingsPos[cursorPos] < 16)
            realPosC = settingsPos[cursorPos];
          else {
            realPosC = settingsPos[cursorPos] - 16;
            realPosR = 1;
          }
          lcd.setCursor(realPosC, realPosR);
          lastCursorPos = cursorPos;
        }
      } break;
    // Frequency setting
    case 2: {
        // Each button press will either enable to change the value of another digit
        // or if all digits have been changed, to apply the setting
        if (button == 0) {
          button = 1;
          if (digitPos < 7)
            digitPos++;
          else {
            digitPos = 0;
            menuState = 0;
            sigGen.setFreq(frequency);
          }
        } else if (button == 2) {
          button = 1;
          digitPos = 0;
          menuState = 0;
        }
        // Set the blinking cursor on the digit you can currently modify
        lcd.setCursor(9 - digitPos, 0);
      } break;

    // Phase setting
    case 4: {
        if (button == 0) {
          button = 1;
          if (digitPos < 3)
            digitPos++;
          else {
            digitPos = 0;
            menuState = 0;
            sigGen.setPhase(phase);
          }
        }
        lcd.setCursor(5 - digitPos, 1);
      } break;
    // Change the mode (sine/triangle/clock)
    case 5: {
        if (button == 0) {
          button = 1;
          menuState = 0;
          sigGen.mode(currentMode);
        }
        lcd.setCursor(13 ,1);
      } break;
    // Just in case we messed something up
    default: {
        menuState = 0;
      }
  }
}
// Function to debounce the button
// 0 = pressed, 1 = depressed, 2 = long press
void checkButton() {
  if ((millis() - lastButtonPress) > 100) {
    if (digitalRead(buttonPin) != lastButtonState) {
      button = digitalRead(buttonPin);
      lastButtonState = button;
      lastButtonPress = millis();
    }
  }
}

void encChange() {
  // Depending in which menu state you are
  // the encoder will either change the value of a setting:
  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), On/Off, mode
  // or it will change the cursor position
  unsigned char state = encoder.process();
  // Direction clockwise
  if (state == DIR_CW) {
    switch (menuState) {
      case 1: {
          if (cursorPos == 3)
            cursorPos = 0;
          else
            cursorPos++;
        } break;

      case 2: {
          // Here we initialise two variables.
          // newFrequency is the value of the frequency after we increment it
          // dispDigit is the digit that we are currently modifing, and we obtain it
          // by a neat little trick, using operator % in conjunction with division
          // We then compare these variables with the maximum value for our
          // frequency, if all is good, make the change
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit =
            frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency <= maxFrequency && dispDigit < 9) {
            frequency += power(10, digitPos);
            updateDisplay = true;
          }

          if (freqRegister == 0) {
            frequency0 = frequency;
          } else if (freqRegister == 1) {
            frequency1 = frequency;
          }

        } break;

      case 4: {
          // if usePhase has been defined, changes in the encoder will vary the phase
          // value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of
          // pi where 2pi = 4096
#ifdef usePhase
          unsigned long newPhase = phase + power(10, digitPos);
          unsigned char dispDigit =
            phase % power(10, digitPos + 1) / power(10, digitPos);
          if (newPhase < maxPhase && dispDigit < 9) {
            phase += power(10, digitPos);
            updateDisplay = true;
          }
#endif
        } break;

      case 5: {
          if (currentMode == 2)
            currentMode = 0;
          else
            currentMode++;
          updateDisplay = true;
        } break;
    }
  }
  // Direction counter clockwise
  else if (state == DIR_CCW) {
    switch (menuState) {
      case 1: {
          if (cursorPos == 0)
            cursorPos = 3;
          else
            cursorPos--;
        } break;

      case 2: {
          unsigned long newFrequency = frequency + power(10, digitPos);
          unsigned char dispDigit =
            frequency % power(10, digitPos + 1) / power(10, digitPos);
          if (newFrequency > 0 && dispDigit > 0) {
            frequency -= power(10, digitPos);
            updateDisplay = true;
          }

          if (freqRegister == 0) {
            frequency0 = frequency;
          } else if (freqRegister == 1) {
            frequency1 = frequency;
          }
        } break;

      case 4: {
          // if usePhase has been defined, changes in the encoder will vary the phase
          // value (upto 4096)
          // A better implementation would be to use increment of pi/4 or submultiples of
          // pi where 2pi = 4096
#ifdef usePhase
          unsigned long newPhase = phase + power(10, digitPos);
          unsigned char dispDigit =
            phase % power(10, digitPos + 1) / power(10, digitPos);
          if (newPhase > 0 && dispDigit > 0) {
            phase -= power(10, digitPos);
            updateDisplay = true;
          }
#endif
        } break;

      case 5: {
          if (currentMode == 0)
            currentMode = 2;
          else
            currentMode--;
          updateDisplay = true;
        } break;
    }
  }
}
// Function to display the current frequency in the top left corner
void displayFrequency() {
  unsigned long frequencyToDisplay = frequency;
  lcd.setCursor(0, 0);
  lcd.print("f=");
  for (int i = 7; i >= 0; i--) {
    unsigned char dispDigit = frequencyToDisplay / power(10, i);
    lcd.print(dispDigit);
    frequencyToDisplay -= dispDigit * power(10, i);
  }
  lcd.print("Hz");
}
// Function to display power state (ON/OFF) in the top right corner
void displayPower() {
  lcd.setCursor(13, 0);
  lcd.print(powerState[currentPowerState]);
}
// Function to display the mode in the bottom right corner
void displayMode() {
  lcd.setCursor(13, 1);
  lcd.print(mode[currentMode]);
}
// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
void displayPhase() {
  unsigned int phaseToDisplay = phase;
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print("=");
  for (int i = 3; i >= 0; i--) {
    unsigned int dispDigit = phaseToDisplay / power(10, i);
    lcd.print(dispDigit);
    phaseToDisplay -= dispDigit * power(10, i);
  }
}
// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
void displayFreqRegister() {
  lcd.setCursor(0, 1);
  lcd.print("FREQ");
  lcd.print(freqRegister);
}

unsigned long power(int a, int b) {
  unsigned long res;
  if (b == 0) {
    res = 1;
  } else {
    res = a;
    for (int i = 0; i < (b - 1); i++) {
      res *= a;
    }
  }
  return res;
}
