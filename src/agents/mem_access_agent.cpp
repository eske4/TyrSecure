#include "mem_access_agent.h"
#include "data_types.h"
#include <cstdio>

auto on_event = [](mem_event e)
{
    auto type = e.type;
    switch (type)
    {
    case PTRACE:
        printf("ptrace called by %s (PID %i), attaching to proc %i\n",
            e.caller_name,
            e.caller,
            e.target);
        break;
    case OPEN:
        printf("open called by %s (PID %i), with filename %s\n",
            e.caller_name,
            e.caller,
            e.filename);
        break;
    case VM_WRITE:
        printf("vm_write called by %s (PID %i), writing to proc %i\n",
            e.caller_name,
            e.caller,
            e.target);
        break;

    case VM_READ:
        printf("vm_read called by %s (PID %i), reading from proc %i\n",
            e.caller_name,
            e.caller,
            e.target);
        break;
    
    default:
        printf("syscall type %i called by %s (PID %i), with filename %s, target pid: %i\n",
            e.type,
            e.caller_name,
            e.caller,
            e.filename,
            e.target);
        break;
    }
};

mem_access_agent::mem_access_agent(pid_t protected_pid)
    : handler(on_event)
{
    handler.LoadAndAttachAll(protected_pid);
}

mem_access_agent::~mem_access_agent()
{
}

void mem_access_agent::set_protected_pid(pid_t protected_pid)
{
    handler.DetachAndUnloadAll();
    handler.LoadAndAttachAll(protected_pid);
}
