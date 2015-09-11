
byte inByte;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial port at a baud rate of 115200 bps
  Serial.begin(115200);
  pinMode(13, OUTPUT);

}

#define RCVSIZE 2

void loop() {
  // put your main code here, to run repeatedly: 
  byte msg[RCVSIZE];
  int count = 0;
  
  while (Serial.available() > 0) {
    
    // get the new byte:
    //byte inByte = (byte)Serial.read();
    String string = Serial.readString();
    Serial.print(string);
    if (string == "sup"){
      digitalWrite(13, HIGH);
    }
    else if (string == "nigga"){
      digitalWrite(13, LOW);
    }
    
    Serial.println("HELLO");
    //msg[count] = inByte;
    //count++;
    
    //received = true;
  }
  
  /*
  if(received) {
    
    //pinMode(msg[0], OUTPUT);
    //digitalWrite(msg[0], msg[1]);
    if (msg[1] ==  1) {
      Serial.print(msg[0]);
      Serial.println(" ON");
    } 
    else if (msg[1] == 0) {
      Serial.print(msg[0]);
      Serial.println(" OFF");
    }
    
    received = false;
  } */
  
  //delay(10);
}

