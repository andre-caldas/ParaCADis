Providing Meshes
================

ParaCADis stores in its document tree only the parameters
that are essential to objects they represent.
A sphere, for instance,
is a "center" Point and a (maybe squared) "radius" real number.

This information needs to be converted to a mesh
in order to be displayed, rendered, printed, simulated, etc.

Mesh providers work in pairs. There are the IgA providers.
The acronym IgA stands for *Isogeometric Analysis*.
Those objects can be directly used for doing different types of analysis.
ParaCADis uses the library `G+Smo` (gismo) to handle IgA objects.
However, in many cases (for printing and rendering),
we need to convert those to meshes that can be used for rendering or printing.
We use the library `Filament` for rendering.
Therefore, a pair `IgaProvider` and a `FilamentMesh` are combined.


Mesh Provider Queue
-------------------

There must be a queue through which the native geometry object signal
must be connected to the `IgaProvider`. On its turn, `IgaProvider`
emits a signal when the IgA object is ready. This signal must be connected
(`MeshProvider` does this) to `FilamentGismoMesh::

And a queue (possibly the same),
where the `IgaProvider` shall post messages to update `FilamentMesh`.
When `FilamentMesh` is updated, a signal must be emitted.
The scene graph, for examples, listens to this signal.


Curves and Surfaces
-------------------

Every native geometry class must be associated to a `MeshProvider`.
To declare a provider for a curve,
specialize MeshProviderCurve<NativeObject>.
To declare a provider for a surface,
specialize MeshProviderSurface<NativeObject>.


FilamentMesh
------------

The `FilamentMesh` produces the `VertexBuffer` and `IndexBuffer`
used by the filament rendering thread through the scene graph.
We use a `SharedPtr<FilamentDestroyer<...>>` for those buffers.
Those buffers are not supposed to be changed. When new buffers
are available, the `SharedPtr` is assigned a new `FilamentDestroyer<...>`.
When the last `SharedPtr` gets replaced or destructed,
the buffers are properly destroyed.
