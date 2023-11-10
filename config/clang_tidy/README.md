# Configuration for clang-tidy

## Setup configuration file
Warnings given by clang-tidy or clangd are configured in `.clang-tidy` yaml file in the nearest folder. Detector specific folders can have different configurations defined in the [global.yml](./global.yml). The customary configuration files in folders for a detector should be symbolic links to a common file located at `config/clang_tidy/${detector_name}.yml`.

Currently there are 3 different folders for each detector:
* `R3BRoot/${detector_name}/`
* `R3BRoot/r3bsource/${detector_name}/`
* `R3BRoot/r3bdata/${detector_name}Data/`

Here is an example of how to create 3 symbolic links in the folders related to NeuLAND:
```shell
# in project root folder
ln -s ../config/clang_tidy/neuland.yml ./neuland/.clang-tidy
ln -s ../../config/clang_tidy/neuland.yml ./r3bsource/neuland/.clang-tidy
ln -s ../../config/clang_tidy/neuland.yml ./r3bdata/neulandData/.clang-tidy
```

## Suppressing undesired warnings

Ways to ignore certain warnings among the code can be found in [clang-tidy official website](https://clang.llvm.org/extra/clang-tidy/#suppressing-undesired-diagnostics). 

Here is a summary with some examples (copied directly from the website):

1. `NOLINT`
```cpp
// Suppress all the diagnostics for the line
  Foo(int param); // NOLINT

  // Consider explaining the motivation to suppress the warning
  Foo(char param); // NOLINT: Allow implicit conversion from `char`, because <some valid reason>

  // Silence only the specified checks for the line
  Foo(double param); // NOLINT(google-explicit-constructor, google-runtime-int)

  // Silence all checks from the `google` module
  Foo(bool param); // NOLINT(google*)

  // Silence all checks ending with `-avoid-c-arrays`
  int array[10]; // NOLINT(*-avoid-c-arrays
```

2. `NOLINTNEXTLINE`
```cpp
// Silence only the specified diagnostics for the next line
  // NOLINTNEXTLINE(google-explicit-constructor, google-runtime-int)
  Foo(bool param);

  // Silence all checks from the `google` module for the next line
  // NOLINTNEXTLINE(google*)
  Foo(bool param);

  // Silence all checks ending with `-avoid-c-arrays` for the next line
  // NOLINTNEXTLINE(*-avoid-c-arrays)
  int array[10];
```

3. `NOLINTBEGIN` and `NOLINTEND`
```cpp
// Silence only the specified checks for all lines between the BEGIN and END
  // NOLINTBEGIN(google-explicit-constructor, google-runtime-int)
  Foo(short param);
  Foo(long param);
  // NOLINTEND(google-explicit-constructor, google-runtime-int)

  // Silence all checks from the `google` module for all lines between the BEGIN and END
  // NOLINTBEGIN(google*)
  Foo(bool param);
  // NOLINTEND(google*)

  // Silence all checks ending with `-avoid-c-arrays` for all lines between the BEGIN and END
  // NOLINTBEGIN(*-avoid-c-arrays)
  int array[10];
  // NOLINTEND(*-avoid-c-arrays)
```
