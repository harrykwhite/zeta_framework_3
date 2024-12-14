#include <zf3_rand.h>

namespace zf3 {

static bool i_rngInitialized;

void init_rng() {
    srand(time(NULL));
    i_rngInitialized = true;
}

int gen_rand_int(const int min, const int max) {
    assert(i_rngInitialized);
    assert(max >= min);

    return min + (rand() % (max - min + 1));
}

float gen_rand_float(const float min, const float max) {
    assert(i_rngInitialized);
    assert(max >= min);

    const float range = max - min;
    return min + (range * (static_cast<float>(rand()) / RAND_MAX));
}

float gen_rand_perc() {
    assert(i_rngInitialized);
    return static_cast<float>(rand()) / RAND_MAX;
}

}
