# Building with CMake

## vcpkg dependencies

This project contains vcpkg port files to install some dependencies, but this
is optional. You may also install those dependencies manually or using any
other package manager, however the port files are already provided and make
automatic dependency management easier.

If you have vcpkg set up, then you have to provide some extra flags to the
configure command, which differs only in how you provide the paths depending on
your OS:

```batch
rem Windows
-D "CMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake"
-D "VCPKG_OVERLAY_PORTS=%cd%/ports"
```

```sh
# Unix based (Linux, macOS)
-D "CMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
-D "VCPKG_OVERLAY_PORTS=$PWD/ports"
```

## Build

Besides the ones for vcpkg dependencies (if you choose to provide them to the
project using vcpkg), this project doesn't require any special command-line
flags to build to keep things simple.

Here are the steps for building in release mode with a single-configuration
generator, like the Unix Makefiles one:

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release # vcpkg flags
cmake --build build
```

Here are the steps for building in release mode with a multi-configuration
generator, like the Visual Studio ones:

```sh
cmake -S . -B build # vcpkg flags
cmake --build build --config Release
```

## Install

This project doesn't require any special command-line flags to install to keep
things simple. As a prerequisite, the project has to be built with the above
commands already.

The below commands require at least CMake 3.15 to run, because that is the
version in which [Install a Project][1] was added.

Here is the command for installing the release mode artifacts with a
single-configuration generator, like the Unix Makefiles one:

```sh
cmake --install build
```

Here is the command for installing the release mode artifacts with a
multi-configuration generator, like the Visual Studio ones:

```sh
cmake --install build --config Release
```

[1]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
