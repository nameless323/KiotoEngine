Kioto engine codestyle.

## General

###


#### Intendation
Tabs are newer used, we use 4 spaces instead.

#### Namespaces
Ontendation is not used inside namespaces
##### namespace Kioto
All Kioto engine code is written inside `namespace Kioto`.

```cpp
namespace Kioto
{
class Foo
{
    void Bar();
};
}
```

Don't add ontendation if you add additional namespaces inside Kioto

```cpp
namespace Kioto
{
namespace MyNamespace
{
}
}
```
Feel free to use namespace Kioto::MyNamespace syntax.

