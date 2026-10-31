/* XMRig
 * Copyright (c) 2018-2022 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2022 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_DONATE_H
#define XMRIG_DONATE_H

#include <cstdint>


/*
 * Fork uses slightly different variable-length donation scheduler;
 * donate-level uses 0.2% units. Every donation burst lasts 30 seconds:
 *
 *   level 1: 30 seconds per 250 minutes (0.2%, lowest configurable value without recompiling to 0)
 *   level 2: 30 seconds per 125 minutes (0.4%, default)
 *   ............
 *   level 5: 30 seconds per 50 minutes  (1.0%)
 * 
 *  Structuring it this way enables 2 major changes: 1) donation mining bursts are only 30 seconds instead of 60 seconds
 *  2) the user, without recompiling our release, can enjoy 60% lower fees by default, and configure it to 80% lower fees   
 *  compared to the original stock xmrig. 
 *
 * Set donate-level to 0 to
 * disable donation mining entirely.
 * 
 * If you disable/recompile, please consider a one-off XMR donation 
 * to the original XMRig developer: 48edfHu7V9Z84YzzMa6fUueoELZ9ZRXq9VetWzYGzKt52XU5xvqgzYnDK9URnRoJMk1j8nLwEVsaSWJ4fhdUyZijBGUicoD
 */
constexpr const int kDefaultDonateLevel = 2;
constexpr const uint64_t kDonationBurstSeconds = 30;
constexpr const uint64_t kDonationCycleSeconds = 15000;

inline uint64_t donationIdleSeconds(int level)
{
    return level > 0 ? kDonationCycleSeconds / static_cast<uint64_t>(level) - kDonationBurstSeconds : 0;
}
constexpr const int kMinimumDonateLevel = 1;


#endif // XMRIG_DONATE_H
