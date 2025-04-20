Providing Meshes
================

ParaCADis stores in its document tree only the parameters
that are essential to objects they represent.
A sphere, for instance,
is a "center" Point and a (maybe squared) "radius" real number.

This information needs to be converted to a mesh
in order to be displayed, rendered, printed, etc.

A MeshProviderXxx connects to a native ParaCADis object
and produces a CGAL mesh from it.
The connection allows the mesh to be updated whenever the native object is.
Following object orientation best practices (I don't know which),
instead of implementing those conversions inside the native object class,
we use a template specialization for this matter.
To declare a provider for a curve,
specialize MeshProviderCurve<NativeObject>.
To declare a provider for a surface,
specialize MeshProviderSurface<NativeObject>.
