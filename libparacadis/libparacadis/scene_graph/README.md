Scene Graph
===========

Rendering is completely separated from ParaCADis' document structure.
The document elements send signals
that allow us to keep our scene graph updated.

The scene has a "root", "container nodes" and "mesh nodes".


Disclaimer
----------

In what is described here there are so many independent threads
that probably the thing is a very unnecessary overkill.
Probably it would be much more efficient to have only one thread
for everything. But I actually do not know how ParaCADis is going
to be used. I don't know how many cores and how many threads will
a regular program in a domestic CPU have. I don't know how things
are going to change... so, I have decided to do the way I did. :-)


Scene Root
----------

The `SceneRoot` is managed in a dedicated thread that "observes" changes
in the document tree. Two other threads are started as well:

1. A `MeshProvider` thread that translates document elements to
   vertex buffers and index buffers and send the results to
   the scene graph.

2. A Filament rendering thread. A dedicated thread that receives
   meshes and transforms (and materials, lights, etc) from
   the scene graph.

The `SceneRoot` holds a `SignalQueue` that is connected
to the document nodes, as well as to the mesh providers.
And it also holds a "root" `ContainerNode`.
The scene tree is populated recursivelly.
For each `Container` in the document,
there shall be a corresponding `ContainerNode` in our scene.
And for each geometric type, there shall be a `MeshNode`.


Container Node
--------------

The `ContainerNode` listens to the Container's modification signals
and adapts the scene graph accordingly.


Mesh Node
---------

The `MeshNode` is a little more complicated.
It instantiates `MeshProvider`.
The `MeshProvider` listens the geometric objects in the document,
updates the Gismo mesh, generates vertex buffers and signals the `MeshNode`.
The `MeshNode` updates the rendering scene acordingly.

There are two types of `MeshNode`.
One for curves and one for surfaces.
There could be one for solids.
The `MeshProvider`s, on the other hand,
are very specific to the geometry type they correspond to.


Signal Queues
-------------

The `SceneRoot` holds three signal queues:

1. One `SignalQueue scene_queue` to be read by its own thread.
2. One `SignalQueue rendering_queue` to be read by the rendering thread.
3. One `SignalQueueWithDiscard mesh_provider_queue` to be read by the
   mesh provider threads.
