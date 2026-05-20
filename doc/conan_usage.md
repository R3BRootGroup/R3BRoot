# How to use Conan package manager {#conan_usage}

Conan package manager is a widely-used package manager for large C++ projects. The benefits of using Conan, in the contrary to Spack, is the third-party packages are attached with the individual project, instead of an environment. Therefore, users don't need to go into a specific environment to use the packages. For more information, please refer to the [Conan Documentation](https://docs.conan.io/2/index.html).

### Setup the project with Conan

First make sure you have installed and updated the git submodule. If not, run the following command:

```bash
git submodule update --init --recursive
```

Then configure CMake and build the project

```bash
export SIMPATH=%PATH_TO_FAIRSOFT%
export FAIRROOTPATH=%PATH_TO_FAIRROOT%
# from %R3BRoot_DIRECTORY%
cmake --preset default
cmake --build --preset default -- -j${number_of_threads}
source build/config.sh
```

### Use Ninja

By default, CMake uses "Gnu Makefile" to build the project under the hood. However, "Gnu makefile" is pretty slow and it's recommended to use "ninja" if ninja is available in your system. To do this, instead run the configuration step with:

```bash
cmake --preset default -Gninja
```

or put the following command in the `.bashrc` (or `.zshrc` if zsh is used):

```bash
export CMAKE_GENERATOR=Ninja
```

### Add third-party packages

Adding a package to the project is pretty easy. Suppose you want to use Eigen library (a famous linear algebra library used by Tensorflow) to R3BRoot. First go to the [conan center](https://conan.io/center) and search for "eigen". You would find an entry related to "eigen/3.4.0", which contains further instructions how to add the package to your project. Under the "conanfile.py" tab, you could find a call to `self.requires`. Add this line to the `conanfile.py` located at the R3BRoot root folder:

```python
    def requirements(self):
        # other packages
        self.requires("eigen/3.4.0")
```
Be aware that you shouldn't change other variables like "settings" or "generators". 

Then in the `CMakeLists.txt`, add
```cmake
if(CONAN_ENABLED)
    find_package(Eigen3 REQUIRED)
endif()
```
and link this target to your executable or library. See [CMake usage](cmake_usage.md) about CMake target linkage.
