
volatile unsigned long g_ping_reply_start_us = 0;
volatile unsigned long g_ping_reply_end_us = 0;
volatile bool g_ping_reply_ready = false;

// Interrupt service routines
void on_ping_reply_change() {
  if(digitalRead(PIN_PING_ECHO) == HIGH) {
      attachInterrupt(digitalPinToInterrupt(PIN_PING_ECHO), on_ping_reply_change, FALLING);
     g_ping_reply_start_us = micros();
  } else {
    g_ping_reply_end_us = micros();
    g_ping_reply_ready = true;
    detachInterrupt(digitalPinToInterrupt(PIN_PING_ECHO));
  }
}

struct Ping {
  int ping_pin, echo_pin;
  unsigned long ping_start_ms = 0;
  unsigned long ping_start_us = 0;
  bool _new_data_ready = false;
  bool _waiting_for_reply = false;

  double last_ping_distance_inches = 0.;

  unsigned long ping_rate_ms = 100;
  const unsigned long ping_timeout_us = 20000; // 20000 microseconds should be about 10 feet

  void init(int _ping_pin, int _echo_pin){
    ping_pin = _ping_pin;
    echo_pin = _echo_pin;
    digitalWrite(ping_pin, LOW);
    _waiting_for_reply = false;
  }

  bool new_data_ready() {
    return _new_data_ready;
  }

  void set_distance_from_us(unsigned long us) {
      double ping_distance_inches = (double) us / 148.; // 148 microseconds for ping round trip per inch
      if(last_ping_distance_inches != ping_distance_inches) {
         last_ping_distance_inches = ping_distance_inches;
         _new_data_ready = true;
      }
  }
  void execute(){
    _new_data_ready = false;
    unsigned long ms = millis();
    unsigned long  us = micros();
    if(_waiting_for_reply) {

       // transfer volatile variables from interrupts
       if(g_ping_reply_ready) {
         cli();
         unsigned long reply_start_us = g_ping_reply_start_us;
         unsigned long reply_end_us = g_ping_reply_end_us;
         sei();

         if(0) {
           Serial.print("reply_start_us");
           Serial.println(reply_start_us);
           Serial.print("reply_end_us");
           Serial.println(reply_end_us);
          }
        _waiting_for_reply = false;
        _new_data_ready = true;
        detachInterrupt(digitalPinToInterrupt(echo_pin));
        unsigned long duration = reply_end_us - reply_start_us;
        set_distance_from_us(duration);
       } else if(us - ping_start_us > ping_timeout_us) {
        detachInterrupt(digitalPinToInterrupt(echo_pin));
        _waiting_for_reply = false;
       }

    } else {
        if( ms - ping_start_ms >= ping_rate_ms) {
          ping_start_ms = ms;
          ping_start_us = us;
          digitalWrite(echo_pin, LOW);
          pinMode(echo_pin, INPUT);

          // prepare interrupt ahead of time since we don't know when it will start
          _waiting_for_reply = true;
          g_ping_reply_ready = false;
          g_ping_reply_start_us = g_ping_reply_end_us = 0;
          attachInterrupt(digitalPinToInterrupt(echo_pin), on_ping_reply_change, RISING);

          // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
          digitalWrite(ping_pin, HIGH);
          delayMicroseconds(10);  // todo: can we get rid of this delay or maybe it's ok?
          digitalWrite(ping_pin, LOW);
        }
    }
  }

  inline double inches() {
    return last_ping_distance_inches;
  }
};
