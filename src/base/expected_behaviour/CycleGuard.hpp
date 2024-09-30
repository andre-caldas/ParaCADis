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

#include "CycleGuard.h"

#include <cassert>

template<typename T>
CycleGuard<T>::Sentinel CycleGuard<T>::sentinel(T* p)
{
  if(push(p)) {
    return {processed, p};
  }
  return {processed, nullptr};
}

template<typename T>
bool CycleGuard<T>::push(T* p)
{
  if(already_processed(p)) {
    return false;
  }
  processed.insert(p);
  return true;
}

template<typename T>
void CycleGuard<T>::pop(T* p)
{
  assert(processed.contains(p) && "Cannot pop an item that was not processed.");
  processed.erase(p);
}

template<typename T>
CycleGuard<T> CycleGuard<T>::clone() const
{
  CycleGuard<T> result;
  result.processed = processed;
  return result;
}
