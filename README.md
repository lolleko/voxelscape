# Voxelscape

![CI](https://github.com/lolleko/voxelscape/workflows/CI/badge.svg)

Custom voxel engine written from scratch with OpenGL 4.1. And a small RTS style game on top of it.

![Screenshot](screenshots/sdf.png?raw=true "Screenshot")

## Features:

* Distance Field Shadows
* Voxel Ambient Occlusion
* Simple point lights (block emission)
* Basic RTS implemented using ECS
* Voxel rendered using advanced instancing, instead of the more popular greedy meshing
* Custom voxel model editor
* Simple terrain gerenation, with a few different biomes

[![YouTube](http://i.ytimg.com/vi/hcl-Bp3sYNE/hqdefault.jpg)](https://www.youtube.com/watch?v=hcl-Bp3sYNE)

| ![RTS](screenshots/rts.png?raw=true "Screenshot") | ![Main](screenshots/main.png?raw=true "Screenshot")    |
|---|---|
| ![Ambient Occlusion](screenshots/ao.png?raw=true "Screenshot")  | ![light](screenshots/light.png?raw=true "Screenshot") |

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
