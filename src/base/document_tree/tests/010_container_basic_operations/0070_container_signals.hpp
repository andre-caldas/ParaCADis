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

#include <catch2/catch_test_macros.hpp>

#include <base/naming_scheme/Uuid.h>
#include <base/threads/locks/writer_locks.h>
#include <base/threads/message_queue/SignalQueue.h>
#include <base/threads/safe_structs/ThreadSafeQueue.h>

#include <format>
#include <semaphore>
#include <string>
#include <vector>

using namespace DocumentTree;
using namespace NamingScheme;
using namespace Threads;
using namespace Threads::SafeStructs;

class ContainerEventProcessor
{
public:
  virtual ~ContainerEventProcessor() = default;

  template<typename A, typename B>
  static std::string msg(std::string c_type, A a, B b)
  {
    return std::format("Action {}: {} --> {}", c_type, a.toString(), b.toString());
  }

  template<typename A, typename B, typename C>
  static std::string msg(std::string c_type, A a, B b, C c)
  {
    return std::format("Action {}: {} -({})-> {}", c_type,
                       a.toString(), b.toString(), c.toString());
  }


  void report_add_non_container(SharedPtr<Container> to,
                                SharedPtr<ExporterBase> what)
  {
    messages.push(
        msg("add non-container", what->getUuid(), to->getUuid()));
  }

  void report_add_container(SharedPtr<Container> to,
                            SharedPtr<Container> what)
  {
    messages.push(
        msg("add container", what->getUuid(), to->getUuid()));
  }


  void report_remove_non_container(SharedPtr<Container> from,
                                   SharedPtr<ExporterBase> what)
  {
    messages.push(
        msg("remove non-container", what->getUuid(), from->getUuid()));
  }

  void report_remove_container(SharedPtr<Container> from,
                               SharedPtr<Container> what)
  {
    messages.push(
        msg("remove container", what->getUuid(), from->getUuid()));
  }


  void report_move_non_container(SharedPtr<Container> from,
                                 SharedPtr<ExporterBase> what,
                                 SharedPtr<Container> to)
  {
    messages.push(
        msg("move non-container", from->getUuid(), what->getUuid(), to->getUuid()));
  }

  void report_move_container(SharedPtr<Container> from,
                             SharedPtr<Container> what,
                             SharedPtr<Container> to)
  {
    messages.push(
        msg("move container", from->getUuid(), what->getUuid(), to->getUuid()));
  }

  void finish()
  {
    messages.push("end");
  }

  void check_equal(ContainerEventProcessor& other)
  {
    std::string message_me;
    do {
      message_me = messages.pull();
      auto message_other = other.messages.pull();
      REQUIRE(message_me == message_other);
    } while(message_me != "end");
  }

private:
  ThreadSafeQueue<std::string> messages;
};


SCENARIO("Signal dispatch and processing", "[simple]")
{
  GIVEN("a container and some objects inside it")
  {
    Real radius{5};
    auto a        = SharedPtr<Container>::make_shared();
    auto b        = SharedPtr<Container>::make_shared();

    auto p        = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto v        = SharedPtr<DeferenceableVector>::make_shared(4, 5, 6);
    auto line     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto circle   = SharedPtr<CirclePointRadiusNormal>::make_shared(*p, radius, *v);

    WHEN("we setup signals and a queue")
    {
      auto queue     = SharedPtr<SignalQueue>::make_shared();
      auto processor = SharedPtr<ContainerEventProcessor>::make_shared();
      auto clone1    = SharedPtr<ContainerEventProcessor>::make_shared();
      auto clone2    = SharedPtr<ContainerEventProcessor>::make_shared();
      auto expected  = SharedPtr<ContainerEventProcessor>::make_shared();

      queue->run_thread(queue);

      auto connect = [a, b, queue, processor, clone1, clone2]
          (auto sig, auto report)
      {
        ((*a).*sig).connect(a, queue, processor, report);
        ((*b).*sig).connect(b, queue, processor, report);
        ((*a).*sig).connect(a, queue, clone1, report);
        ((*b).*sig).connect(b, queue, clone1, report);
        ((*a).*sig).connect(a, queue, clone2, report);
        ((*b).*sig).connect(b, queue, clone2, report);
      };

      connect(&Container::add_container_sig,
              &ContainerEventProcessor::report_add_container);
      connect(&Container::add_non_container_sig,
              &ContainerEventProcessor::report_add_non_container);

      connect(&Container::remove_container_sig,
              &ContainerEventProcessor::report_remove_container);
      connect(&Container::remove_non_container_sig,
              &ContainerEventProcessor::report_remove_non_container);

      connect(&Container::move_container_sig,
              &ContainerEventProcessor::report_move_container);
      connect(&Container::move_non_container_sig,
              &ContainerEventProcessor::report_move_non_container);

      AND_WHEN("we add and remove elements")
      {
        auto add_element = [expected](auto& x, auto& y) {
          x->addElement(y);
          expected->report_add_non_container(x, y);
        };

        auto add_container = [expected](auto& x, auto& y) {
          x->addElement(y);
          expected->report_add_container(x, y);
        };

        auto remove_element = [expected](auto& x, auto& y) {
          x->removeElement(y->getUuid());
          expected->report_remove_non_container(x, y);
        };

        auto remove_container = [expected](auto& x, auto& y) {
          x->removeElement(y->getUuid());
          expected->report_remove_container(x, y);
        };

        auto move_element = [expected](auto& x, auto& y, auto& z) {
          x->moveElementTo(y->getUuid(), z);
          expected->report_move_non_container(x, y, z);
        };

        auto move_container = [expected](auto& x, auto& y, auto& z) {
          x->moveElementTo(y->getUuid(), z);
          expected->report_move_container(x, y, z);
        };

        add_element(a, p);
        add_element(a, v);
        add_element(a, line);
        add_element(a, circle);
        add_container(a, b);

        add_element(b, p);
        add_element(b, v);
        add_element(b, line);
        add_element(b, circle);
        add_container(b, a);

        remove_element(a, p);
        remove_element(b, p);
        remove_element(a, v);
        remove_element(b, v);

        remove_container(a, b);
        add_container(a, b);

        remove_element(b, line);
        move_element(a, line, b);
        move_element(b, line, a);
        move_element(a, line, b);
        add_element(a, line);

        remove_element(b, circle);
        move_element(a, circle, b);
        move_element(b, circle, a);
        move_element(a, circle, b);
        add_element(a, circle);

        move_container(b, a, a);
        move_container(a, a, b);

        move_container(a, b, b);
        move_container(b, b, a);

        THEN("messages should have been received in order")
        {
          queue->push([processor]{processor->finish();}, processor.get());
          queue->push([clone1]{clone1->finish();}, clone1.get());
          queue->push([clone2]{clone2->finish();}, clone2.get());
          expected->finish();

          processor->check_equal(*clone1);
          expected->check_equal(*clone2);
        }
      }
    }
  }
}
