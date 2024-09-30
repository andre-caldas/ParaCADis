Scene Graph
===========

Rendering is completely separated from ParaCADis' document structure.
The document elements send signals
that allow us to keep our scene graph updated.

The scene has a "root", "container nodes" and "mesh nodes".


Scene Root
----------

The SceneRoot holds a SignalQueue and OGRE's rendering infrastructure.
And it also holds a "root" ContainerNode.
The scene tree is populated recursivelly.
For each Container in the document,
there shall be a corresponding ContainerNode in our scene.
And for each non-container ExporterBase,
there shall be a MeshNode.


Container Node
--------------

The ContainerNode listens to the Container's modification signals.


Mesh Node
---------

The MeshNode is a little more complicated.
It instantiates MeshProvider.
The MeshProvider listen for modification to the geometric objects in the document,
updates the Gismo mesh and signals modifications (to the MeshNode).
The MeshNode updates the OGRE's scene graph corresponding mesh acordingly.

There are two types of MeshNode.
One for curves and one for surfaces.
There could be one for solids.
The MeshProviders, on the other hand,
are very specific to the geometry type they correspond to.
