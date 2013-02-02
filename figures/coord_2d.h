#ifndef COORD_2D_H
#define COORD_2D_H

/*
 * Represens a 2D coordinate. It´s used for 2d "maps" such as polygons,
 * textures, etc.
 *
 * u: Horizontal coordinate.
 * v: Vertical coordinate.
 */
typedef struct
{
	long double u;
	long double v;
} Coord2D;

#endif
