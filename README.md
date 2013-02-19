= Basic Ray Tracer =

A basic C engine to generate graphics through the classic ray tracing algorithm.

== Features ==

=== Multiple figures ===

It can draw spheres, planes, polygons, discs, cylinders and cones.

=== Illumination ===

It can use multiple light sources with different colors using:
* Diffuse reflection
* Specular reflection
* Shadows
* Light degradation according to distance

=== Other features ===

* Cutting planes
* Adaptive Antialiasing
* Transparency
* Trasnlucency
* Mirrors

=== Configurable scence ===

It loads the image contents from a configuration file

=== BMP output ===
It outputs the generated scene as a BMP file.

== How to compile and use ==

This project was developed using Code::Blocks, so you should be able to edit and run this project by opening the project file 'Ray Tracer.cbp' in Code::Blocks.

You can also build the project with the gcc compiler using the following command at the root of the project:

gcc -I./libs ray_tracer.c tracing/*.c utilities/*.c figures/*.c loading/*.c -o ray_tracer.exe libs/libconfig_d.a

This should create a ray_tracer.c executable. Make sure to have the 'scene.cfg' and the 'libconfig_d.dll' files in the same folder as the ray_tracer.exe.

You can find the 'scene.cfg' at the root of the project, and the 'libconfig_d.dll' at the 'libs' folder.

You can generate an image by executing 'ray_tracer.exe'.

== Configuration ==

You can find a sample scene configuration at the root of the projec in the 'scene.cfg' file. This is the information that will be loaded by the ray tracer in order to draw the scene.

== For more information

Feel free to message me on Github (ferlocar-gap).

== Pending features ==

* More configuration file validations
* Textures
* Bump Mapping
* Refractive transparencies
* Texture maps

== Special thanks ==
Special thanks to 

* Francisco Torres Rojas, my computer graphics professor at the Instituto Tecnológico de Costa Rica.
* Mark Lindner, for providing the library used to read the configuration file (libconfig). Site: http://www.hyperrealm.com/libconfig/

== Copyright ==

Copyright (c) 2013 Carlos Fernandez.
