enum eSpeedCommand : char{
  speed_forward,
  speed_reverse,
  speed_neutral
};
const char * speed_command_name( eSpeedCommand e) {
  const char * names[] = {
    "speed_forward",
    "speed_reverse",
    "speed_neutral"

  };
  return names[e];
}
