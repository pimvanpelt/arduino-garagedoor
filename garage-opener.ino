
/* Simple program that listens to two inputs, a pushbutton (BUTTON_PIN) and a
 * fingerprint sensor such as http://www.adafruit.com/products/751
 * If either is triggered, a relais (RELAIS_PIN) is pulled for a small amount
 * of time. There are some two-colored (red/green) LEDs that show the program
 * state. Some logging happens on Serial.
 * 
 * Used in my house as a garage opener (fingerprint scanner exterior, pushbutton
 * and status LEDs interior). 
 * 
 * Date: July 2016
 * Author: Pim van Pelt <pim@ipng.nl>
 */
 
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int useFingerprint = 0;

#define RELAIS_PIN 7
#define BUTTON_PIN 8
#define BOTTOMGREEN_PIN 9
#define BOTTOMRED_PIN 10
#define TOPGREEN_PIN 11
#define TOPRED_PIN 12

#define LED_TOP 0
#define LED_BOTTOM 1
#define COLOR_NONE 0
#define COLOR_RED 1
#define COLOR_GREEN 2

void pinModeOutPutDefault(int pin, int state)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinModeOutPutDefault(RELAIS_PIN, HIGH);
  pinModeOutPutDefault(TOPRED_PIN, LOW);
  pinModeOutPutDefault(BOTTOMRED_PIN, LOW);
  pinModeOutPutDefault(TOPGREEN_PIN, LOW);
  pinModeOutPutDefault(BOTTOMGREEN_PIN, LOW);

  while (!Serial);  
  Serial.begin(9600);
  
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("setup(): Found fingerprint sensor!");
    useFingerprint=1;
  } else {
    Serial.println("setup(): Did not find fingerprint sensor :(");
  }
}

void setLed(int pos, int col)
{
  if (pos == LED_TOP) {
    if (col == COLOR_RED) {
      digitalWrite(TOPRED_PIN, HIGH);
      digitalWrite(TOPGREEN_PIN, LOW);
    } else if (col == COLOR_GREEN) {
      digitalWrite(TOPRED_PIN, LOW);
      digitalWrite(TOPGREEN_PIN, HIGH);
    } else {
      digitalWrite(TOPRED_PIN, LOW);
      digitalWrite(TOPGREEN_PIN, LOW);
    }
  }
  if (pos == LED_BOTTOM) {
    if (col == COLOR_RED) {
      digitalWrite(BOTTOMRED_PIN, HIGH);
      digitalWrite(BOTTOMGREEN_PIN, LOW);
    } else if (col == COLOR_GREEN) {
      digitalWrite(BOTTOMRED_PIN, LOW);
      digitalWrite(BOTTOMGREEN_PIN, HIGH);
    } else {
      digitalWrite(BOTTOMRED_PIN, LOW);
      digitalWrite(BOTTOMGREEN_PIN, LOW);
    }
  }
}

void pullRelais()
{
  Serial.println("pullRelais(): actuating relais");
  setLed(LED_BOTTOM, COLOR_GREEN);
  digitalWrite(RELAIS_PIN, LOW);
  delay(250);
  digitalWrite(RELAIS_PIN, HIGH);
  setLed(LED_BOTTOM, COLOR_NONE);
}

// Returns 0 if the sensor matched a fingerprint.
// Returns non-zero otherwise (upon either error, or invalid fingerprint found).
uint8_t getFingerprintID() {
  // If the fingerprint scanner was not detected, or failed, just skip over
  if (useFingerprint != 1)
    return FINGERPRINT_NOFINGER;
    
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("getFingerprintID(): Image taken");
      setLed(LED_BOTTOM, COLOR_NONE);
      break;
    case FINGERPRINT_NOFINGER:
      // Serial.println("getFingerprintID(): No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("getFingerprintID(): Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("getFingerprintID(): Imaging error");
      return p;
    default:
      Serial.println("getFingerprintID(): getImage: Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("getFingerprintID(): Image converted");
      setLed(LED_BOTTOM, COLOR_RED);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("getFingerprintID(): Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("getFingerprintID(): Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("getFingerprintID(): Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("getFingerprintID(): Invalid image");
      return p;
    default:
      Serial.println("getFingerprintID(): image2Tz: Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("getFingerprintID(): Found a print match");
    setLed(LED_BOTTOM, COLOR_GREEN);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("getFingerprintID(): Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("getFingerprintID(): Did not find a match");
    return p;
  } else {
    Serial.println("getFingerprintID(): fingerFastSearch: Unknown error");
    return p;
  }   
  
  // found a match!
  return 0;
}

// Returns 1 if the button was pressed for 500ms
// Returns 2 if the fingerprint sensor was activated successfully
// Returns 0 otherwise.
uint8_t getButtonPress_or_FingerprintID()
{
  int ret; 
    
  if (LOW == digitalRead(BUTTON_PIN)) {
    Serial.println("getButtonPress_or_FingerprintID(): Button down initially");
    setLed(LED_BOTTOM, COLOR_RED);
    delay(500);
    if (LOW == digitalRead(BUTTON_PIN)) {
      Serial.println("getButtonPress_or_FingerprintID(): Button still down, returning");
      return 1;
    }
  }
  ret = getFingerprintID();
  if (0 == ret) {
    Serial.print("getButtonPress_or_FingerprintID(): Found print ID #"); Serial.print(finger.fingerID); 
    Serial.print(" with confidence of "); Serial.println(finger.confidence); 
    return 2;
  } else if (ret != FINGERPRINT_NOFINGER) {
    for (int i=0; i<4; i++) {
      setLed(LED_BOTTOM, COLOR_RED);
      delay(250);
      setLed(LED_BOTTOM, COLOR_NONE);
      delay(250);
    }
  }
  return 0;
}

void loop() {
 setLed(LED_TOP, COLOR_RED);
 setLed(LED_BOTTOM, COLOR_NONE);
 
 if (0 != getButtonPress_or_FingerprintID()) {
  setLed(LED_TOP, COLOR_GREEN);
  pullRelais();
  delay(1750);
 }
 delay(150);
}
