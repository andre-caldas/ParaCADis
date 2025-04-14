Data Description
================

Describes and "translates" data for use in user interfaces.

A `Description` is a class that implements
methods that return a string for each variable it holds.

By "translate", we mean presenting the data
differently from how it is actually stored.
For example,
internally, a circle might have a "squared radius",
while the user should certainly deal with the "radius".

A `DataTranslator` takes a class and associates it to a `Description`.
Like `UnreliableMirror`, it has methods (blocking and non-blocking)
to keep both data sides in sync.
