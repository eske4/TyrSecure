#include "ptrace_agent.h"
#include <iostream>

void ptrace_agent::print_ptrace() {
    auto data = this->handler.GetData();
    if (!data.has_value()) return;
    
    std::cout << "ptrace called by " << data.value().caller_name
            << " (pid " << data.value().caller
            << "), attaching to proc " << data.value().target
            << std::endl;
}

ptrace_agent::ptrace_agent(pid_t protected_pid)
{
    this->handler.LoadAndAttachAll(protected_pid);
}

ptrace_agent::~ptrace_agent()
{
}
