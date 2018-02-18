//
// Created by doom on 09/02/18.
//

#include <iostream>
#include <core/lib/Alias.hpp>
#include "HTTPSSLNetwork.hpp"

namespace zia::api
{
    struct ImplSocket
    {
    };
}

namespace zia::modules
{
    HTTPSSLNetwork::HTTPSSLNetwork() noexcept : _iosPool(6u),
                                                _sigSet(_iosPool.getContext(), SIGINT, SIGTERM),
                                                _acceptor(_iosPool.getContext()),
                                                _sslContext(asio::ssl::context::sslv23)
    {
    }

#define fail_if(x)                                                          \
    if (x) {                                                                \
        return false;                                                       \
    }

    bool HTTPSSLNetwork::_setupAcceptor() noexcept
    {
        boost::system::error_code ec;

        if (_acceptor.is_open()) {
            _acceptor.close(ec);
            fail_if(ec);
        }
        _acceptor.open(tcp::v4(), ec);
        fail_if(ec);
        _acceptor.set_option(tcp::acceptor::reuse_address(true));
        _acceptor.bind(tcp::endpoint(tcp::v4(), _port), ec);
        fail_if(ec);
        _acceptor.listen(tcp::socket::max_connections, ec);
        fail_if(ec);
        return true;
    }

    void HTTPSSLNetwork::_handleRequest(const boost::system::error_code &ec, Net::Raw raw, api::NetInfo info)
    {
        if (!ec) {
            _reqCallback(std::move(raw), std::move(info));
            ConnectionID id = info.sock;
            _connectedClients[id]->readRequest(boost::bind(&HTTPSSLNetwork::_handleRequest, this, _1, _2, _3));
        } else {
            ConnectionID id = info.sock;
            _connectedClients.erase(id);
        }
    }

    void HTTPSSLNetwork::_handleAccept(const boost::system::error_code &ec) noexcept
    {
        if (!ec) {
            _log(logging::Debug) << "Received a new connection" << std::endl;
            auto id = reinterpret_cast<ConnectionID>(_nextConn.get());
            auto pair = _connectedClients.emplace(id, HTTPSSLConnection::Pointer(_nextConn.release())).first;
            pair->second->readRequest(boost::bind(&HTTPSSLNetwork::_handleRequest, this, _1, _2, _3));
        }
        _startAcceptor();
    }

    void HTTPSSLNetwork::_startAcceptor() noexcept
    {
        _nextConn.reset(new HTTPSSLConnection(_iosPool.getContext(), _sslContext));
        _acceptor.async_accept(_nextConn->socket().lowest_layer(), [this](const boost::system::error_code &ec) {
            this->_handleAccept(ec);
        });
    }

    bool HTTPSSLNetwork::config(const zia::api::Conf &conf)
    {
        auto it = conf.find("port");

        if (it != conf.end() && std::holds_alternative<long long>(it->second.v)) {
            _port = static_cast<unsigned short>(std::get<long long>(it->second.v));
        } else
            _port = 80;

        auto keyIt = conf.find("ssl_key");
        auto certIt = conf.find("ssl_cert");
        fail_if(keyIt == conf.end());
        fail_if(certIt == conf.end());
        fail_if(!std::holds_alternative<std::string>(keyIt->second.v));
        fail_if(!std::holds_alternative<std::string>(certIt->second.v));
        boost::system::error_code ec;
        _sslContext.use_private_key_file(std::get<std::string>(keyIt->second.v), asio::ssl::context::pem, ec);
        fail_if(ec);
        _sslContext.use_certificate_chain_file(std::get<std::string>(certIt->second.v), ec);
        fail_if(ec);
        return true;
    }

#undef fail_if

    bool HTTPSSLNetwork::run(api::Net::Callback cb)
    {
        _reqCallback = std::move(cb);
        _sigSet.async_wait([this](const boost::system::error_code &, int) {
            this->stop();
        });
        if (!_setupAcceptor())
            return false;
        _log(logging::Debug) << "Ready to accept connections on port " << _port << std::endl;
        _startAcceptor();
        _iosPool.run();
        return true;
    }

    bool HTTPSSLNetwork::send(ConnectionID id, const api::Net::Raw &buffer)
    {
        auto ptr = _connectedClients[id];
        //TODO: compare performance with asynchronous writes
        boost::system::error_code ec;
        size_t written = 0;
        while (written < buffer.size() && !ec) {
            auto buf = asio::buffer(buffer.data() + written, buffer.size() - written);
            written += ptr->socket().write_some(buf, ec);;
        }
        _log(logging::Debug) << "Done writing: " << ec.message() << std::endl;
        return true;
    }

    bool HTTPSSLNetwork::stop()
    {
        _iosPool.stop();
        return true;
    }

    api::Net *create() noexcept
    {
        return new HTTPSSLNetwork;
    }
}

lib_alias_function(zia::modules::create, create);
