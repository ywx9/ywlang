#include "ywstd.hpp"
#include "ywlib.hpp"
using namespace yw;
#define nat size_t
#define fat double
#if !defined(YWSTD_IMPORT) || !YWSTD_IMPORT
#warning "YWSTD_IMPORT not defined or false"
#include "ywstd.hpp"
#endif

#if !defined(YWLIB_IMPORT) || !YWLIB_IMPORT
#warning "YWLIB_IMPORT not defined or false"
#include "ywlib.hpp"
#endif

int a = 1;
int b = 2;
int c = a + b;

auto aa = intrin::mm_set_ps(1.f, 2.f, 3.f, 4.f);
auto bb = intrin::mm_set_ps(5.f, 6.f, 7.f, 8.f);
auto cc = intrin::mm_add_ps(aa, bb);

int main() {}
