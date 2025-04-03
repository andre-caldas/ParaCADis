Dedicated Thread Scope
======================

Although our software is multithreaded,
there are things that are executed in a single thread that cannot block.
For example,
things that are supposed to be executed in the rendering thread.

A *Dedicated Thread Scope* is a protected structure
that can (or is supposed to) be directly accessed by a single thread.
There is no concurrency and no locks involved except for an *Action Queue*.
Whenever another thread wants to have access to the protected structure,
it pushes a callable to the queue.
In the proper moment,
the callable is executed by the thread that accesses the data.
The protected data is passed as reference to the callable.

In order to have the callable executed again (and again),
it must return `true`.
When it returns `false`,
the callable is discarded from the list of registered callables.
Also, callables shall never block.
They can `try_lock`, but they cannot wait.
When dealing with data protected by mutexes,
an `UnreliableMirrorGate` can be used.

This infrastructure was developed with Dear ImGui library in mind.
In every rendering frame we need to call ImGui functions in order
to have GUI elements drawn, input processed and values saved
to their corresponding variables.
The same methods are called over and over again each frame.

Any thread that wants to render GUI elements appends callable
methods to the `ImGuiRenderingScope`.
The appended methods are actually pushed to a queue.
Every frame the `ImGuiRenderingScope` will consume the queue,
add new callables to the list of executed callables.
After that, all registered callables are executed.
Callables that return false are removed from the list,
and callables that return true are kept.
For example,
when a GUI window is closed,
the callable that generates it must return false.

Each type of thing being executed every frame must have a "scope":
variables that are kept across those repetitive calls.
For that reason, we call it a "scope".
Our hypothetical `ImGuiRenderingScope`
shall descend from `DedicatedThreadScopeBase`.
The pure virtual member `DedicatedThreadScopeBase::execute`
is supposed to execute all registered actions.

But you do not need to define your own `execute()`.
You shall use our
[curiously recurring template pattern (CRTP)](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern),
`DedicatedThreadScopeT`.
```cpp
  struct ImGuiRenderingScope
    : public DedicatedThreadScopeT<ImGuiRenderingScope>
  {
    int count = 0;
  };
  
  ImGuiRenderingScope gui;
  float f = 0.0f;
  
  // You can pass by reference if you know it will remain vaild.
  gui.addAction([&f](ImGuiRenderingScope& gui){
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ++gui.count;
    ImGui::Text("Count: %d.", gui.count);
  });
```
This will setup a class you can add "actions" that take
`ImGuiRenderingScope&` as argument.
So, those "actions" will have access to their "scope".

A collection of scopes for one dedicated thread is a `DedicatedThreadContext`.
This class itself is a `DedicatedThreadScopeBase`.
It has its own queue and keeps a list of `DedicatedThreadScopeBase`.
