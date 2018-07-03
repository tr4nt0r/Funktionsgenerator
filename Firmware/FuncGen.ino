#include <gfxfont.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "definitions.h"
#include "AD9833.h"
#include "Rotary.h"




AD9833 sigGen(AD9833_FsyncPin, 24000000); // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD Initialise
Rotary encoder(rotEncPinA, rotEncPinB);// Initialise the encoder on pins 2 and 3 (interrupt pins)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup() {
	Serial.begin(115200);
	initTFT();
	printBootUpMsg();
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
  //lcd.setCursor(0, 0);
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
        //lcd.noBlink();
        if (button == 0) {
          button = 1;
          //lcd.setCursor(0, 0);
          //lcd.blink();
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
          //lcd.setCursor(realPosC, realPosR);
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
        //lcd.setCursor(9 - digitPos, 0);
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
        //lcd.setCursor(5 - digitPos, 1);
      } break;
    // Change the mode (sine/triangle/clock)
    case 5: {
        if (button == 0) {
          button = 1;
          menuState = 0;
          sigGen.mode(currentMode);
        }
        //lcd.setCursor(13 ,1);
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
          unsigned long newFrequency = frequency + pow(10, digitPos);
          unsigned char dispDigit =
            frequency % (unsigned long) pow(10, digitPos + 1) / pow(10, digitPos);
          if (newFrequency <= maxFrequency && dispDigit < 9) {
            frequency += pow(10, digitPos);
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
          unsigned long newPhase = phase + pow(10, digitPos);
          unsigned char dispDigit =
            phase % pow(10, digitPos + 1) / pow(10, digitPos);
          if (newPhase < maxPhase && dispDigit < 9) {
            phase += pow(10, digitPos);
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
          unsigned long newFrequency = frequency + pow(10, digitPos);
          unsigned char dispDigit = frequency % (unsigned long) pow(10, digitPos + 1) / pow(10, digitPos);

          if (newFrequency > 0 && dispDigit > 0) {
            frequency -= pow(10, digitPos);
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
          unsigned long newPhase = phase + pow(10, digitPos);
          unsigned char dispDigit =
            phase % pow(10, digitPos + 1) / pow(10, digitPos);
          if (newPhase > 0 && dispDigit > 0) {
            phase -= pow(10, digitPos);
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
  
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print(F("F="));

  if (frequency >= 1000000) {
	  tft.print(frequency / 1000000.0, 4);
	  tft.print(F(" GHz"));
  } else if (frequency >= 1000) {
	  tft.print(frequency / 1000.0, 3);
	  tft.print(F(" MHz"));
  } else {
	  tft.print(frequency, 0);
	  tft.print(F(" Hz"));
  }
 
}
// Function to display power state (ON/OFF) in the top right corner
void displayPower() {
	tft.setCursor(0, 30);
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(3);
	tft.print(powerState[currentPowerState]);
}
// Function to display the mode in the bottom right corner
void displayMode() {
	tft.setTextColor(ILI9341_WHITE);
	tft.setTextSize(3);
	tft.setCursor(0, 60);
	tft.print(mode[currentMode]);
}
// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
void displayPhase() {
  unsigned int phaseToDisplay = phase;
  //lcd.setCursor(0, 1);
  //lcd.write(0);
  //lcd.print("=");
  for (int i = 3; i >= 0; i--) {
    unsigned int dispDigit = phaseToDisplay / pow(10, i);
    //lcd.print(dispDigit);
    phaseToDisplay -= dispDigit * pow(10, i);
  }
}
// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
void displayFreqRegister() {
	tft.setCursor(0, 90);
	tft.setTextSize(3);
	tft.setTextColor(ILI9341_WHITE);
	tft.print("Preset ");
	tft.print(freqRegister);
	
	//lcd.setCursor(0, 1);
  //lcd.print("FREQ");
  //lcd.print(freqRegister);
}



void initTFT() {

	tft.begin();

	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print(F("Display Power Mode: 0x")); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print(F("MADCTL Mode: 0x")); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print(F("Pixel Format: 0x")); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print(F("Image Format: 0x")); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print(F("Self Diagnostic: 0x")); Serial.println(x, HEX);
	Serial.println(F("Done!"));

	tft.setRotation(3);

	tft.fillScreen(PINK);
}

void printBootUpMsg() {
	tft.fillScreen(PINK);
	tft.drawRect(0, 0, tft.width(), tft.height() / 5, ILI9341_BLACK);
	tft.fillRect(0, 0, tft.width(), tft.height() / 5, BLUE);

	tft.setTextColor(PINK);

	printAlignCenter(F("Signal Generator"), 3, tft.width() / 2, tft.height() / 10);
	tft.drawBitmap(tft.width() / 2 - 48, tft.height() / 2 -19, sappz, 96, 38, PINK, ILI9341_BLACK);
	tft.setTextColor(ILI9341_BLACK);

	printAlignCenter(_SIGGEN_VERSION_, 2, tft.width()/2, 200);
	printAlignLeft(F("Powered by"), 1, tft.width() / 2 - 48 - 5, tft.height() / 2 + 10);
	delay(2000);
	tft.fillScreen(ILI9341_BLACK);
}


void printAlignCenter(const String text, uint8_t s, uint16_t x, uint16_t y) {
	tft.setCursor(x - text.length() * 3 * s, y - 4 * s);
	tft.setTextSize(s);
	tft.print(text);
}

void printAlignLeft(const String text, uint8_t s, uint16_t x, uint16_t y) {
	tft.setCursor(x - text.length() * 6 * s, y - 4 * s);
	tft.setTextSize(s);
	tft.print(text);
}

int pow(int base, int exp) {
	int result = 1;
	for (;;) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}

	return result;
}