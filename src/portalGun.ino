// Code to control a Rick and Morty Portal Gun
// Written by Brandon Pomeroy, 2015
// Reworked and move to ShiftDisplay2 by Dimitris Zervas, 2022

#define ENC_BUTTONINTERVAL 50
#define ENC_DECODER (1 << 2)

#include <Wire.h>
#include "Adafruit_GFX.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <ShiftDisplay2.h>

// Set up our LED display
ShiftDisplay2 display(COMMON_ANODE, 4, STATIC_DRIVE);
char displayBuffer[4];
uint8_t dimensionLetter='C';

// Set up the click encoder
ClickEncoder *encoder;
int16_t value, lastValue;
#define encoderPinA          A0
#define encoderPinB          A1
#define encoderButtonPin     A2

// Steps per notch can be 1, 2, 4, or 8. If your encoder is counting
// to fast or too slow, change this!
#define stepsPerNotch        4

// Comment this line to make the encoder increment in the opposite direction
#define reverseEncoderWheel


// FX Board output delay (ms)
const int msDelay = 500;

// Set up the Green LEDs
#define topBulbPin           12
#define frontRightPin        8
#define frontCenterPin       4
#define frontLeftPin         2
#define maximumBright        255
#define mediumBright         127
int topBulbBrightness = 255;

// Set up what we need to sleep/wake the Trinket
// Define the pins you'll use for interrupts - CHANGE THESE to match the input pins
// you are using in your project
#define NAV0_PIN A2

//Let us know if our Trinket woke up from sleep
volatile bool justWokeUp;

void timerIsr() {
	encoder->service();
}

void setup() {
	Serial.begin(9600);
	justWokeUp = false;
	enablePinInterupt(NAV0_PIN);

	//Set up pin modes
	pinMode(topBulbPin, OUTPUT);
	pinMode(frontRightPin, OUTPUT);
	pinMode(frontLeftPin, OUTPUT);
	pinMode(frontCenterPin, OUTPUT);


	digitalWrite(frontRightPin, HIGH);
	digitalWrite(frontLeftPin, HIGH);
	digitalWrite(frontCenterPin, HIGH);
	digitalWrite(topBulbPin, HIGH);

	encoderSetup();
}

void loop() {
	if (justWokeUp) {
		digitalWrite(frontRightPin, HIGH);
		digitalWrite(frontLeftPin, HIGH);
		digitalWrite(frontCenterPin, HIGH);
		digitalWrite(topBulbPin, HIGH);
		justWokeUp = false;
		return;
	}


	ClickEncoder::Button b = encoder->getButton();
	switch (b) {
		case ClickEncoder::Held:
			// Holding the button will put your trinket to sleep.
			// The trinket will wake on the next button press
			display.clear();
			display.set("RICK");
			digitalWrite(frontRightPin, LOW);
			digitalWrite(frontLeftPin, LOW);
			digitalWrite(frontCenterPin, LOW);
			digitalWrite(topBulbPin, LOW);
			display.update();
			delay(2000);
			display.clear();
			display.set("");
			display.update();
			digitalWrite(frontRightPin, HIGH);
			digitalWrite(frontLeftPin, HIGH);
			digitalWrite(frontCenterPin, HIGH);
			digitalWrite(topBulbPin, HIGH);
			justWokeUp = true;
			goToSleep();
		break;
		case ClickEncoder::Clicked:
			// When the encoder wheel is single clicked
			display.clear();
			display.set("-");
			display.update();
			digitalWrite(frontRightPin, LOW);
			delay(100);
			digitalWrite(frontLeftPin, LOW);
			delay(100);
			digitalWrite(frontCenterPin, LOW);
			delay(100);
			digitalWrite(topBulbPin, LOW);
			delay(200);

			digitalWrite(frontRightPin, HIGH);
			digitalWrite(frontLeftPin, HIGH);
			digitalWrite(frontCenterPin, HIGH);
			delay(500);

			display.clear();
			display.set("--");
			display.update();
			digitalWrite(frontRightPin, LOW);
			delay(100);
			digitalWrite(frontLeftPin, LOW);
			delay(100);
			digitalWrite(frontCenterPin, LOW);
			delay(100);
			digitalWrite(topBulbPin, HIGH);
			delay(200);

			digitalWrite(frontRightPin, HIGH);
			digitalWrite(frontLeftPin, HIGH);
			digitalWrite(frontCenterPin, HIGH);
			delay(500);

			display.clear();
			display.set("---");
			display.update();
			digitalWrite(frontRightPin, LOW);
			delay(100);
			digitalWrite(frontLeftPin, LOW);
			delay(100);
			digitalWrite(frontCenterPin, LOW);
			delay(100);
			digitalWrite(topBulbPin, LOW);
			display.clear();
			display.set("----");
			display.update();
			delay(1000);

			digitalWrite(frontRightPin, HIGH);
			digitalWrite(frontLeftPin, HIGH);
			digitalWrite(frontCenterPin, HIGH);
			digitalWrite(topBulbPin, HIGH);
			display.set("oooo");
			display.update();
		break;
		case ClickEncoder::DoubleClicked:
			//If you double click the button, it sets the dimension to C137
			dimensionLetter = 'C';
			value = 137;
		break;
		case ClickEncoder::Open:
			// The dimension will increment from 0-999, then roll over to the next
			// letter. (A999 -> B000)
			lastValue = updateDimension();
		break;
		default:
		break;
	}
}


void encoderSetup() {
	// set up encoder
	encoder = new ClickEncoder(encoderPinA, encoderPinB, encoderButtonPin, stepsPerNotch);
	encoder->setAccelerationEnabled(true);

	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerIsr);
	lastValue = -1;
	value = 137;
}


int16_t updateDimension() {
	#ifdef reverseEncoderWheel
		value -= encoder->getValue();
	#endif

	#ifndef reverseEncoderWheel
		value += encoder->getValue();
	#endif

	if (value != lastValue) {
		if (value > 999) {
			value = 0;
			if (dimensionLetter == 'Z') {
				dimensionLetter = 'A';
			} else {
				dimensionLetter++;
			}
		} else if (value < 0) {
			value = 999;
			if (dimensionLetter == 'A') {
				dimensionLetter = 'Z';
			} else {
				dimensionLetter--;
			}
		}
		Serial.print("Updating dimension ");
		Serial.print(lastValue);
		Serial.print(" ");
		Serial.println(value);

		sprintf(displayBuffer, "%c%03i", dimensionLetter, value);
		// display.clear();
		display.set(displayBuffer);
		display.update();
		Serial.println("Updated");
	}

	return value;
}


/*
============== Sleep/Wake Methods ==================
====================================================
*/

// Most of this code comes from seanahrens on the adafruit forums
// http://forums.adafruit.com/viewtopic.php?f=25&t=59392#p329418


void enablePinInterupt(byte pin) {
	*digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin)); // enable pin
	PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void goToSleep() {
	// The ATmega328 has five different sleep states.
	// See the ATmega 328 datasheet for more information.
	// SLEEP_MODE_IDLE -the least power savings
	// SLEEP_MODE_ADC
	// SLEEP_MODE_PWR_SAVE
	// SLEEP_MODE_STANDBY
	// SLEEP_MODE_PWR_DOWN - the most power savings
	// I am using the deepest sleep mode from which a
	// watchdog timer interrupt can wake the ATMega328




	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode.
	sleep_enable(); // Enable sleep mode.
	sleep_mode(); // Enter sleep mode.
	// After waking the code continues
	// to execute from this point.

	sleep_disable(); // Disable sleep mode after waking.
	lastValue = -1;
}

ISR (PCINT0_vect) { // handle pin change interrupt for D8 to D13 here
	// if I wired up D8-D13 then I'd need some code here
}

ISR (PCINT1_vect) { // handle pin change interrupt for A0 to A5 here // NAV0
	/* This will bring us back from sleep. */

	/* We detach the interrupt to stop it from
	 * continuously firing while the interrupt pin
	 * is low.
	*/

	detachInterrupt(0);
}

ISR (PCINT2_vect) { // handle pin change interrupt for D0 to D7 here // NAV1, NAV2
	// Check it was NAV1 or NAV2 and nothing else
}
