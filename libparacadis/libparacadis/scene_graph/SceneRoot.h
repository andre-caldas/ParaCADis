// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024-2025 André Caldas <andre.em.caldas@gmail.com>       *
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

#include "forwards.h"

#include <libparacadis/base/document_tree/Container.h>
#include <libparacadis/base/document_tree/DocumentTree.h>
#include <libparacadis/base/threads/message_queue/SignalQueue.h>
#include <libparacadis/base/threads/safe_structs/ThreadSafeMap.h>

#include <memory>
#include <unordered_map>

namespace SceneGraph
{
  /**
   * The SceneRoot is the Filament side of the DocumentTree.
   *
   * @todo
   * Move the engine and renderer to outside this class.
   */
  class SceneRoot
  {
    friend class ContainerNode;

  public:
    /**
     * Creates a scene root and manages views and cameras.
     *
     * @param
     * The native window handler shall be passed cast to `void*`.
     */
    SceneRoot(void* native_window_handler);
    ~SceneRoot();

    /**
     * Initiates the bridge structure between the document and the Filament scene.
     */
    static void populate(const SharedPtr<SceneRoot>& self,
                         const SharedPtr<Document::DocumentTree>& document);

    void startRenderingThread();

    const SharedPtr<Threads::SignalQueue>& getQueue() { return signalQueue; }
    const SharedPtr<RenderingScope>& getRenderingScope() { return renderingScope; }

  private:
    WeakPtr<SceneRoot>              self;
    SharedPtr<Threads::SignalQueue> signalQueue;
    SharedPtr<RenderingScope>       renderingScope;

    SharedPtr<ContainerNode> rootContainer;

    /**
     * The document tree structure is kept flat.
     * We do not use a tree here.
     *
     * The document tree strucutre is transported directly to the Filament scene.
     */
    /// @{
    template<typename Key, typename Val>
    using multimap_t = Threads::SafeStructs::ThreadSafeUnorderedMultimap<Key, Val>;
    multimap_t<geometry_t*, SharedPtr<MeshNode>> meshNodes;
    /// @}

  /* Filament stuff */
  public:
    // TODO: create all this outside this class.
    filament::Engine*    engine;
    filament::SwapChain* swap_chain;
    filament::Renderer*  renderer;
    filament::Scene*     scene;

    // All cameras (and views) associated to this scene.
    std::unordered_map<CameraWrapper*, std::unique_ptr<CameraWrapper>> cameras;
  };
}
