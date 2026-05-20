# NeuLAND Convention and Code Standard {#neuland_style}

[TOC]

To build a robust and readable software, programmers have to follow good coding standards and consistent conventions. The coding standards, either relating specific styles or following the C++ language rules, are created after decades of experience from various industries. The coding styles, probably different from companies to companies though, must be consistent throughout the whole project. The code related to NeuLAND follows strictly [the C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) and **latest industrial standards**. Reasons why a certain rule exists or a style is preferred are also given in the following sections.

## Coding standards and styles

### Rule 0: Don't write C code

Always prefer **C++ ways** over the C ways (There is no such thing as "C/C++"!). This includes:

- Use `static_cast` or `dynamic_cast` instead of C cast.
- Use `constexpr` instead of C macro.
- Use `#pragma once` instead of include guard.

### Variable declaration and initialization

There are only two ways (both with **curly brackets**) to declare and initialize a variable:

1. (preferred) Use `auto`:

   ```c++
   auto value = float {1.};
   auto val_double = 0.;   // this is also ok. Implicit type is double.
   auto val_int = 1;       // this is also ok. Implicit type is int.
   ```

2. Type in the front:

   ```c++
   float a_float_value {1.};
   MyClass obj {};
   ```

_**Reason:**_ Using curly brackets `{}` makes sure the expression is an initialization instead of a function declaration. For example, `int val();` is a declaration for a function called `val` while `int val {};` is a variable initialization.

### AAA (Almost Always Auto)

Always use `auto` for the variable initialization and function declaration:

```c++
auto my_obj = MyClass{};

auto my_func(int input) -> double
{
    // ...
}
```

_**Reason:**_ Using auto (even for a simple integer) to initialize a variable **guarantees** the variable always get initialized. For example, `int i;` fails to initialize the variable but still compiles while `auto i = int;` never compiles.

### Use signed values over unsigned values
