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

#ifndef NamingScheme_IExport_H
#define NamingScheme_IExport_H

#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/safe_structs/ThreadSafeStruct.h>
#include <base/threads/locks/LockPolicy.h>

#include <map>


namespace NamingScheme
{

  template<typename T>
  class NameSearchResult;

  /**
   * @brief Any class that exports some type T must subclass @class IExport<T>.
   *
   * @attention
   * In order for this class to be part of a chain of @class Exporter,
   * it does need to subclass @class Exporter.
   * We opted for **not making** `IExport<T>` a public virtual
   * subclass of @class Exporter and instead giving the developer the
   * **responsibility** to subclass it.
   * The optional template parameter may be used if you do not want
   * your class to derive from @class Exporter.
   */
  template<typename T>
  class IExport
  {
  public:
    using NameSearchResult = NameSearchResult<T>;
    using token_iterator   = NamingScheme::token_iterator;

    /**
     * Exporters can override resolve().
     *
     * @attention If you override resolve,
     * you probably do not want to override resolve_share() or resolve_ptr().
     * Unless you call this original IExport<T>::resolve() from the override.
     */
    virtual SharedPtr<T> resolve(token_iterator& tokens);

  protected:
    /**
     * Implement, to return a member of this object.
     *
     * To be more precise, you can return a pointer if the pointer
     * is guaranteed to exist as long as this object exists.
     *
     * @attention This is called from virtual resolve().
     * If you override resolve(), this might not get called at all.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     *
     * @todo REMOVE this T* = nullptr.
     */
    virtual T* resolve_ptr(token_iterator& tokens, T* = nullptr);

    /**
     * @brief Implement, to return a @class SharedPtr to any object.
     *
     * @attention This is called from virtual resolve().
     * If you override resolve(), this might not get called at all.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     *
     * @todo REMOVE this T* = nullptr.
     */
    virtual SharedPtr<T> resolve_share(token_iterator& tokens, T* = nullptr);
  };


  /**
   * Put those as static members of your final class.
   */
  template<class C, class T>
  struct EachExportedData
  {
    constexpr EachExportedData(T C::* local_ptr, const char* name)
        : local_ptr(local_ptr), name(name) {}
    using data_type = T;
    T C::* local_ptr;
    const char* name;
  };


  /**
   * Holds local pointers to exported data.
   *
   * @example The file at base/geometric_primitives/lines.h.
   */
  template<Threads::C_MutexHolderWithGates C, typename T,
           EachExportedData<typename C::record_t, T>... dataInfo>
  struct ExportedData
  {
    const std::map<std::string, T C::record_t::*> data
        = {{dataInfo.name,dataInfo.local_ptr}...};

    const T* get(const C::ReaderGate& gate, std::string_view id) const {
      if(!data.contains(id)) {
        return nullptr;
      }
      return gate->*data.at(id);
    }

    T* get(const C::WriterGate& gate, std::string_view id) const {
      if(!data.contains(id)) {
        return nullptr;
      }
      return gate->*data.at(id);
    }
  };


  /**
   * Exports data managed by Exporter<DataStruct>.
   */
  template<typename T, typename DataStruct,
           EachExportedData<DataStruct, T>... dataInfo>
  class SafeIExport : public IExport<T>
  {
  protected:
    SharedPtr<T> resolve(token_iterator& tokens) override;

  private:
    const std::map<std::string, T DataStruct::*> map
        = {{dataInfo.name,dataInfo.local_ptr}...};
  };

}  // namespace NamingScheme

#endif
