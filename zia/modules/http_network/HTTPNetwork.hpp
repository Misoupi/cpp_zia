//
// Created by doom on 09/02/18.
//

#ifndef ZIA_HTTPNETWORK_HPP
#define ZIA_HTTPNETWORK_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <api/net.h>
#include <core/log/Logger.hpp>
#include "RoundRobinIOContextPool.hpp"
#include "HTTPConnection.hpp"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace zia::modules
{
    class HTTPNetwork : public api::Net
    {
    public:
        HTTPNetwork() noexcept;
        ~HTTPNetwork() override = default;
        bool config(const api::Conf &conf) override;
        bool run(Callback cb) override;
        bool send(api::ImplSocket *sock, const Raw &resp) override;
        bool stop() override;

    private:
        bool _setupAcceptor() noexcept;
        void _startAcceptor() noexcept;
        void _handleAccept(const boost::system::error_code &ec) noexcept;
        void _handleRequest(const boost::system::error_code &ec, Net::Raw raw, api::NetInfo info);

    private:
        logging::Logger _log{"network", logging::Debug};
        RoundRobinIOContextPool _iosPool;
        asio::signal_set _sigSet;
        tcp::acceptor _acceptor;
        std::unique_ptr<HTTPConnection> _nextConn;
        Callback _reqCallback;
        unsigned short _port;

        using ConnectionID = api::ImplSocket *;
        std::unordered_map<ConnectionID, HTTPConnection::Pointer> _connectedClients;
        std::mutex _connectedClientsMutex;
    };
}

#endif //ZIA_HTTPNETWORK_HPP
