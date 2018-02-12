//
// Created by milerius on 07/02/18.
//

#ifndef ZIA_STATICFILE_HPP
#define ZIA_STATICFILE_HPP

#include <experimental/filesystem>
#include <api/module.h>

namespace fs = std::experimental::filesystem;

namespace zia::modules
{
    class StaticFile : public zia::api::Module
    {
    public:
        virtual ~StaticFile() = default;
        bool config(const api::Conf &conf) override;
        bool exec(api::HttpDuplex &http) override;

    private:
        using FileExtension = std::string;
        using MimeType = std::string;
        using FileExtensionMap = std::unordered_map<FileExtension, MimeType>;
        FileExtensionMap _map
            {
                {".aac",   "audio/aac"},
                {".abw",   "application/x-abiword"},
                {".arc",   "application/octet-stream"},
                {".avi",   "video/x-msvdieo"},
                {".azw",   "application/vnd.amazon.ebook"},
                {".bin",   "application/octet-stream"},
                {".bz",    "application/x-bzip"},
                {".bz2",   "application/x-bzip2"},
                {".csh",   "application/x-csh"},
                {".css",   "text/css"},
                {".doc",   "application/msword"},
                {".docx",  "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
                {".eot",   "application/vnd.ms-fontobject"},
                {".epub",  "application/epub+zip"},
                {".gif",   "image/gif"},
                {".htm",   "text/html"},
                {".html",  "text/html"},
                {".ico",   "image/x-icon"},
                {".ics",   "text/calendar"},
                {".jar",   "application/java-archive"},
                {".jpeg",  "image/jpeg"},
                {".jpg",   "image/jpeg"},
                {".js",    "application/javascript"},
                {".json",  "application/json"},
                {".mid",   "audio/midi"},
                {".midi",  "audio/midi"},
                {".mpeg",  "video/mpeg"},
                {".mpkg",  "application/vnd.apple.installer+xml"},
                {".odp",   "application/vnd.oasis.opendocument.presentation"},
                {".ods",   "application/vnd.oasis.opendocument.spreadsheet"},
                {".odt",   "application/vnd.oasis.opendocument.text"},
                {".oga",   "audio/ogg"},
                {".ogg",   "audio/ogg"},
                {".ogv",   "video/ogg"},
                {".ogx",   "application/ogg"},
                {".otf",   "font/otf"},
                {".png",   "image/png"},
                {".pdf",   "application/pdf"},
                {".ppt",   "application/vnd.ms-powerpoint"},
                {".pptx",  "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
                {".rar",   "application/x-rar-compressed"},
                {".rtf",   "application/rtf"},
                {".sh",    "application/x-sh"},
                {".svg",   "image/svg+xml"},
                {".swf",   "application/x-shockwave-flash"},
                {".tar",   "application/x-tar"},
                {".tif",   "image/tiff"},
                {".tiff",  "image/tiff"},
                {".ts",    "application/typescript"},
                {".ttf",   "font/ttf"},
                {".vsd",   "application/vnd.visio"},
                {".wav",   "audio/x-wav"},
                {".weba",  "audio/webm"},
                {".webp",  "image/webp"},
                {".woff",  "font/woff"},
                {".woff2", "font/woff2"},
                {".xtml",  "application/xhtml+xml"},
                {".xls",   "application/vnd.ms-excel"},
                {".xlsx",  "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
                {".xml",   "application/xml"},
                {".xul",   "application/vnd.mozilla.xul+xml"},
                {".zip",   "application/zip"},
                {".3gp",   "audio/3gpp"},
                {".3g2",   "audio/3gpp2"},
                {".7z",    "application/x-7z-compressed"},
                {"",       "text/plain"}
            };
    };
}

#endif //ZIA_STATICFILE_HPP
