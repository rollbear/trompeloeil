#include "trompeloeil.hpp"
#include <iterator>
#include <thread>
#include <iostream>

class C
{
public:
  MAKE_MOCK0(func, int(void));
  std::unique_ptr<trompeloeil::expectation> allow;
};

std::mutex ptr_mutex;
inline auto get_lock()
{
  return std::unique_lock<std::mutex>{ ptr_mutex };
}

std::shared_ptr<C> obj;

inline std::shared_ptr<C> get_obj()
{
  auto lock = get_lock();
  return obj;
}

std::atomic<std::size_t> call_count[7];
std::atomic<std::size_t> ret_count[7];

void init_obj()
{
  auto m = std::make_shared<C>();
  m->allow = NAMED_ALLOW_CALL(*m, func())
    .SIDE_EFFECT(++call_count[0])
    .RETURN(0);
  auto lock = get_lock();
  obj = m;
}

void make(size_t count)
{
  while (count--)
  {
    init_obj();
  }
}

void call(size_t count)
{
  while (count--)
  {
    if (auto m = get_obj())
    {
      ret_count[m->func()]++;
    }
  }
}

void allow(size_t count, int id)
{
  while (count--)
  {
    if (auto m = get_obj())
    {
      ALLOW_CALL(*m, func())
        .SIDE_EFFECT(++call_count[id])
        .RETURN(id);
    }
  }
}

int main()
{
  trompeloeil::set_reporter([](auto s, auto file, auto line, auto& msg)
    {
      abort();
    });
  init_obj();

  size_t count = 100000;
  std::vector<std::thread> allowers;
  std::vector<std::thread> callers;
  allowers.reserve(6);
  callers.reserve(6);
  auto maker = std::thread(make, count);
  for (int i = 1; i <= 6; ++i)
  {
    allowers.emplace_back(allow, count, i);
    callers.emplace_back(call, count);
  }
  for (auto& t : callers) t.join();
  for (auto& t : allowers) t.join();
  maker.join();
  std::cout << "calls   ";
  std::copy(std::begin(call_count), std::end(call_count),
            std::ostream_iterator<std::size_t>(std::cout, " "));
  std::cout << "\nreturns ";
  std::copy(std::begin(ret_count), std::end(ret_count),
            std::ostream_iterator<std::size_t>(std::cout, " "));
  std::cout << "\n";
  obj.reset();
}
