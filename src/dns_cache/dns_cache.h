#pragma once

#include <algorithm>
#include <list>
#include <map>
#include <shared_mutex>
#include <string>
#include <vector>

class DNSCache final
{
    friend class DNS_Cache_ReadNotExistingValue_Test;
    friend class DNS_Cache_WriteReadValue_Test;
    friend class DNS_Cache_WriteReadValues_Test;
    friend class DNS_Cache_Overflow_Test;
    friend class DNS_Cache_OverrideIp_Test;
    friend class DNS_Cache_OneThreadWritesSecondReads_Test;
    friend class DNS_Cache_SeveralThreadsWriteSeveralRead_Test;

public:
    static DNSCache& get_instance();
    DNSCache(const DNSCache& rhs) = delete;
    DNSCache& operator=(const DNSCache& rhs) = delete;
    void update(const std::string& name, const std::string& ip);
    std::string resolve(const std::string& name);

private:
    explicit DNSCache(std::size_t max_size);
    struct ip_info
    {
        uint64_t m_timestamp_counter;
        std::string m_dns;
        std::string m_ip;
    };

    struct bucket
    {
        std::list<ip_info> m_ips;
        std::shared_mutex m_mutex{};

        auto find_ip(const std::string& name)
        {
            return std::find_if(m_ips.begin(), m_ips.end(), [&name](const auto& value) { return value.m_dns == name; });
        }
    };

    struct entry_info
    {
        std::size_t m_bucket_idx;
        std::list<ip_info>::iterator m_iterator;
    };

    std::size_t get_bucket_idx(const std::string& name) const;
    bucket& get_bucket(const std::string& name);

    std::vector<bucket> m_buckets;
    std::hash<std::string> m_hasher;
    std::map<uint64_t, entry_info> m_timestamps;
    std::mutex m_timestamps_mutex;
    int64_t m_timestamp_counter{0};
};
