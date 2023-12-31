#include "Timer.h"
#include <cassert>
//--------------------------------------------------------------------------------------------------
Timer::Timer() : _s(0) {
}
//--------------------------------------------------------------------------------------------------
Timer::~Timer() {
  if (_s) delete _s;
}
//--------------------------------------------------------------------------------------------------
void Timer::start() {
  assert(!_s);
  _s = new timeval();
  gettimeofday(_s, 0);
}
//--------------------------------------------------------------------------------------------------
double Timer::stop() {
  assert(_s);
  struct timeval* e = new timeval();
  gettimeofday(e, 0);
  long ms_s = _s->tv_sec * 1000000 + _s->tv_usec;
  long ms_e = e->tv_sec * 1000000 + e->tv_usec;
  double time = (ms_e - ms_s) / 1000.0;
  delete e;
  delete _s;
  _s = 0;
  return time;
}
//--------------------------------------------------------------------------------------------------

