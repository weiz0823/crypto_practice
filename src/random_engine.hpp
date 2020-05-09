#pragma once
#include <random>
namespace cryp {
// use one random engine for every instance
inline std::random_device g_rnd_dev;
inline std::mt19937 g_rnd_engine32(g_rnd_dev());
inline std::mt19937_64 g_rnd_engine64(g_rnd_dev());
}  // namespace cryp
