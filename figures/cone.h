#ifndef CONE_H
#define CONE_H

#include "cylinder.h"

/*
 * IMPORTANT NOTE: A Cone uses the same strucutre as a cylinder. A cylinder and
 * a cone only differ on their intersection and normal functions. A cone struct
 * represents two cones connected by their top at 'anchor'. Also, the following
 * attributes on a Cone struct are interpreted differently from a Cylinder.
 * radius: Width::Height ratio. A ratio of 2 means that the cone is two times
 *        wider than its height. A ratio of 0.5 means that the cone is two
 *        times higher than its width.
 * front_length: Length of the cone that points to 'direction'.
 * back_length: Length of the cone that point to inverse 'direction'.
 * anchor: Point from which both cones emerge.
 */
typedef Cylinder Cone;

void* get_cone_intersection(Vector eye, Vector dir_vec, void* object_ptr, int *length);
Vector get_cone_normal_vector(Vector posn, void* cone_ptr);

#endif
