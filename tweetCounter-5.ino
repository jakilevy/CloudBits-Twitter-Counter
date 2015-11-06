/*
CloudBits Arduino Counter
Based on http://littlebits.cc/projects/the-littlebits-count
by Richard Born
May 7, 2014

Modified by Jaki Levy
November 2015

Written for Arduino Leonardo board , CloudBits and LittleBits
Counts up to 5, lights up a bargraph light, sends out a PING on Pin d1
*/

//SETUP THE INPUTS AND OUTPUT VARIABLES
const int resetButton = A0; // this is a RESET button will set all counters to zero when pressed
const int countButton = 0; // this button counts up & down presses of the count button (d0)
const int cloudBitsPin = A1; // this sets up the CLOUDBITS Module as an INPUT
const int goGordonGoPin = 1; // this is the wireLess Transmitter & TEST LED OutPut (d1)
const int barGraph = 9; // BARGRAPH LittleBits OUTPUT Pin (d9)

// INITIATE ALL THE BUTTON VALUES and INPUTS
int resetButtonState = 0;        // current state of the reset button
int countButtonState = 0;        // current state of the count button
int countOldButtonState = 0;     // previous state of the count button
int count = 0; // counts the number of presses of the count button
int upsAndDowns = 0; // counts presses and releases of the count button

//SETUP VARIABLES for Counters and Lights and OUTPUTS
int barGraphValue = 0;    // variable to hold the value of the analog output on D9
int cloudBitsValue = 0; // variable to hold the value of CloudBits Data input
int goGordonGoState = 0; // variable to hold value of the GoGordonGo WireLess transmitter on D1 , set to 0 (OFF)
int tweetCounter = 0; // variable to hold how many tweets came in

//SETUP THE INPUTS AND OUTPUTS FOR ARDUINO
void setup() {
  pinMode(resetButton, INPUT);     // sets pin to INPUT mode
  pinMode(countButton, INPUT);     // sets pin to INPUT mode
  pinMode(cloudBitsPin, INPUT); // sets CloudBits to INPUT mode
  pinMode(barGraph, OUTPUT);    // sets pin to OUTPUT mode
  pinMode(goGordonGoPin, OUTPUT);    // sets GoGordonGo pin to OUTPUT mode
  Serial.begin(9600);      // open the serial port at 9600 bps for Monitoring outputs on Laptop

}

//OK , We're ready to GoGoGo!
void loop() {

  resetButtonState = digitalRead(resetButton); // get the state of the reset button
  countButtonState = digitalRead(countButton); // get the state of the counter button
  
  //SET ANALOG IN VALUE TO CLOUDBITS
  cloudBitsValue = analogRead(cloudBitsPin); // Checks the "VALUE" Percentage of CloudBits Coming In via IFTTT

  // Allows the BarGraph Bits to be controlled via http://control.littlebitscloud.cc/
  // analogWrite(barGraph, cloudBitsValue); // REMOVED BECAUSE THIS CONFUSES THINGS.
  
  // CHECK TO SEE IF CLOUDBITS IS GETTING ANY "PINGS" from IFTTT.COM
  if (cloudBitsValue >950) countButtonState = HIGH; //JB
  else if (cloudBitsValue <=950) countButtonState = LOW; //JB

  //ADD BUTTON HIGH CODE
  // RESET BUTTON CODE - SETS EVERYTHING TO ZERO WHEN RESET BUTTON IS PRESSED
  if (resetButtonState == HIGH) {
    // reset upDown counter to zero and Turn Off Lights
    upsAndDowns = 0;
    count = 0; //resets Count
    tweetCounter = 0; // resets the "TweetCounter"
    digitalWrite(goGordonGoPin, LOW); //turns off WireLess Transmitter
    analogWrite(barGraph, 0); //turns off LIGHTS for BarGraph Tower
  }

  /* I'm using this next block of code to increase BOTH the UpDown Button AND The CloudBits. 
   * Essentially, I want things to "move up" regardless of whether a button is pressed or a Tweet Comes in
   * 
   * So this block of code does this :
   * When BUTTON or CLOUDBITS is Pressed (effectively CloudBits is Pinged by IFTTT) ... 
   * ....increase tweetCounter and Count
   */
  
  // INCREASE BUTTON COUNT
  if ((countButtonState == HIGH && countOldButtonState == LOW) || (countButtonState == LOW && countOldButtonState == HIGH)) {
    // counter button has been pressed down or has been released
    upsAndDowns += 1;
    count = upsAndDowns / 2; // actual Button count is half the ups and downs
    tweetCounter = upsAndDowns / 2; // actual CloudBits count is half the ups and downs
    if (count == 10000) {
      upsAndDowns = 0;
      count = 0;
    }
    countOldButtonState = countButtonState; // saves the old state of the count button
  }

  /* This Next Section Will LIGHT UP the BarGraph Light based on the CLOUDBITS Perecentage Value
   * I checked the "output" value via Serial and mapped the values (0 to 1023) to each light on the bargraph.
   * 815 = The 1st Bargraph Light
   * 611 = The 2nd Bargraph Light
   * 407 = The 3rd Bargraph Light
   * 203 = The 4th Bargraph Light
   * 1023 = The 5th Bargraph Light (full on Mode)
   */
  if (tweetCounter == 1) {
    analogWrite(barGraph, 815); // Green 1st Spot
  }
  if (tweetCounter == 2) {
    analogWrite(barGraph, 611); // GREEN 2nd Spot
  }
  if (tweetCounter == 3) {
    analogWrite(barGraph, 407); // GREEN 3rd Spot
  }
  if (tweetCounter == 4) {
    analogWrite(barGraph, 203); // YELLOW 4th Spot
  }
  if (tweetCounter >= 5) {
    digitalWrite(goGordonGoPin, HIGH); // Hit The WireLess Transmitter
    analogWrite(barGraph, 1023); // RED 5th Spot , Full on GoGordonGo Mode
  }
  
  /* THIS SECTION LIGHTS THINGS UP VIA THE BUTTON - This is a manual Mode in case the Internet Craps Out
  if (count == 1) {
    analogWrite(barGraph, 815); // Green 1st Spot
  }
  if (count == 2) {
    analogWrite(barGraph, 611); // GREEN 2nd Spot
  }
  if (count == 3) {
    analogWrite(barGraph, 407); // GREEN 3rd Spot
  }
  if (count == 4) {
    analogWrite(barGraph, 203); // YELLOW 4th Spot
  }
  if ((count >= 5) || (cloudBitsValue > 229))  {
    digitalWrite(goGordonGoPin, HIGH);
    analogWrite(barGraph, 1023); // Red 5th Spot , Full on GoGordonGo Mode
  }
  if (count == 42) {
    Serial.print( "You've achieved Nirvana"); // Rest well knowing you are One with Everything
  }
  */
  
  // THIS BLOCK OF CODE OUTPUTS SERIAL VALUES ON EL COMPUTADOR
  Serial.print( "ButtonCount: " + String(count) + " , "); // print the variable "COUNT"
  Serial.print( "TweetCount : " + String(tweetCounter) + " , "); // print the variable "tweetCounter"
  Serial.print( "CloudBit Value : " + String(cloudBitsValue) + " , "); // print the variable "cloubdBitsValue"
  Serial.println( "UpsDown Count : " + String(upsAndDowns) + " "); // print the variable "upsAndDowns"


  delay(20); // debounce time for buttons
}
