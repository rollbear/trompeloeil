#include <trompeloeil.hpp>
#include <iterator>
#include <thread>
#include <atomic>
#include <iostream>
#include <cassert>

class C
{
public:
  MAKE_MOCK0(func, int(void));
  std::unique_ptr<trompeloeil::expectation> allow;
};

static std::mutex ptr_mutex;
inline auto get_lock()
{
  return std::unique_lock<std::mutex>{ ptr_mutex };
}

static std::shared_ptr<C> obj;

inline std::shared_ptr<C> get_obj()
{
  auto lock = get_lock();
  return obj;
}

static std::atomic<std::size_t> call_count[7];
static std::atomic<std::size_t> ret_count[7];

static void init_obj()
{
  auto m = std::make_shared<C>();
  m->allow = NAMED_ALLOW_CALL(*m, func())
    .SIDE_EFFECT(++call_count[0])
    .RETURN(0);
  auto lock = get_lock();
  obj = m;
}

static void make(size_t count)
{
  while (count--)
  {
    init_obj();
  }
}

static void call(size_t count)
{
  while (count--)
  {
    if (auto m = get_obj())
    {
      ret_count[m->func()]++;
    }
  }
}

static void allow(size_t count, int id)
{
  std::unique_ptr<trompeloeil::expectation> exp;
  while (count--)
  {
    if (auto m = get_obj())
    {
      exp = NAMED_ALLOW_CALL(*m, func())
        .SIDE_EFFECT(++call_count[id])
        .RETURN(id);
      assert(exp->is_satisfied());
      assert(!exp->is_saturated());
    }
  }
}

int main()
{
  trompeloeil::set_reporter([](auto , auto , auto,  auto&)
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
  assert(std::equal(std::begin(call_count), std::end(call_count),
                    std::begin(ret_count), std::end(ret_count)));
  obj.reset();
}
