#pragma once

#include "Arduino.h"
#include "Command.h"

class CommandInterpreter{
public:
  static const int buf_max = 100;
  char buffer[buf_max+1];
  int	 buf_size = 0;
  const Command * commands;
  int command_count;
  String command_args;

  void init(const Command * _commands, int _command_count);

  void execute();

  void process_command(String s);
};

