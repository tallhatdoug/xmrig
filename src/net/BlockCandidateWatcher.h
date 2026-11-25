#ifndef XMRIG_BLOCK_CANDIDATE_WATCHER_H
#define XMRIG_BLOCK_CANDIDATE_WATCHER_H

#include "base/kernel/interfaces/IHttpListener.h"
#include "core/config/CandidateConfig.h"

#include <cstdint>
#include <memory>
#include <string>

namespace xmrig {

class HttpListener;
class JobResult;

class BlockCandidateWatcher : public IHttpListener
{
public:
    explicit BlockCandidateWatcher(const CandidateConfig &config);
    ~BlockCandidateWatcher() override;

    void start();
    void tick(uint64_t now);
    void onJobHeight(uint64_t height);
    void check(const JobResult &result);

protected:
    void onHttpData(const HttpData &data) override;

private:
    bool healthy(uint64_t now) const;
    void fetchDifficulty();
    void warn(const std::string &message);

    CandidateConfig m_config;
    std::shared_ptr<HttpListener> m_httpListener;
    bool m_inFlight = false;
    bool m_hasDifficulty = false;
    bool m_synchronized = false;
    uint64_t m_difficulty = 0;
    uint64_t m_height = 0;
    uint64_t m_updatedAt = 0;
    uint64_t m_nextFetchAt = 0;
    uint64_t m_lastWarningAt = 0;
    uint64_t m_lastJobHeight = 0;
    uint64_t m_alertCount = 0;
    std::string m_nettype;
};

} // namespace xmrig

#endif
