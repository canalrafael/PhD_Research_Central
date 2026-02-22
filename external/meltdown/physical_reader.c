#include "libkdump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  size_t phys;
  if (argc < 2) {
    printf("Usage: %s <physical address> [<direct physical map>]\n", argv[0]);
    return 0;
  }

  phys = strtoull(argv[1], NULL, 0);

  libkdump_config_t config;
  memset(&config, 0, sizeof(libkdump_config_t));
  config.cache_miss_threshold = 100;
  config.measurements = 1; // Faster for live streaming
  config.physical_offset = 0x80000000;

  if (argc > 2) {
    config.physical_offset = strtoull(argv[2], NULL, 0);
  }

  libkdump_init(config);

  printf("\x1b[32;1m[+]\x1b[0m Physical address       : \x1b[33;1m0x%zx\x1b[0m\n", phys);
  printf("\x1b[32;1m[+]\x1b[0m Physical offset        : \x1b[33;1m0x%zx\x1b[0m\n", config.physical_offset);

  while (1) {
    int value = libkdump_read(phys);
    if (value >= 32 && value <= 126) printf("%c", value);
    else printf(".");
    fflush(stdout);
    phys++;
  }

  libkdump_cleanup();
  return 0;
}