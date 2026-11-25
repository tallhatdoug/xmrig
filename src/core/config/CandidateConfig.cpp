#include "core/config/CandidateConfig.h"

#include <cctype>
#include <cstdlib>

namespace {

uint32_t clamp32(uint64_t value)
{
    if (value < 1) return 1;
    if (value > 86400) return 86400;
    return static_cast<uint32_t>(value);
}

bool parsePort(const std::string &text, uint16_t &result)
{
    if (text.empty() || text.size() > 5) return false;
    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    const unsigned long value = std::strtoul(text.c_str(), nullptr, 10);
    if (value == 0 || value > 65535) return false;
    result = static_cast<uint16_t>(value);
    return true;
}

} // namespace

namespace xmrig {

void CandidateConfig::setRefresh(uint64_t seconds)
{
    m_refreshSeconds = clamp32(seconds);
}

void CandidateConfig::setMaxStale(uint64_t seconds)
{
    m_maxStaleSeconds = clamp32(seconds);
}

bool CandidateConfig::parseRpc(const std::string &url)
{
    m_enabled = false;
    m_tls = false;
    m_port = 0;
    m_host.clear();
    m_path = "/get_info";
    m_endpoint.clear();

    if (url.empty()) return true;
    if (url.size() > 2048) return false;

    std::string rest;
    std::string scheme;
    if (url.compare(0, 7, "http://") == 0) {
        scheme = "http://";
        rest = url.substr(7);
    }
    else if (url.compare(0, 8, "https://") == 0) {
        scheme = "https://";
        m_tls = true;
        rest = url.substr(8);
    }
    else {
        return false;
    }

    const size_t slash = rest.find('/');
    const std::string authority = rest.substr(0, slash);
    if (authority.empty() || authority.find('@') != std::string::npos) {
        return false;
    }

    if (slash != std::string::npos) {
        m_path = rest.substr(slash);
        if (m_path.empty() || m_path.size() > 1024) return false;
    }

    const uint16_t defaultPort = m_tls ? 443 : 18081;
    std::string portText;

    if (authority[0] == '[') {
        const size_t end = authority.find(']');
        if (end == std::string::npos || end < 2) return false;
        m_host = authority.substr(1, end - 1);
        const std::string remainder = authority.substr(end + 1);
        if (!remainder.empty()) {
            if (remainder[0] != ':') return false;
            portText = remainder.substr(1);
        }
    }
    else {
        const size_t colon = authority.rfind(':');
        if (colon == std::string::npos) {
            m_host = authority;
        }
        else {
            if (authority.find(':') != colon) return false;
            m_host = authority.substr(0, colon);
            portText = authority.substr(colon + 1);
        }
    }

    if (m_host.empty() || m_host.size() > 255) return false;
    m_port = defaultPort;
    if (!portText.empty() && !parsePort(portText, m_port)) return false;

    m_endpoint = scheme + authority + m_path;
    m_enabled = true;
    return true;
}

} // namespace xmrig
