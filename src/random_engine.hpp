#pragma once
#include <random>
#include "type_aliases.hpp"
namespace cryp {
// use one random engine for every instance
template <typename T>
class RandomNumberGen {
   public:
    virtual T Generate(T l, T r) = 0;
};
inline std::random_device g_rnd_dev;
template <typename T = int>
class DefaultRNG32 final : public RandomNumberGen<T> {
    std::mt19937 rnd_eng_32_ = std::mt19937(g_rnd_dev());
    std::uniform_int_distribution<T> rnd_;
    using param_type = typename std::uniform_int_distribution<T>::param_type;

   public:
    explicit DefaultRNG32(T l = 0, T r = std::numeric_limits<T>::max())
        : rnd_(l, r) {}
    T Generate(T l, T r) override {
        return rnd_(rnd_eng_32_, param_type(l, r));
    }
};
template <typename T = int>
class DefaultRNG64 final : public RandomNumberGen<T> {
    std::mt19937_64 rnd_eng_64_ = std::mt19937_64(g_rnd_dev());
    std::uniform_int_distribution<T> rnd_;
    using param_type = typename std::uniform_int_distribution<T>::param_type;

   public:
    explicit DefaultRNG64(T l = 0, T r = std::numeric_limits<T>::max())
        : rnd_(l, r) {}
    T Generate(T l, T r) override {
        return rnd_(rnd_eng_64_, param_type(l, r));
    }
};
inline DefaultRNG32<ByteT> g_rng32;
inline DefaultRNG64<ByteT> g_rng64;
}  // namespace cryp
