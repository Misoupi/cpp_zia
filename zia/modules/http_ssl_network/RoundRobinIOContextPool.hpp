//
// Created by doom on 11/02/18.
//

#ifndef ZIA_ROUNDROBINIOSERVICEPOOL_HPP
#define ZIA_ROUNDROBINIOSERVICEPOOL_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace asio = boost::asio;

namespace zia::modules
{
    class RoundRobinIOContextPool
    {
    public:
        explicit RoundRobinIOContextPool(size_t size) noexcept;
        void run() noexcept;
        void stop() noexcept;
        asio::io_context &getContext() noexcept;

    private:
        using IOContextPtr = boost::shared_ptr<asio::io_context>;
        using IOWorkGuard = asio::executor_work_guard<asio::io_context::executor_type>;

        std::vector<IOContextPtr> _ios;
        std::vector<IOWorkGuard> _workGuards;
        size_t _nextIdx{0};
    };
}

#endif //ZIA_ROUNDROBINIOSERVICEPOOL_HPP
