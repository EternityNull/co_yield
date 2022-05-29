#include <ucontext.h>
#include <cassert>
#include <cstring>
#include "coroutine.h"
#include "schedule.h"

Coroutine::STATUS Coroutine::Status() const { return status_; }

bool Coroutine::IsFree() const { return status_ == STATUS::FREE; }

void Coroutine::SetStatus(Coroutine::STATUS status) { this->status_ = status; }

void Coroutine::RunFunc() { co_func_(); }

void Coroutine::Yield() {
  assert(this->status_ == Coroutine::RUNNING);
  assert(this->sch_->IsRunningMe(this));
  this->sch_->Yield();
}

void Coroutine::Resume() { this->sch_->Resume(this); }

void Coroutine::SaveStack(const char *stack_top) { memcpy(stack_, stack_top, stack_size_); }
void Coroutine::RestoreStack(char *stack_top) { memcpy(stack_top, stack_, stack_size_); }

ucontext_t &Coroutine::Context() { return this->ctx_; }

void Coroutine::AllocScheduler(class Scheduler *sch) { this->sch_ = sch; }
