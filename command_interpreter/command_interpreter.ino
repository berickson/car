

class AsynchSerial {
  const char * read_line() {
    return "";
  }
};




CommandLineInterpreter interpreter;


void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Printing to serial");
  Serial.setTimeout(0);
  Serial.println("Printing to serial2");
  interpreter.init(commands,2);

}


void loop() {
  Serial.println("looping");
  interpreter.execute();
}
