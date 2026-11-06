# CPURender

[🇬🇧 English](README.md) | 🇫🇷 Français

Un **moteur de rendu 3D logiciel écrit en C++20** qui s'exécute entièrement sur le CPU. Aucun OpenGL, DirectX ou Vulkan : le traitement des sommets, le clipping, la rasterisation et le calcul des pixels sont tous codés à la main, et le framebuffer final est simplement envoyé à l'écran via [SDL3](https://www.libsdl.org/).

Le projet reproduit une API de type GPU (vertex buffers, constant buffers, vertex/pixel shaders, draw commands) au-dessus d'un rasterizer multithreadé découpé en tuiles.

## Fonctionnalités

- **Pipeline de rendu maison** : vertex shader → clipping → backface culling → rasterisation → pixel shader
- **Rasterizer par tuiles** (32×32 px) parallélisé avec **OpenMP**
- **Interpolation perspective-correcte** de la position monde, des normales et des UV, avec un depth buffer en 1/w
- **Frustum culling** des meshes à partir de leurs bounding boxes
- **Shaders programmables** via les interfaces `IVertexShader` / `IPixelShader` :
  - shader d'éclairage simple (texture d'albédo + spéculaire)
  - shader de terrain procédural (mélange de textures selon la hauteur)
  - shader de skybox (cube map)
  - shader de particules
- **Éclairage** : lumière ambiante, directionnelle, ponctuelle et spot (les spots sont signalés comme expérimentaux dans le code), avec un tone mapping simple
- **Terrain procédural** généré par **bruit de Perlin / fractal Brownian motion** (height map 256×256)
- **Eau animée** avec des **vagues de Gerstner** dans le vertex shader
- **Système de particules** (jusqu'à 100 000) avec des émetteurs de feu, fumée, pluie, étincelles, braises, neige, lucioles, poussière et fontaine
- **Chargement de modèles OBJ** et de textures via `stb_image`
- **Skybox** en cube map
- Caméra libre avec contrôle à la souris

## Pipeline de rendu

```
Engine::Update()
  ├─ frustum culling (par mesh)
  ├─ remplissage du ConstantBuffer (matrices, lumières, caméra, temps…)
  └─ RenderAPI::AddVertexBufferToDraw()  → DrawCommand

RenderAPI::FlushDraw()
  ├─ VertexShaderPass()      exécute le vertex shader sur chaque sommet
  ├─ ClippingPass()          découpe les triangles contre les plans du frustum
  ├─ BackfaceCullingPass()   élimine les faces arrière (sens d'enroulement inversable)
  └─ RasterizingPass()
       ├─ Rasterizer::Update()   setup des triangles (bounding box, gradients barycentriques / attributs)
       └─ Rasterizer::Render()   répartit les triangles par tuile, puis shade les tuiles en parallèle (OpenMP)
```

La fenêtre est créée en `2 × WIDTH` par `2 × HEIGHT`, tandis que le framebuffer interne fait **960×720** et est agrandi avec un filtrage « nearest neighbor ». Cela donne un rendu pixelisé et garde une charge CPU raisonnable. Le framerate est plafonné à 60 FPS et le FPS moyen est affiché dans la console toutes les secondes.

## Prérequis

- **Windows** (x64)
- **Visual Studio** avec le **toolset MSVC v145** (celui configuré dans le projet) et la charge de travail C++ pour le bureau
- C++20 et OpenMP (`/openmp:llvm`), déjà configurés dans le projet

Les dépendances sont **fournies** dans le dossier `Libs/` : il n'y a rien à installer séparément.

| Bibliothèque | Rôle | Emplacement |
|---|---|---|
| [SDL3](https://www.libsdl.org/) | Fenêtre, entrées, affichage | `Libs/SDL3` |
| [GLM](https://github.com/g-truc/glm) | Maths vecteurs / matrices | `Libs/glm` |
| [stb_image](https://github.com/nothings/stb) | Chargement d'images | `CPURender/stb_image.h` |

## Compilation et exécution

1. Ouvrir `CPURender.sln` dans Visual Studio.
2. Choisir la plateforme **`x64`**.
3. Choisir une configuration :
   - **Release** est fortement recommandée : un rendu logiciel en Debug est très lent.
   - Debug reste utile pour suivre le pipeline pas à pas.
4. Compiler et lancer (`F5` / `Ctrl+F5`).

Le fichier `.vcxproj.user` définit déjà le **répertoire de travail du débogueur sur le dossier de la solution** (pour que les chemins relatifs `Assets/...` soient résolus) et ajoute `Libs/SDL3/lib/x64` au `PATH` (pour que `SDL3.dll` soit trouvée).

> **Lancer l'`.exe` en dehors de Visual Studio ?** Vérifiez que :
> - le répertoire de travail est la racine du projet (le dossier qui contient `Assets/`), et
> - `SDL3.dll` (dans `Libs/SDL3/lib/x64/`) est à côté de l'exécutable ou dans le `PATH`.

## Contrôles

| Touche | Action |
|---|---|
| `W` / `Z` | Avancer |
| `S` | Reculer |
| `A` / `Q` | Aller à gauche |
| `D` | Aller à droite |
| `Espace` | Monter |
| `Maj gauche` | Descendre |
| `Souris` | Regarder autour (quand la souris est verrouillée) |
| `Échap` | Verrouiller / déverrouiller la souris |
| `C` | Inverser le sens d'enroulement du backface culling |
| `M` | Afficher / masquer la carte de bruit de Perlin |
| `L` | Déclencher un log ponctuel (`Logger`) |

Les dispositions QWERTY (`W/A/S/D`) et AZERTY (`Z/Q/S/D`) sont toutes deux prises en charge.

## Structure du projet

```
.
├── Assets/              Textures, faces de skybox et modèles OBJ
├── Libs/
│   ├── glm/             Headers GLM
│   └── SDL3/            Headers SDL3 et libs / DLL x64
├── CPURender/           Code source
│   ├── main.cpp/.h          Fenêtre SDL, boucle principale, framebuffer (couche plateforme fournie)
│   ├── engine.cpp/.h        Mise en place de la scène, gestion des entrées, update par frame
│   ├── RenderAPI.*          API de type GPU : ressources, draw commands, passes du pipeline
│   ├── Rasterizer.*         Rasterizer de triangles par tuiles, parallélisé avec OpenMP
│   ├── Vertex.*, Mesh.*     Données de sommets, génération de meshes (cube, cube inversé, particules)
│   ├── ObjLoader.*          Chargeur Wavefront OBJ
│   ├── Texture.*, CubeMap.* Stockage et échantillonnage des textures
│   ├── VertexShader*.cpp    Vertex shaders (simple, terrain procédural, particules)
│   ├── PixelShader*.cpp     Pixel shaders (simple, terrain procédural, skybox, particules)
│   ├── LightManager.*       Lumières ambiante / directionnelle / ponctuelle / spot
│   ├── PerlinNoise.*        Génération du terrain procédural
│   ├── Particle*.*          Gestionnaire de particules et émetteurs
│   ├── Camera*.*            Caméra et matrices vue/projection
│   ├── TransformMatrix.*    Transformations (position / rotation / échelle)
│   └── Xoshiro128.*         Générateur de nombres pseudo-aléatoires rapide
└── CPURender.sln
```

## Configuration

Quelques constantes intéressantes à modifier :

| Paramètre | Fichier | Valeur par défaut |
|---|---|---|
| Résolution interne (`WIDTH`, `HEIGHT`) | `main.h` | 960 × 720 |
| Taille des tuiles du rasterizer (`RASTER_CHUNK_SIZE`) | `Rasterizer.h` | 32 |
| Nombre max de particules | `Engine::Engine()` dans `engine.cpp` | 100 000 |
| Vitesse caméra / sensibilité souris | `engine.h` | 30 / 0,1 |
| Taille du terrain (`WIDTHMAP`, `HEIGHTMAP`) | `PerlinNoise.h` | 256 × 256 |
| Near / far / FOV de la caméra | `Camera.h` | 0,1 / 2500 / 45° |

## Ajouter ses propres shaders

1. Créer une classe qui dérive de `IVertexShader` et/ou `IPixelShader`.
2. L'enregistrer dans `Engine::init()` avec `m_renderAPI.CreateVertexShader<T>()` / `CreatePixelShader<T>()`.
3. Affecter les handles retournés à un `Mesh` (`m_vertexShaderHandle`, `m_pixelShaderHandle`).