## Falling sand sim - Compute shader

**I just love falling sand simulations and found all the execuses to make
another one!**

This is a test of compute shaders which at the start were completely new to me.
Figured out how to get multiple textures (one to draw on and one as a world
collision texture - *the smiley face*) into different shaders, how to make it
all run.

![p1](./pics/p1.jpg)

I thought there might be some issues with race conditions among the sand
particles (*because I'm not taking care of that at all at this point*), but it
feels like it's running without problems!
