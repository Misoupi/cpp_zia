//
// Created by doom on 20/02/18.
//

#ifndef ZIA_RESPONSEGZIPCOMPRESSOR_HPP
#define ZIA_RESPONSEGZIPCOMPRESSOR_HPP

#include <api/module.h>

namespace zia::modules
{
    class ResponseGZipCompressor : public api::Module
    {
    public:
        ~ResponseGZipCompressor() override = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        bool _isReqAcceptingGzip(api::HttpDuplex &http) const noexcept;

    private:
        size_t _minSize;
    };
}

#endif //ZIA_RESPONSEGZIPCOMPRESSOR_HPP
