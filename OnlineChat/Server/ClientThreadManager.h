#pragma once
#include <thread>
#include <vector>
#include <utility>

// https://www.youtube.com/watch?v=6re5U82KwbY

namespace sockets::server
{
class ClientThreadManager
{
public:
    template<typename Fn, typename... Args>
    void start(Fn&& fn, Args&&... args)
    {
        threads_.emplace_back(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }
    void joinAll();

private:
    std::vector<std::thread> threads_;
};
}
