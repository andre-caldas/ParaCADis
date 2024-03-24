// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#ifndef NamingScheme_ReferenceToObject_H
#define NamingScheme_ReferenceToObject_H

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "PathToken.h"
#include "PathToObject.h"
#include "types.h"

namespace NamingScheme
{

  /**
   * A  PathToObject is not aware of the type of variable it points to.
   *
   * Through the means of the templated  IExport<variable_type>,
   * objects can "export" the correct type to be referenced to by
   * ReferenceTo<variable_type>.
   *
   * @example ReferenceTo<double> ref(root, "start point", "x");
   *
   * @see  IExport<>.
   */
  template<typename T>
  class ReferenceTo : public PathToObject
  {
  public:
    ReferenceTo(ReferenceTo&&)                 = default;
    ReferenceTo(const ReferenceTo&)            = default;
    ReferenceTo& operator=(const ReferenceTo&) = default;
    ReferenceTo& operator=(ReferenceTo&&)      = default;

    using PathToObject::PathToObject;
    using lock_type = PathToObject::lock_type;

    /**
     * @brief Locks the last object and gets a raw pointer to  T.
     * We assume the managed object owns the referenced  T.
     *
     * The class  PathToObject is not aware of the
     * type of object being referenced. It resolves the token chain
     * up to the last object and "locks" it through a shared_ptr.
     * This lock also contains an iterator to the remaining tokens.
     *
     * To complete the resolution process, we expect that
     * this last found lock.last_object is of type:
     * *  T, when there are no remaining tokens.
     * *  IExport<T>, where there are tokens to be resolved.
     *
     * After resolving a pointer to T, we store the lock.last_object
     * and the T pointer into a shared_ptr<T>.
     */
    using locked_resource = std::shared_ptr<T>;

    /**
     * @brief Fully resolves the chain up to the last token.
     * @return A  locked_resource holding a lock and a pointer.
     */
    locked_resource resolve() const;

    /**
     * @brief We keep an internal  locked_resource.
     * This function resoleves the token path (again) and
     * stores the result internally (lockedResult).
     * @return True if lookup is successful.
     */
    bool refreshLock();

    /**
     * @brief Releases the internal  locked_resource.
     */
    void releaseLock();

    bool isLocked() const;

    /**
     * @brief Gets a pointer to the locked resource.
     * It throws an exception if the internal  locked_resource is not locked.
     * @return A pointer to the referenced resource.
     */
    T* get() const;

    bool hasChanged() const { return (old_reference != lockedResult.get()); }

    T* getOldReference() const { return old_reference; }

    template<typename X, typename... PathToken>
    ReferenceTo<X> goFurther(PathToken&&... furtherPath) const;

    static ReferenceTo<T> unserialize(Xml::Reader& reader)
    {
      return ReferenceTo<T>{PathToObject::unserialize(reader)};
    }

  private:
    locked_resource lockedResult;
    /**
     * @brief The previous value of lockedResult.reference.
     * @attention It might be an invalid pointer!
     */
    T* old_reference = nullptr;
  };

}  // namespace NamingScheme

#endif

