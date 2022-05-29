#ifndef COYIELD_COROUTINE_H
#define COYIELD_COROUTINE_H

#include <ucontext.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <utility>
#include <vector>

class Scheduler;

using CoFunc = std::function<void()>;

const int STACK_SIZE = 1024 * 1024;
const int CO_CONTAIN_DEFAULT_SIZE = 20;

class Coroutine {
 public:
  enum STATUS { FREE, READY, RUNNING, SUSPEND };

 private:
  STATUS status_;

  // entry function
  CoFunc co_func_;

  // stack saved
  char *stack_;
  int64_t stack_size_;

  Scheduler *sch_;
  ucontext_t ctx_;

 public:
  template <typename F, typename... Args>
  Coroutine(F &&f, Args &&...args) {
    AddFunc(f, args...);

    this->stack_ = new char[STACK_SIZE];
    this->stack_size_ = STACK_SIZE;
    this->sch_ = nullptr;

    this->status_ = READY;
  }

  ~Coroutine() { delete stack_; };

  template <typename F, typename... Args>
  void AddFunc(F &&f, Args &&...args) {
    this->co_func_ = std::bind(f, args...);
  }

  STATUS Status() const;
  bool IsFree() const;
  void SetStatus(Coroutine::STATUS status);

  ucontext_t &Context();
  void AllocScheduler(class Scheduler *sch);

  // running entry
  void RunFunc();
  void Yield();
  void Resume();

  // save stack
  void SaveStack(const char *stack_top);
  void RestoreStack(char *stack_top);
};

#endif  // COYIELD_COROUTINE_H
