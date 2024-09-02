# bigworld

OpenGL experiments

This is just a means for me to tinker around with OpenGL and get a fundamental grasp of how to perform 3D rendering for video games from C++. In this journey, I am using C++ and am following the [Learn OpenGL](https://learnopengl.com/) tutorial to learn OpenGL.

Currently tested to run on Windows only, but should compile and work for Linux (CI soon:tm:). Builds with Clang 18.1.1 targeting `x86_64-pc-windows-msvc`.

Checklist of things I want to do of varying complexity, in no particular order:

- [x] Get a basic GL app working (using glad+glfw)
- [ ] Get basic 3D rendering working (using cglm)
- [ ] Finish LearnOpenGL tutorial and whatever else
- [ ] Mess with boolean operations for sculpting 3D meshes
- [ ] Experiment with shaders (I don't really understand geometry shaders just yet)
- [ ] Shadow mapping
- [ ] Shadow volumes that are then stenciled (and get this working with a vertex or geometry shader)
- [ ] Post processing effects (MSAA, SSAA, FXAA, Bloom, Color correction, Blurring, Sun rays)
- [ ] PBR materials
- [ ] Atmospheric effects (God rays, clouds, fog, haze, glare)
- [ ] Have a nice shader development pipeline, since I realize shaders get more complex as visual effects become more varied
- [ ] Eventually migrate over to SDL2 and start processing inputs from KB+M and a gamepad
- [ ] Experiment with getting this on Linux and Android and WASM (GL ES)
- [ ] Experiment with 3D audio (SoLoud)
- [ ] Experiment with particles, soft particles, whatever else
- [ ] Ambient occlusion with a fragment shader (SSAO/HBAO)
- [ ] High refresh rate rendering
- [ ] 3D Camera controls :D
- [ ] Physics with BulletPhysics
- [ ] libsm64 support

## Resources

Some resources that I have found useful.

**High level info:**

- [LearnOpenGL](https://learnopengl.com)
- [All LearnOpenGL C++ chapter examples](https://github.com/JoeyDeVries/LearnOpenGL/tree/master)
- [ogldev](https://ogldev.org)
- [hello triangle by Anton Gerdelan](http://antongerdelan.net/opengl/hellotriangle.html) [(more)](http://antongerdelan.net/opengl/vertexbuffers.html)
- [hello triangle by Alexander Overvoorde](https://open.gl/drawing)
- [Debugging in OpenGL](https://learnopengl.com/In-Practice/Debugging)

**Shadow volumes**:

- [Wikipedia article on it](https://en.wikipedia.org/wiki/Shadow_volume)
- [Shadow Algorithms for Computer Graphics from SIGGRAPH](http://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S13/papers/crow_shadows_77.pdf)
- [GameDev post on "The Theory of Stencil Shadow Volumes"](www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/the-theory-of-stencil-shadow-volumes-r1873)
- [Carmack on shadow volumes](https://web.archive.org/web/20090127020935/http://developer.nvidia.com/attach/6832)
- [Shadow volume extrusion w/ VS](http://developer.amd.com/wordpress/media/2012/10/ShaderX_ShadowExtrusion.pdf)
- [NVIDIA dev article excerpt of GPU rendering book doing this w/ a GS and occlusion](https://web.archive.org/web/20110516024500/http://developer.nvidia.com/node/168)
- [Pauls Projects technical info on this](https://www.paulsprojects.net/opengl/shadvol/technical.html)
- [Pauls Projects impl of this](https://www.paulsprojects.net/opengl/shadvol/shadvol.html)
- [Silhouette detection from ogldev](https://ogldev.org/www/tutorial39/tutorial39.html)
- [Stencil shadow volume from ogldev](https://ogldev.org/www/tutorial40/tutorial40.html)
- [Some tutorial on shadow volumes](http://nuclear.mutantstargoat.com/articles/volume_shadows_tutorial_nuclear.pdf)
- [Nvidia article from McGuire :D](https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-9-efficient-shadow-volume-rendering)
- [Presentation on this](https://web.cse.ohio-state.edu/~shen.94/781/Site/Slides_files/shadow.pdf)
- [StackOverflow thing on silhouette detection/edge detection with overlapping bodies](https://stackoverflow.com/questions/65010520/opengl-how-to-write-to-stencil-buffer-when-stencil-test-fails-and-depth-test-suc/65021525#65021525)
- [Shadow volumes from dev fjord](https://dev-fjord.blogspot.com/2012/03/jogl-part-3-shadow-volumes.html)
- [Ogre C++ impl](https://github.com/OGRECave/ogre/blob/master/OgreMain/src/OgreShadowCaster.cpp)

**Shadow mapping**:

- [LearnOpenGL explainer](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)
- [Wikipedia page on it](https://en.wikipedia.org/wiki/Shadow_mapping) - see "Principle of a shadow and a shadow map"

**Soft particles**:

- [dev.to Implementing soft particles in WebGL and OpenGL ES](https://dev.to/keaukraine/implementing-soft-particles-in-webgl-and-opengl-es-3l6e)

**PBR**:

- [LearnOpenGL explainer on PBR](https://learnopengl.com/PBR/Theory)

**Post Processing effects**:

- [LearnOpenGL DoF](https://en.wikibooks.org/wiki/OpenGL_Programming/Depth_of_Field)
- [LearnOpenGL Bloom](https://learnopengl.com/Advanced-Lighting/Bloom) - get highlighted frags, blur, composite :D

## License

bigworld is licensed under the [MIT license](https://github.com/rjindael/bigworld/blob/trunk/LICENSE.md). A copy of it has been included with bigworld.

bigworld uses the following third-party libraries:

- [cxxopts](https://github.com/jarro2783/cxxopts), a header-only C++ argument parser library, licensed under the [MIT license](https://github.com/jarro2783/cxxopts/blob/master/LICENSE)
- [glad](https://github.com/Dav1dde/glad), a header-only C++ OpenGL function loader library, licensed under the [MIT license](https://github.com/Dav1dde/glad/blob/master/LICENSE)
- [SDL2](https://github.com/libsdl-org/SDL), a cross-platform development library, licensed under the [zlib license](https://github.com/libsdl-org/SDL/blob/main/COPYING.txt)
- [irrKlang](https://www.ambiera.com/irrklang/), a C++ audio engine, licensed under the [irrKlang license](https://www.ambiera.com/irrklang/license.html)