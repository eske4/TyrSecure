#pragma once
#include "mem_access_handler.h"

class mem_access_agent
{
private:
    mem_access_handler handler;
public:
    void set_protected_pid(pid_t protected_pid);

    mem_access_agent(pid_t protected_pid);
    ~mem_access_agent();
};

