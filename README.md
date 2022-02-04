# Game of Life in OpenGL
A project I made while learning OpenGL and trying to wrap my head around CMake.
After opening, you can draw any starting generation you want. Then press Space to run it.

![Glider gun](https://s10.gifyu.com/images/ezgif.com-gif-maker80c41a9cb0dbc82c.gif)

Other "controls":  
While drawing:

 - Space to run
 - C to clear the board

While running:

 - P to pause (U to unpause)
 - R to reset (goes back to drawing without clearing the board)

It wasn't meant to be like very serious project but a way to use OpenGL somehow, so there aren't too many features (next to none really).

To run it, first clone this repo

    git clone --recursive https://github.com/KomorXD/Game-of-Life-OpenGL
   
Make sure to do it recursively, since the repo depends on a few libraries stored as a submodule (glfw, glad, glm) built from source.
Then simply CMake it however you'd like.
On default it runs a 1280x720 window with 10px cells, might add a better option than changing variables some time.
