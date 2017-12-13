

// mapping suggestion from Waveshare 2.9inch e-Paper to Wemos D1 mini
// BUSY -> D2, RST -> D4, DC -> D3, CS -> D8, CLK -> D5, DIN -> D7, GND -> GND, 3.3V -> 3.3V

#include "otarie.h"

extern "C" {
#include "user_interface.h"
extern struct rst_info resetInfo;
}

// include library, include base class, make path known
#include <GxEPD.h>

// select the display class to use, only one
#include <GxGDEP015OC1/GxGDEP015OC1.cpp>    // 1.54" b/w

// FreeFonts from Adafruit_GFX
// #include <Fonts/FreeMonoBold12pt7b.h>
// #include <Fonts/FreeMonoBold18pt7b.h>
#include "DejaVuSansMono16pt8b.h"

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>

// GxIO_SPI(SPIClass& spi, int8_t cs, int8_t dc, int8_t rst = -1, int8_t bl = -1);
GxIO_Class io(SPI, SS, 0, 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
// GxGDEP015OC1(GxIO& io, uint8_t rst = 2, uint8_t busy = 4);
GxEPD_Class display(io); // default selection of D4(=2), D2(=4)

#define VERSION "1.0"
#define SLEEP_SEC 10

#define NB_QUOTES 8
char* quotes[] = { // note: max 10 characters
	"OH PUTAIN,\nBordeaux!\n",
	"Je teste\navant de\ncommiter!\n",
	"Ça marche\nsur ma\nmachine\n",
	"Ça marche\nsous\nWindows\n",
	"Je ne\ncomprends\npas, j'ai\npas touché\n",
	"C'est pas\nmoi,\npour une\nfois\n",
	"Ça doit\nêtre\nSVN qui\ndéconne\n",
	"Il n'y\naura pas\nrégression\n"
};
uint32_t quote = 0;

// size of one character
// uint16_t w1, h1;
#define w1 18
#define h1 21

void setup() {
	Serial.begin(115200);
	Serial.println();
	Serial.printf ("\n\n%s\n", VERSION);

// 	Serial.println("setup");

	// if we want to blink, uncomment
	pinMode (BUILTIN_LED, OUTPUT);

	display.init ();
	// init font
	display.setTextColor(GxEPD_BLACK);
	display.setFont(&DejaVuSansMono16pt8b);
	display.setRotation (1);

	// get one char size. as this is a Mono font, every char will be the same size
	// replaced by #define
// 	int16_t  x1, y1;
// 	display.getTextBounds("AA\nAA", 10, 10, &x1, &y1, &w1, &h1);
// 	w1>>=1;h1>>=1;
	Serial.print(h1);Serial.print("x");Serial.println(w1);

// 	Serial.println("setup done");
	Serial.println (ESP.getResetReason());

	if ( resetInfo.reason == REASON_DEEP_SLEEP_AWAKE ) {
		// wake up, display quote
		// read current quote num from RTC Memory
		ESP.rtcUserMemoryRead(0, (uint32_t*) &quote, sizeof(quote));
		displayQuote ( quotes[quote++ % NB_QUOTES] );
		// write current quote num to RTC Memory
		ESP.rtcUserMemoryWrite(0, (uint32_t*) &quote, sizeof(quote));
// 		displayQuote ( quotes[secureRandom(NB_QUOTES)] );
	} else {
		// power on, display Otarie
		displayQuote ("Pour\nThierry\n~\nl'équipe\nOtarie\n");
		Serial.println("Frédéric, Olivier, Mickael, Emmanuel, Lionel, Jérôme");
		delay (3000);
		display.drawBitmap (otarie, sizeof(otarie) );
	}
// 	Serial.printf("Sleep for %d seconds ...\n", SLEEP_SEC);
	ESP.deepSleep(SLEEP_SEC * 1000000);
}

void loop() {
	// nothing to be done here
}

void displayQuote (char* quote) {
	uint8_t lines = 0;
	uint8_t i = 0;
	uint16_t h;
	char str[11*5];
	char *pb = str, *pe=str;

	strcpy (str, quote);

	// count number of lines (\n) of the quote
	for (i=0; quote[i] != 0; i++) {
		lines += (quote[i] == '\n');
	}
	h = lines * h1;

// 	display.fillScreen(GxEPD_WHITE); // not needed, we comming from sleep, so screen just inited
	display.setCursor(0, (200-h)/2);

	// dislpay lines
	while (lines-- > 0) {
		while (*pe != '\n') {
			pe++;
		}
		*pe = 0;
		display.setCursor ( (200-((pe-pb)*w1))/2 , display.getCursorY());
		display.println (pb);
		pe++;
		pb = pe;
	}

	display.update();
}

void showFont(const char name[], const GFXfont* f)
{
	display.fillScreen(GxEPD_WHITE);
	display.setTextColor(GxEPD_BLACK);
	display.setFont(f);
	display.setCursor(0, 0);
	display.println();
	display.print(name);
	display.println(" !\"#$%&'()*+,-./");
	display.println("0123456789:;<=>?");
	display.println("@ABCDEFGHIJKLMNO");
	display.println("PQRSTUVWXYZ[\\]^_");
	display.println("`abcdefghijklmno");
	display.println("pqrstuvwxyz{|}~ ");
	display.update();
	delay(5000);
}


/** blink the builtin led
 *
 * usage:
 *  pinMode(BUILTIN_LED, OUTPUT);
 *  blink (5, 100);
 *
 * @param nb number of blinks
 * @param wait delay (ms) between blinks
 */
void blink (uint8_t nb, uint32_t wait) {
   // LED: LOW = on, HIGH = off
   for (int i = 0; i < nb; i++)
   {
      digitalWrite(BUILTIN_LED, LOW);
      delay(wait);
      digitalWrite(BUILTIN_LED, HIGH);
      delay(wait);
   }
}
