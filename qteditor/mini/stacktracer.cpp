#include <execinfo.h>
#include <cstdlib>
#include <iostream>

void print_stack_trace() {
  const int max_frames = 32;
  void* buffer[max_frames];

  // Get void*'s for all entries on the stack
  int num_frames = backtrace(buffer, max_frames);

  // Translate into human-readable strings
  char** symbols = backtrace_symbols(buffer, num_frames);

  if (symbols == nullptr) {
    perror("backtrace_symbols");
    return;
  }

  std::cout << "--- Call Stack ---" << std::endl;
  for (int i = 0; i < num_frames; i++) {
    std::cout << symbols[i] << std::endl;
  }

  free(symbols);
}
