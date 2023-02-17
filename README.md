# Game of Life in OpenGL
A project I made while learning OpenGL and trying to wrap my head around CMake.
After opening, you can draw any starting generation you want. Then press Space to run it.

Other "controls":  
While drawing:

 - Space to run
 - C to clear the board

While running:

 - P to pause (U to unpause)
 - R to reset (goes back to drawing without clearing the board)

It wasn't meant to be like very serious project but a way to use OpenGL somehow, so there aren't too many features (next to none really).  
Also I'm pretty sure CMakePresets needs config to run on anything else than Windows (note from the future me)

To run it, first clone this repo

    git clone --recursive https://github.com/KomorXD/Game-of-Life-OpenGL
   
Make sure to do it recursively, since the repo depends on a few libraries stored as a submodule (glfw, glad, glm) built from source.
Then simply CMake it however you'd like.
On default it runs a 1280x720 window with 10px cells, might add a better option than changing variables some time.

# Libraries used  
- [glfw](https://github.com/glfw/glfw)
- [glad](https://github.com/Dav1dde/glad)
- [glm](https://github.com/g-truc/glm)
