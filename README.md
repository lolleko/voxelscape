## Requirements:

**Install vcpkg**

https://github.com/microsoft/vcpkg#quick-start

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

./bootstrap-vcpkg.sh
```

**Install packages**

`./vcpkg install @vcpkg_response_file.txt`

## Build:

clone this repo and cd to it

**Run cmake wih vcpkg toolchainfile**

`cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=$VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake`

Replace `$VCPKG_INSTALLATION_ROOT` with the path to your vcpkg repo.

You can also set `$VCPKG_INSTALLATION_ROOT` as enviroment variable this way you can ommit `-DCMAKE_TOOLCHAIN_FILE=`

e.g: `export VCPKG_INSTALLATION_ROOT=~/Repos/vcpkg && cmake -Bbuild`

**Excute the build**

`cmake --build build`

**Optional:** Symmlink compile_commands.json if you use clang based tooling in your IDE/Editor

`ln -s build/compile_commands.json .`

## Run

`./build/voxelscape`

## Recommended editor setup:

* VSCode
* clangd extension for auto completion
* cpp extenstion **(with intellisense disabled)** for debugging