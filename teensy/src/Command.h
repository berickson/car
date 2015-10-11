#pragma once

typedef void (*voidfunction)();

struct Command {
  const char * name;
  const char * description;
  voidfunction f;
};
