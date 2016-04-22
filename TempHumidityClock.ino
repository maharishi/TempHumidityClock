#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "dht11.h"

#define LCD_ADDR 0x3F // <<- LCD Mem address
#define CLOCK_ADDR 0x68 // <<- CLOCK Mem address
#define CLOCK_TEMP_ADDR 0x57 // <<- CLOCK Temperature Sensor Mem address
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define DHT11PIN 2

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
LiquidCrystal_I2C lcd(LCD_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
dht11 DHT11;

void setup() {
	Serial.begin(9600);
	Wire.begin();
	//setDS3231time(0, 49, 23, 5, 21, 4, 16);
	lcd.begin(20, 4); // <<-- LCD is a 20x4, change for your LCD if needed

	// LCD Backlight ON
	lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
	lcd.setBacklight(HIGH);

	lcd.home(); // go home on LCD
	lcd.print("NYT TIME         t C");
	lcd.setCursor(0, 2); // go to start of 3nd line
	lcd.print("IST TIME          H%");
	readDS3231temp();
}

void loop() {
	// retrieve data from DS3231
	readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
	
	lcd.setCursor(0, 1); // go to start of 2nd line
	if (dayOfMonth < 10) {
		lcd.print("0");
	}
	lcd.print(dayOfMonth);
	lcd.print(" ");
	lcd.print(month_name(month));
	lcd.print(" ");
	lcd.print(year);
	lcd.print(" ");
	if (hour < 10) {
		lcd.print("0");
	}
	lcd.print(hour);
	lcd.print(":");
	if (minute < 10) {
		lcd.print("0");
	}
	lcd.print(minute);
	lcd.print("   ");
	//lcd.print(readDS3231temp());

	int chk = DHT11.read(DHT11PIN);

	switch (chk)
	{
	case DHTLIB_OK:
		//Serial.println("OK");
		break;
	case DHTLIB_ERROR_CHECKSUM:
		Serial.println("Checksum error");
		break;
	case DHTLIB_ERROR_TIMEOUT:
		Serial.println("Time out error");
		break;
	default:
		Serial.println("Unknown error");
		break;
	}
	/*Serial.print("Humidity (%): ");
	Serial.println(DHT11.humidity);

	Serial.print("Temperature (°C): ");
	Serial.println(DHT11.temperature);*/

	lcd.print(DHT11.temperature);
	lcd.setCursor(18, 3); // go to 18th char of 2nd line
	lcd.print(DHT11.humidity);
	
	delay(1000);
}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
	return((val / 10 * 16) + (val % 10));
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
	return((val / 16 * 10) + (val % 16));
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
	dayOfMonth, byte month, byte year)
{
	// sets time and date data to DS3231
	Wire.beginTransmission(CLOCK_ADDR);
	Wire.write(0); // set next input to start at the seconds register
	Wire.write(decToBcd(second)); // set seconds
	Wire.write(decToBcd(minute)); // set minutes
	Wire.write(decToBcd(hour)); // set hours
	Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
	Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
	Wire.write(decToBcd(month)); // set month
	Wire.write(decToBcd(year)); // set year (0 to 99)
	Wire.endTransmission();
}

void readDS3231time(byte *second,
	byte *minute,
	byte *hour,
	byte *dayOfWeek,
	byte *dayOfMonth,
	byte *month,
	byte *year)
{
	Wire.beginTransmission(CLOCK_ADDR);
	Wire.write(0); // set DS3231 register pointer to 00h
	Wire.endTransmission();
	Wire.requestFrom(CLOCK_ADDR, 7);
	// request seven bytes of data from DS3231 starting from register 00h
	*second = bcdToDec(Wire.read() & 0x7f);
	*minute = bcdToDec(Wire.read());
	*hour = bcdToDec(Wire.read() & 0x3f);
	*dayOfWeek = bcdToDec(Wire.read());
	*dayOfMonth = bcdToDec(Wire.read());
	*month = bcdToDec(Wire.read());
	*year = bcdToDec(Wire.read());
}

int readDS3231temp()
{
	Wire.beginTransmission(CLOCK_ADDR);
	Wire.write(0x11); // set DS3231 register pointer to 00h
	Wire.endTransmission();
	Wire.requestFrom(CLOCK_ADDR,1);
	byte _msb = bcdToDec(Wire.read());
	return _msb;
}

char* month_name(int month) {
	char *output = "xxx";
	char *monthShort[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
	output = monthShort[month - 1];
	return output;
}