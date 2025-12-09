#pragma once
#include "ptrace.skel.h"
#include <memory>
#include <future>

class ptrace_handler {
public:
  ptrace_handler() = default;

  ~ptrace_handler();

  /// @param protected_pid The pid of the game/process to protect
  int LoadAndAttachAll(pid_t protected_pid);
  void DetachAndUnloadAll();

private:
    void loop_func();

  std::unique_ptr<struct ptrace, decltype(&ptrace__destroy)>
      skel_obj{nullptr, ptrace__destroy};

    struct ring_buffer *rb = nullptr;

    std::future<void> loop_thread;
    bool run = true;
};
