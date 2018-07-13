// definitions.h
#define DEBUG


const PROGMEM String __FIRMWARE_VERSION__ = "0.1pre-alpha" ;

//Uncomment the line below if you want to change the Phase instead of the FREQ register
//#define usePhase

const uint8_t rotaryEncoderPinBtn = 1;
const uint8_t rotaryEncoderPinA = 3;
const uint8_t rotaryEncoderPinB = 2;
int16_t rotaryEncoderLastPos = 0;
int16_t rotaryEncoderPos = 0;

enum rotaryEncoderDir {
	DIR_NONE,
	DIR_CW,
	DIR_CCW,
};

enum Menu {
	NullItem,
	FreqSet,
	Mode,
	Power,
	PhasePreset,
	FreqPreset,		
	Settings,
	lastItem
};


const PROGMEM String MenuLabel[] = {
	"",
	"FREQUENCY",	
	"PHASE",
	"OUTPUT",
	"WAVE",
	"PRESETS",
	"SETTINGS",
};

const uint8_t AD9833_FsyncPin = 10;
const uint32_t AD9833_ClkFreq = 24000000;

const uint8_t TFT_CS = 9;
const uint8_t TFT_DC = 8;

// Variables used to input data and walk through menu

unsigned char settingsPos[] = { 0, 14, 20, 29 };
unsigned char button;
unsigned char lastCursorPos = 0;
unsigned char cursorPos = 0;
Menu menuSelected = NullItem; //highligted menu item selected with rotary encoder
Menu menuActive = NullItem; //currently active menu item
bool buttonIsHeld = false;  //check if button is still held or is a new hold event after a release event
int digitPos = 0;
const unsigned long maxFrequency = 14000000;
const unsigned int maxPhase = 4095; // Only used if you enable PHASE setting instead of FREQ register
unsigned long newFrequency = 1000;
volatile bool updateDisplay = true;

int freqRegister = 0; // Default FREQ register is 0
					  // LCD constants
const String powerState[] = { " ON", "OFF" };
const String mode[] = { "SIN", "TRI", "CLK" };
// Variables used to store phase, frequency, mode and power
unsigned char currentMode = 0;
unsigned long frequency0 = 100;
unsigned long frequency1 = 1000;
unsigned long frequency = frequency0;
unsigned long currFrequency; // Current frequency used, either 0 or 1
unsigned long phase = 0; // Only used if you enable PHASE setting instead of FREQ register
bool currentPowerState = false;
// Greek PHI symbol for phase shift
// Only used if you enable PHASE setting instead of FREQ register
uint8_t phi[8] = { 0b01110, 0b00100, 0b01110, 0b10101,
0b10101, 0b01110, 0b00100, 0b01110
};



//some colors based on https://xkcd.com/color/rgb/ 
#define ROSEPINK 0xFCB3
#define APPLEGREEN 0x5725
#define LIGHTBLUE 0x07FF
#define PALEVIOLET 0xBD5F
#define AQUA 0x1759
#define CERULEAN 0x03F8
#define HOTMAGENTA 0xE078
#define SAPPHIRE 0x3995
#define MINTYGREEN 0x176E
#define TOPAZ 0x3618
#define MUDBROWN 0x6A64
#define PISSYELLOW 0xEF00
#define DULLORANGE 0xCC69
#define MEDIUMGREY 0x83EE

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF


//bitmaps created with http://javl.github.io/image2cpp/
// 'sappz', 91x33px
const uint8_t sappz[96 * 38] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x03, 0xff,
	0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x01, 0xff, 0xff, 0xf8, 0x3f, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xc1, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf9, 0x9f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff,
	0xff, 0xf9, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf1, 0xdf, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf1, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf3, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff,
	0xff, 0xf3, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf3, 0xef, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xf1, 0xff, 0xff, 0xf3, 0xef, 0xc3, 0xc0, 0x3c, 0x00, 0xf0,
	0x03, 0xfb, 0xf1, 0xff, 0xff, 0xe3, 0xef, 0x93, 0x80, 0x1c, 0x00, 0x70, 0x01, 0xfb, 0xf1, 0xff,
	0xff, 0xe7, 0xef, 0x9d, 0x9f, 0x8c, 0x7e, 0x33, 0xf8, 0xfb, 0xf1, 0xff, 0xff, 0xe7, 0xe7, 0xbd,
	0x9f, 0x8c, 0x7e, 0x33, 0xf8, 0xfb, 0xf1, 0xff, 0xff, 0xe7, 0xf7, 0xbd, 0x9f, 0x8c, 0x7e, 0x33,
	0xf8, 0xfb, 0xf1, 0xff, 0xff, 0xc7, 0xf7, 0x3c, 0x1f, 0x8c, 0x7e, 0x33, 0xf8, 0xfb, 0xf1, 0xff,
	0xff, 0x87, 0xf7, 0x7e, 0x00, 0x0c, 0x7e, 0x33, 0xf8, 0x03, 0xf1, 0xff, 0xff, 0x8f, 0xf7, 0x7f,
	0x80, 0x0c, 0x7e, 0x33, 0xf8, 0xff, 0xf1, 0xff, 0xff, 0x8f, 0xfa, 0x7f, 0x9f, 0xcc, 0x7e, 0x33,
	0xf8, 0xff, 0xf1, 0xff, 0xff, 0x8f, 0xf8, 0xff, 0x9f, 0xcc, 0x7e, 0x33, 0xf8, 0xff, 0xf1, 0xff,
	0xff, 0x8f, 0xff, 0xff, 0x9f, 0xc0, 0x00, 0x00, 0x00, 0xff, 0xf1, 0xff, 0xff, 0x1f, 0xff, 0xff,
	0x9f, 0xc0, 0x00, 0x00, 0x01, 0xff, 0xf1, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xbf, 0xe0, 0x00, 0x00,
	0x03, 0xff, 0xf1, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xf3, 0xff, 0xff, 0xf1, 0xff,
	0xf0, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xf3, 0xff, 0xff, 0xf0, 0xff, 0xe0, 0x7f, 0xff, 0xff,
	0xff, 0xfc, 0x7f, 0xf3, 0xff, 0xff, 0xf0, 0x1f, 0xe0, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xf3,
	0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
//16x16px
const uint8_t powerButton[] PROGMEM = {
	0xfe, 0x7f, 0xfe, 0x7f, 0xf6, 0x6f, 0xe2, 0x47, 0xc6, 0x63, 0x8e, 0x71, 0x8e, 0x71, 0x9e, 0x79,
	0x9e, 0x79, 0x9f, 0xf9, 0x9f, 0xf9, 0x8f, 0xf1, 0xc7, 0xe3, 0xc1, 0x83, 0xf0, 0x0f, 0xf8, 0x1f
};
//24x24
const uint8_t menuButton[] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8,
	0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00,
	0x1f, 0xf8, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f,
	0xf8, 0x00, 0x1f, 0xf8, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};