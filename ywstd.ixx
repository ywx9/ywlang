module;
#ifndef YWSTD_COMPILE
#define YWSTD_COMPILE false
#endif
#include "ywstd.hpp"
export module ywstd;
static_assert(YWSTD_COMPILE, "This source file is used for compiling `module ywstd`");
