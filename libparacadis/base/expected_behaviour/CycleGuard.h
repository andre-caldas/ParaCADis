// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
 *                                                                          *
 *   This file is part of ParaCADis.                                        *
 *                                                                          *
 *   ParaCADis is free software: you can redistribute it and/or modify it   *
 *   under the terms of the GNU General Public License as published         *
 *   by the Free Software Foundation, either version 2.1 of the License,    *
 *   or (at your option) any later version.                                 *
 *                                                                          *
 *   ParaCADis is distributed in the hope that it will be useful, but       *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
 *   See the GNU General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with ParaCADis. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                          *
 ***************************************************************************/

#pragma once

#include <unordered_set>

/**
 * Checks if a certain pointer has already been processed.
 *
 * The idea is to pass a CycleGuard to a recursive function that
 * parses a grafo. Even if the grafo is cyclic, the recursive function
 * can avoid recursing nodes that were already processed.
 */
template<typename T>
class CycleGuard
{
public:
  CycleGuard() = default;
  CycleGuard(const CycleGuard&) = delete;
  CycleGuard& operator=(const CycleGuard&) = delete;

  struct Sentinel {
    Sentinel(std::unordered_set<T*>& _set, T* _element)
        : set(_set), element(_element) {}

    Sentinel(const Sentinel&) = delete;
    Sentinel(Sentinel&&) = delete;
    Sentinel& operator=(const Sentinel&) = delete;
    Sentinel& operator=(Sentinel&&) = delete;

    ~Sentinel() {
      if(element) set.erase(element);
    }

    bool success() const {return (element != nullptr);}

    std::unordered_set<T*>& set;
    T* element = nullptr;
  };

  /**
   * Pushes a "new" pointer to the list.
   *
   * @returns If pointer did not already exist, returns true.
   *
   * @example
   * { // sentinel scope
   *   auto sentinel = guard.push(p);
   *   if(sentinel.success()) {
   *     my_recursion(guard, p);
   *   }
   * }
   */
  [[nodiscard]]
  Sentinel sentinel(T* p);
  bool already_processed(T* p) const {return processed.contains(p);}

  CycleGuard clone() const;

private:
  bool push(T* p);
  void pop(T* p);

  std::unordered_set<T*> processed;
};

#include "CycleGuard.hpp"
