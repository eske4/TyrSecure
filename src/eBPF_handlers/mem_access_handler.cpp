#include "mem_access_handler.h"
#include "string.h"
#include <bpf/libbpf.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>

int mem_access_handler::ring_buffer_callback(void *ctx, void *data,
                                             size_t data_sz) {
  if (data_sz != sizeof(mem_event)) {
    std::cerr << "Size mitch match in event";
    return 1; // Return non-zero to indicate a processing error
  }

  auto *handler = static_cast<mem_access_handler *>(ctx);

  mem_event e;
  std::memcpy(&e, data, sizeof(e));
  handler->on_event(e);

  return 0;
}

int mem_access_handler::LoadAndAttachAll(pid_t protected_pid) {
  if (!on_event) {
    std::cerr << "No on_event callback set\n";
    return -1;
  }

  skel_obj.reset(mem_access__open());

  if (!(skel_obj.reset(mem_access__open()), skel_obj)) {
    std::cerr << "ERROR: Failed to open BPF skeleton object." << std::endl;
    return -1;
  }

  skel_obj.get()->rodata->PROTECTED_PID = protected_pid;

  if (int err = mem_access__load(skel_obj.get())) {
    std::cerr << "ERROR: Failed to load BPF programs into kernel: " << err
              << std::endl;
    skel_obj.reset();
    return -1;
  }

  rb.reset(ring_buffer__new(bpf_map__fd(skel_obj->maps.rb),
                            mem_access_handler::ring_buffer_callback, this,
                            nullptr));

  if (!(rb.reset(ring_buffer__new(bpf_map__fd(skel_obj->maps.rb), 
    ring_buffer_callback, this, nullptr)), rb)) 
      return (std::cerr << "Failed to create ring buffer\n", -1);

  if (int err = mem_access__attach(skel_obj.get())) 
    return (std::cerr << "Failed to attach: " << err << "\n", rb.reset(), err);


  loop_thread = std::jthread([this](std::stop_token st) {
    while (!st.stop_requested()) {
      if (ring_buffer__poll(rb.get(), 100) < 0 && errno != EINTR) break;
    }
  });

  return 0;
}

mem_access_handler::mem_access_handler(std::function<void(mem_event)> cb)
    : on_event(std::move(cb)) {}

void mem_access_handler::DetachAndUnloadAll() {

  if (loop_thread.joinable()) {
    loop_thread.request_stop();
    // Wait for the thread to terminate before cleanup
    loop_thread.join();
  }

  rb.reset();
  skel_obj.reset();

  std::cout << "mem_access eBPF program detached and unloaded.\n";
}

mem_access_handler::~mem_access_handler() { DetachAndUnloadAll(); }
