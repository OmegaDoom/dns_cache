#include "dns_cache.h"

#include <mutex>

DNSCache::DNSCache(std::size_t max_size) : m_buckets(max_size)
{
}

DNSCache& DNSCache::get_instance()
{
    static DNSCache instance{0xff};
    return instance;
}

void DNSCache::update(const std::string& name, const std::string& ip)
{
    std::lock_guard lock(m_timestamps_mutex);
    uint16_t current_timestamp_counter = ++m_timestamp_counter;

    const auto bucket_idx = get_bucket_idx(name);
    auto& bucket = m_buckets[bucket_idx];
    std::unique_lock bucket_lock(bucket.m_mutex);

    const auto iter = bucket.find_ip(name);
    const bool is_add_entry = bucket.m_ips.end() == iter;
    if (is_add_entry)
    {
        bucket.m_ips.push_front({current_timestamp_counter, name, ip});
        m_timestamps[current_timestamp_counter] = {bucket_idx, bucket.m_ips.begin()};
        if (m_timestamps.size() > m_buckets.size())
        {
            const auto oldest_timestamp_iter = m_timestamps.begin();
            const auto& entry_info = oldest_timestamp_iter->second;
            auto& bucket_with_enty_to_delete = m_buckets[entry_info.m_bucket_idx];
            std::unique_lock bucket_with_entry_to_delete_lock{bucket_with_enty_to_delete.m_mutex, std::defer_lock};
            if (entry_info.m_bucket_idx != bucket_idx)
            {
                bucket_lock.unlock();
                bucket_with_entry_to_delete_lock.lock();
            }
            bucket_with_enty_to_delete.m_ips.erase(entry_info.m_iterator);
            m_timestamps.erase(oldest_timestamp_iter);
        }
    }
    else
    {
        m_timestamps.erase(iter->m_timestamp_counter);
        *iter = {current_timestamp_counter, name, ip};
        m_timestamps[current_timestamp_counter] = {bucket_idx, iter};
    }
}

std::string DNSCache::resolve(const std::string& name)
{
    auto& bucket = get_bucket(name);
    std::shared_lock lock(bucket.m_mutex);

    const auto iter = bucket.find_ip(name);
    return bucket.m_ips.end() == iter ? "" : iter->m_ip;
}

std::size_t DNSCache::get_bucket_idx(const std::string& name) const
{
    const auto hash = m_hasher(name);
    return hash % m_buckets.size();
}

DNSCache::bucket& DNSCache::get_bucket(const std::string& name)
{
    return m_buckets[get_bucket_idx(name)];
}
