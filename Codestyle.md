Kioto engine codestyle.

## General

###

### Branches
The main repository branch is master. You can create pull request to master, get at least one reviewer to approve and your branch will be merged into master soon.

### License
License info should be at the beggining of each .cpp and .h file.
```cpp
//
// Copyright (C) Author1, Author2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//
```

#### Indentation
Tabs are never used, we use 4 spaces instead.

#### Namespaces
Indentation is not used inside namespaces
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

Don't add indentation if you add additional namespaces inside Kioto

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
Header files are guarded by `#pragma once`

#### Order of Includes
Use standard order for readability and to avoid hidden dependencies: class .h file in .cpp, C library, C++ library, other libraries' `.h`, your project's `.h`.
Your project `.h` must be included with `#include "You/Project/Path.h"` while external `.h` should be included with `#include <Library/Path.h>`
```cpp
#include "Source/MyClass.h"

#include <vector>

#include <curl/curl.h>
#include <zlib/zlib.h>

// project includes
#include "Source/Base.h"
#include "Source/Debug/Logger.h"
```

All of the project's header files should be listed as descendants of the project's source directory without the use of UNIX directory shortcuts `.` (the current directory) or `..` (the parent directory). For example, Source/Base.h should be included as:

```cpp
#include "Source/Base.h"
```

Within each section the includes should be ordered alphabetically. Note that older code might not conform to this rule and should be fixed when convenient.

#### Commented code
Remove commented code, we have git for history.

#### If-else curly braces
Omit the curly braces if both If and Else branches have only one statement. In other cases use braces.

```cpp
if (cond)
    DoSmth();
else
    DoSmthElse();

if (cond)
{
    DoSmth();
    DoSmth2();
}
else
{
    DoSmthElse();
}```

## Naming
Names are written in English, class members should start with m_ prefix.
```cpp
class MyClass
{
private:
    int m_myInt;
};
```

#### Classes, functions, namespaces
Camel notation beginning with a capital letter.

```cpp
class MyObject;
void Foo();
namespace MyNamespace{}
```

#### Functions Variables
Camel notation beginning with a lower-case letter.
```cpp
MyObject myObjectInstance;
```

#### Public, protected variables
Camel notation beginning with an upper-case letter.

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
Camel notation with leading 'e'. Enum members - camel-case. Use enum class when possible.
```cpp
enum class eError
{
    Overflow,
    ReadError
};
```

## Documentation
Use 
```cpp
///
/// ... descrption ...
///
```
to describe any class, struct, method or function which will be seen outside KiotoEngine dll.
If you use KIOTO_API macro, then you probably want to document your class or function.

## Classes
#### Class members initialization
Constructor member initialization lists are preferred over all other methods.
Initialize members in order of declaration!

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
    : m_width(0) // Order preserved.
    , m_heigth(0)
{
}
```

#### Virtual functions
Either `virtual`, `override`, or `final` keyword can be used in function declaration. **Do not** mix `virtual` with `override/final` keywords in one declaration.

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
Use auto only with long template types, lambda function types, loops.

#### Comments
Start with a space and a capital letter, end with a dot, space, and the author's name in square brackets.

```cpp
// This is comment. [Name]

/*
 Multiline comment. [Name]
*/
```

### Indentation.
Use Allman indentation style (see example).

#### Example
The following example shows properly formatted code.
```cpp
//
// Copyright (C) Author1, Author2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

namespace Kioto
{

class MyClass : public MyBase
{
public: // Public block goes first. [Author]
    MyClass();
    virtual ~MyClass();

protected: // Then protected.
    virtual void Foo();
    void Bar() override;

private: // Private goes last.
    enum class eEnum // First enums. [Author]
    {
        One,
        Two
    };

    struct InnerStruct // Then structs. [Author]
    {
        int SomeVar;
    };

    void FooOne(); // Then methods. [Author]
    template <typename T>
    T TemplateFun(T t);

    int m_var0; // Then fields. [Author]
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
// Copyright (C) Author1, Author2. year
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Source/Folder/MyClass.h"

#include <map>

#include "Source/Folder/Cl.h"

namespace Kioto
{

MyClass::MyClass()
    : m_var0(8) // In order of appearance in .h file. [Author]
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

void MyClass::Foo() // In order of appearance in .h file. [Author]
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
