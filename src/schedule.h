#ifndef COYIELD_SCHEDULE_H
#define COYIELD_SCHEDULE_H

#include <ucontext.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "coroutine.h"

using std::shared_ptr;
using std::vector;

class Coroutine;

class Scheduler {
 public:
 private:
  char global_stack_[STACK_SIZE];
  vector<Coroutine*> co_contain_;

  ucontext_t main_ctx_;
  Coroutine* running_co_;

 public:
  Scheduler();
  ~Scheduler();

  template <typename F, typename... Args>
  Coroutine* RegisterCo(F&& f, Args&&... args) {
    for (auto& co : this->co_contain_) {
      if (!co || co->Status() == Coroutine::FREE) {
        co = new Coroutine(f, args...);
        co->AllocScheduler(this);
        return co;
      }
    }

    assert(false);
  }

  void InitReadyContext(Coroutine* co);
  bool IsRunningMe(Coroutine* co);

  void Yield();
  void Resume(Coroutine* co);

  static void EntryFunc(Coroutine* co);
};

#endif  // COYIELD_SCHEDULE_H
