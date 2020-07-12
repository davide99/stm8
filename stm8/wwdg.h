/*
 * Window watchdog
 */

#ifndef STM8_WWDG_H
#define STM8_WWDG_H

#include "common.h"

#define WWDG_CR __RMM(0xD1)
#define WWDG_WR __RMM(0xD2)

#define reset() (WWDG_CR = 0xC0)

#endif