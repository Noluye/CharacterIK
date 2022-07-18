# CharacterIK
Inverse kinematics solvers and its application in character animation.

## Supported platform

- Windows

## Dependencies

CharacterIK library only depends on:

- [Math3D](https://github.com/Noluye/Math3D)

There are various sample projects, which depend on:

- [raylib](https://github.com/raysan5/raylib)
- [raygui](https://github.com/raysan5/raygui)

All dependencies are set as submodules, so you can automatically set them up.

## How to use

Clone the repository and its submodules:

```shell
git clone --recurse-submodules https://github.com/Noluye/CharacterIK.git
```

Double click `Win-Premake-VS2022.bat` or `Win-Premake-VS2019` to generate visual studio projects.

Set a sample project as startup project to run the application.

### How to update the repository and its dependencies

```shell
git pull
git submodule update --remote
```

## Sample projects

### ThreeBoneChain

![ThreeBoneChain](https://github.com/Noluye/CharacterIK/blob/main/doc/gif/Three-Bone-Chain-2022-07-14-18-12-40.gif)
