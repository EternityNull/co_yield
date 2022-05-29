#include <cassert>
#include "schedule.h"
#include "coroutine.h"

Scheduler::Scheduler() : co_contain_(CO_CONTAIN_DEFAULT_SIZE), running_co_(nullptr) {}

Scheduler::~Scheduler() {
  assert(!running_co_);
  running_co_ = nullptr;

  co_contain_.clear();
}

void Scheduler::Yield() {
  auto co = running_co_;
  co->SetStatus(Coroutine::STATUS::SUSPEND);
  co->SaveStack(this->global_stack_);
  running_co_ = nullptr;
  swapcontext(&co->Context(), &main_ctx_);
}

void Scheduler::Resume(Coroutine* co) {
  switch (co->Status()) {
    case Coroutine::READY: {
      InitReadyContext(co);
      this->running_co_ = co;
      makecontext(&co->Context(), reinterpret_cast<void (*)()>(EntryFunc), 1, co);
      swapcontext(&main_ctx_, &co->Context());
      break;
    }
    case Coroutine::SUSPEND: {
      co->RestoreStack(this->global_stack_);
      co->SetStatus(Coroutine::RUNNING);
      this->running_co_ = co;
      swapcontext(&main_ctx_, &co->Context());
      break;
    }
    default:
      assert(false);
  }
}

void Scheduler::InitReadyContext(Coroutine* co) {
  assert(co->Status() == Coroutine::READY);
  ucontext_t& ctx = co->Context();
  getcontext(&ctx);
  ctx.uc_link = &main_ctx_;
  ctx.uc_stack.ss_sp = this->global_stack_;
  ctx.uc_stack.ss_size = STACK_SIZE;
  co->SetStatus(Coroutine::RUNNING);
}

bool Scheduler::IsRunningMe(Coroutine* co) { return co == this->running_co_; }

void Scheduler::EntryFunc(Coroutine* co) { co->RunFunc(); }