#include <benchmark/benchmark.h>

static void BM_Baseline(benchmark::State& state)
{
}

BENCHMARK(BM_Baseline)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
