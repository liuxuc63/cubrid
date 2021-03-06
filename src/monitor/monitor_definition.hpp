/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

//
// monitor_definition.hpp - definition interface for monitor
//

#if !defined _MONITOR_DEFINITION_HPP_
#define _MONITOR_DEFINITION_HPP_

#include <chrono>

#include <cstdint>

namespace cubmonitor
{
  // statistic common representation used on monitor fetching its values
  using statistic_value = std::uint64_t;

  // clocking
  using clock_type = std::chrono::high_resolution_clock;
  using time_point = clock_type::time_point;
  using duration = clock_type::duration;

  // fetching global & transaction sheet
  using fetch_mode = bool;
  const fetch_mode FETCH_GLOBAL = true;
  const fetch_mode FETCH_TRANSACTION_SHEET = false;

} // namespace cubmonitor

#endif // _MONITOR_DEFINITION_HPP_
