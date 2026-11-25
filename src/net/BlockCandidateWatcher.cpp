#include "net/BlockCandidateWatcher.h"

#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/net/http/HttpData.h"
#include "base/net/http/HttpListener.h"
#include "base/tools/Chrono.h"
#include "base/tools/Cvt.h"
#include "net/JobResult.h"

#ifdef XMRIG_FEATURE_HTTP
#   include "base/net/http/Fetch.h"
#endif

#include <cinttypes>
#include <cstring>
#include <stdexcept>

namespace xmrig {

BlockCandidateWatcher::BlockCandidateWatcher(const CandidateConfig &config) :
    m_config(config)
{
    m_httpListener = std::make_shared<HttpListener>(this);
}

BlockCandidateWatcher::~BlockCandidateWatcher() = default;

void BlockCandidateWatcher::start()
{
    if (!m_config.enabled()) {
        LOG_NOTICE("%s Block candidates: disabled", Tags::network());
        return;
    }

    LOG_NOTICE("%s Block candidates: enabled rpc=%s refresh=%us max-stale=%us",
               Tags::network(), m_config.endpoint().c_str(),
               m_config.refreshSeconds(), m_config.maxStaleSeconds());
    fetchDifficulty();
}

bool BlockCandidateWatcher::healthy(uint64_t now) const
{
    if (!m_config.enabled() || !m_hasDifficulty || !m_synchronized ||
        m_difficulty == 0 || now < m_updatedAt) {
        return false;
    }

    return now - m_updatedAt <=
           static_cast<uint64_t>(m_config.maxStaleSeconds()) * 1000;
}

void BlockCandidateWatcher::tick(uint64_t now)
{
    if (m_config.enabled() && !m_inFlight && now >= m_nextFetchAt) {
        fetchDifficulty();
    }
}

void BlockCandidateWatcher::onJobHeight(uint64_t height)
{
    if (!m_config.enabled() || height == 0 || height == m_lastJobHeight) {
        return;
    }

    m_lastJobHeight = height;
    if (!m_inFlight) fetchDifficulty();
}

void BlockCandidateWatcher::fetchDifficulty()
{
    if (!m_config.enabled() || m_inFlight) return;

#ifdef XMRIG_FEATURE_HTTP
    m_inFlight = true;
    try {
        FetchRequest request(
            HTTP_GET, m_config.host().c_str(), m_config.port(),
            m_config.path().c_str(), m_config.tls(), true);
        request.timeout = 5000;
        fetch("netdiff", std::move(request), m_httpListener);
    }
    catch (const std::exception &error) {
        m_inFlight = false;
        warn(error.what());
    }
#else
    warn("HTTP support is unavailable; candidate alerts are disabled");
#endif
}

void BlockCandidateWatcher::warn(const std::string &message)
{
    const uint64_t now = Chrono::steadyMSecs();
    if (m_lastWarningAt != 0 && now - m_lastWarningAt < 60000) return;
    m_lastWarningAt = now;
    LOG_WARN("%s [netdiff] %s; mining continues", Tags::network(),
             message.c_str());
}

void BlockCandidateWatcher::onHttpData(const HttpData &data)
{
    m_inFlight = false;
    const uint64_t now = Chrono::steadyMSecs();
    m_nextFetchAt = now +
        static_cast<uint64_t>(m_config.refreshSeconds()) * 1000;

    try {
        if (data.status != 200) {
            throw std::runtime_error("daemon returned a non-200 response");
        }

        rapidjson::Document doc = data.json();
        const char *status = Json::getString(doc, "status");
        if (!status || strcmp(status, "OK") != 0) {
            throw std::runtime_error("daemon status is not OK");
        }
        if (!Json::getBool(doc, "synchronized")) {
            throw std::runtime_error("daemon is not synchronized");
        }

        const uint64_t difficulty = Json::getUint64(doc, "difficulty");
        const uint64_t height = Json::getUint64(doc, "height");
        const uint64_t difficultyTop64 = Json::getUint64(doc, "difficulty_top64");
        if (difficulty == 0 || height == 0 || difficultyTop64 != 0) {
            throw std::runtime_error("daemon difficulty is invalid or unsupported");
        }

        const bool changed = !m_hasDifficulty || m_height != height ||
                             m_difficulty != difficulty;
        m_difficulty = difficulty;
        m_height = height;
        m_synchronized = true;
        m_hasDifficulty = true;
        m_updatedAt = now;
        const char *nettype = Json::getString(doc, "nettype");
        m_nettype = nettype ? nettype : "unknown";

        if (changed) {
            LOG_NOTICE("%s [netdiff] ready height=%" PRIu64
                       " difficulty=%" PRIu64 " nettype=%s",
                       Tags::network(), height, difficulty, m_nettype.c_str());
        }
    }
    catch (const std::exception &error) {
        warn(error.what());
    }
}

void BlockCandidateWatcher::check(const JobResult &result)
{
    const uint64_t now = Chrono::steadyMSecs();
    if (result.algorithm != Algorithm::RX_0 || !healthy(now)) return;

    const uint64_t actual = result.actualDiff();
    if (actual < m_difficulty) return;

    ++m_alertCount;
    char nonce[9];
    char hash[65];
    const uint32_t value = static_cast<uint32_t>(result.nonce);
    Cvt::toHex(nonce, sizeof(nonce),
               reinterpret_cast<const uint8_t *>(&value), sizeof(value));
    Cvt::toHex(hash, sizeof(hash), result.result(), 32);

    LOG_WARN("%s\n"
        "============================================================\n"
        "***             POTENTIAL BLOCK CANDIDATE                ***\n"
        "============================================================\n"
        "  actual difficulty : %" PRIu64 "\n"
        "  network difficulty: %" PRIu64 "\n"
        "  job               : %s\n"
        "  nonce             : %s\n"
        "  result            : %s\n"
        "  alert             : %" PRIu64 "\n"
        "============================================================",
        Tags::network(), actual, m_difficulty, result.jobId.data(),
        nonce, hash, m_alertCount);
}

} // namespace xmrig
