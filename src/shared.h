#pragma once

#define TASK_COMM_LEN 16

struct ptrace_event {
    pid_t caller;
    pid_t target;
    char caller_name[TASK_COMM_LEN];
};

