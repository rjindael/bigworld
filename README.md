# shambhala

Experimental 3D playground in C using OpenGL

Currently, Shambhala exists solely as a means for me to learn OpenGL (by following the [Learn OpenGL tutorial](https://learnopengl.com/) to spec) and 3D rendering overall, as well as more fundamental concepts of memory management by using C (high level assembler) instead of C++ (a high level language).

Using only SDL3 (for window management & GL window management) and GLAD (to wrangle GL pointers) for now as external libraries. No plans to add any more external libraries in the future, as many only support C++.

Checklist of things I want to do of varying complexity, in no particular order:

- [x] Get a basic GL app working (using SDL3)
- [x] Abstract a basic OpenGL rendering engine
- [x] Abstract a basic world data model
- [ ] Get basic 3D rendering working
- [ ] Finish LearnOpenGL tutorial and whatever else
- [ ] Mess with boolean operations for sculpting 3D meshes
- [ ] Experiment with shaders
- [ ] Shadow mapping
- [ ] Shadow volumes that are then stenciled (and get this working with a vertex or geometry shader)
- [ ] Post processing effects (MSAA, SSAA, FXAA, bloom, color correction, blurring, sunrays)
- [ ] PBR materials
- [ ] Atmospheric effects (God rays, clouds, fog, haze, glare)
- [ ] Have a nice shader development pipeline, since I realize shaders get more complex as visual effects become more varied
- [ ] Eventually migrate over to SDL2 and start processing inputs from KB+M and a gamepad
- [ ] Experiment with getting this on Linux and Android and WASM (GL ES)
- [ ] Experiment with 3D audio with SDL2 audio
- [ ] Experiment with particles, soft particles, whatever else
- [ ] Ambient occlusion with a fragment shader (SSAO/HBAO)
- [ ] High refresh rate rendering
- [ ] 3D camera controls
- [ ] Basic physics with BulletPhysics
- [ ] libsm64 support

## Resources

Resources I have collected that I have found useful:

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
- [Pauls Projects impl. of this](https://www.paulsprojects.net/opengl/shadvol/shadvol.html)
- [Silhouette detection from ogldev](https://ogldev.org/www/tutorial39/tutorial39.html)
- [Stencil shadow volume from ogldev](https://ogldev.org/www/tutorial40/tutorial40.html)
- [Some tutorial on shadow volumes](http://nuclear.mutantstargoat.com/articles/volume_shadows_tutorial_nuclear.pdf)
- [nVidia article from McGuire](https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-9-efficient-shadow-volume-rendering)
- [Presentation on this](https://web.cse.ohio-state.edu/~shen.94/781/Site/Slides_files/shadow.pdf)
- [StackOverflow thing on silhouette detection/edge detection with overlapping bodies](https://stackoverflow.com/questions/65010520/opengl-how-to-write-to-stencil-buffer-when-stencil-test-fails-and-depth-test-suc/65021525#65021525)
- [Shadow volumes from dev fjord](https://dev-fjord.blogspot.com/2012/03/jogl-part-3-shadow-volumes.html)
- [Ogre C++ impl.](https://github.com/OGRECave/ogre/blob/master/OgreMain/src/OgreShadowCaster.cpp)
- [fastShadows by Morgan McGuire](https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/fastShadows.pdf)
- [OpenGL stencil talk](https://opengl.org/archives/resources/features/StencilTalk/)
- [Half-Life stencil shadows](https://twhl.info/wiki/page/Tutorial%3A_Adding_hack-free_Stencil_Shadows_to_Half-Life)
- [Guide to shadows by Josh Beam](https://joshbeam.com/articles/stenciled_shadow_volumes_in_opengl/)

**Shadow mapping**:

- [LearnOpenGL explainer](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)
- [Wikipedia page on it](https://en.wikipedia.org/wiki/Shadow_mapping) - see "Principle of a shadow and a shadow map"

**Soft particles**:

- [dev.to Implementing soft particles in WebGL and OpenGL ES](https://dev.to/keaukraine/implementing-soft-particles-in-webgl-and-opengl-es-3l6e)

**PBR**:

- [LearnOpenGL explainer on PBR](https://learnopengl.com/PBR/Theory)

**Post Processing effects**:

- [LearnOpenGL DoF](https://en.wikibooks.org/wiki/OpenGL_Programming/Depth_of_Field)
- [LearnOpenGL Bloom](https://learnopengl.com/Advanced-Lighting/Bloom) - get highlighted frags, blur, then composite :D

## License

shambhala is licensed under the [GPL v2.0](https://github.com/rjindael/shambhala/blob/trunk/LICENSE.md). A copy of it has been included with shambhala.
