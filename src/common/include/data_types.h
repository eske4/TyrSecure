#pragma once

#define MY_TASK_COMM_LEN 16

struct ptrace_event {
  int caller;
  int target;
  char caller_name[MY_TASK_COMM_LEN];
};
