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

#pragma once

#include "Exporter.h"
#include "ResultHolder.h"
#include "types.h"

#include <base/expected_behaviour/SharedPtrWrap.h>
#include <base/threads/safe_structs/ThreadSafeStruct.h>

#include <algorithm>
#include <concepts>
#include <map>


namespace NamingScheme
{
  /**
   * Any class that exports some type T must subclass IExport<T>.
   *
   * @attention
   * In order for this class to be part of a chain of Exporter,
   * it does need to subclass Exporter.
   * We opted for **not making** `IExport<T>` a public virtual
   * subclass of Exporter and instead giving the developer the
   * **responsibility** to subclass it.
   */
  template<typename T>
  class IExport
      : public virtual ExporterBase
  {
  protected:
    IExport() = default;
    IExport(const IExport&) = default;
    IExport& operator=(const IExport&) = default;

  public:
    /**
     * Calls resolve_ptr() and resolve_share().
     */
    virtual ResultHolder<T> resolve(const ResultHolder<ExporterBase>& current,
                                    token_iterator& tokens, T* = nullptr);

  protected:
    /**
     * Implement, to return a member of this object.
     *
     * To be more precise, the ResultHolder<T> that will be finally
     * constructed by resolve(), will be constructed directly from @a current,
     * and from the pointer returned by resolve_ptr().
     * You can implement this virtual method if the returned pointer
     * is guaranteed to exist as long as this object exists.
     * In general, it will be a pointer to a member object.
     *
     * @attention This is called from virtual resolve().
     * If you override resolve(), this might not get called at all.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     *
     * @todo Is is possible to REMOVE this T* = nullptr.
     */
    virtual T* resolve_ptr(token_iterator& tokens, T* = nullptr);

    /**
     * Implement, to return a ResultHolder to object managed by a SharedPtr<T>.
     *
     * This is to be used only for a T* pointer that is not protected by a mutex.
     *
     * @attention This is called from virtual resolve().
     * If you override resolve(), this might not get called at all.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     *
     * @todo Is is possible to REMOVE this T* = nullptr.
     */
    virtual SharedPtr<T> resolve_shared(token_iterator& tokens, T* = nullptr);
  };

  template<std::size_t N>
  struct TemplateString
  {
    constexpr TemplateString(const char (&str)[N])
    {
      // TODO: temporary check. Remove in future.
      static_assert(N <= 20, "Exported variable's name is too long.");
      std::ranges::copy(str, string);
    }
    char string[N];
  };

  template<typename T, class C, std::size_t N>
  struct EachExportedData
  {
    constexpr EachExportedData(T C::* local_ptr, const char (&str)[N])
        : local_ptr(local_ptr)
        , name(str)
    {}

    constexpr EachExportedData(T C::* local_ptr, TemplateString<N> str)
        : local_ptr(local_ptr)
        , name(str)
    {}

    using data_type = T;
    using pointer_to_type = T*;
    T C::* local_ptr;
    TemplateString<N> name;
  };

  template<typename T, class C, std::size_t N>
  struct EachExportedData<SharedPtrWrap<T>, C, N>
  {
    constexpr EachExportedData(SharedPtrWrap<T> C::* local_ptr, const char (&str)[N])
        : local_ptr(local_ptr)
        , name(str)
    {}

    constexpr EachExportedData(SharedPtrWrap<T> C::* local_ptr, TemplateString<N> str)
        : local_ptr(local_ptr)
        , name(str)
    {}

    using data_type = T;
    using pointer_to_type = SharedPtrWrap<T>;
    SharedPtrWrap<T> C::* local_ptr;
    TemplateString<N> name;
  };

  template<typename T, typename P, typename ExpData>
  concept C_ExportedDataOfType =
      (std::same_as<T, typename ExpData::data_type>)
      &&
      (std::same_as<P, typename ExpData::pointer_to_type>);

  template<typename T, typename P, typename... ExpData>
  concept C_AllExportedDataOfType = (C_ExportedDataOfType<T, P, ExpData> &&...);


  /**
   * Exports data managed by Exporter<DataStruct>.
   *
   * If you are getting linker erros,
   * it probably means the exported data is inconsistent.
   */
  template<typename T, class DataStruct, EachExportedData... dataInfo>
  class IExportStruct
  {
  };

  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires C_AllExportedDataOfType<T, T, decltype(dataInfo)...>
  class IExportStruct<T, DataStruct, dataInfo...>
      : public IExport<T>
  {
  protected:
    IExportStruct();

    /**
     * Assignments and copy/move constructors were disabled to avoid complications,
     * because the modification signals need to be chained.
     *
     * @attention
     * Notice that ExporterBase is SelfSharedPtr and therefore, it should always
     * be managed by a SharedPtr. This way, deferenceable stuff
     * (that is, things derived from ExporterBase,
     * like DeferenceablePoint or CirclePointRadius2Normal)
     * should be wrapped in a SharedPtrWrap anyways.
     *
     * Therefore, the correct way to handle ExporterBase estensions inside
     * IExportStruct is to declare a Data structure that contains a
     * SharedPtrWrap<MyChildExporter> my_child_exporter. This shall
     * make the Data structure movable.
     *
     * If needed, the parent, implementing IExport<MyChildExporter, Data, ...>,
     * shall provide a method for replacing the ExporterBase object
     * handled by the SharedPtrWrap. For an example,
     * se the management of coordinate systems in the Container class.
     */
    /// @{
    IExportStruct(IExportStruct&&) = delete;
    IExportStruct(const IExportStruct&) = delete;
    IExportStruct& operator=(IExportStruct&&) = delete;
    IExportStruct& operator=(const IExportStruct&) = delete;
    /// @}

    void init();
    T* resolve_ptr(token_iterator& tokens, T* = nullptr) override;

  private:
    const std::map<std::string, T DataStruct::*> map
        = {{dataInfo.name.string, dataInfo.local_ptr}...};
  };



  /**
   * Template specialization that exports SharedPtr data managed by Exporter<DataStruct>.
   */
  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires C_AllExportedDataOfType<T, SharedPtrWrap<T>, decltype(dataInfo)...>
  class IExportStruct<T, DataStruct, dataInfo...>
      : public IExport<T>
  {
  protected:
    IExportStruct() {}

    /**
     * Assignments and copy/move constructors were disabled to avoid complications,
     * because the modification signals need to be chained.
     *
     * @attention
     * Notice that ExporterBase is SelfSharedPtr and therefore, it should always
     * be managed by a SharedPtr. This way, deferenceable stuff
     * (that is, things derived from ExporterBase,
     * like DeferenceablePoint or CirclePointRadius2Normal)
     * should be wrapped in a SharedPtrWrap anyways.
     *
     * Therefore, the correct way to handle ExporterBase estensions inside
     * IExportStruct is to declare a Data structure that contains a
     * SharedPtrWrap<MyChildExporter> my_child_exporter. This shall
     * make the Data structure movable.
     *
     * If needed, the parent, implementing IExport<MyChildExporter, Data, ...>,
     * shall provide a method for replacing the ExporterBase object
     * handled by the SharedPtrWrap. For an example,
     * se the management of coordinate systems in the Container class.
     */
    /// @{
    IExportStruct(IExportStruct&&) = delete;
    IExportStruct(const IExportStruct&) = delete;
    IExportStruct& operator=(IExportStruct&&) = delete;
    IExportStruct& operator=(const IExportStruct&) = delete;
    /// @}

    void init();
    SharedPtr<T> resolve_shared(token_iterator& tokens, T* = nullptr) override;

  private:
    const std::map<std::string, SharedPtrWrap<T> DataStruct::*> map
        = {{dataInfo.name.string, dataInfo.local_ptr}...};
  };
}  // namespace NamingScheme

// TODO: remove this and use modules.
#include "IExport.hpp"
