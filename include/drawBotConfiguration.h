// DrawBotConfiguration.h

#ifndef _DRAWBOTCONFIGURATION_h
#define _DRAWBOTCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

/* GPIO Pinout Menu for Electrical Components */
#define RED_PIN D2
#define GREEN_PIN D3
#define BLUE_PIN D4

#define LEFT_MOTOR_PIN D5
#define RIGHT_MOTOR_PIN D6
#define PEN_ACTUATOR_MOTOR_PIN D7

/* General configuration */
#define WIFI_PORTAL_PASS "WIFI_PORTAL_PASS" // This is the password that you will use to connect to the robot to configure it.

