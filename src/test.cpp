#include <gtest/gtest.h>
#include <thread>

#include "dns_cache/dns_cache.h"

TEST(DNS_Cache, ReadNotExistingValue)
{
    DNSCache cache{0xff};
    EXPECT_EQ("", cache.resolve("a"));
}

TEST(DNS_Cache, WriteReadValue)
{
    DNSCache cache{0xff};
    cache.update("a", "1.1.1.1");
    EXPECT_EQ("1.1.1.1", cache.resolve("a"));
}

TEST(DNS_Cache, WriteReadValues)
{
    DNSCache cache{0xff};
    cache.update("a", "1.1.1.1");
    cache.update("b", "2.2.2.2");
    EXPECT_EQ("1.1.1.1", cache.resolve("a"));
    EXPECT_EQ("2.2.2.2", cache.resolve("b"));
}

TEST(DNS_Cache, Overflow)
{
    DNSCache cache{0x1};
    cache.update("a", "1.1.1.1");
    cache.update("b", "2.2.2.2");
    EXPECT_EQ("", cache.resolve("a"));
    EXPECT_EQ("2.2.2.2", cache.resolve("b"));
}

TEST(DNS_Cache, OverrideIp)
{
    DNSCache cache{0xff};
    cache.update("a", "1.1.1.1");
    cache.update("a", "2.2.2.2");
    EXPECT_EQ("2.2.2.2", cache.resolve("a"));
}

TEST(DNS_Cache, OneThreadWritesSecondReads)
{
    DNSCache cache{0xffff};

    int counter = 0xffff;

    auto resolve = [&cache, &counter]() {
        for (int i = 0; i < 0xffff; ++i)
        {
            std::string ip = "";
            int max_count = 0xfffff;
            while (ip == "" && max_count != 0)
            {
                --max_count;
                ip = cache.resolve(std::to_string(i));
                if (ip == std::to_string(i))
                {
                    --counter;
                }
            }
        }
    };

    auto update = [&cache]() {
        for (int i = 0; i < 0xffff; ++i)
        {
            cache.update(std::to_string(i), std::to_string(i));
        }
    };

    std::thread th0{resolve};
    std::thread th1{update};
    th0.join();
    th1.join();

    EXPECT_EQ(0, counter);
}

TEST(DNS_Cache, SeveralThreadsWriteSeveralRead)
{
    DNSCache cache{0xff};

    std::atomic<int> counter = 2 * 0xffff;

    auto resolve0 = [&cache, &counter]() {
        for (int i = 0; i < 0xffff; ++i)
        {
            std::string ip = "";
            while (ip == "")
            {
                ip = cache.resolve(std::to_string(i));
                if (ip == std::to_string(i))
                {
                    --counter;
                }
            }
        }
    };

    auto resolve1 = [&cache, &counter]() {
        for (int i = 0; i < 0xffff; ++i)
        {
            std::string ip = "";
            while (ip == "")
            {
                ip = cache.resolve(std::to_string(i + 0xffff));
                if (ip == std::to_string(i + 0xffff))
                {
                    --counter;
                }
            }
        }
    };

    auto update0 = [&cache, &counter]() {
        while (counter != 0)
        {
            for (int i = 0; i < 0xffff; ++i)
            {
                cache.update(std::to_string(i), std::to_string(i));
            }
        }
    };

    auto update1 = [&cache, &counter]() {
        while (counter != 0)
        {
            for (int i = 0; i < 0xffff; ++i)
            {
                cache.update(std::to_string(i + 0xffff), std::to_string(i + 0xffff));
            }
        }
    };

    std::thread th0{resolve0};
    std::thread th1{resolve1};
    std::thread th2{update0};
    std::thread th3{update1};
    th0.join();
    th1.join();
    th2.join();
    th3.join();

    EXPECT_EQ(0, counter);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
