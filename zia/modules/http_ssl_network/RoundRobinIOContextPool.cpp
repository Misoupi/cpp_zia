//
// Created by doom on 11/02/18.
//

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include "RoundRobinIOContextPool.hpp"

namespace zia::modules
{
    RoundRobinIOContextPool::RoundRobinIOContextPool(size_t size) noexcept
    {
        for (size_t i = 0; i < size; ++i) {
            IOContextPtr newContext = boost::make_shared<asio::io_context>();
            _ios.push_back(newContext);
            _workGuards.push_back(asio::make_work_guard(*newContext));
        }
    }

    void RoundRobinIOContextPool::run() noexcept
    {
        std::vector<boost::thread> threads;

        for (size_t i = 0; i < _ios.size(); ++i) {
            threads.emplace_back(boost::bind(&asio::io_context::run, _ios[i]));
        }

        for (auto &curThread : threads) {
            curThread.join();
        }
    }

    void RoundRobinIOContextPool::stop() noexcept
    {
        for (auto &curService : _ios) {
            curService->stop();
        }
    }

    asio::io_context &RoundRobinIOContextPool::getContext() noexcept
    {
        auto &ret = *_ios[_nextIdx++];

        if (_nextIdx == _ios.size()) {
            _nextIdx = 0;
        }
        return ret;
    }
}