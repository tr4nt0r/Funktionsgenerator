

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST77xx.h>
#include <Adafruit_ST7735.h>
#include "definitions.h"
#include "AD9833.h"
#include <Fonts\Org_01.h>
#include <TimerOne.h>
#include <ClickEncoder.h>




AD9833 sigGen(AD9833_FsyncPin, AD9833_ClkFreq); // Initialise our AD9833 with FSYNC pin = 10 and a master clock frequency of 24MHz
ClickEncoder encoder(rotaryEncoderPinA, rotaryEncoderPinB, rotaryEncoderPinBtn, 4);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, -1);

//for some reason, compiler sometimes needs forward declaration ¯\_(ツ)_/¯ 
void drawMenuBar();
void drawMainScreen();
void drawMainScreenWaveform(bool drawPartial = false);
void drawMainScreenFrequency(bool drawPartial = false);
Menu::Menu_e getNextMenuItem(Direction::dir direction);
void changeFrequencyDigit(Direction::dir dir, uint8_t digitPos);
void changeFrequency(Direction::dir dir);
void callButtonEvent(ClickEncoder::Button button);


void setup() {
	initTFT();
	displayBootUpMsg(); //display Bootscreen for 2s
	drawMenuBar();
	drawMainScreen(); //renders Mainscreen Boxes


					  // Initialise the AD9833 with 1KHz sine output, no phase shift for both
					  // registers and remain on the FREQ0 register
	sigGen.reset(1);
	sigGen.setFreq(frequency);
	sigGen.setPhase(phase);
	sigGen.setFPRegister(1);
	sigGen.setFreq(frequency);
	sigGen.setPhase(phase);
	sigGen.setFPRegister(0);
	sigGen.mode(selectedWaveform);
	sigGen.reset(0);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(ISRcallback);

	encoder.setAccelerationEnabled(true);
}

void onRotaryEncoderTurnEvent() {

	rotaryEncoderPos += encoder.getValue();
	Direction::dir dir = Direction::NONE;

	int8_t rotaryEncoderMovement = rotaryEncoderPos - rotaryEncoderLastPos;
	if (rotaryEncoderMovement != 0) {
		dir = (rotaryEncoderMovement > 0) ? Direction::CW : Direction::CCW;
		rotaryEncoderLastPos = rotaryEncoderPos;
		isAccelerated = (abs(rotaryEncoderMovement) > 1) ? true : false;
 	}
	else {
		dir = Direction::NONE;
	}

	//navigate menu if no Menu item currently selected
	if (dir != Direction::NONE && menuSelected != Menu::NullItem && menuSelected != menuActive) {
		if (menuSelected != getNextMenuItem(dir)) {
			menuSelected = getNextMenuItem(dir);
			drawMenuBar();
		}
	}
	//rotary encoder actions if a menu item is active
	else if (dir != Direction::NONE && menuSelected == menuActive) {
		switch (menuActive)
		{
		case Menu::NullItem:
			changeFrequency(dir);
			drawMainScreenFrequency(true);
			sigGen.setFreq(frequency);
			break;
		case Menu::FreqSet:
			changeFrequencyDigit(dir, digitPos);
			break;
		case Menu::Mode:
			if (dir == Direction::CW && selectedWaveform < 2) {
				selectedWaveform++;
				drawMainScreenWaveform(true);
				sigGen.mode(selectedWaveform);
			} else if (dir == Direction::CCW && selectedWaveform > 0) {
				selectedWaveform--;
				drawMainScreenWaveform(true);
				sigGen.mode(selectedWaveform);
			}
			break;
		//case Menu::Power:
		//	break;
		//case Menu::Phase:
		//	break;
		//case Menu::Settings:
		//	break;
		//case Menu::lastItem:
		//	break;
		//default:
		//	break;
		}
	}
}

//change digit or unit in frequency set menu
void changeFrequencyDigit(Direction::dir dir, uint8_t digitPos) {
	int8_t rnew;

	if (digitPos == numDigits) {
		if (dir == Direction::CW)
			newFrequencyExp = (newFrequencyExp == 6) ? 6 : newFrequencyExp + 3;
		else
			newFrequencyExp = (newFrequencyExp == 0) ? 0 : newFrequencyExp - 3;
	} else {
		if (dir == Direction::CW)
			newFrequency[digitPos] = (newFrequency[digitPos] == 9) ? 0 : newFrequency[digitPos] + 1;
		else
			newFrequency[digitPos] = (newFrequency[digitPos] == 0) ? 9 : newFrequency[digitPos] - 1;
	}	

	drawMainScreenFreqSet();
}

//change frequency in MainScreen
void changeFrequency(Direction::dir dir) {
	uint32_t step;

	if (frequency > 1000000) { //MHz
		step = (isAccelerated) ? 100000 : 10000;
		if (dir == Direction::CCW && frequency - step < 1000000)
			step = abs(frequency - 1000000);
		if (dir == Direction::CW && frequency + step > maxFrequency)
			step = maxFrequency - frequency;
	} else if (frequency > 1000) { //kHz
		step = (isAccelerated) ? 1000 : 10;
		if (dir == Direction::CCW && frequency - step < 1000)
			step = abs(frequency - 1000);
	} else {
		step = (isAccelerated) ? 100 : 1;
		if (dir == Direction::CCW && frequency <= step)
			step = abs(frequency - 1);
	}
	frequency += (dir == Direction::CW) ? step : step*-1;
}

//Interrupt Service Routine 
void ISRcallback() {
	encoder.service();
}



Menu::Menu_e getNextMenuItem(Direction::dir direction) {
	uint8_t next = (int)menuSelected;
	if (direction == Direction::CW && next < Menu::lastItem - 1) {
		next++;
	}
	else if (direction == Direction::CCW && next > Menu::NullItem + 1) {
		next--;
	}
	return (Menu::Menu_e)next;
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
	case Menu::NullItem: //No Menu Item selected
		if (menuActive == Menu::NullItem) {
			menuSelected = Menu::FreqSet;
			drawMenuBar();
		}
		break;
	default:			
			menuSelected = Menu::NullItem;
			menuActive = Menu::NullItem;
			digitPos = 0;
			drawMenuBar();
			drawMainScreen();
		break;
	}
}

void onButtonHold() {	
	switch (menuSelected) {
	case Menu::NullItem: //No Menu Item selected
		if (menuActive == Menu::NullItem && !buttonIsHeld) {
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
	// Turn DAC and clock ON/OFF
	(currentPowerState) ? sigGen.sleep(3) : sigGen.sleep(0);
}

void onClick() {

	if (menuSelected != Menu::NullItem && menuSelected != menuActive) {
		menuActive = menuSelected;
		drawMenuBar();
		drawMainScreen();
	} else if (menuSelected == menuActive) {

		//drawMainScreen();
		switch (menuActive) {
		case Menu::NullItem:
			break;
		case Menu::FreqSet:

			//with each click cursor switches to next digit			

			if (digitPos < numDigits) {
				digitPos++;
				drawMainScreenFreqSet();

			} else {
				digitPos = 0;

				for (uint8_t i = 0; i < numDigits; i++) {

				}

				frequency = getFrequencyFromDigits();
				menuActive = Menu::NullItem;
				drawMenuBar();
				drawMainScreenFrequency();
				sigGen.setFreq(frequency);
			}

			break;
		//case Mode:
		//	break;
		//case Power:
		//	break;
		//case Phase:
		//	break;
		//case Sweep:
		//	break;
		//case Settings:
		//	break;
		//case Help:
		//	break;
		//case lastItem:
		//	break;
		default:
			menuActive = Menu::NullItem;
			drawMenuBar();
			break;
		}

	}

}

uint32_t getFrequencyFromDigits() {
	uint32_t f = 0;

	for (uint8_t i = 0; i < numDigits; i++) {
		if (newFrequencyExp != 0 || i <= 2)
			f += newFrequency[i] * pow(10, newFrequencyExp - 2 + numDigits - i - 1);		
	}
	if (f > maxFrequency)
		return maxFrequency;
	if (f == 0)
		return 1;
	return f;
}
void onButtonUp() {
	//reset button hold status
	if (buttonIsHeld == true) {
		buttonIsHeld = false;
	}
};

//
//void encChange() {
//  // Depending in which menu state you are
//  // the encoder will either change the value of a setting:
//  //-+ frequency, change between FREQ0 and FREQ1 register (or -+ phase), On/Off, mode
//  // or it will change the cursor position
//
//
//	rotaryEncoderDir dir = DIR_NONE;
//  // Direction clockwise
//  if (dir == DIR_CW) {
//    switch (menuSelected) {
//      case 1: {
//          if (cursorPos == 3)
//            cursorPos = 0;
//          else
//            cursorPos++;
//        } break;
//
//      case 2: {
//          // Here we initialise two variables.
//          // newFrequency is the value of the frequency after we increment it
//          // dispDigit is the digit that we are currently modifing, and we obtain it
//          // by a neat little trick, using operator % in conjunction with division
//          // We then compare these variables with the maximum value for our
//          // frequency, if all is good, make the change
//          unsigned long newFrequency = frequency + pow(10, digitPos);
//          unsigned char dispDigit =
//            frequency % (unsigned long) pow(10, digitPos + 1) / pow(10, digitPos);
//          if (newFrequency <= maxFrequency && dispDigit < 9) {
//            frequency += pow(10, digitPos);
//            updateDisplay = true;
//          }
//
//          if (freqRegister == 0) {
//            frequency0 = frequency;
//          } else if (freqRegister == 1) {
//            frequency1 = frequency;
//          }
//
//        } break;
//
//      case 4: {
//          // if usePhase has been defined, changes in the encoder will vary the phase
//          // value (upto 4096)
//          // A better implementation would be to use increment of pi/4 or submultiples of
//          // pi where 2pi = 4096
//#ifdef usePhase
//          unsigned long newPhase = phase + pow(10, digitPos);
//          unsigned char dispDigit =
//            phase % pow(10, digitPos + 1) / pow(10, digitPos);
//          if (newPhase < maxPhase && dispDigit < 9) {
//            phase += pow(10, digitPos);
//            updateDisplay = true;
//          }
//#endif
//        } break;
//
//      case 5: {
//          if (selectedWaveform == 2)
//            selectedWaveform = 0;
//          else
//            selectedWaveform++;
//          updateDisplay = true;
//        } break;
//    }
//  }
//  // Direction counter clockwise
//  else if (dir == DIR_CCW) {
//    switch (menuSelected) {
//      case 1: {
//          if (cursorPos == 0)
//            cursorPos = 3;
//          else
//            cursorPos--;
//        } break;
//
//      case 2: {
//          unsigned long newFrequency = frequency + pow(10, digitPos);
//          unsigned char dispDigit = frequency % (unsigned long) pow(10, digitPos + 1) / pow(10, digitPos);
//
//          if (newFrequency > 0 && dispDigit > 0) {
//            frequency -= pow(10, digitPos);
//            updateDisplay = true;
//          }
//
//          if (freqRegister == 0) {
//            frequency0 = frequency;
//          } else if (freqRegister == 1) {
//            frequency1 = frequency;
//          }
//        } break;
//
//      case 4: {
//          // if usePhase has been defined, changes in the encoder will vary the phase
//          // value (upto 4096)
//          // A better implementation would be to use increment of pi/4 or submultiples of
//          // pi where 2pi = 4096
//#ifdef usePhase
//          unsigned long newPhase = phase + pow(10, digitPos);
//          unsigned char dispDigit =
//            phase % pow(10, digitPos + 1) / pow(10, digitPos);
//          if (newPhase > 0 && dispDigit > 0) {
//            phase -= pow(10, digitPos);
//            updateDisplay = true;
//          }
//#endif
//        } break;
//
//      case 5: {
//          if (selectedWaveform == 0)
//            selectedWaveform = 2;
//          else
//            selectedWaveform--;
//          updateDisplay = true;
//        } break;
//    }
//  }
//}


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
//void displayPower() {
//	tft.setCursor(4, 30);
//	tft.setTextColor(GREEN);
//	tft.setTextSize(1);
//	//tft.print(powerState[currentPowerState]);
//}
//// Function to display the mode in the bottom right corner
//void displayMode() {
//	tft.setTextColor(GREEN);
//	tft.setTextSize(1);
//	tft.setCursor(4, 60);
//	tft.print(waveformLabel[selectedWaveform]);
//}
// Function to display the mode in the bottom left corner
// Only used if you enable PHASE setting instead of FREQ register
//void displayPhase() {
//  unsigned int phaseToDisplay = phase;
//  //lcd.setCursor(0, 1);
//  //lcd.write(0);
//  //lcd.print("=");
//  for (int i = 3; i >= 0; i--) {
//    unsigned int dispDigit = phaseToDisplay / pow(10, i);
//    //lcd.print(dispDigit);
//    phaseToDisplay -= dispDigit * pow(10, i);
//  }
//}
// Function to display the FREQ register (either 0 or 1) in the bottom left
// corner
//void displayFreqRegister() {
//	tft.setCursor(4, 90);
//	tft.setTextSize(1);
//	tft.setTextColor(GREEN);	
//	tft.print("Preset ");
//	tft.print(freqRegister);
//}

//void displayChange() {
//	return;
//	// Update display if needed
//	if (updateDisplay == true) {
//		displayFrequency();
//#ifdef usePhase
//		displayPhase();
//#endif
//		displayPower();
//#ifndef usePhase
//		displayFreqRegister();
//#endif
//		displayMode();
//		updateDisplay = false;
//	}
//}

void displayDebugMsg(uint32_t msg) { displayDebugMsg((String)msg); }
void displayDebugMsg(String msg) {
#ifdef DEBUG
	tft.setTextSize(1);	
	tft.setCursor(4, 120);
	int16_t x1, y1;
	uint16_t w, h;
	tft.getTextBounds((char*)msg.c_str(), 4, 120, &x1, &y1, &w, &h);
	tft.fillRect(x1, y1, tft.width(), h, PALEVIOLET);
	tft.print(msg);
#endif // DEBUG
}

void drawMainScreen() {	
	tft.fillRect(0, tft.height() / 5 + 4, tft.width(), tft.height() * 4 / 5, PALEVIOLET);	
	switch (menuActive) {
	case Menu::NullItem:
		if (menuSelected == Menu::NullItem)
			drawMainScreenFrequency();
		break;
	case Menu::FreqSet:
		//when entering FreqSet Menu initialize digits with actual frequency
		if (digitPos == 0) {
			numDigits = 5;
			newFrequencyExp = 0;
			if (frequency >= 1000000)
				newFrequencyExp = 6;
			else if (frequency >= 1000)
				newFrequencyExp = 3;

			for (uint8_t i = 0; i < numDigits; i++) {
				if (newFrequencyExp == 0 && i > 2)
					newFrequency[i] = 0;
				else {
					newFrequency[i] = frequency / pow(10, numDigits - 1 - i + newFrequencyExp - 2) % 10;
				}
			}
		}

		drawMainScreenFreqSet();
		break;
	case Menu::Mode:
		drawMainScreenWaveform();
		break;
	case Menu::Power:
		break;
	case Menu::Phase:
		break;
	case Menu::Settings:
		break;
	case Menu::lastItem:
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

void drawMainScreenFrequency(bool drawPartial) {
	uint8_t exp = 0, dec = 0;	
	String PROGMEM unit;
	bool isTrailZero = true;

	tft.setTextColor(BLACK);
	if (!drawPartial) {
		tft.fillRect(0, 45, tft.width(), 10, PALEVIOLET);
		tft.setCursor(4, 50);
		tft.setTextSize(1);
		tft.print(F("Frequency"));
	}

	if (frequency >= 1000000) {
		unit = F("MHz");
		exp = 6;
		numDigits = 5;  //show 5 digits, 2 digits as decimal
		dec = 2;
	} else if (frequency >= 1000) {
		unit = F("kHz");
		exp = 3;
		numDigits = 5;
		dec = 2;
	} else {
		unit = F("Hz");
		numDigits = 3;	
	}
	tft.fillRect(0, 65, tft.width(), 30, PALEVIOLET);
	String output;

	for (uint8_t i = numDigits; i > 0; i--) {
		uint32_t r = frequency / pow(10, (i - 1) + exp - dec) % 10;
		if (!isTrailZero || r != 0) { //suppress trailing zeros
			if (i == dec)
				output += F(",");
			output += (String)r;
			isTrailZero = false;
		}
	}
	printAlignCenter(output, 4, tft.width() / 2, 75);
	tft.setTextSize(3);
	tft.setCursor(100, 110);
	tft.fillRect(100, 95, 100, 20, PALEVIOLET);
	tft.print(unit);
}

void drawMainScreenFreqSet() {
	uint8_t dec = 2;
	String unit = F("Hz");

	numDigits = 5;


	tft.setTextColor(BLACK);
	tft.setCursor(4, 50);
	tft.setTextSize(1);
	tft.print(F("Set Frequency"));

	if (newFrequencyExp == 6)
		unit = F("MHz");
	else if (newFrequencyExp == 3)
		unit = F("kHz");



	tft.fillRect(0, 65, tft.width(), 30, PALEVIOLET);
	tft.setFont();
	tft.setCursor(20, 70);
	tft.setTextSize(3);

	for (uint8_t i = 0; i < numDigits; i++) {

		if (i == numDigits - dec)
			tft.print(F(","));

		if (i == digitPos)
			tft.setTextColor(HOTMAGENTA);

		tft.print(newFrequency[i]);

		if (i == digitPos)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
			tft.setTextColor(BLACK);

	}	
	tft.setFont(&Org_01);
	tft.setTextSize(3);
	tft.setCursor(100, 110);
	tft.fillRect(100, 95, 100, 20, PALEVIOLET);
	tft.setTextColor((digitPos == numDigits) ? HOTMAGENTA : BLACK); 
	tft.print(unit);
}


void drawMenuBar() {
	
	tft.setTextSize(2);
	tft.setCursor(4, 16);
	if (menuSelected == menuActive && menuSelected != Menu::NullItem) {
		tft.fillRect(0, 0, tft.width(), tft.height() / 5, AQUA);
		tft.setTextColor(BLACK);
	} else {
		tft.fillRect(0, 0, tft.width(), tft.height() / 5, HOTMAGENTA);
		tft.setTextColor(WHITE);
	}
	

	switch (menuSelected) {
	case Menu::NullItem:
		//Menu Symbol
		tft.drawBitmap(0, 0, menuButton, 24, 24, HOTMAGENTA, WHITE);
		//draw power symbol
		tft.drawBitmap(tft.width() - 25, 0, powerButton, 24, 24, (currentPowerState == true) ? APPLEGREEN : RED, HOTMAGENTA);
		//draw waveform for current mode

		switch (selectedWaveform) {
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
		//case Phase:
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

int32_t pow(int base, int exp) {
	int32_t result = 1;
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


void loop() {
	// Check to see if the button has been pressed  
	callButtonEvent(encoder.getButton());
	//checks if rotary encoder has been turned and                                                                                                                                                                   
	onRotaryEncoderTurnEvent();


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

}