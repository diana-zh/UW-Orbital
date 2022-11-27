#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//buzzer pin
int buzzerPin = 7;

//servo
Servo myServo;
int servoPin = 9;
int servoPos = 90;
int servoAdd = 1;

//hall pin
int hallPin = 2;            // Changed hallPin from volatile int to int (pin doesn't need to be changed, as 'volatile' suggests)
volatile int hallState;

// "ok" push button
int okButton = 10;
int okButtonState;

//potentiometer pin
int potPin = A0;
int potState;

//motor relay
int motorPin = 3;

int totalWinding;
volatile int windCount = 0;
int mappedPotState;

LiquidCrystal_I2C lcd (0x27, 16, 2);

void setup() {
  // Tell the Arduino to get ready to exchange messages with the Serial Monitor at a data rate of 9600 bits per second.
  Serial.begin(9600);                       
  //Configures specified pin to behave either as input or output.
  pinMode (okButton, INPUT);
  pinMode (hallPin, INPUT);
  pinMode (buzzerPin, OUTPUT);
  pinMode (motorPin, OUTPUT);
  // Attach servo variable to a pin
  myServo.attach(servoPin);

  // Read value from specified digital pin, either HIGH or LOW
  okButtonState = digitalRead (okButton);

  //lcd related stuff
  lcd.init();         // Initialize the lcd
  lcd.backlight();    // Open the backlight

  introFunction();    // Print credits and product name
  promtFunction();    // Prompt user for weight
  promptFunction();


  lcd.clear ();
  windCalc();         // Calculate and print total amount of windings
  lcd.clear();
  countDown();        // Countdown to winding start
  lcd.clear();
  // Continuously update windCounter based on signal from Hall sensor
  attachInterrupt (digitalPinToInterrupt(hallPin), windCounter, FALLING);

  // motorSpinFunction();  // WTF DOES THIS DO????? Doesn't actual winding happen in loop()?
}

void loop() {
  hallState = digitalRead(hallPin);   // Read value from Hall sensor
  Serial.println (hallState);         // Print data to serial port as human-readable ASCII text
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print ("Windings: ");
  lcd.print (windCount);

  if (servoPos == 135 || servoPos == 40) {
    servoAdd = -servoAdd;
  }
  servoPos = servoPos + servoAdd;
  myServo.write (servoPos);           // Control servo shaft (set angle for standard servo, set speed for continuous rotation servo)

  // Winding complete, notify user with buzzer and message
  while (windCount >= totalWinding) {
    //digitalWrite (motorPin,0);
    digitalWrite(buzzerPin, HIGH);
    delay (500);
    digitalWrite(buzzerPin, LOW);
    delay (500);
    switchOnFunction();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print ("Winding Complete");
    lcd.setCursor(0, 1);
    lcd.print ("Reset --> restart");
    delay (1000);
    //myServo.write (0);
  }
}

// Prompt user for weight
void promtFunction() {
  lcd.setCursor(0, 0);
  lcd.print ("Enter Weight: ");
  lcd.setCursor(0, 1);
  lcd.print ("Ok to confirm");
  delay (1000);
}

// Essentially printing who made this
void introFunction () {
  lcd.setCursor (0, 0);
  lcd.print("Coil Winder");
  lcd.setCursor (0, 1);
  lcd.print ("A Prabal Product");
  delay (3000);
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print ("  Developed by  ");
  lcd.setCursor(0, 1);
  lcd.print ("   Team Prabal   ");
  delay(2000);
  lcd.clear();
}

// 
int promptFunction() {
  while (okButtonState == HIGH) {
    // Reads value from specified analog pin (potPin == A0, potentiometer pin)
    potState = analogRead (potPin);
    // Re-map function from range 0-1023 to 0-50
    mappedPotState = map(potState, 0, 1023, 0, 50);
    lcd.setCursor(14, 0);
    lcd.print (mappedPotState);
    delay(500);
    // Exit promptFunction if button is released
    if (digitalRead(okButton) == LOW) {
      break;
    }
  }
}

// Calculate and print total amount of windings
int windCalc() {
  lcd.setCursor(0, 0);
  lcd.print ("Chosen wght=");
  lcd.print (mappedPotState);
  lcd.print("g");
  totalWinding = mappedPotState * 46.666666666;
  lcd.setCursor (0, 1);
  lcd.print ("Windings:");
  lcd.print (totalWinding);
  delay(3000);
}

// Countdown to winding start
void countDown() {
  lcd.print ("Starting in ");
  lcd.setCursor (0, 0);
  lcd.setCursor(0, 1);
  lcd.print("Reset -> Cancel");
  for (int i = 9; i >= 0; i--) {
    lcd.setCursor(13, 0);
    lcd.print (i);
    delay (1000);
  }
}

// 
volatile int windCounter () {
  hallState = digitalRead (hallPin);
  if (hallState == LOW) {
    windCount = windCount + 1;
    detachInterrupt(hallPin);
  }
}

void switchOnFunction() {
  myServo.write(150);
  delay (1000);
  myServo.write(90);
}