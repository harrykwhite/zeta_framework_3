#pragma once

#include <stdlib.h>
#include <time.h>
#include <assert.h>

namespace zf3 {

void init_rng();
int gen_rand_int(const int min, const int max);
float gen_rand_float(const float min, const float max);
float gen_rand_perc();

}