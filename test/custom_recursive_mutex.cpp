#define TROMPELOEIL_CUSTOM_RECURSIVE_MUTEX
#include <trompeloeil.hpp>

namespace trompeloeil {

std::unique_ptr<custom_recursive_mutex> create_custom_recursive_mutex() {

  class custom : public custom_recursive_mutex {
    void lock() override { mtx.lock(); }
    void unlock() override { mtx.unlock(); }

  private:
    std::recursive_mutex mtx;
  };

  return std::make_unique<custom>();
}

} // namespace trompeloeil

class C {
public:
  MAKE_MOCK0(func, int(void));
};

int main() {
  C c;
}
