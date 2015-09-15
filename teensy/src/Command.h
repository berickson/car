#pragma once

typedef void (*voidfunction)();

struct Command {
  const char * name;
  voidfunction f;
};
