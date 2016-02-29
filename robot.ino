#include <SoftwareSerial.h>

#define rxPin 6
#define txPin 7

#define leftDirection 8
#define leftSpeed 4

#define rightDirection 9
#define rightSpeed 10

#define trigPin 11
#define echoPin 12
 
String command = "";         
boolean commandComplete = false;  

int minSpeed = 135;
int speedDelta = (255 - minSpeed)/10;

SoftwareSerial bluetooth(rxPin, txPin); // RX, TX

void setup() {
  
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  pinMode(leftDirection, OUTPUT);
  pinMode(leftSpeed, OUTPUT);
  
  pinMode(rightDirection, OUTPUT);
  pinMode(rightSpeed, OUTPUT);
  
  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
  bluetooth.begin(9600); 
  
  command.reserve(200);
}

void loop() { 
  //slong distance = getDistance();
  //writeLog((String)distance);
  readCommand();  
  if (commandComplete) {
    processCommand(command);
    clearCommand();
  }
}

long getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

String readCommand() {
  while (bluetooth.available()) {
    char inChar = (char)bluetooth.read();
    commandComplete = false;
    if (inChar == '#') {
      commandComplete = true; 
      return command;   
    }
    else {
      command += inChar;
    }
  }
  return "";
}

void writeLog(String message) {
  Serial.println(message);
}

void clearCommand() {
  command = "";
  commandComplete = false;
}

void processCommand(String command) {
  int turn = getValue(command, ':', 0).toInt();
  int direct = getValue(command, ':', 1).toInt();
  
  if (direct == 0)
  {
    stop();
  }
  else
  { 
    if (direct <= -2)
    {
      goForvard(-direct, turn); 
    }
    else if (direct >= 2)
    {
      goBack(direct, turn);
    }
  }
}

void goForvard(int speed, int turn)
{
  speed = minSpeed + speed*speedDelta;
  int lSpeed = speed;
  int rSpeed = speed;
  
  if (turn != 0)
  {
    if (turn < 0)
    {
      lSpeed = speed - (int)(speed/10*turn*(-1));
    }
    else
    {
      rSpeed = speed - (int)(speed/10*turn);
    }
  }
  
  goLeftForward(lSpeed); 
  goRightForward(rSpeed);
}

void goBack(int speed, int turn)
{
  speed = 255 - minSpeed - speed * speedDelta;
 
  int lSpeed = speed;
  int rSpeed = speed;
  
  if (turn != 0)
  {
    if (turn < 0)
    {
      lSpeed = speed + (int)((255 - speed)/10*turn*(-1));
    }
    else
    {
      rSpeed = speed + (int)((255 - speed)/10*turn);
    }
  }
  
  goLeftBack(lSpeed); 
  goRightBack(rSpeed);
}

void stop()
{
  analogWrite(leftDirection, 0);
  analogWrite(leftSpeed, 0);
  analogWrite(rightDirection, 0);
  analogWrite(rightSpeed, 0);
}

void goLeftBack(int speed)
{
  digitalWrite(leftDirection, HIGH);
  analogWrite(leftSpeed, speed);
}

void goRightBack(int speed)
{
  digitalWrite(rightDirection, HIGH);
  analogWrite(rightSpeed, speed);
}

void goLeftForward(int speed)
{
  digitalWrite(leftDirection, 0);
  analogWrite(leftSpeed, speed);
}

void goRightForward(int speed)
{
  digitalWrite(rightDirection, 0);
  analogWrite(rightSpeed, speed);
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}



