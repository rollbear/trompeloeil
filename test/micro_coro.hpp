#ifndef TROMPELOEIL_MICRO_CORO_HPP
#define TROMPELOEIL_MICRO_CORO_HPP

#include <coroutine>
#include <variant>
#include <exception>
#include <cstdlib>

namespace coro
{
  template<typename>
  struct task;

  template<typename T>
  struct promise
  {
    promise() noexcept = default;

    promise &operator=(promise &&) = delete;

    std::suspend_never
    initial_suspend()
      const
      noexcept
    {
      return {};
    }

    std::suspend_always
    final_suspend()
      const
      noexcept
    {
      return {};
    }

    template<typename P>
    std::coroutine_handle<>
    await_suspend(std::coroutine_handle <P> p)
      noexcept
    {
      if (p.promise().continuation_)
      {
        return p.promise().continuation_;
      }
      return std::noop_coroutine();
    }

    task<T>
    get_return_object()
      noexcept;

    void
    return_value(T t)
    {
      data_.template emplace<data>(std::move(t));
    }

    void
    unhandled_exception()
      noexcept
    {
      data_.template emplace<std::exception_ptr>(std::current_exception());
    }

    const T&
    result()
      const &
    {
      if (auto v = std::get_if<data>(&data_))
      {
        return v->value;
      }
      else if (auto e = std::get_if<std::exception_ptr>(&data_))
      {
        std::rethrow_exception(*e);
      }
      else
      {
        abort();
      }
    }
    T &
    result()
      &
    {
      if (auto v = std::get_if<data>(&data_))
      {
        return v->value;
      }
      else if (auto e = std::get_if<std::exception_ptr>(&data_))
      {
        std::rethrow_exception(*e);
      }
      else
      {
        abort();
      }
    }
    T&&
    result()
      &&
    {
      if (auto v = std::get_if<data>(&data_))
      {
        return std::move(v->value);
      }
      else if (auto e = std::get_if<std::exception_ptr>(&data_))
      {
        std::rethrow_exception(*e);
      }
      else
      {
        abort();
      }
    }

    void
    continuation(std::coroutine_handle<> next)
      noexcept
    {
      continuation_ = next;
    }
  private:
    struct data
    {
      data(T&& t) : value(std::move(t)) {}
      T value;
    };
    std::variant <std::monostate, data, std::exception_ptr> data_;
    std::coroutine_handle<> continuation_{nullptr};
  };

  template<>
  struct promise<void>
  {
    promise() noexcept = default;
    promise &operator=(promise &&) = delete;

    std::suspend_never
    initial_suspend()
      const
      noexcept
    {
      return {};
    }

    std::suspend_always
    final_suspend()
      const
      noexcept
    {
      return {};
    }

    template<typename P>
    std::coroutine_handle<>
    await_suspend(std::coroutine_handle <P> p)
      noexcept
    {
      if (p.promise().continuation_)
      {
        return p.promise().continuation_;
      }
      return std::noop_coroutine();
    }

    task<void>
    get_return_object()
      noexcept;

    void
    return_void()
    {
      exception_ = nullptr;
    }

    void
    unhandled_exception()
      noexcept
    {
      exception_ = std::current_exception();
    }

    void
    result()
      const
    {
      if (exception_)
      {
        std::rethrow_exception(exception_);
      }
    }

    void
    continuation(std::coroutine_handle<> next)
      noexcept
    {
      continuation_ = next;
    }
  private:
    std::exception_ptr exception_ = nullptr;
    std::coroutine_handle<> continuation_{nullptr};
  };

  template<typename T>
  struct task {
    using promise_type = coro::promise<T>;

    explicit task(std::coroutine_handle <promise_type> h = nullptr) : coroutine_(h) {}

    task(task &&h) : coroutine_(std::exchange(h.coroutine_, nullptr)) {}

    ~task() {
      if (coroutine_) {
        coroutine_.destroy();
      }
    }

    bool
    is_ready()
      const
      noexcept
    {
      return !coroutine_ || coroutine_.done();
    }

    bool
    resume()
    {
      if (!coroutine_.done())
      {
        coroutine_.resume();
      }
      return !coroutine_.done();
    }

    bool
    destroy()
    {
      if (coroutine_)
      {
        coroutine_.destroy();
        coroutine_ = nullptr;
        return true;
      }
      return false;
    }

    struct awaitable_base
    {
      bool
      await_ready()
      const
      noexcept
      {
        return coroutine_.done();
      }

      std::coroutine_handle<>
      await_suspend(std::coroutine_handle<> next)
      noexcept
      {
        coroutine_.promise().continuation(next);
        return coroutine_;
      }


      std::coroutine_handle<promise_type> coroutine_;
    };

    auto
    operator co_await()
      const &
      noexcept

    {
      struct awaitable : awaitable_base {
        decltype(auto)
        await_resume()
        {
          return awaitable_base::coroutine_.promise().result();
        }

      };
      return awaitable{coroutine_};
    }

    auto
      operator co_await()
    const &&
    noexcept

    {
      struct awaitable : awaitable_base {
        decltype(auto)
        await_resume()
        {
          return std::move(awaitable_base::coroutine_.promise()).result();
        }

      };
      return awaitable{coroutine_};
    }

    promise_type&
    promise()
      const
    {
      return coroutine_.promise();
    }

  private:
    std::coroutine_handle <promise_type> coroutine_;
  };

  template<typename T>
  task<T>
  promise<T>::get_return_object()
    noexcept
  {
    return task<T>{std::coroutine_handle<promise>::from_promise(*this)};
  }

  task<void>
  promise<void>::get_return_object()
    noexcept
  {
    return task<void>{std::coroutine_handle<promise>::from_promise(*this)};
  }

} // namespace coro

#endif //TROMPELOEIL_MICRO_CORO_HPP
