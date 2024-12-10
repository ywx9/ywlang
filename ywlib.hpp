#pragma once

#if defined(YWLIB_IMPORT) && YWLIB_IMPORT
#pragma message ("YWLIB_IMPORT")
import ywlib;
#else
#pragma message ("not YWLIB_IMPORT")
#include "ywstd.hpp"

export namespace yw {

inline const int number = 1234;

}

#endif
