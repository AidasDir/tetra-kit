#ifndef CALLIDENTIFIER_H
#define CALLIDENTIFIER_H
#include <cstdint>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Call identifier class
 *
 * TODO handle Txxx timers
 *
 */

class call_identifier_t {
public:
    call_identifier_t(uint32_t cid);
    ~call_identifier_t();

    uint32_t m_cid;                                                             ///< CID value
    uint8_t m_usage_marker;                                                     ///< Usage marker
    double m_data_received;                                                     ///< Data received in Kb

    static const     int    MAX_USAGES         = 64;                            ///< maximum usages defined by norm
    static constexpr double TIMEOUT_S          = 30.0;                          ///< maximum timeout between messages TODO handle Txxx timers
    static constexpr double TIMEOUT_RELEASE_S  = 120.0;                         ///< maximum timeout before releasing the usage_marker (garbage collector)

    string m_file_name[MAX_USAGES];                                             ///< File names to use for usage marker/cid
    time_t m_last_traffic_time[MAX_USAGES];                                     ///< Last traffic seen to know when to start new record

    vector<ssi_t> m_ssi;                                                        ///< List of SSI associated with this cid

    void clean_up();                                                            ///< Garbage collector release the traffic usage marker when timeout exceeds TIMEOUT_RELEASE_S
    void push_traffic(const char * data, uint32_t len);
    void update_usage_marker(uint8_t usage_marker);
};


#endif /* CALLIDENTIFIER_H */
