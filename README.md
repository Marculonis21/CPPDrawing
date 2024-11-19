# C++/OpenGL graphics projects

*I always loved computer graphics so from time to time some nice visualization
catches my eye and then the best way how to learn new things is to do it on
your own!*

This is my sandbox folder for different shader/graphic programs which use C++
with OpenGL or SFML.

*** 

#### Building
Building of each project is done using CMAKE. To build a project just navigate
into its directory and run:
```bash
cmake -B build && cd build
make
./<name of the project> # run the compiled program
```
I usually leave projects in runnable state. However, it might happen that there
are some problems and/or compatibility issues and stuff like that (I am going
to try to help resolve your issues if you need help).

## Interesting projects:
### [Terrain with erosion](./terrainOpenGL/)
<p align="center">
  <img src="./terrainOpenGL/imgs/11.png" width=400/>
  <img src="./terrainOpenGL/imgs/erosion_normals1.png" width=400/>
</p>

#
### [Compute Slime](./slime/)
<p align="center">
  <img src="./slime/pics/s1.jpg" width=600/>
</p>

#
### [RayMarching](./RayMarch/)
<p align="center">
  <img src="./RayMarch/test1/pics/rm1.png" width=600/>
</p>

#
### [Compute Falling sand sim test](./falling_sand/)
<p align="center">
  <img src="./falling_sand/pics/p1.jpg" width=600/>
</p>

#
### [More SFML projects](./SFML/)
<p align="center">
  <img src="./SFML/DummyFluid/pics/fluid1.jpg" width=400/>
  <img src="./SFML/Cloth/pics/cloth1.jpg" width=400/>
  <img src="./SFML/Fire/pics/fire2.jpg" width=400/>
  <img src="./SFML/QuadTree/pics/qt1.jpg" width=400/>
</p>

