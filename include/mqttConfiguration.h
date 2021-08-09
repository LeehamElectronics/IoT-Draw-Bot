// drawBotConfiguration.h

#ifndef _DRAWBOTCONFIGURATION_h
#define _DRAWBOTCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#endif

#define MQTT_SERVER "your IP address or domain name"
#define MQTT_PORT 1883  // Could be anything
#define MQTT_USERNAME "username_here"
#define MQTT_PASSWORD "password_here"  // Use a strong password, or don't