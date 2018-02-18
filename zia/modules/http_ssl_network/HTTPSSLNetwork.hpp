//
// Created by doom on 09/02/18.
//

#ifndef ZIA_HTTPSSLNETWORK_HPP
#define ZIA_HTTPSSLNETWORK_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ssl.hpp>
#include <api/net.h>
#include <core/log/Logger.hpp>
#include "RoundRobinIOContextPool.hpp"
#include "HTTPSSLConnection.hpp"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace zia::modules
{
    class HTTPSSLNetwork : public api::Net
    {
    public:
        HTTPSSLNetwork() noexcept;
        ~HTTPSSLNetwork() override = default;
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
        asio::ssl::context _sslContext;
        std::unique_ptr<HTTPSSLConnection> _nextConn;
        Callback _reqCallback;
        unsigned short _port;

        using ConnectionID = api::ImplSocket *;
        std::unordered_map<ConnectionID, HTTPSSLConnection::Pointer> _connectedClients;
    };
}

#endif //ZIA_HTTPSSLNETWORK_HPP
