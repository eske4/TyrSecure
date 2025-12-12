#include "mem_access_handler.h"
#include "data_types.h"
#include "string.h"
#include <bpf/libbpf.h>
#include <iostream>


int mem_access_handler::ring_buffer_callback(void *ctx, void *data, size_t data_sz)
{
    mem_access_handler *handler = static_cast<mem_access_handler *>(ctx);
    mem_event e;
    memmove(&e, data, data_sz);
    handler->on_event(e);

    return 0;
}

int mem_access_handler::LoadAndAttachAll(pid_t protected_pid) {
  int err = 0;

  skel_obj.reset(mem_access__open());
  if (!skel_obj) {
    std::cerr << "ERROR: Failed to open BPF skeleton object." << std::endl;
    return -1;
  }

  skel_obj.get()->rodata->PROTECTED_PID = protected_pid;

    err = mem_access__load(skel_obj.get());
    if (err)
    {
        std::cerr << "ERROR: Failed to load BPF programs into kernel: " << err
                  << std::endl;
        skel_obj.reset();
        return err;
    }

    auto buf = ring_buffer__new(
        bpf_map__fd(skel_obj.get()->maps.rb),
        mem_access_handler::ring_buffer_callback,
        this,
        nullptr);
    rb.reset(buf);

    err = mem_access__attach(skel_obj.get());
    if (err)
    {
        std::cerr << "ERROR: Failed to attach BPF programs to hook points: " << err
                  << std::endl;
        skel_obj.reset();
        return err;
    }

    run = true;
    loop_thread = std::async([this]() {
        while (this->run) {
            ring_buffer__poll(this->rb.get(), 100);
        }
    });

  return 0;
}

mem_access_handler::mem_access_handler(std::function<void(mem_event)> on_event) {
    this->on_event = on_event;
}

void mem_access_handler::DetachAndUnloadAll() {
  skel_obj.reset();
  run = false;
  loop_thread.wait();
  std::cout << "mem_access eBPF program detached and unloaded." << std::endl;
}

mem_access_handler::~mem_access_handler() { DetachAndUnloadAll(); }
