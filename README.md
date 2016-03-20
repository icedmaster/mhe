#mhe (Mighty Hamster Engine)
 
##What is it? 
This is a very simple framework that I use mostly for educational purposes.
I started it in order to get myself familiar with different rendering techniques and
algorithms.

So far it includes primitive core library (STL-like containers, strings and so forth), math library (without SIMD optimizations)
and an OpenGL based deferred renderer. The framework still carries a heavy burden of my inexperience, sometimes you
can find a STL string or vector or some files use tabs while mainly I switched to spaces. However, I'm working on it
and going to clean all this stuff up in the nearest future.
The code uses C++ without any C++ 11 features.

Here come a few words about features. In the mhe you can find:
- deferred renderer with extremely fat GBuffer;
- cascaded shadow maps (something like Stable Cascaded Shadow Maps);
- some posteffects, such as SSR, SSAO, DOF (fake, though), Bloom and FXAA;
- spherical harmonics usage (for sample 12);
- compute shaders usage (sample 13);
- skeletal animation usage;
- ImGUI for debugging.

Again, I've been using this code for my own education, and there hasn't been any necessity to keep all the samples
working. But these examples should work:
08_sponza - a famous scene that was shared by Crytek, with posteffects;
11_animation - a skeletal animation sample;
12_sh - primitive precomputed radiance transfer sample;
13_compute - some image manipulations using compute shaders.

Mostly, the code requires OpenGL 3.3 support, but currently it's only possible to use it with drivers and GPUs which
support OpenGL 4.3.

##BUILD

You have to install cmake (I built it with cmake 2.8 and 3.1).

###For linux users:
I did test mhe with gcc but I suppose it should be successfully compiled using clang.
The framework depends on SDL2 and OpenGL. meshconverter tool requires assimp library. In order to use a script that makes
this conversion more convenient you should also install racket.
After all additional packets are installed do the following:

cd build

cmake . -DCMAKE_BUILD_TYPE=Debug

make

After that you can build the examples:

cmake . -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=True

make

All output files will be placed in the mhe/bin directory.

It's possible to use QtCreator, but I saw a bug when the debug doesn't start from the IDE. If you have faced this problem
go to the build directory and remove the CMakeList.txt.user file. The re-open the project.

###For Windows users

Build with Visual Studio
I tested the engine with Visual Studio 10 and 13. All third-party libraries, such as SDL, are already included.
Do the following steps (change the version of the Visual Studio to the one you have installed):

cd build

cmake . -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 10"

Open the solution and build the library. Then you need to run cmake again in order to build the examples:

cmake . -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=True

Build the example you want to check out then in the Project's Properties choose the Debug tab and change the working
directory to the mhe/bin/Debug.

Build with MinGW
If you have an intention to build it using mingw then you need to build SDL2 library first.

##LICENSE AND CREDITS
Feel free to use any part of the code for any purposes.
If you have any questions or recommendations my e-mail is:

icedmaster@gmail.com

