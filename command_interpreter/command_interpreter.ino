

class AsynchSerial {
  const char * read_line() {
    return "";
  }
};



typedef void (*voidfunction)();

struct command {
  const char * name;
  voidfunction f;
};


bool verbose = true;
void verbose_command() {
  Serial.println("verbose turned on");
  verbose = true;
}

void noverbose_command() {
  Serial.println("verbose turned off");
  verbose = false;
}



class CommandLineInterpreter{
public:
  String buffer;
  const command * commands;
  int command_count;

  void init(const command * _commands, int _command_count)
  {
    commands = _commands;
    command_count = _command_count;
  }
  
  void execute() {
    while(Serial.available()>0) {
      char c = Serial.read();
      if(verbose) Serial.print(".");
      if( c == '\n') {
        Serial.println(buffer);
        process_command(buffer);
        buffer = "";
      } else {
        buffer += c;      
      }
    }  
  }

  void process_command(String s) {
    for(int i = 0; i < command_count; i++) {
      if(verbose) {
        Serial.print("Checking ");
        Serial.println(commands[i].name);
      }
      if(s.equals(commands[i].name)) {
        Serial.print("Executing command ");
        Serial.println(commands[i].name);
        commands[i].f();
      }
    }
  }
};


CommandLineInterpreter interpreter;

const command commands[2] = {
  {"verbose", verbose_command},
  {"noverbose", noverbose_command}
};


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


