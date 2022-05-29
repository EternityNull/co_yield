#include "src/coroutine.h"
#include "src/schedule.h"

void test(Coroutine **co, std::string s) {
  auto real_co = *co;
  int max_times = 10;
  int i = 0;
  while (true) {
    if (i == max_times) {
      real_co->Yield();
      i = 0;
    }

    printf("%s\n", s.c_str());

    i++;
  }
}

int main() {
  auto *sch = new Scheduler();
  Coroutine *co1 = nullptr;
  co1 = sch->RegisterCo(test, &co1, "aaa");

  Coroutine *co2 = nullptr;
  co2 = sch->RegisterCo(test, &co2, "bbbb");

  int times = 0;
  while (!co1->IsFree() && !co2->IsFree()) {
    times++;
    if (times == 10) {
      break;
    }
    co1->Resume();
    co2->Resume();
  }

  return 0;
}