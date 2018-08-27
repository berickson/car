#include "CommandInterpreter.h"
#include "Logger.h"

void CommandInterpreter::init(const Command * _commands, int _command_count) {
  commands = _commands;
  command_count = _command_count;
}

void CommandInterpreter::execute() {
  while(Serial.available()>0) {
    char c = Serial.read();
    if( c==-1) break;
    if( c==0) break;
    if( c == '\n' || c =='\r') {
      process_command(String(buffer));
      buffer[0] = 0;
      buf_size = 0;
    } else {
      if(buf_size+1 >= buf_max) {
         log(LOG_ERROR, "Read buffer overflow");
         log(LOG_ERROR, buffer);
         buffer[0] = 0;
         buf_size = 0;
      }
      buffer[buf_size] = c;
      buffer[buf_size+1] = 0;
      buf_size++;
    }
  }
}

void CommandInterpreter::process_command(String s) {
  for(int i = 0; i < command_count; i++) {
    String name = commands[i].name;
    if(s.startsWith(name)) {
      command_args = s.substring(name.length());
      log(LOG_TRACE, "Executing command " + name + " with args (" + command_args + ")");
      commands[i].f();
      return;
    }
  }
  log(LOG_ERROR, "Unknown command: ");
}
