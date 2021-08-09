/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                      PLEASE READ THE FOLLOWING!                                                         *
 *                               This open source software was written by Liam Price 2020 and is FREE to use...                            *
 *                                                                                                                                         *
 * The purpose of this software is to compile and run on an Arduino based IoT ESP8266 device, like a Wemos D1 Mini. This robot simply      *
 * draws text on a piece of paper using 3 servo motors and a pencil. I built two models of this robot as gifts for friends. The robot can  *
 * automatically draw text when turned on and it can also be remotely controlled from a web browser via MQTT. Full details of how I        *
 * designed and constructed the robot can be found on my GitHub page:                                                                      *
 * https://github.com/LeehamElectronics/IoT-Draw-Bot                                                                                       *
 *                                                                                                                                         *
 * If you'd like to support my open source work, buy me a coffee:                                                                          *
 * https://www.paypal.com/paypalme/liamproice/                                                                                             *
 *                                                                                                                                         *
 * Check out my website and portfolio at https://ldprice.com                                                                               *
 *                                                                                                                                         *
 * And if you need help, feel free to email me at liam@ldprice.com                                                                         *
 *                                                                                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

 /*
 ██╗░░░░░██████╗░██████╗░██████╗░██╗░█████╗░███████╗     ░██████╗░█████╗░███████╗████████╗░██╗░░░░░░░██╗░█████╗░██████╗░███████╗
 ██║░░░░░██╔══██╗██╔══██╗██╔══██╗██║██╔══██╗██╔════╝     ██╔════╝██╔══██╗██╔════╝╚══██╔══╝░██║░░██╗░░██║██╔══██╗██╔══██╗██╔════╝
 ██║░░░░░██║░░██║██████╔╝██████╔╝██║██║░░╚═╝█████╗░░     ╚█████╗░██║░░██║█████╗░░░░░██║░░░░╚██╗████╗██╔╝███████║██████╔╝█████╗░░
 ██║░░░░░██║░░██║██╔═══╝░██╔══██╗██║██║░░██╗██╔══╝░░     ░╚═══██╗██║░░██║██╔══╝░░░░░██║░░░░░████╔═████║░██╔══██║██╔══██╗██╔══╝░░
 ███████╗██████╔╝██║░░░░░██║░░██║██║╚█████╔╝███████╗     ██████╔╝╚█████╔╝██║░░░░░░░░██║░░░░░╚██╔╝░╚██╔╝░██║░░██║██║░░██║███████╗
 ╚══════╝╚═════╝░╚═╝░░░░░╚═╝░░╚═╝╚═╝░╚════╝░╚══════╝     ╚═════╝░░╚════╝░╚═╝░░░░░░░░╚═╝░░░░░░╚═╝░░░╚═╝░░╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝
 */

 /* ---------------------------------------------------------------------------------------------------------------------------
 --------------------------------------------------    Include Libraries   ----------------------------------------------------
 --------------------------------------------------------------------------------------------------------------------------- */
#include "include/drawBotConfiguration.h" // general GPIO config ect...
#include "include/mqttConfiguration.h" // mqtt configuration, you need to edit this for your own use case...
#include <Servo.h> // should be included with Arduino.h ? You may need a custom lib if using ESP32
#include <EEPROM.h> // we use this to save data into the chips memory during run time
#include <ESP8266WiFi.h> // library for ESP8266 based chips, ESP32 requires a diff lib
#include <DNSServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h> // mqtt client

/* Create main objects */
#pragma region
/* Networking related objects (MQTT / WiFi) */
WiFiClient espClient;
WiFiManager wifiManager;
PubSubClient client(espClient);
IPAddress dns_server(8, 8, 8, 8);  // Google DNS Server IP...  

/* Create servo objects */
Servo leftMotorServo;
Servo rightMotorServo;
Servo penActuatorServo;
#pragma endregion


/* Declare global variables */
#pragma region
// firebase input placeholders
float movement = 0;
float previous = 0;
float servoAngleDown = 0;
float servoAngleUp = 0;
float servoAngleDownFB = 0;
float servoAngleUpFB = 0;

/* Variables for anaylysing the input string from firebase */
String customStringForDrawing;
int spacePosition = 0;
int countVal = 1;
int finishedVal = 0;
int previousCountVal = 1;

/* EEPROM Setup */
int rightRotationEfficiencyAddr = 0;
int leftRotationEfficiencyAddr = 1;
int penActuatorDownAngleAddr = 2;
int penActuatorUpAngleAddr = 3;
float eepomValueDivider = 10; //we use this number to divide the value stored in eepom because eeprom cant store floats unfortuantly
float wheelRotationEfficiency = 0; //multiplying factor for calibrating the rotation movement      2000ms time for it to rotate 360 degress, so I divided 2000 by 360 and got 5.55         6.5
float wheelFowardEfficiency = 0; //multiplying factor for going foward for x amount of seconds  was 128.2         96.5
float angleRotationValue = 0; //just a varibale for passing value into the rotate function, no need to change this value as it does nothing.
float fowardMovementValue = 0; //same as angleRotationValue variable but for going in  straight line

/* I wrote this part so long ago I don't even know what it's for so yeh sorry lol */
int forwardLeft = 180;
int forwardRight = 1;
float forwardRightS = 1.4; //increase makes it go more the right, decrease goes to the left  91.4
float forwardRightSB = 150.4;
#pragma endregion


/* Setup function, this runs once when the Robot starts up */
void setup() {
    Serial.begin(9600); //start the serial monitor for debugging
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    setLedRed();

    wifiManager.setAPCallback(configModeCallback);

    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);

    Serial.println("Connecting to WiFi...");
    /* If no known WiFi networks are found, enter config AP portal mode... */
    if (!wifiManager.autoConnect("DrawBot WiFi Connect", WIFI_PORTAL_PASS)) {
        Serial.println("failed to connect and hit timeout");
        /* Reset and try again */
        ESP.restart();
        delay(1000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    /* Begin reading from EEPROM memory */
    EEPROM.begin(512);
    wheelRotationEfficiency = EEPROM.read(rightRotationEfficiencyAddr) / eepomValueDivider;
    wheelFowardEfficiency = EEPROM.read(leftRotationEfficiencyAddr); 
    servoAngleDown = EEPROM.read(penActuatorDownAngleAddr);
    servoAngleUp = EEPROM.read(penActuatorUpAngleAddr);

    /* Initialize Pen Actuator */
    penActuatorServo.attach(PEN_ACTUATOR_MOTOR_PIN); 
    penActuatorServo.write(69);

    /* Print values to serial monitor for debugging */
    Serial.print("FE Val found in eeprom is: ");
    Serial.println(wheelFowardEfficiency);
    Serial.print("RE Val found in eeprom is: ");
    Serial.println(wheelRotationEfficiency);
}


/* Loop functions runs continuously */
void loop() {
    if (!client.connected()) {
        Serial.println("Not connected to MQTT, trying to connect to broker...");
        reconnect();
    }
    else {
        client.loop();
    }


    /* TODO: Convert the following code from Firebase to using MQTT server. */

    movement = 5; // this comes form MQTT callback func
    
    /* If the incoming data is "forward", run the "MotorForward" function */
    if (movement != previous) {
        if (movement == 1) {
            previous = movement;
            forward();
            setLedGreen();
        }
        /* If the incoming data is "backward", run the "MotorBackward" function */
        else if (movement == 2) {
            previous = movement;
            motorBackward();
            setLedTest1();
        }
        /* If the incoming data is "left", run the "TurnLeft" function */
        else if (movement == 3) {
            previous = movement;
            left();
            setLedTest2();
        }
        /* If the incoming data is "right", run the "TurnRight" function */
        else if (movement == 4) {
            previous = movement;
            right();
            setLedTest3();
        }
        /* If the incoming data is "stop", run the "motorStop" function */
        else if (movement == 0) {
            previous = movement;
            motorStop();
            setLedRed();
        }
        else if (movement == 5) {
            previous = movement;
            raisePen();
        }
        else if (movement == 6) {
            previous = movement;
            lowerPen();
        }
        else if (movement == 9) {
            previous = movement;
            finishedVal = 0; //printing will begin when drawCustomString() is started, finsihedVal will go back to "1" when print is done
            drawCustomString();
        }
        else if (movement == 10) {//this is the return value for when you need to press a button twice or more!
            previous = movement;
        }
        else if (movement == 13) {
            previous = movement;
            Serial.print("Calibration button hit");
            printCalibrate();
        }
        else if (movement == 20) {//now we check for an update in the RE Value
            previous = movement;
            Serial.print("new RE Val found");
            int REFB = 4; // Firebase.getFloat("RE/REVal");
            Serial.print(REFB);
            EEPROM.write(rightRotationEfficiencyAddr, REFB);
            wheelRotationEfficiency = EEPROM.read(rightRotationEfficiencyAddr) / eepomValueDivider;
            Serial.print("eeprom says RE: ");
            Serial.println(wheelRotationEfficiency); //print what value has been read from eeprom for diagnostics
            EEPROM.commit();
        }
        else if (movement == 21) {//now we check for an update in the FE Value
            previous = movement;
            Serial.print("new FE Val found");
            int FEFB = 4; //Firebase.getFloat("FE/FEVal");
            Serial.print(FEFB);
            EEPROM.write(leftRotationEfficiencyAddr, FEFB);
            wheelFowardEfficiency = EEPROM.read(leftRotationEfficiencyAddr);
            Serial.print("eeprom says FE: ");
            Serial.println(wheelFowardEfficiency); //print what value has been read from eeprom for diagnostics
            EEPROM.commit();
        }
        else if (movement == 11) {//now we check for an update in the servoangleup value
            previous = movement;
            Serial.print("new servoAngleDownFB Val found");
            servoAngleDownFB = 4; //Firebase.getFloat("pencilDownAngle/servoAngle");
            Serial.print(servoAngleDownFB);
            EEPROM.write(penActuatorDownAngleAddr, servoAngleDownFB);
            servoAngleDown = EEPROM.read(penActuatorDownAngleAddr);
            Serial.print("eeprom says FE: ");
            Serial.println(servoAngleDown); //print what value has been read from eeprom for diagnostics
            EEPROM.commit();
        }
        else if (movement == 12) {//now we check for an update in the servoangledown value
            previous = movement;
            Serial.print("new servoAngleUp Val found");
            servoAngleUpFB = 4; //Firebase.getFloat("pencilUpAngle/servoAngle");
            Serial.print(servoAngleUpFB);
            EEPROM.write(penActuatorUpAngleAddr, servoAngleUpFB);
            servoAngleUp = EEPROM.read(penActuatorUpAngleAddr);
            Serial.print("eeprom says FE: ");
            Serial.println(servoAngleUp); //print what value has been read from eeprom for diagnostics
            EEPROM.commit();
        }
        else if (movement == 30) {
            previous = movement;
            printDate();
        }
    }
}


/*******************************************************************************
* RAW movement functions only activate motors to move in a specific direction, * 
* the advanced movement functions actually control the amount of time these    *
* functions run for in order to produce an accurate print                      *
********************************************************************************/
#pragma region
void forward(void)
{
    leftMotorServo.attach(LEFT_MOTOR_PIN);  
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    leftMotorServo.write(forwardLeft);
    rightMotorServo.write(forwardRightS);
    Serial.print("forward");
}

void motorBackward(void)
{
    leftMotorServo.attach(LEFT_MOTOR_PIN); 
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    rightMotorServo.write(forwardRightSB);
    leftMotorServo.write(forwardRight);
    Serial.print("BACK");
}

void left(void)
{
    leftMotorServo.attach(LEFT_MOTOR_PIN);  
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    rightMotorServo.write(forwardRight);
    leftMotorServo.write(forwardRight);

    Serial.print("left");
}

void right(void)
{
    leftMotorServo.attach(LEFT_MOTOR_PIN);  
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    leftMotorServo.write(forwardLeft);
    rightMotorServo.write(forwardLeft);
    Serial.print("right");
}

void motorStop(void)
{
    leftMotorServo.detach();
    rightMotorServo.detach();
    penActuatorServo.detach(); 
    Serial.print("stop");
}

void raisePen(void)
{
    penActuatorServo.attach(PEN_ACTUATOR_MOTOR_PIN);
    penActuatorServo.write(servoAngleUp);
}

void lowerPen(void)
{
    penActuatorServo.attach(PEN_ACTUATOR_MOTOR_PIN); 
    penActuatorServo.write(servoAngleDown);
}
#pragma endregion


/*****************************************************************
* These function control the color of the front LED on the robot * 
******************************************************************/
#pragma region
void setLedRed(void)
{
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
}

void setLedGreen(void)
{
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
}

void setLedBlue(void)
{
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
}


void setLedTest1(void)
{
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
}

void setLedTest2(void)
{
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, HIGH);
}

void setLedTest3(void)
{
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
}
#pragma endregion


/*******************************************************************************
* Predefined movement instructions for each ASCII character are below here.    *
* Each function speficies how a character is drawn by the robot                *
* More characters need to be added in the future.                              *
********************************************************************************/
#pragma region
void print0(void)
{
    int fin2 = 44;
    goForward(4);
    rotateRight(90);
    goForward(.6);
    lowerPen();
    while (fin2 > 1) {
        fin2 = fin2 - 1;
        goForward(.3);
        rotateRight(8.8);
        Serial.print("cycle");
    }
}
void print1(void)
{
    lowerPen();
    goForward(5.5);
    raisePen();
    goBack(5.5);
    rotateRight(90);
    goForward(1);
    rotateLeft(90);
}

void print2(void)
{
    lowerPen();
    rotateRight(90);
    goForward(.4);
    rotateRight(7.2);
}

void print2pls(void)
{
    int fin2 = 22;
    lowerPen();
    rotateRight(90);
    while (fin2 > 1) {
        fin2 = fin2 - 1;
        goForward(.3);
        rotateRight(8.8);
        Serial.print("cycle");
    }
    goBack(3.4);
    raisePen();
    goBack(.9);
    rotateRight(90);
}

void print2B(void)
{
    int fin2 = 22;

    raisePen();
    goForward(3.9);
    lowerPen();
    rotateRight(90);
    while (fin2 > 1) {
        fin2 = fin2 - 1;
        goForward(.3);
        rotateRight(8.8);
        Serial.print("cycle");
    }
    goBack(3.4);
    raisePen();
    goBack(.9);
    rotateRight(90);
}

void print3(void)
{
    goForward(4);
    lowerPen();
    rotateRight(180);
    raisePen();
    goForward(3.2);
    lowerPen();
    rotateLeft(180);
    raisePen();
    goBack(3.2);
    rotateRight(90);
    goForward(.9);
    rotateLeft(90);
}

void print4(void)
{
    Serial.print("printing 4");
}

void print9(void)
{
    rotateRight(90);
    goForward(3);
    rotateLeft(90);
    goBack(1.5);
    lowerPen();
    goForward(5.5);
    rotateLeft(180);
    raisePen();
    rotateRight(180);
    goBack(5.5);
    rotateRight(90);
    goForward(1);
    rotateLeft(90);
}

void printSlash(void)
{
    rotateRight(25);
    goBack(1);
    lowerPen();
    goForward(5.5);
    raisePen();
    goBack(5.5);
    rotateRight(73);
    goForward(3.2);
    rotateLeft(90);
}
#pragma endregion


/* ---------------------------------------------------------------------------------------------------------------------------
--------------------------------------------    Hardware Interaction Functions   ---------------------------------------------
-- All movement functions take one required argument which specifies how many milimeters the movement event should occur for -
--------------------------------------------------------------------------------------------------------------------------- */
#pragma region
void rotateLeft(float sec)
{
    angleRotationValue = sec * wheelRotationEfficiency;
    leftMotorServo.attach(LEFT_MOTOR_PIN);
    rightMotorServo.attach(RIGHT_MOTOR_PIN);
    rightMotorServo.write(forwardRight);
    leftMotorServo.write(forwardRight);
    delay(angleRotationValue);
}

void rotateRight(float sec)
{
    angleRotationValue = sec * wheelRotationEfficiency;
    leftMotorServo.attach(LEFT_MOTOR_PIN);
    rightMotorServo.attach(RIGHT_MOTOR_PIN);
    leftMotorServo.write(forwardLeft);
    rightMotorServo.write(forwardLeft);
    delay(angleRotationValue);
}

void goForward(float sec)
{
    fowardMovementValue = sec * wheelFowardEfficiency;
    leftMotorServo.attach(LEFT_MOTOR_PIN); 
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    leftMotorServo.write(forwardLeft);
    rightMotorServo.write(forwardRight);
    delay(fowardMovementValue);
}

void goBack(float sec)
{
    fowardMovementValue = sec * wheelFowardEfficiency;
    leftMotorServo.attach(LEFT_MOTOR_PIN); 
    rightMotorServo.attach(RIGHT_MOTOR_PIN); 
    rightMotorServo.write(forwardLeft);
    leftMotorServo.write(forwardRight);
    delay(fowardMovementValue);
}
#pragma endregion


/* Predefined drawing functions define drawings such as the date, calibration testing, and custom prints */
#pragma region

/* Predefined significant date to be drawn for the recipient of the robot gift, could be a wedding gift, baptism gift, ect...*/
void printDate(void)
{
    print2pls();  // 2
    print2B();  // 2
    printSlash(); // /
    print2B();  // 2
    printSlash(); // /
    print2B();  // 2
    print0();     // 0
    motorStop();  // STOP
}

/* This function tests a predefined value on the printer mech to test for accuracy with a protractor or similar tool. */
void printCalibrate(void)
{
    Serial.print("Calibration Test");
    raisePen();
    rotateRight(360);
    lowerPen();
    goForward(3);
    motorStop();
}

/* This function will make the robot draw a custom string that is recieved from user */
void drawCustomString(void)
{
    customStringForDrawing = "sddf";//Firebase.getString("letterString/customStringForDrawing");
    spacePosition = customStringForDrawing.indexOf('!');
    while (finishedVal == 0) {
        previousCountVal = countVal;
        if (customStringForDrawing.charAt(spacePosition + countVal) == '1') {
            Serial.println("1");
            countVal = countVal + 1;
            print1();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '2') {
            Serial.println("2");
            countVal = countVal + 1;
            print2();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '3') {
            Serial.println("3");
            countVal = countVal + 1;
            print3();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '4') {
            Serial.println("4");
            countVal = countVal + 1;
            print4();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '9') {
            Serial.println("9");
            countVal = countVal + 1;
            print9();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '/') {
            Serial.println("/");
            countVal = countVal + 1;
            printSlash();
        }
        if (customStringForDrawing.charAt(spacePosition + countVal) == '!') {
            Serial.println("print finished!");
            countVal = 1;
            finishedVal = 1;
            motorStop();
        }
        if (countVal == previousCountVal) {
            Serial.println("Read Failure!");
            countVal = 1;
            finishedVal = 1;
            motorStop();
        }
    }
}
#pragma endregion


/* ---------------------------------------------------------------------------------------------------------------------------
--------------------------------------    Networking Callback and Reconnect Functions   --------------------------------------
--------------------------------------------------------------------------------------------------------------------------- */
#pragma region
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Topic is:");
    Serial.println(topic);
    if (strcmp(topic, "drawbot/in") == 0) {
        Serial.println("msg recieved");
        // do stuff here
    }
    else {
        Serial.print("error: topic not relevant");
    }
}

/* This function runs when we are trying to connect to the MQTT server */
void reconnect() {
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("drawBot", MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected to mqtt server");
            client.subscribe("drawbot/in");
            Serial.println("MQTT Connected!");
            delay(2000);
        }
        else {
            Serial.print("failed to connect to MQTT! rc=");
            Serial.print(client.state());
  
            Serial.println("try again in 1 seconds");

            // Wait 2 seconds before retrying
            delay(3000);
        }
    }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());
}
#pragma endregion
