Kioto engine codestyle.

## General

###

### Branches
The main repository branch is master. You can create pull request to master, get at least one reviewer approve and your brunch will be merged to master soon.

### License
License info should be at the beggining of each .cpp and .h files.
```cpp
//
// Copyright (C) Autor1, Autor2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//
```

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


#### Header guard
Header files are guarder by `#pragma once`

#### Order of Includes
Use standard order for readability and to avoid hidden dependencies: class .h file in .cpp, C library, C++ library, other libraries' `.h`, your project's `.h`.
Your project `.h` must be included with `#include "You\Project\Path.h"` while external `.h` should be included with `#include <Library\Path.h>`
```cpp
#include "Source\MyClass.h"

#include <vector>

#include <curl\curl.h>
#include <zlib\zlib.h>

// project includes
#include "Source\Base.h"
#include "Source\Debug\Logger.h"
```

All of a project's header files should be listed as descendants of the project's source directory without use of UNIX directory shortcuts `.` (the current directory) or `..` (the parent directory). For example, Source/Base.h should be included as:

```cpp
#include "Source\Base.h"
```

Within each section the includes should be ordered alphabetically. Note that older code might not conform to this rule and should be fixed when convenient.

#### Commented code
Remove commented code, we have git for history. 

## Naming
Names are written in english, class members should start with m_ prefix.
```cpp
class MyClass
{
private:
    int m_myInt;
};
```

#### Classes, functions, namespaces
Camel notation begin with capital letter.

```cpp
class MyObject;
void Foo();
namespace MyNamespace{}
```

#### Functions Variables
Camel notation begin with lower-case.
```cpp
MyObject myObjectInstance;
```

#### Public, protected variables
Camel notation begin with upper-case.

```cpp
class MyClass
{
protected:
    float MyFloat;

public:
    int MyInt;
};
```

#### Static constants, defines.
All capitals with underscores.

```cpp
static const int MAX_WIDTH = 100;
#define KIOTO_DEBUG
```

#### Enums.
Camel notation with leading 'e'. Enum members - camel-case. Use enum class when it's possible
```cpp
enum class eError
{
    Overflow,
    ReadError
};
```

## Classes
#### Class members initialization
Constructor member initialization lists is preferred over all other methods.
Initialize member in order of declaration!

```cpp
class MyClass
{
public:
    MyClass();
private:
    int32 m_width;
    int32 m_height;
};

MyClass::MyClass()
    : m_width(0)
    , m_heigth(0)
{
}
```

#### Virtual functions
Either `virtual`, `override` or `final` keyword can be used in function declaration. **Do not** mix `virtual` with `override/final` keywords in one declaration.

```cpp
virtual void Foo(); //declared for the first time
void Bar() override; //overriding virtual function
```

#### Inline functions
Separate inline function definition from declaration and use `inline` keyword only in declaration. Declaration should be placed in the end of header file.

```cpp
class MyObject
{
    void Foo();
};

inline void MyObject::Foo()
{
}
```

#### Autos
Use auto only with long template types, lambda function type, loops.

#### Comments
Start with space and capital letter, ends with dot, space and autor name in square bracets.

```cpp
// This is comment. [Name]

/*
 Multiline comment. [Name]
*/
```

### Indentation.
Use allman indentation style (see example).

#### Example
The following example shows properly formatted code sample.
```cpp
//
// Copyright (C) Autor1, Autor2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

namespace Kioto
{

class MyClass : public MyBase
{
public: // Public block goes first. [Autor]
    MyClass();
    virtual ~MyClass();

protected: // Then protected.
    virtual void Foo();
    void Bar() override;

private: // Private goes last.
    enum class eEnum // First enums. [Autor]
    {
        One,
        Two
    };

    struct InnerStruct // Then structs. [Autor]
    {
        int SomeVar;
    };

    void FooOne(); // Then methods. [Autor]
    template <typename T>
    T TemplateFun(T t);

    int m_var0; // Then vars. [Autor]
    int m_var1;
};

inline void MyClass::FooOne()
{
    m_var0++;
}

template <typename T>
T MyClass::TemplateFun(T t)
{
    return t > 0.0f ? t : -t;
}

}

// .cpp
//
// Copyright (C) Autor1, Autor2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Source/Folder/MyClass.h"

#include <map>

#include "Source/Folder/Cl.h"

namespace Kioto
{

MyClass::MyClass()
    : m_var0(8) // In order of appearance in .h file. [Autor]
    , m_var1(2)
{
    for (int i = 0; i < m_var0; ++i)
    {
        m_var1 += m_var0;
        m_var1--;
    }    
}

MyClass::~MyClass()
{
    m_var0 = 0;
}

void MyClass::Foo() // In order of appearance in .h file. [Autor]
{
    std::vector<int> myVector { 3, 4, 5 };
    for (auto i : myVector)
    {
        my_var0 += i;
    }
}

void MyClass::Bar()
{
    if (my_var0 > 0)
    {
        my_var1 = 0;
    }
}

}
```