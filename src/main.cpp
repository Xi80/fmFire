#include <mbed.h>
#include "SD-1/sd-1.hpp"

SD1 sd1(PB_4,PB_5,PB_3,PB_6,PA_15);

int main() {
  sd1.programChange(0,0);
  sd1.noteOn(0x00,0x50,0x70);
  thread_sleep_for(500);
  sd1.noteOff(0x00,0x50);
  // put your setup code here, to run once:

  while(1) {
    // put your main code here, to run repeatedly:
  }
}