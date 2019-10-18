void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

//  if (Serial.available()) {
//    Serial.write(Serial.parseInt());
//    Serial.flush();
//  
//  }
//  for (byte n = 0; n < 255; n++) 
//  {
//    Serial.write(n);
//    delay(50);
//  }
  Serial.println("Hello world");
  if (Serial.available()) 
  {
    Serial.print("There are bytes available: ");
    while (Serial.available()) 
    {
      Serial.print(Serial.read(), DEC);
    }
    Serial.println();
    
  }
  delay(1000);
}
