String nom = "Arduino";
String msg;

// Left side motor
const int EnableL = 5;
const int HighL = 6;       
const int LowL = 7;

// Right side motor
const int EnableR = 10;
const int HighR = 8;       
const int LowR = 9;

void setup() {
  Serial.begin(9600);
  pinMode(EnableL, OUTPUT);
  pinMode(HighL, OUTPUT);
  pinMode(LowL, OUTPUT);

  pinMode(EnableR, OUTPUT);
  pinMode(HighR, OUTPUT); 
  pinMode(LowR, OUTPUT);
}

void loop() {
  readSerialPort();

  if (msg != "") {
    sendData();
  }
  delay(500);
  //cases();
}

void readSerialPort() {
  msg = "";
  if (Serial.available()) {
    delay(10);
    while (Serial.available() > 0) {
      msg += (char)Serial.read();
    }
    Serial.flush();
  }
}

void sendData() {
  //write data
  //Serial.println(msg.compareTo("1"));
  // Serial.print(nom);
  // Serial.print(" received : ");
  // Serial.print(msg);
  unsigned int l=msg.length()+1;
  char x[l];
  msg.toCharArray(x, l);
  switch(x[0])
  {
    case 'a':Forward();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'b':Backward();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'c': Left1();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'd': Left2();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'e': Left3();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'f': Right1();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'g': Right2();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    case 'h': Right3();
    //delay(5000);
    //Stop();
    Serial.println(msg);
    break;
    default:Stop();
    Serial.println(msg);
    break;
  }

}

void Forward() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,75);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,75);
  
}

void Backward() {
  digitalWrite(HighL, LOW);
  digitalWrite(LowL, HIGH);
  analogWrite(EnableL,130);

  digitalWrite(HighR, LOW);
  digitalWrite(LowR, HIGH);
  analogWrite(EnableR,130);
  
}

// right side slightly
void Left1() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,100);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,130);
}

// right side moderately
void Left2() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,80);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,130);
  
}

// right side extremely 
void Left3() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,60);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,130);
  
}

// left side slightly
void Right1() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,130);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,100);
}

// left side moderately
void Right2() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,130);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,80);
}

// left side extremely 
void Right3() {
  digitalWrite(HighL, HIGH);
  digitalWrite(LowL, LOW);
  analogWrite(EnableL,130);

  digitalWrite(HighR, HIGH);
  digitalWrite(LowR, LOW);
  analogWrite(EnableR,60);
}

void Stop() {
  // digitalWrite(HighL, LOW);
  // digitalWrite(LowL, HIGH);
  analogWrite(EnableL,0);

  // digitalWrite(HighR, LOW);
  // digitalWrite(LowR, HIGH);
  analogWrite(EnableR,0);
  
}

// void cases(){
//   unsigned int l=msg.length()+1;
//   char x[l];
//   msg.toCharArray(x, l);
//   switch(x[0])
//   {
//     case 'a':Serial.println("zero");
//     break;
//     case 'b':Serial.println("one");
//     break;
//     case 'c':Serial.println("two");
//     break;
//     default:Serial.println("Other");
//   }
// }