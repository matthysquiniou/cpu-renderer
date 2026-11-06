# CPURender

🇬🇧 English | [🇫🇷 Français](README_FR.md)

A **software 3D renderer written in C++20** that runs entirely on the CPU. There is no OpenGL, DirectX or Vulkan involved: vertex processing, clipping, rasterization and pixel shading are all implemented by hand, and the final framebuffer is simply pushed to the screen through [SDL3](https://www.libsdl.org/).

The project mimics a GPU-style API (vertex buffers, constant buffers, vertex/pixel shaders, draw commands) on top of a multithreaded tile-based rasterizer.

## Features

- **Custom rendering pipeline**: vertex shader → clipping → backface culling → rasterization → pixel shader
- **Tile-based rasterizer** (32×32 px tiles) parallelized with **OpenMP**
- **Perspective-correct interpolation** of world position, normals and UVs, with a 1/w depth buffer
- **Frustum culling** of meshes using their bounding boxes
- **Programmable shaders** through the `IVertexShader` / `IPixelShader` interfaces:
  - simple lit shader (albedo texture + specular)
  - procedural terrain shader (multi-texture blending by height)
  - skybox shader (cube map)
  - particle shader
- **Lighting**: ambient, directional, point and spot lights (spot lights are flagged as experimental in the code), with simple tone mapping
- **Procedural terrain** generated with **Perlin noise / fractal Brownian motion** (256×256 height map)
- **Animated water** using **Gerstner waves** in the vertex shader
- **Particle system** (up to 100 000 particles) with emitters for fire, smoke, rain, sparks, embers, snow, fireflies, dust and fountain
- **OBJ model loading** and texture loading via `stb_image`
- **Cube map skybox**
- Free-fly camera with mouse look

## Rendering pipeline

```
Engine::Update()
  ├─ frustum culling (per mesh)
  ├─ fill ConstantBuffer (matrices, lights, camera, time…)
  └─ RenderAPI::AddVertexBufferToDraw()  → DrawCommand

RenderAPI::FlushDraw()
  ├─ VertexShaderPass()      run the vertex shader on every vertex
  ├─ ClippingPass()          clip triangles against the frustum planes
  ├─ BackfaceCullingPass()   cull back faces (winding order toggleable)
  └─ RasterizingPass()
       ├─ Rasterizer::Update()   triangle setup (bounding box, barycentric/attribute gradients)
       └─ Rasterizer::Render()   bin triangles into tiles, then shade tiles in parallel (OpenMP)
```

The window is created at `2 × WIDTH` by `2 × HEIGHT`, while the internal framebuffer is **960×720** and is upscaled with nearest-neighbor filtering, which gives a chunky pixel look and keeps the CPU workload reasonable. Frame rate is capped at 60 FPS and the average FPS is printed to the console every second.

## Requirements

- **Windows** (x64)
- **Visual Studio** with the **MSVC v145 toolset** (as configured in the project) and the C++ desktop workload
- C++20 and OpenMP (`/openmp:llvm`), already set in the project

Dependencies are **bundled** in the `Libs/` folder, so nothing needs to be installed separately:

| Library | Purpose | Location |
|---|---|---|
| [SDL3](https://www.libsdl.org/) | Window, input, display | `Libs/SDL3` |
| [GLM](https://github.com/g-truc/glm) | Vector / matrix math | `Libs/glm` |
| [stb_image](https://github.com/nothings/stb) | Image loading | `CPURender/stb_image.h` |

## Build & run

1. Open `CPURender.sln` in Visual Studio.
2. Select the **`x64`** platform.
3. Select a configuration:
   - **Release** is strongly recommended: Debug builds of a software renderer are very slow.
   - Debug is useful for stepping through the pipeline.
4. Build and run (`F5` / `Ctrl+F5`).

The `.vcxproj.user` file already sets the debugger **working directory to the solution folder** (so that the relative `Assets/...` paths resolve) and adds `Libs/SDL3/lib/x64` to the `PATH` (so that `SDL3.dll` is found).

> **Running the `.exe` outside Visual Studio?** Make sure that:
> - the working directory is the project root (the folder that contains `Assets/`), and
> - `SDL3.dll` (from `Libs/SDL3/lib/x64/`) is next to the executable or on your `PATH`.

## Controls

| Key | Action |
|---|---|
| `W` / `Z` | Move forward |
| `S` | Move backward |
| `A` / `Q` | Move left |
| `D` | Move right |
| `Space` | Move up |
| `Left Shift` | Move down |
| `Mouse` | Look around (when the mouse is locked) |
| `Esc` | Lock / unlock the mouse |
| `C` | Toggle backface culling winding order |
| `M` | Show / hide the Perlin noise map overlay |
| `L` | Trigger a one-shot log (`Logger`) |

Both QWERTY (`W/A/S/D`) and AZERTY (`Z/Q/S/D`) layouts are supported.

## Project structure

```
.
├── Assets/              Textures, skybox faces and OBJ models
├── Libs/
│   ├── glm/             GLM headers
│   └── SDL3/            SDL3 headers and x64 libs / DLL
├── CPURender/           Source code
│   ├── main.cpp/.h          SDL window, main loop, framebuffer (provided platform layer)
│   ├── engine.cpp/.h        Scene setup, input handling, per-frame update
│   ├── RenderAPI.*          GPU-like API: resources, draw commands, pipeline passes
│   ├── Rasterizer.*         Tile-based, OpenMP-parallel triangle rasterizer
│   ├── Vertex.*, Mesh.*     Vertex data, mesh generation (cube, inverted cube, particles)
│   ├── ObjLoader.*          Wavefront OBJ loader
│   ├── Texture.*, CubeMap.* Texture storage and sampling
│   ├── VertexShader*.cpp    Vertex shaders (simple, procedural map, particles)
│   ├── PixelShader*.cpp     Pixel shaders (simple, procedural map, skybox, particles)
│   ├── LightManager.*       Ambient / directional / point / spot lights
│   ├── PerlinNoise.*        Procedural terrain generation
│   ├── Particle*.*          Particle manager and emitters
│   ├── Camera*.*            Camera and view/projection handling
│   ├── TransformMatrix.*    Transform hierarchy (position / rotation / scale)
│   └── Xoshiro128.*         Fast pseudo-random number generator
└── CPURender.sln
```

## Configuration

A few constants worth tweaking:

| Setting | File | Default |
|---|---|---|
| Internal resolution (`WIDTH`, `HEIGHT`) | `main.h` | 960 × 720 |
| Rasterizer tile size (`RASTER_CHUNK_SIZE`) | `Rasterizer.h` | 32 |
| Max particles | `Engine::Engine()` in `engine.cpp` | 100 000 |
| Camera speed / mouse sensitivity | `engine.h` | 30 / 0.1 |
| Terrain size (`WIDTHMAP`, `HEIGHTMAP`) | `PerlinNoise.h` | 256 × 256 |
| Camera near / far / FOV | `Camera.h` | 0.1 / 2500 / 45° |

## Adding your own shaders

1. Create a class deriving from `IVertexShader` and/or `IPixelShader`.
2. Register it in `Engine::init()` with `m_renderAPI.CreateVertexShader<T>()` / `CreatePixelShader<T>()`.
3. Assign the returned handles to a `Mesh` (`m_vertexShaderHandle`, `m_pixelShaderHandle`).