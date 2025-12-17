#include "kmod_tracker_agent.h"
#include "mem_access_agent.h"
#include <iostream>
#include <signal.h>

bool stop = false;
void siginthandler(int param) {
  (void)param;
  stop = true;
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  std::cout << "--- Anti-Cheat Handler Test ---" << std::endl;

  pid_t protected_pid = (argc > 1) ? static_cast<pid_t>(std::stoi(argv[1]))
                                   : static_cast<pid_t>(792);
                                   

  mem_access_agent mem_agent = mem_access_agent(protected_pid);
  kmod_tracker_agent module_agent = kmod_tracker_agent();


  mem_agent.set_block_access(false); //Set to true to make eBPF block access

  // 1. Load and Attach
  std::cout << "\n========================================================"<< std::endl;
  std::cout << "program protected pid: " << protected_pid << std::endl;
  std::cout << "Check the trace pipe in a new terminal:" << std::endl;
  std::cout << "run \"sudo cat /sys/kernel/tracing/trace_pipe\" or \"make debug\"" << std::endl;
  std::cout << "Press CTRL+C to unload the programs..." << std::endl;

  signal(SIGINT, siginthandler);
  while (!stop) {
    // Try to get the next event
    auto maybe_module_event = module_agent.get_next_event();
    auto maybe_mem_agent = mem_agent.get_next_event();

    while (maybe_module_event) {
      const module_event &e = *maybe_module_event;
      module_agent.printEventData(e);
      maybe_module_event = module_agent.get_next_event();
    }

    while (maybe_mem_agent) {
      const mem_event &e2 = *maybe_mem_agent;
      if (e2.type == WRITE || e2.type == READ || e2.type == VM_WRITE || e2.type == VM_READ || e2.type == OPEN || e2.type == PTRACE ) {
        mem_agent.printEventData(e2);
      }
      maybe_mem_agent = mem_agent.get_next_event();
    }

    // Sleep briefly to avoid busy-waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
