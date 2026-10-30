#ifndef XMRIG_CANDIDATE_CONFIG_H
#define XMRIG_CANDIDATE_CONFIG_H

#include <cstdint>
#include <string>

namespace xmrig {

class CandidateConfig
{
public:
    bool parseRpc(const std::string &url);

    bool enabled() const { return m_enabled; }
    bool tls() const { return m_tls; }
    uint16_t port() const { return m_port; }
    uint32_t refreshSeconds() const { return m_refreshSeconds; }
    uint32_t maxStaleSeconds() const { return m_maxStaleSeconds; }
    const std::string &host() const { return m_host; }
    const std::string &path() const { return m_path; }
    const std::string &endpoint() const { return m_endpoint; }

    void setRefresh(uint64_t seconds);
    void setMaxStale(uint64_t seconds);

private:
    bool m_enabled = false;
    bool m_tls = false;
    uint16_t m_port = 0;
    uint32_t m_refreshSeconds = 30;
    uint32_t m_maxStaleSeconds = 120;
    std::string m_host;
    std::string m_path = "/get_info";
    std::string m_endpoint;
};

} // namespace xmrig

#endif
