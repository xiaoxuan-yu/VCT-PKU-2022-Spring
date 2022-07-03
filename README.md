# README

Final assignment of course *computer graphics* at PKU, an implementation of voxel cone tracing for global illumination.

## Directory Structure

- `src` Source code
- `data` Models and textures
- `third_party` Thrid party libraries
- `shader` GLSL Shader files

## Build Instruction

```bash
mkdir build
cd build
cmake ..
make
```

or using cmale-gui on windows.

## Using Instruction

- UI
  - Change rendering mode: scene / depth
  - Enable/Disable: cone tracing / diffuse / specular / ambient
  - Change the position of light source
- Keyboard
  - `Q` `E` `W` `A` `S` `D` for moving camera (Up / Down / Front / Back / Left / Right)
  - `Z` `X` `C` `V` for enable / disable: cone tracing / diffuse / specular / ambient
  - `Control` for unlock / lock the cursor
  - `ESC` for quit
- Mouse
  - Swipe the mouse to change the direction of camera.

## Third party libraries 

- assimp
- glad
- glfw
- glm
- imgui
- stb

**Keep the executable file in the same directory of `shader` and `data` !**

