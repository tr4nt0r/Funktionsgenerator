

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "definitions.h"
#include "AD9833.h"
#include <Fonts\Org_01.h>
#include <TimerOne.h>
#include <ClickEncoder.h>




AD9833 sigGen(AD9833_FsyncPin, 24000000); // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
ClickEncoder encoder(rotaryEncoderPinA, rotaryEncoderPinB, rotaryEncoderPinBtn, 4);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, -1);

void drawMenuBar();
void drawMainScreenWaveform(bool drawPartial = false);
void onRotaryEncoderTurnEvent() {

	rotaryEncoderPos += encoder.getValue();
	rotaryEncoderDir dir = DIR_NONE;

	int8_t rotaryEncoderMovement = rotaryEncoderPos - rotaryEncoderLastPos;
	if (rotaryEncoderMovement != 0) {
		dir = (rotaryEncoderMovement > 0) ? DIR_CW : DIR_CCW;
		rotaryEncoderLastPos = rotaryEncoderPos;
	}
	else {
		dir = DIR_NONE;
	}

	switch (dir) {
	case DIR_NONE:
		//nothing more todo here
		break;
	case DIR_CW:
		displayDebugMsg("->");
		break;
	case DIR_CCW:
		displayDebugMsg("<-");
		break;
	default:
		break;
	}

	//navigate menu if no Menu item currently selected
	if (dir != DIR_NONE && menuSelected != NullItem && menuSelected != menuActive) {
		if (menuSelected != getNextMenuItem(dir)) {
			menuSelected = getNextMenuItem(dir);
			drawMenuBar();
		}
	}
	//rotary encoder actions if a menu item is active
	else if (dir != DIR_NONE && menuActive != NullItem) {
		switch (menuSelected)
		{
		case NullItem:
			break;
		case FreqSet:
			break;
		case Mode:
			if (dir == DIR_CW) {
				if (selectedWaveform < 2) {
					selectedWaveform++;
					drawMainScreenWaveform(true);
				}
			}
			else if (dir == DIR_CCW) {
				if (selectedWaveform > 0) {
					selectedWaveform--;
					drawMainScreenWaveform(true);
				}
			}			
			break;
		case Power:
			break;
		case PhasePreset:
			break;
		case FreqPreset:
			break;
		case Settings:
			break;
		case lastItem:
			break;
		default:
			break;
		}
	}
}

void callButtonEvent(ClickEncoder::Button button);
void drawMainScreen();
void drawMenuBar();

void setup() {
	initTFT();
	displayBootUpMsg(); //display Bootscreen for 2s
	drawMenuBar();
	drawMainScreen(); //renders Mainscreen Boxes
	updateDisplay = true;
	displayChange();  //write values on MainScreen

  // Initialise the AD9833 with 1KHz sine output, no phase shift for both
  // registers and remain on the FREQ0 register
  sigGen.reset(1);
  sigGen.setFreq(frequency0);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(1);
  sigGen.setFreq(frequency1);
  sigGen.setPhase(phase);
  sigGen.setFPRegister(0);
  sigGen.mode(selectedWaveform);
  sigGen.reset(0);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(ISRcallback);

  encoder.setAccelerationEnabled(true);  

  // Set Cursor to initial possition
  //lcd.setCursor(0, 0);
}

//Interrupt Service Routine 
void ISRcallback() {
	encoder.service();
}

void loop() {
  // Check to see if the button has been pressed
  ClickEncoder::Button button = encoder.getButton();
  callButtonEvent(button);

  
  onRotaryEncoderTurnEvent();

  switch (menuSelected) {
  case NullItem:
  default:
	  break;
  case Settings:
	  break;
  case Power:
	  break;
  case FreqPreset:
	  break;
  case PhasePreset:
	  break;
  }



//  switch (menuState) {
//    // Default state
//    case 0: {
//        
//          //button = 1;
//          //lcd.setCursor(0, 0);
//          //lcd.blink();
//          menuState = 1;
//          cursorPos = 0;
//   
//      } break;
//    // Settings mode
//    case 1: {
//        if (button == 0) {
//          //button = 1;
//          // If the setting in Power just toggle between on and off
//          if (cursorPos == 1) {
//            currentPowerState = abs(1 - currentPowerState);
//            updateDisplay = true;
//            menuState = 0;
//            if (currentPowerState == 1)
//              sigGen.sleep(3); // Both DAC and clock turned OFF
//            else
//              sigGen.sleep(0); // DAC and clock are turned ON
//          }
//          // If usePhase has not been set
//#ifndef usePhase
//          else if (cursorPos == 2) {
//            updateDisplay = true;
//            menuState = 0; // return to "main menu"
//            if (freqRegister == 0) {
//              freqRegister = 1;
//              sigGen.setFPRegister(1);
//              frequency = frequency1;
//            } else {
//              freqRegister = 0;
//              sigGen.setFPRegister(0);
//              frequency = frequency0;
//            }
//          }
//#endif
//          // Otherwise just set a new state
//          else
//            menuState = cursorPos + 2;
//        }
//        // Move the cursor position in case it changed
//        if (lastCursorPos != cursorPos) {
//          unsigned char realPosR = 0;
//          unsigned char realPosC;
//          if (settingsPos[cursorPos] < 16)
//            realPosC = settingsPos[cursorPos];
//          else {
//            realPosC = settingsPos[cursorPos] - 16;
//            realPosR = 1;
//          }
//          //lcd.setCursor(realPosC, realPosR);
//          lastCursorPos = cursorPos;
//        }
//      } break;
//    // Frequency setting
//    case 2: {
//        // Each button press will either enable to change the value of another digit
//        // or if all digits have been changed, to apply the setting
//        if (button == 0) {
//          //button = 1;
//          if (digitPos < 7)
//            digitPos++;
//          else {
//            digitPos = 0;
//            menuState = 0;
//            sigGen.setFreq(frequency);
//          }
//        } else if (button == 2) {
//          //button = 1;
//          digitPos = 0;
//          menuState = 0;
//        }
//        // Set the blinking cursor on the digit you can currently modify
//        //lcd.setCursor(9 - digitPos, 0);
//      } break;
//
//    // Phase setting
//    case 4: {
//        if (button == 0) {
//          //button = 1;
//          if (digitPos < 3)
//            digitPos++;
//          else {
//            digitPos = 0;
//            menuState = 0;
//            sigGen.setPhase(phase);
//          }
//        }
//        //lcd.setCursor(5 - digitPos, 1);
//      } break;
//    // Change the mode (sine/triangle/clock)
//    case 5: {
//        if (button == 0) {
//          //button = 1;
//          menuState = 0;
//          sigGen.mode(selectedWaveform);
//        }
//        //lcd.setCursor(13 ,1);
//      } break;
//    // Just in case we messed something up
//    default: {
//        menuState = 0;
//      }
//  }

//  encChange();
}

Menu getNextMenuItem(rotaryEncoderDir direction) {
	uint8_t next = (int)menuSelected;
	if (direction == DIR_CW && next < lastItem - 1) {
		next++;
	}
	else if (direction == DIR_CCW && next > NullItem + 1) {
		next--;
	}
	return (Menu)next;
}

void callButtonEvent(ClickEncoder::Button button) {
	switch (button) {
	case ClickEncoder::Open:
	default:
		//	  
		break;
	case ClickEncoder::Clicked:
		onClick();
		break;
	case ClickEncoder::DoubleClicked:
		onDblClick();
		break;

	case ClickEncoder::Held:
		onButtonHold();
		break;
	case ClickEncoder::Released:
		onButtonUp();
		break;
	}
}

void onDblClick() {
	switch (menuSelected) {
	case NullItem: //No Menu Item selected
		if (menuActive == NullItem && !buttonIsHeld) {
			menuSelected = FreqSet;
			drawMenuBar();
		}
		break;
	default:
			menuSelected = NullItem;
			menuActive = NullItem;
			drawMenuBar();
			drawMainScreen();
		break;
	}
}

void onButtonHold() {	
	switch (menuSelected) {
	case NullItem: //No Menu Item selected
		if (menuActive == NullItem && !buttonIsHeld) {
			togglePowerState();
			drawMenuBar();
		}
		break;
	default:
		if (!buttonIsHeld) { //close menu
			//menuSelected = NullItem;
			//menuActive = NullItem;
			//drawMenuBar();
			//drawMainScreen();
		}
		break;
	}
	buttonIsHeld = true;  //prevents repeated button held events
};

void togglePowerState() {
	currentPowerState ^= true;
}

void onClick() {

	if (menuSelected != NullItem && menuSelected != menuActive) {
		menuActive = menuSelected;
		drawMenuBar();
		drawMainScreen();
	} else if (menuSelected != NullItem && menuSelected == menuActive) {
		menuActive = NullItem;
		drawMenuBar();
		//drawMainScreen();
	}

}

void onButtonUp() {
	//reset button hold status
	if (buttonIsHeld == true) {
		buttonIsHeld = false;
	}
};


void encChange() {
  // Depending in which menu state you are
  // the encoder will either change the value of a setting:
  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), On/Off, mode
  // or it will change the cursor position


	rotaryEncoderDir dir = DIR_NONE;
  // Direction clockwise
  if (dir == DIR_CW) {
    switch (menuSelected) {
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
          if (selectedWaveform == 2)
            selectedWaveform = 0;
          else
            selectedWaveform++;
          updateDisplay = true;
        } break;
    }
  }
  // Direction counter clockwise
  else if (dir == DIR_CCW) {
    switch (menuSelected) {
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
          if (selectedWaveform == 0)
            selectedWaveform = 2;
          else
            selectedWaveform--;
          updateDisplay = true;
        } break;
    }
  }
}
// Function to display the current frequency in the top left corner
void displayFrequency() {
  unsigned long frequencyToDisplay = frequency;
  
  tft.setCursor(4, 10);
  tft.setTextColor(GREEN);
  tft.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(F("F="), 4, 10, &x1, &y1, &w, &h);
  tft.fillRect(x1, y1, tft.width() - 8, h, BLACK);
  tft.print(F("F="));

  if (frequency >= 1000000) {
	  tft.print(frequency / 1000000.0, 4);
	  tft.println(F(" GHz"));
  } else if (frequency >= 1000) {
	  tft.print(frequency / 1000.0, 3);
	  tft.println(F(" MHz"));
  } else {
	  tft.print(frequency);
	  tft.println(F(" Hz"));
  }
 
}
// Function to display power state (ON/OFF) in the top right corner
void displayPower() {
	tft.setCursor(4, 30);
	tft.setTextColor(GREEN);
	tft.setTextSize(1);
	//tft.print(powerState[currentPowerState]);
}
// Function to display the mode in the bottom right corner
void displayMode() {
	tft.setTextColor(GREEN);
	tft.setTextSize(1);
	tft.setCursor(4, 60);
	tft.print(waveformLabel[selectedWaveform]);
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
	tft.setCursor(4, 90);
	tft.setTextSize(1);
	tft.setTextColor(GREEN);	
	tft.print("Preset ");
	tft.print(freqRegister);
}

void displayChange() {
	return;
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
}

void displayDebugMsg(String msg) {
#ifdef DEBUG
	tft.setTextSize(1);
	tft.setTextColor(GREEN);
	tft.setCursor(4, 125);
	int16_t x1, y1;
	uint16_t w, h;
	tft.getTextBounds((char*)msg.c_str(), 4, 125, &x1, &y1, &w, &h);
	tft.fillRect(x1, y1, tft.width(), h, PALEVIOLET);
	tft.print(msg);
#endif // DEBUG
}

void drawMainScreen() {	
	tft.fillRect(0, tft.height() / 5 + 4, tft.width(), tft.height() * 4 / 5, PALEVIOLET);	
	switch (menuActive)
	{
	case NullItem:
		if(menuSelected == NullItem)
		drawMainScreenFrequency();
		break;
	case FreqSet:
		break;
	case Mode:
		drawMainScreenWaveform();
		break;
	case Power:
		break;
	case PhasePreset:
		break;
	case FreqPreset:
		break;
	case Settings:
		break;
	case lastItem:
		break;
	default:
		break;
	}
}

void drawMainScreenWaveform(bool drawPartial) {
	//don't draw if partial redraw requested, reduces flickering
	if (drawPartial == false) {
		tft.setTextColor(BLACK);
		tft.setTextSize(1);
		tft.setCursor(4, 50);
		tft.print(F("Select Waveform:"));

		tft.drawBitmap(16, 59, sineWave32, 32, 32, PALEVIOLET, WHITE);
		tft.drawBitmap(64, 59, triangleWave32, 32, 32, PALEVIOLET, WHITE);
		tft.drawBitmap(112, 59, squareWave32, 32, 32, PALEVIOLET, WHITE);
	}
	tft.setTextColor(BLACK);
	tft.fillRect(0, 95, tft.width(), 15, PALEVIOLET);
	printAlignCenter(waveformLabel[selectedWaveform], 2, tft.width() / 2, 100);
	//highlight active waveform by drawing a rectangle bihind the icon
	tft.drawRect(14, 57, 36, 36, (selectedWaveform == 0) ? HOTMAGENTA : PALEVIOLET);
	tft.drawRect(62, 57, 36, 36, (selectedWaveform == 1) ? HOTMAGENTA : PALEVIOLET);
	tft.drawRect(110, 57, 36, 36, (selectedWaveform == 2) ? HOTMAGENTA : PALEVIOLET);

}

void drawMainScreenFrequency() {
	tft.setTextColor(BLACK);
	tft.setCursor(4, 50);
	tft.setTextSize(1);
	tft.print(F("Frequency"));
	String unit = F(" Hz");
	float f = frequency;
	char buffer[50];
	if (frequency >= 1e+9) {
		f = frequency / 1e+9F;
		unit = F("GHz");
	}
	else if (frequency >= 1e+6) {
		f = frequency / 1e+6F;
		unit = F("MHz");
	}
	else if (frequency >= 1e+3) {
		f = frequency / 1e+3F;
		unit = F("kHz");
	}

	sprintf(buffer, "%d,%02d ", (int)f, (int)(f * 100) % 100);
	printAlignCenter((String) buffer, 4, tft.width()/2, 75);
	tft.setTextSize(3);
	tft.setCursor(100, 110);
	tft.print(unit);
}

void drawMenuBar() {
	
	tft.setTextSize(2);
	tft.setCursor(4, 16);
	if (menuSelected == menuActive && menuSelected != NullItem) {
		tft.fillRect(0, 0, tft.width(), tft.height() / 5, AQUA);
		tft.setTextColor(BLACK);
	} else {
		tft.fillRect(0, 0, tft.width(), tft.height() / 5, HOTMAGENTA);
		tft.setTextColor(WHITE);
	}
	

	switch (menuSelected) {
	case NullItem:
		//Menu Symbol
		tft.drawBitmap(0, 0, menuButton, 24, 24, HOTMAGENTA, WHITE);
		//draw power symbol
		tft.drawBitmap(tft.width() - 25, 0, powerButton, 24, 24, (currentPowerState == true) ? APPLEGREEN : RED, HOTMAGENTA);
		//draw waveform for current mode

		switch (selectedWaveform)
		{
		case 0:
		default:
			tft.drawBitmap(tft.width() - 50, 0, sineWave24, 24, 24, WHITE, HOTMAGENTA);
			break;
		case 1:
			tft.drawBitmap(tft.width() - 50, 0, triangleWave24, 24, 24, WHITE, HOTMAGENTA);
			break;
		case 2:
			tft.drawBitmap(tft.width() - 50, 0, squareWave24, 24, 24, WHITE, HOTMAGENTA);
			break;
		}
		
	//	break;
	//case FreqSet:		
	//	break;
	//case Mode:
	//	break;
	//case Power:
	//	break;
	//case PhasePreset:
	//	break;
	//case FreqPreset:
	//	break;
	//case Settings:
	//	break;
	default:
		tft.print(MenuLabel[menuSelected]);
		break;
	}

}

void initTFT() {
	tft.initR(INITR_GREENTAB);
	tft.setRotation(1);
	tft.invertDisplay(true);
	tft.setTextWrap(false);
	tft.setFont(&Org_01);
}

void displayBootUpMsg() {
	tft.fillScreen(BLACK);
	tft.fillRect(0, 0, tft.width(), tft.height() / 5, HOTMAGENTA);
	tft.fillRect(0, tft.height() / 5 + 4, tft.width(), tft.height() *4/5  , PALEVIOLET);


	tft.setTextColor(MINTYGREEN);

	printAlignCenter(F("Signal Generator"), 2, tft.width() / 2, tft.height() / 10);
	tft.drawBitmap(tft.width() / 2 - 48, tft.height() / 2 -19, sappz, 96, 38, PALEVIOLET, BLACK);
	tft.setTextColor(BLACK);

	printAlignCenter(__FIRMWARE_VERSION__, 1, tft.width()/2, tft.height()-tft.height()/5);
	printAlignCenter(F("Powered by"), 1, tft.width() / 2, tft.height() / 2 -25);
	delay(1000);
	tft.fillScreen(BLACK);	
}





void printAlignCenter(const String text, uint8_t s, int16_t x, int16_t y) {
	int16_t x1, y1;
	uint16_t w, h;

	tft.setTextSize(s);	
	tft.getTextBounds((char*)text.c_str(), 0, 0, &x1, &y1, &w, &h);

	if (y == y1) {
		tft.setCursor(x - w / 2, y - h / 2);
	} else {
		tft.setCursor(x - w / 2, y + h / 2);		
	}
	tft.println(text);
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