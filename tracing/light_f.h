#ifndef LIGHT_F_H
#define LIGHT_F_H

#include "../scene_config.h"
#include "vector.h"

Color get_color(Vector eye, Vector dir_vec, int mirror_level, SceneConfig conf);

#endif
