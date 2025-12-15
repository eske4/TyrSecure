#include "mem_access_agent.h"
#include "module_tracker_agent.h"
#include "module_tracker_handler.h"
#include <iostream>
#include <signal.h>


bool stop = false;
void siginthandler(int param)
{
  (void)param;
  stop = true;
  std::cout << std::endl;
}

int main() {
  std::cout << "--- Anti-Cheat Handler Test ---" << std::endl;

  pid_t protected_pid = 792;

  mem_access_agent mem_access_agent(protected_pid);
  module_tracker_agent module_agent = module_tracker_agent();

  // 1. Load and Attach
  std::cout << "\n========================================================"
            << std::endl;
  std::cout << "Check the trace pipe in a new terminal:" << std::endl;
  std::cout << "sudo cat /sys/kernel/tracing/trace_pipe" << std::endl;
  std::cout << "Press CTRL+C to unload the programs..."
            << std::endl;

  signal(SIGINT, siginthandler);
  while (!stop) {
    // Try to get the next event
    auto maybe_event = module_agent.get_next_event();
    while (maybe_event) {
      const module_event &e = *maybe_event;
      module_agent.printEventData(e);

      // Get the next event in the queue
      maybe_event = module_agent.get_next_event();
    }

    // Sleep briefly to avoid busy-waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
