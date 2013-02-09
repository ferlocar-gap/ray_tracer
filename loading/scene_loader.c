/* scene_loader.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Reads the configuration file for the scene, and initializes the scene
 * environment and objects.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
#include <ctype.h>
#include <math.h>
#include "../scene_config.h"
#include "../utilities/memory_handler.h"
#include "../utilities/error_handler.h"
#include "../tracing/vector.h"
#include "../tracing/window.h"
#include "../tracing/color.h"
#include "../tracing/object.h"
#include "../tracing/light.h"
#include "../figures/sphere.h"
#include "../figures/plane.h"
#include "../figures/polygon.h"
#include "../figures/coord_2d.h"
#include "../figures/cylinder.h"
#include "../figures/cone.h"
#include "../figures/disc.h"

// Figure types codes
#define SPHERE_CODE 0
#define PLANE_CODE 1
#define POLYGON_CODE 2
#define DISC_CODE 3
#define CYLINDER_CODE 4
#define CONE_CODE 5

// Methods

/*
 * Loads an integer from a configuration setting.
 *
 * setting: setting where the boolean attribute is located.
 * attr_path: path to the boolean attribute inside the setting.
 */
long double load_boolean(config_setting_t *setting, char *attr_path)
{
    int result;
    if (!config_setting_lookup_bool(setting, attr_path, &result)) throw_config_error(setting, attr_path, "boolean");
    return result;
}

/*
 * Loads an integer from a configuration setting.
 *
 * setting: setting where the integer attribute is located.
 * attr_path: path to the integer attribute inside the setting.
 */
long double load_int(config_setting_t *setting, char *attr_path)
{
    int result;
    if (!config_setting_lookup_int(setting, attr_path, &result)) throw_config_error(setting, attr_path, "int");
    return result;
}

/*
 * Loads a long double from a configuration setting.
 *
 * setting: setting where the long double attribute is located.
 * attr_path: path to the long double attribute inside the setting.
 */
long double load_long_double(config_setting_t *setting, char *attr_path)
{
    double attr;
    long double result;
    if (config_setting_lookup_float(setting, attr_path, &attr))
        result = attr;
    else
        throw_config_error(setting, attr_path, "double");
    return result;
}

/*
 * Loads a setting from the configuration file.
 *
 * cfg: Loaded configuration file.
 * setting_path: path to the setting inside the configuration file.
 */
config_setting_t* load_setting_from_cfg(config_t *cfg, char *setting_path)
{
    config_setting_t *setting = config_lookup(cfg, setting_path);
    if (!setting) exit(MISSING_CONFIGURATION_ATTR_ERROR);
    return setting;
}

/*
 * Loads a sub setting from a setting.
 *
 * setting: setting where the sub setting is located.
 * setting_path: path to the sub setting inside the setting.
 */
config_setting_t* load_setting(config_setting_t *setting, char *setting_path)
{
    config_setting_t *child_setting = config_setting_get_member(setting, setting_path);
    if (!child_setting) throw_config_error(setting, setting_path, "setting");
    return child_setting;
}

/*
 * Loads a vector from a configuration setting.
 *
 * setting: setting where the vector is located.
 */
Vector load_vector(config_setting_t *setting)
{
    Vector vec;
    vec.x = load_long_double(setting, "x");
    vec.y = load_long_double(setting, "y");
    vec.z = load_long_double(setting, "z");
    return vec;
}

/*
 * Loads a color from a configuration setting.
 *
 * setting: setting where the color is located.
 */
Color load_color(config_setting_t *setting)
{
    Color color;
    color.red = load_long_double(setting, "red");
    color.green = load_long_double(setting, "green");
    color.blue = load_long_double(setting, "blue");
    return color;
}

/*
 * Loads a sphere figure from a setting.
 *
 * sphere_setting: setting where the sphere is located.
 * obj: object where the sphere intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 */
Sphere* load_sphere(config_setting_t *sphere_setting, Object *obj)
{
    Sphere *sphere;
    config_setting_t *center_setting;

    sphere = (Sphere*)get_memory(sizeof(Sphere), NULL);
    sphere->radius = load_long_double(sphere_setting, "radius");
    center_setting = load_setting(sphere_setting, "center");
    sphere->center = load_vector(center_setting);
    if(obj)
    {
        obj->get_intersections = &get_sphere_intersection;
        obj->get_normal_vector = &get_sphere_normal_vector;
    }
    return sphere;
}

/*
 * Creates a plane figure using a direction vector and a 3D anchor.
 * This functions makes sure that the plane normal is looking towards the eye.
 *
 * dir_vec: Direction at which the plane is looking.
 * anchor: Single 3D point that is located inside the plane.
 * eye: Position towards which the plane should be facing.
 */
Plane create_plane(Vector dir_vec, Vector anchor, Vector eye)
{
    Vector eye_dir;
    Plane plane;
    int dir_length;

    plane.offset = - do_dot_product(dir_vec, anchor);
    dir_length = normalize_vector(&dir_vec);
    plane.offset /= dir_length;
    plane.direction = dir_vec;
    // Invert the plane normal if it is not facing the scene eye
    eye_dir = subtract_vectors(anchor, eye);
    normalize_vector(&eye_dir);
    if(do_dot_product(eye_dir, plane.direction))
    {
        plane.offset *= -1;
        plane.direction = multiply_vector(-1, plane.direction);
    }
    return plane;
}

/*
 * Loads a plane figure from a setting.
 * This functions makes sure that the plane normal is looking towards the eye.
 *
 * plane_setting: setting where the plane is located.
 * obj: object where the plane intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 * eye: Position towards which the plane should be facing.
 */
Plane* load_plane(config_setting_t *plane_setting, Object *obj, Vector eye)
{
    Plane *plane;
    config_setting_t *dir_setting, *anchor_setting;
    Vector direction, anchor;

    plane = (Plane*) get_memory(sizeof(Plane), NULL);
	anchor_setting = load_setting(plane_setting, "anchor");
	dir_setting = load_setting(plane_setting, "direction");
	direction = load_vector(dir_setting);
	anchor = load_vector(anchor_setting);
	*plane = create_plane(direction, anchor, eye);
    if(obj)
    {
        obj->get_intersections = &get_plane_intersection;
        obj->get_normal_vector = &get_plane_normal_vector;
    }
    return plane;
}

/*
 * Loads a polygon figure from a setting.
 *
 * polygon_setting: setting where the polygon is located.
 * obj: object where the polygon intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 * eye: Position towards which the plane should be facing.
 */
Polygon* load_polygon(config_setting_t *polygon_setting, Object *obj, Vector eye)
{
    Polygon *polygon;
    int vertex_i;
    Axis discarded_axis;
    Vector *poly_points;
    Vector plane_dir_vec, vec1, vec2;
    config_setting_t *all_vertex_setting, *vertex_setting;

    polygon = (Polygon*)get_memory(sizeof(Polygon), NULL);
    all_vertex_setting = load_setting(polygon_setting, "vertex");
    polygon->vertex_amount = config_setting_length(all_vertex_setting);
    if(polygon->vertex_amount >= 3)
    {
        // Load vertex
        polygon->vertex = (Coord2D*) get_memory(sizeof(Coord2D) * polygon->vertex_amount, NULL);
        poly_points = (Vector*) get_memory(sizeof(Vector) * polygon->vertex_amount, NULL);
        for(vertex_i = 0; vertex_i < polygon->vertex_amount; vertex_i++)
        {
            vertex_setting = config_setting_get_elem(all_vertex_setting, vertex_i);
            poly_points[vertex_i] = load_vector(vertex_setting);
        }
        // Load plane using first 3 vertex
        vec1 = subtract_vectors(poly_points[1], poly_points[0]);
        vec2 = subtract_vectors(poly_points[2], poly_points[0]);
        plane_dir_vec = do_cross_product(vec1, vec2);
        polygon->plane = create_plane(plane_dir_vec, poly_points[0], eye);
        discarded_axis = get_discarded_axis(polygon->plane);
        // Map 3d vertex to 2d vertex
        for(vertex_i = 0; vertex_i < polygon->vertex_amount; vertex_i++)
            polygon->vertex[vertex_i] = transform_3d_to_2d(poly_points[vertex_i], discarded_axis);
    }
    else
    {
        print_error(MISSING_VERTEX_ERROR);
        exit(MISSING_VERTEX_ERROR);
    }
    if(obj)
    {
        obj->get_intersections = &get_polygon_intersection;
        obj->get_normal_vector = &get_polygon_normal_vector;
    }
    return polygon;
}

/*
 * Loads a disc figure from a setting.
 *
 * disc_setting: setting where the disc is located.
 * obj: object where the disc intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 * eye: Position towards which the plane should be facing.
 */
Disc* load_disc(config_setting_t *disc_setting, Object *obj, Vector eye)
{
    Disc *disc;
    Vector plane_dir_vec, vec1, vec2;
    config_setting_t *in_focus1_sett, *in_focus2_sett, *ext_focus1_sett, *ext_focus2_sett;

    disc = (Disc*) get_memory(sizeof(Disc), NULL);
    in_focus1_sett = load_setting(disc_setting, "inner_focus1");
    in_focus2_sett = load_setting(disc_setting, "inner_focus2");
    ext_focus1_sett = load_setting(disc_setting, "external_focus1");
    ext_focus2_sett = load_setting(disc_setting, "external_focus2");
    disc->inner_focus1 = load_vector(in_focus1_sett);
    disc->inner_focus2 = load_vector(in_focus2_sett);
    disc->ext_focus1 = load_vector(ext_focus1_sett);
    disc->ext_focus2 = load_vector(ext_focus2_sett);
    disc->inner_dist = load_long_double(disc_setting, "inner_distance");
    disc->ext_dist = load_long_double(disc_setting, "external_distance");
    // Load plane using focus points
    vec1 = subtract_vectors(disc->ext_focus2, disc->ext_focus1);
    vec2 = subtract_vectors(disc->inner_focus1, disc->ext_focus1);
    plane_dir_vec = do_cross_product(vec1, vec2);
    disc->plane = create_plane(plane_dir_vec, disc->ext_focus1, eye);
    if(obj)
    {
        obj->get_intersections = &get_disc_intersection;
        obj->get_normal_vector = &get_disc_normal_vector;
    }
    return disc;
}

/*
 * Loads a cylinder figure from a setting.
 *
 * cylinder_setting: setting where the cylinder is located.
 * obj: object where the cylinder intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 */
Cylinder* load_cylinder(config_setting_t *cylinder_setting, Object *obj)
{
    Cylinder *cylinder;
    config_setting_t *direction_setting, *anchor_setting;

    cylinder = (Cylinder*) get_memory(sizeof(Cylinder), NULL);
    direction_setting = load_setting(cylinder_setting, "direction");
    cylinder->direction = load_vector(direction_setting);
    normalize_vector(&(cylinder->direction));
    anchor_setting = load_setting(cylinder_setting, "anchor");
    cylinder->anchor = load_vector(anchor_setting);
	cylinder->radius = load_long_double(cylinder_setting, "radius");
	cylinder->is_finite = load_boolean(cylinder_setting, "is_finite");
	cylinder->front_length = load_long_double(cylinder_setting, "front_length");
	cylinder->back_length = - load_long_double(cylinder_setting, "back_length");
    if(obj)
    {
        obj->get_intersections = &get_cylinder_intersection;
        obj->get_normal_vector = &get_cylinder_normal_vector;
    }
    return cylinder;
}

/*
 * Loads a cone figure from a setting.
 *
 * cone_setting: setting where the cone is located.
 * obj: object where the cone intersection and normal functions will be loaded.
 *      If the parameter comes null, the functions will not be assigned.
 */
Cone* load_cone(config_setting_t *cone_setting, Object *obj)
{
    Cone *cone = (Cone*) load_cylinder(cone_setting, NULL);
    if(obj)
    {
        obj->get_intersections = &get_cone_intersection;
        obj->get_normal_vector = &get_cone_normal_vector;
    }
    return cone;
}

/*
 * Loads the figure of an object from a setting.
 *
 * obj_setting: object setting where the figure is located.
 * obj: object where the figure functions will be loaded.
 * conf: Structure where the scene configuration is being loaded.
 */
void* load_figure(config_setting_t *obj_setting, Object *obj, SceneConfig conf)
{
    int figure_code;
    config_setting_t *figure_setting;
    void *figure;

    figure_code = load_int(obj_setting, "figure_code");
    figure_setting = load_setting(obj_setting, "figure");
    switch(figure_code)
    {
    case SPHERE_CODE:
        figure = load_sphere(figure_setting, obj);
        break;
    case PLANE_CODE:
        figure = load_plane(figure_setting, obj, conf.eye);
        break;
    case POLYGON_CODE:
        figure = load_polygon(figure_setting, obj, conf.eye);
        break;
    case DISC_CODE:
        figure = load_disc(figure_setting, obj, conf.eye);
        break;
    case CYLINDER_CODE:
        figure = load_cylinder(figure_setting, obj);
        break;
    case CONE_CODE:
        figure = load_cone(figure_setting, obj);
        break;
    default:
        figure = NULL;
        break;
    }
    return figure;
 }

/*
 * Loads all the scene objects and stores them in the 'conf->objs' variable. The
 * size of the array will be stored in the 'conf->objs_length' variable.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_objects(config_t *cfg, SceneConfig *conf)
{
    int obj_i, cut_plane_i;
    Object curr_obj;
    Plane *cut_plane;
    config_setting_t *objs_setting, *obj_setting, *color_setting, *planes_setting, *plane_setting;

    objs_setting = load_setting_from_cfg(cfg, "objects");
    conf->objs_length = config_setting_length(objs_setting);
    if(!conf->objs_length) return;
    conf->objs = (Object*) get_memory(sizeof(Object)*conf->objs_length, NULL);
    for(obj_i = 0; obj_i < conf->objs_length; obj_i++)
    {
        obj_setting = config_setting_get_elem(objs_setting, obj_i);
        curr_obj.light_ambiental = load_long_double(obj_setting, "light_ambiental");
        curr_obj.light_material = load_long_double(obj_setting, "light_material");
        curr_obj.specular_material = load_long_double(obj_setting, "specular_material");
        curr_obj.mirror_material = load_long_double(obj_setting, "mirror_material");
        curr_obj.transparency_material = load_long_double(obj_setting, "transparency_material");
        curr_obj.translucency_material = load_long_double(obj_setting, "translucency_material");
        curr_obj.specular_pow = load_long_double(obj_setting, "specular_pow");
        color_setting = load_setting(obj_setting, "color");
        curr_obj.color = load_color(color_setting);
        // Cutting planes
        planes_setting = load_setting(obj_setting, "cutting_planes");
        curr_obj.cutting_planes_length = config_setting_length(planes_setting);
        if(curr_obj.cutting_planes_length)
        {
            curr_obj.cutting_planes = (Plane*) get_memory(sizeof(Plane) * curr_obj.cutting_planes_length, NULL);
            for(cut_plane_i = 0; cut_plane_i < curr_obj.cutting_planes_length; cut_plane_i++)
            {
                plane_setting = config_setting_get_elem(planes_setting, cut_plane_i);
                cut_plane = load_plane(plane_setting, NULL, conf->eye);
                curr_obj.cutting_planes[cut_plane_i] = *cut_plane;
                free(cut_plane);
            }
        }
        curr_obj.figure = load_figure(obj_setting, &curr_obj, *conf);
        conf->objs[obj_i] = curr_obj;
    }
}

/*
 * Loads all the scene light sources and stores them in the 'conf->lights' variable.
 * The size of the array will be stored in the 'conf->lights_length' variable.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_lights(config_t *cfg, SceneConfig *conf)
{
    int light_i;
    Light curr_light;
    config_setting_t *src_setting, *light_setting, *color_setting, *anchor_setting;

    src_setting = load_setting_from_cfg(cfg, "lights.sources");
    conf->lights_length = config_setting_length(src_setting);
    if(!conf->lights_length) return;
    conf->lights = (Light*) get_memory(sizeof(Light) * conf->lights_length, NULL);
    for(light_i = 0; light_i < conf->lights_length; light_i++)
    {
        light_setting = config_setting_get_elem(src_setting, light_i);
        curr_light.const_att_factor = load_long_double(light_setting, "const_att_factor");
        curr_light.lin_att_factor = load_long_double(light_setting, "lin_att_factor");
        curr_light.expo_att_factor = load_long_double(light_setting, "expo_att_factor");
        color_setting = load_setting(light_setting, "color");
        anchor_setting = load_setting(light_setting, "anchor");
        curr_light.anchor = load_vector(anchor_setting);
        curr_light.color = load_color(color_setting);
        conf->lights[light_i] = curr_light;
    }
}

/*
 * Loads the window position in the 'conf->window' variable.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_scene_window(config_t *cfg, SceneConfig *conf)
{
    config_setting_t *win_setting = load_setting_from_cfg(cfg, "window");
    conf->window.x_min = load_long_double(win_setting, "x_min");
    conf->window.y_min = load_long_double(win_setting, "y_min");
    conf->window.x_max = load_long_double(win_setting, "x_max");
    conf->window.y_max = load_long_double(win_setting, "y_max");
    conf->window.z_anchor = load_long_double(win_setting, "z_anchor");
}

/*
 * Loads the eye position in the 'conf->eye' variable.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_eye(config_t *cfg, SceneConfig *conf)
{
    config_setting_t *eye_setting = load_setting_from_cfg(cfg, "eye");
    conf->eye = load_vector(eye_setting);
    g_eye = conf->eye;
}

/*
 * Loads the background color in the 'conf->background' variable.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_background(config_t *cfg, SceneConfig *conf)
{
    config_setting_t *bg_setting = load_setting_from_cfg(cfg, "background");
    conf->background = load_color(bg_setting);
}

/*
 * Loads the enviornmental light factor in the variable 'lights.environment_light'
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_environment_light(config_t *cfg, SceneConfig *conf)
{
    config_setting_t *environment_setting = load_setting_from_cfg(cfg, "lights.environment_light");
    conf->environment_light = load_color(environment_setting);
}

/*
 * Loads the configuration for image generation. It includes maximum transparency level,
 * maximum antialiasing level, maximum mirror level, and the dimensions of the image.
 *
 * cfg: loaded configuration file.
 * conf: Structure where the scene configuration is being loaded.
 */
void load_image_gen_config(config_t *cfg, SceneConfig *conf)
{
    int cache_i;

    config_setting_t *config_setting = load_setting_from_cfg(cfg, "config");
    conf->max_transparency_level = load_int(config_setting, "max_transparency_level");
    conf->max_antialiase_level = load_int(config_setting, "max_antialiase_level");
    conf->max_mirror_level = load_int(config_setting, "max_mirror_level");
    conf->width_res = load_int(config_setting, "image_width");
    conf->height_res = load_int(config_setting, "image_height");

    conf->pixel_density = pow(2, conf->max_antialiase_level - 1);
	conf->row_ray_count = (conf->width_res * conf->pixel_density) + 1;
	conf->cache_size = (conf->pixel_density + 1) * conf->row_ray_count;
	conf->ray_cache = get_memory(sizeof(CachedRay) * conf->cache_size, NULL);
	// Initialize cache
	for(cache_i = 0; cache_i < conf->cache_size; cache_i++)
        conf->ray_cache[cache_i].row = -1;
}

/*
 * Loads the scene objects and its environment (window size, light sources, etc).
 *
 * scene_file_path: Path to the file that contains the scene configuration
 */
SceneConfig load_scene(char* scene_file_path)
{
    config_t cfg;
    SceneConfig scene_config;

    config_init(&cfg);
    // Read the file. If there is an error, report it and exit.
    if(! config_read_file(&cfg, scene_file_path))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
        config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        print_error(MISSING_CONFIGURATION_FILE_ERROR);
        exit(MISSING_CONFIGURATION_FILE_ERROR);
    }
    load_eye(&cfg, &scene_config);
    load_scene_window(&cfg, &scene_config);
    load_background(&cfg, &scene_config);
    load_objects(&cfg, &scene_config);
    load_lights(&cfg, &scene_config);
    load_environment_light(&cfg, &scene_config);
    load_image_gen_config(&cfg, &scene_config);
    config_destroy(&cfg);
    return scene_config;
}
