#pragma once
#include "kmod_tracker_handler.h"
#include <mutex>
#include <optional>
#include <queue>

class kmod_tracker_agent {
private:
  kmod_tracker_agent(const kmod_tracker_agent &) = delete;
  kmod_tracker_agent &operator=(const kmod_tracker_agent &) = delete;
  kmod_tracker_agent(kmod_tracker_agent &&) = delete;
  kmod_tracker_agent &operator=(kmod_tracker_agent &&) = delete;

  void on_event_cb(const module_event &e);

  kmod_tracker_handler handler;
  std::queue<module_event> event_queue;
  std::mutex queue_mutex;

public:
  kmod_tracker_agent();
  ~kmod_tracker_agent();
  std::optional<module_event> get_next_event();
  void printEventData(const module_event &e);
};
