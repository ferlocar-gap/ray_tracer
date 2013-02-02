#ifndef CACHED_RAY_H
#define CACHED_RAY_H

#include "color.h"

/*
 * Represents a thrown ray color.
 *
 * color: Color that returned the ray.
 * row: Row to which the ray belongs.
 */
typedef struct
{
	Color color;
	int row;
} CachedRay;

#endif
