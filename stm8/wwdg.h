/*
 * Window watchdog
 */

#include "common.h"

#define WWDG_CR __RMM(0xD1)
#define WWDG_WR __RMM(0xD2)

#define reset() (WWDG_CR = 0xC0)