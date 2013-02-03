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

// Global variables
extern Window g_window;
extern Vector g_eye;
extern Color g_background;
extern Object *g_objs;
extern int g_objs_length;
extern Light *g_lights;
extern int g_lights_length;
extern long double g_environment_light;

// Methods

/*
 * Loads an integer from a configuration setting.
 */
long double load_boolean(config_setting_t *setting, char *attr_path)
{
    int result;
    if (!config_setting_lookup_bool(setting, attr_path, &result)) throw_config_error(setting, attr_path, "boolean");
    return result;
}

/*
 * Loads an integer from a configuration setting.
 */
long double load_int(config_setting_t *setting, char *attr_path)
{
    int result;
    if (!config_setting_lookup_int(setting, attr_path, &result)) throw_config_error(setting, attr_path, "int");
    return result;
}

/*
 * Loads a long double from a configuration setting.
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
 */
config_setting_t* load_setting_from_cfg(config_t *cfg, char *setting_path)
{
    config_setting_t *setting = config_lookup(cfg, setting_path);
    if (!setting) exit(MISSING_CONFIGURATION_ATTR_ERROR);
    return setting;
}

/*
 * Loads a sub setting from a setting.
 */
config_setting_t* load_setting(config_setting_t *setting, char *setting_path)
{
    config_setting_t *child_setting = config_setting_get_member(setting, setting_path);
    if (!child_setting) throw_config_error(setting, setting_path, "setting");
    return child_setting;
}

/*
 * Loads a vector from a configuration setting.
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
 * Loads the eye position in the 'g_eye' variable.
 */
void load_eye(config_t *cfg)
{
    config_setting_t *eye_setting = load_setting_from_cfg(cfg, "eye");
    g_eye = load_vector(eye_setting);
}

/*
 * Loads the window position in the 'g_window' variable.
 */
void load_scene_window(config_t *cfg)
{
    config_setting_t *win_setting = load_setting_from_cfg(cfg, "window");
    g_window.x_min = load_long_double(win_setting, "x_min");
    g_window.y_min = load_long_double(win_setting, "y_min");
    g_window.x_max = load_long_double(win_setting, "x_max");
    g_window.y_max = load_long_double(win_setting, "y_max");
    g_window.z_anchor = load_long_double(win_setting, "z_anchor");
}

/*
 * Loads the background color in the 'g_background' variable.
 */
void load_background(config_t *cfg)
{
    config_setting_t *bg_setting = load_setting_from_cfg(cfg, "background");
    g_background = load_color(bg_setting);
}

/*
 * Loads a sphere figure.
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
 * Loads a plane figure.
 */
Plane* load_plane(config_setting_t *plane_setting, Object *obj)
{
    Plane *plane;
    config_setting_t *dir_setting, *anchor_setting;
    Vector direction, anchor, eye_dir;
    long double dir_length;

    plane = (Plane*) get_memory(sizeof(Plane), NULL);
	anchor_setting = load_setting(plane_setting, "anchor");
	dir_setting = load_setting(plane_setting, "direction");
	direction = load_vector(dir_setting);
	anchor = load_vector(anchor_setting);
	plane->offset = - do_dot_product(direction, anchor);
	dir_length = normalize_vector(&direction);
	plane->offset /= dir_length;
	plane->direction = direction;
	// Make validation to make sure the normal vector is facing the eye.
	eye_dir = subtract_vectors(anchor, g_eye);
    normalize_vector(&eye_dir);
    if(do_dot_product(eye_dir, direction) >= 0)
    {
        plane->offset *= -1;
        plane->direction = multiply_vector(-1, plane->direction);
    }
    if(obj)
    {
        obj->get_intersections = &get_plane_intersection;
        obj->get_normal_vector = &get_plane_normal_vector;
    }
    return plane;
}

/*
 * Loads a polygon figure.
 */
Polygon* load_polygon(config_setting_t *polygon_setting, Object *obj)
{
    Polygon *polygon;
    Plane *poly_plane;
    int vertex_i;
    config_setting_t *plane_setting, *all_vertex_setting, *vertex_setting;

    polygon = (Polygon*)get_memory(sizeof(Polygon), NULL);
    plane_setting = load_setting(polygon_setting, "plane");
    poly_plane = load_plane(plane_setting, NULL);
    polygon->plane = *poly_plane;
    free(poly_plane);
    all_vertex_setting = load_setting(polygon_setting, "vertex");
    polygon->vertex_amount = config_setting_length(all_vertex_setting);
    polygon->vertex = (Coord2D*) get_memory(sizeof(Coord2D) * polygon->vertex_amount, NULL);
    for(vertex_i = 0; vertex_i < polygon->vertex_amount; vertex_i++)
    {
        vertex_setting = config_setting_get_elem(all_vertex_setting, vertex_i);
        polygon->vertex[vertex_i].u = load_long_double(vertex_setting, "u");
        polygon->vertex[vertex_i].v = load_long_double(vertex_setting, "v");
    }
    if(obj)
    {
        obj->get_intersections = &get_polygon_intersection;
        obj->get_normal_vector = &get_polygon_normal_vector;
    }
    return polygon;
}

/*
 * Loads a disc figure.
 */
Disc* load_disc(config_setting_t *disc_setting, Object *obj)
{
    Disc *disc;
    Plane *disc_plane;
    config_setting_t *plane_sett, *in_focus1_sett, *in_focus2_sett, *ext_focus1_sett, *ext_focus2_sett;

    disc = (Disc*) get_memory(sizeof(Disc), NULL);
    plane_sett = load_setting(disc_setting, "plane");
    disc_plane = load_plane(plane_sett, NULL);
    disc->plane = *disc_plane;
    free(disc_plane);
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
    if(obj)
    {
        obj->get_intersections = &get_disc_intersection;
        obj->get_normal_vector = &get_disc_normal_vector;
    }
    return disc;
}

/*
 * Loads a cylinder figure.
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
 * Loads a cone figure.
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
 * Loads the figure of an object
 */
void* load_figure(config_setting_t *obj_setting, Object *obj)
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
        figure = load_plane(figure_setting, obj);
        break;
    case POLYGON_CODE:
        figure = load_polygon(figure_setting, obj);
        break;
    case DISC_CODE:
        figure = load_disc(figure_setting, obj);
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
 * Loads all the scene objects and stores them in the 'g_objs' variable. The
 * size of the array will be stored in the 'g_objs_length' variable.
 */
void load_objects(config_t *cfg)
{
    int obj_i, cut_plane_i;
    Object curr_obj;
    Plane *cut_plane;
    config_setting_t *objs_setting, *obj_setting, *color_setting, *planes_setting, *plane_setting;

    objs_setting = load_setting_from_cfg(cfg, "objects");
    g_objs_length = config_setting_length(objs_setting);
    g_objs = (Object*) get_memory(sizeof(Object)*g_objs_length, NULL);
    for(obj_i = 0; obj_i < g_objs_length; obj_i++)
    {
        obj_setting = config_setting_get_elem(objs_setting, obj_i);
        curr_obj.light_ambiental = load_long_double(obj_setting, "light_ambiental");
        curr_obj.light_material = load_long_double(obj_setting, "light_material");
        curr_obj.specular_material = load_long_double(obj_setting, "specular_material");
        curr_obj.specular_pow = load_long_double(obj_setting, "specular_pow");
        color_setting = load_setting(obj_setting, "color");
        curr_obj.color = load_color(color_setting);
        // Cutting planes
        planes_setting = load_setting(obj_setting, "cutting_planes");
        curr_obj.cutting_planes_length = config_setting_length(planes_setting);
        curr_obj.cutting_planes = (Plane*) get_memory(sizeof(Plane) * curr_obj.cutting_planes_length, NULL);
        for(cut_plane_i = 0; cut_plane_i < curr_obj.cutting_planes_length; cut_plane_i++)
        {
            plane_setting = config_setting_get_elem(planes_setting, cut_plane_i);
            cut_plane = load_plane(plane_setting, NULL);
            curr_obj.cutting_planes[cut_plane_i] = *cut_plane;
            free(cut_plane);
        }
        curr_obj.figure = load_figure(obj_setting, &curr_obj);
        g_objs[obj_i] = curr_obj;
    }
}

/*
 * Loads all the scene light sources and stores them in the 'g_lights' variable.
 * The size of the array will be stored in the 'g_lights_length' variable.
 */
void load_lights(config_t *cfg)
{
    int light_i;
    Light curr_light;
    config_setting_t *src_setting, *light_setting, *color_setting, *anchor_setting;

    src_setting = load_setting_from_cfg(cfg, "lights.sources");
    g_lights_length = config_setting_length(src_setting);
    g_lights = (Light*) get_memory(sizeof(Light) * g_lights_length, NULL);
    for(light_i = 0; light_i < g_lights_length; light_i++)
    {
        light_setting = config_setting_get_elem(src_setting, light_i);
        curr_light.intensity = load_long_double(light_setting, "intensity");
        curr_light.const_att_factor = load_long_double(light_setting, "const_att_factor");
        curr_light.lin_att_factor = load_long_double(light_setting, "lin_att_factor");
        curr_light.expo_att_factor = load_long_double(light_setting, "expo_att_factor");
        color_setting = load_setting(light_setting, "color");
        anchor_setting = load_setting(light_setting, "anchor");
        curr_light.anchor = load_vector(anchor_setting);
        curr_light.color = load_color(color_setting);
        g_lights[light_i] = curr_light;
    }
}

/*
 * Loads the enviornmental light factor in the variable 'g_environment_light'
 */
void load_environment_light(config_t *cfg)
{
    config_setting_t *lights_setting = load_setting_from_cfg(cfg, "lights");
    g_environment_light = load_long_double(lights_setting, "environment_light");
}

/*

// Description:
// Creates a plane structure from the definition of a normal vector
// and an anchor.
PLANE* CreatePlaneFromVector(VECTOR normal_vector, VECTOR anchor)
{
	PLANE* plane = GetMemory(sizeof(PLANE), NULL);
	plane->d = - DotProduct(normal_vector, anchor);
	long double length = NormalizeVector(&normal_vector);
	// We need the normal vector to face the g_eye
	VECTOR dir_vector = SubtractVectors(anchor, g_eye);
	NormalizeVector(&dir_vector);
	if(DotProduct(dir_vector, normal_vector) < 0)
	{
		plane->d /= length;
		plane->a = normal_vector.x;
		plane->b = normal_vector.y;
		plane->c = normal_vector.z;
	}
	else
	{
		plane->d /= -length;
		plane->a = -normal_vector.x;
		plane->b = -normal_vector.y;
		plane->c = -normal_vector.z;
	}
	return plane;
}

// Description:
// Creates a plane object from an array of 3 vertixes.
PLANE* CreatePlane(VECTOR* points)
{
	// We find the normal vector for the plane and establish its definition
	VECTOR vec1 = SubtractVectors(points[1], points[0]);
	VECTOR vec2 = SubtractVectors(points[2], points[0]);
	VECTOR normal_vector = CrossProduct(vec1, vec2);
	return CreatePlaneFromVector(normal_vector, points[0]);
}

// Description:
// Sets the specific information for the elipsis, that can be found in
// 'buffer', in the object '*object'. This includes the elipsis functions
// and its specific information.
void SetElipsisDisc(char* buffer, OBJECT* object)
{
	ELIPSIS* elipsis = GetMemory(sizeof(ELIPSIS), NULL);
	char* focus1_str = GetPropertyBeginning(buffer, ELIPSIS_FOCUS1_PROPERTY);
	elipsis->focus1.x = strtold(GetPropertyBeginning(focus1_str, X_PROPERTY), NULL);
	elipsis->focus1.y = strtold(GetPropertyBeginning(focus1_str, Y_PROPERTY), NULL);
	elipsis->focus1.z = strtold(GetPropertyBeginning(focus1_str, Z_PROPERTY), NULL);
	char* focus2_str = GetPropertyBeginning(buffer, ELIPSIS_FOCUS2_PROPERTY);
	elipsis->focus2.x = strtold(GetPropertyBeginning(focus2_str, X_PROPERTY), NULL);
	elipsis->focus2.y = strtold(GetPropertyBeginning(focus2_str, Y_PROPERTY), NULL);
	elipsis->focus2.z = strtold(GetPropertyBeginning(focus2_str, Z_PROPERTY), NULL);
	char* dir_str = GetPropertyBeginning(buffer, NORMAL_VECTOR_PROPERTY);
	VECTOR normal_vector;
	normal_vector.x = strtold(GetPropertyBeginning(dir_str, X_PROPERTY), NULL);
	normal_vector.y = strtold(GetPropertyBeginning(dir_str, Y_PROPERTY), NULL);
	normal_vector.z = strtold(GetPropertyBeginning(dir_str, Z_PROPERTY), NULL);
	elipsis->plane = *(CreatePlaneFromVector(normal_vector, elipsis->focus1));
	elipsis->circular_distance = strtold(GetPropertyBeginning(buffer, CIRCULAR_DISTANCE_PROPERTY), NULL);
	object->figure = elipsis;
	object->get_intersections = &GetElipsisIntersection;
	object->get_normal_vector = &GetElipsisNormalVector;
}

// Description:
// Sets the specific information for the circular disc, that can be found in
// 'buffer', in the object '*object'. This includes the circular disc functions
// and its specific information.
void SetCircularDisc(char* buffer, OBJECT* object)
{
	CIRCULAR_DISC* disc = GetMemory(sizeof(CIRCULAR_DISC), NULL);
	char* anchor_str = GetPropertyBeginning(buffer, DISC_CENTER_PROPERTY);
	disc->center.x = strtold(GetPropertyBeginning(anchor_str, X_PROPERTY), NULL);
	disc->center.y = strtold(GetPropertyBeginning(anchor_str, Y_PROPERTY), NULL);
	disc->center.z = strtold(GetPropertyBeginning(anchor_str, Z_PROPERTY), NULL);
	char* dir_str = GetPropertyBeginning(buffer, NORMAL_VECTOR_PROPERTY);
	VECTOR normal_vector;
	normal_vector.x = strtold(GetPropertyBeginning(dir_str, X_PROPERTY), NULL);
	normal_vector.y = strtold(GetPropertyBeginning(dir_str, Y_PROPERTY), NULL);
	normal_vector.z = strtold(GetPropertyBeginning(dir_str, Z_PROPERTY), NULL);
	disc->plane = *(CreatePlaneFromVector(normal_vector, disc->center));
	disc->interior_radio = strtold(GetPropertyBeginning(buffer, INTERIOR_RADIO_PROPERTY), NULL);
	disc->exterior_radio = strtold(GetPropertyBeginning(buffer, EXTERIOR_RADIO_PROPERTY), NULL);
	object->figure = disc;
	object->get_intersections = &GetDiscIntersection;
	object->get_normal_vector = &GetDiscNormalVector;
}

// Description:
// Sets all the information that the cone and the cylinder have in common
CYLINDER* SetCylConeProperties(char* buffer)
{
	CYLINDER* cylinder = GetMemory(sizeof(CYLINDER), NULL);
	char* anchor_str = GetPropertyBeginning(buffer, CYLINDER_ANCHOR_PROPERTY);
	cylinder->anchor.x = strtold(GetPropertyBeginning(anchor_str, X_PROPERTY), NULL);
	cylinder->anchor.y = strtold(GetPropertyBeginning(anchor_str, Y_PROPERTY), NULL);
	cylinder->anchor.z = strtold(GetPropertyBeginning(anchor_str, Z_PROPERTY), NULL);
	char* dir_str = GetPropertyBeginning(buffer, CYLINDER_DIRECTION_PROPERTY);
	cylinder->dir_vector.x = strtold(GetPropertyBeginning(dir_str, X_PROPERTY), NULL);
	cylinder->dir_vector.y = strtold(GetPropertyBeginning(dir_str, Y_PROPERTY), NULL);
	cylinder->dir_vector.z = strtold(GetPropertyBeginning(dir_str, Z_PROPERTY), NULL);
	NormalizeVector(&(cylinder->dir_vector));
	if(IsType(GetPropertyBeginning(buffer, IS_FINITE_PROPERTY), TRUE_PROPERTY))
	{
		cylinder->is_finite = TRUE;
		cylinder->front_length = strtold(GetPropertyBeginning(buffer, FRONT_LENGTH_PROPERTY), NULL);
		cylinder->back_length = strtold(GetPropertyBeginning(buffer, BACK_LENGTH_PROPERTY), NULL);
	}
	else
	{
		cylinder->is_finite = FALSE;
		cylinder->front_length = INVALID_DISTANCE;
		cylinder->back_length = INVALID_DISTANCE;
	}
	return cylinder;
}

// Description:
// Sets the specific information for the cone, that can be found in
// 'buffer', in the object '*object'. This includes the cone functions
// and its specific information.
void SetCone(char* buffer, OBJECT* object)
{
	CYLINDER* cylinder = SetCylConeProperties(buffer);
	cylinder->radio = strtold(GetPropertyBeginning(buffer, WIDTH_PER_LENGTH_PROPERTY), NULL);
	object->figure = cylinder;
	object->get_intersections = &GetConeIntersection;
	object->get_normal_vector = &GetConeNormalVector;
}

// Description:
// Sets the specific information for the cylinder, that can be found in
// 'buffer', in the object '*object'. This includes the cylinder functions
// and its specific information.
void SetCylinder(char* buffer, OBJECT* object)
{
	CYLINDER* cylinder = SetCylConeProperties(buffer);
	cylinder->radio = strtold(GetPropertyBeginning(buffer, RADIO_PROPERTY), NULL);
	object->figure = cylinder;
	object->get_intersections = &GetCylinderIntersection;
	object->get_normal_vector = &GetCylinderNormalVector;
}

// Description:
// Loads the next 'amount' points from the 'buffer' and returns an
// array containing them.
VECTOR* GetPoints(char *buffer, int amount)
{
	VECTOR *points = GetMemory(sizeof(VECTOR) * amount, NULL);
	int point_index;
	char* point_str = buffer;
	for(point_index = 0; point_index < amount; point_index++)
	{
		point_str = GetPropertyBeginning(point_str, POINT_PROPERTY);
		points[point_index].x = strtold(GetPropertyBeginning(point_str, X_PROPERTY), NULL);
		points[point_index].y = strtold(GetPropertyBeginning(point_str, Y_PROPERTY), NULL);
		points[point_index].z = strtold(GetPropertyBeginning(point_str, Z_PROPERTY), NULL);
	}
	return points;
}

// Description:
// Sets the specific information for the plane, that can be found in
// 'buffer', in the object '*object'. This includes the plane functions
// and its specific information.
void SetPlane(char* buffer, OBJECT* object)
{
	// We get the 3 points that define a plane
	VECTOR *points = GetPoints(GetPropertyBeginning(buffer, POINTS_PROPERTY), 3);
	PLANE* plane = CreatePlane(points);
	object->figure = plane;
	object->get_intersections = &GetPlaneIntersection;
	object->get_normal_vector = &GetPlaneNormalVector;
}

// Description:
// We transform the 3D points to 2D coordinates. That means we project
// the points to a specific 2D plane that can be X_Y, Z_Y or X_Z for
// horizontal and vertical coords respectively. The projection depens
// on the plane of the polygon.
COORD_2D* PointsToVertix(VECTOR* points, int point_amount, PLANE plane)
{
	COORD_2D *vertix_list = GetMemory(sizeof(COORD_2D) * point_amount, NULL);
	// We project the points to a specific plane which can be X_Y, X_Z
	// or Z_Y for horizontal and vertical coordinates respectively
	int coord_discarded = GetDiscardedCoord(plane);
	int vertix_index;
	for(vertix_index = 0; vertix_index < point_amount; vertix_index++)
		vertix_list[vertix_index] = TransformPoint(points[vertix_index], coord_discarded);
	return vertix_list;
}

// Description:
// Sets the specific information for the polygon, that can be found in
// 'buffer', in the object '*object'. This includes the polygon functions
// and its specific information.
void SetPolygon(char* buffer, OBJECT* object)
{
	POLYGON *polygon = GetMemory(sizeof(POLYGON), NULL);
	// We get the polygon vertixes and define a plane with the first 3.
	char* point_str = GetPropertyBeginning(buffer, POINTS_PROPERTY);
	int point_amount = (int) strtold(point_str, NULL);
	VECTOR *points = GetPoints(point_str, point_amount);
	PLANE* plane = CreatePlane(points);
	// We transform the polygon to a 2d figure, to check for intersections later on.
	COORD_2D *vertix = PointsToVertix(points, point_amount, *plane);
	free(points);
	polygon->plane = *plane;
	free(plane);
	polygon->vertix = vertix;
	polygon->vertix_amount = point_amount;
	object->figure = polygon;
	object->get_intersections = &GetPolygonIntersection;
	object->get_normal_vector = &GetPolygonNormalVector;
}

// Description:
// Sets the specific information for the sphere, that can be found in
// 'buffer', in the object '*object'. This includes the sphere functions
// and its specific information.
void SetSphere(char* buffer, OBJECT* object)
{
	SPHERE* sphere = GetMemory(sizeof(SPHERE), NULL);
	sphere->radio = strtold(GetPropertyBeginning(buffer, RADIO_PROPERTY), NULL);
	char* anchor_str = GetPropertyBeginning(buffer, SPHERE_CENTER_PROPERTY);
	sphere->center.x = strtold(GetPropertyBeginning(anchor_str, X_PROPERTY), NULL);
	sphere->center.y = strtold(GetPropertyBeginning(anchor_str, Y_PROPERTY), NULL);
	sphere->center.z = strtold(GetPropertyBeginning(anchor_str, Z_PROPERTY), NULL);
	object->figure = sphere;
	object->get_intersections = &GetSphereIntersection;
	object->get_normal_vector = &GetSphereNormalVector;
}

// Description:
// Fills the specific information of the next object in 'buffer' on the
// pointer 'object'. The specific information refers to the kind of
// figure the object represents. For example: a sphere, a cylinder, etc.
SetObjectType(char* buffer, OBJECT* object)
{
	char* type = GetPropertyBeginning(buffer, OBJ_TYPE_PROPERTY);
	if(IsType(type, SPHERE_PROPERTY))
		SetSphere(type, object);
	else if(IsType(type, PLANE_PROPERTY))
		SetPlane(type, object);
	else if(IsType(type, POLYGON_PROPERTY))
		SetPolygon(type, object);
	else if(IsType(type, CYLINDER_PROPERTY))
		SetCylinder(type, object);
	else if(IsType(type, CONE_PROPERTY))
		SetCone(type, object);
	else if(IsType(type, CIRCULAR_DISC_PROPERTY))
		SetCircularDisc(type, object);
	else if(IsType(type, ELIPSIS_PROPERTY))
		SetElipsisDisc(type, object);
	else
	{
		PrintError(UNDEFINED_TYPE_ERROR);
		printf("%s", type);
		exit(UNDEFINED_TYPE_ERROR);
	}
}

// Description:
// Loads the general information of the next object in the 'buffer' in
// the index 'object_index' on the 'Scene_Objects' array.
void LoadObject(char* buffer, int object_index)
{
	Scene_Objects[object_index].color.red = strtold(GetPropertyBeginning(buffer, RED_PROPERTY), NULL);
	Scene_Objects[object_index].color.green = strtold(GetPropertyBeginning(buffer, GREEN_PROPERTY), NULL);
	Scene_Objects[object_index].color.blue = strtold(GetPropertyBeginning(buffer, BLUE_PROPERTY), NULL);
	Scene_Objects[object_index].light_material = strtold(GetPropertyBeginning(buffer, OBJ_LIGHT_MATERIAL_PROPERTY), NULL);
	Scene_Objects[object_index].light_ambiental = strtold(GetPropertyBeginning(buffer, OBJ_LIGHT_AMBIENTAL_PROPERTY), NULL);
	Scene_Objects[object_index].specular_material = strtold(GetPropertyBeginning(buffer, OBJ_SPECULAR_MATERIAL_PROPERTY), NULL);
	Scene_Objects[object_index].specular_pow = strtold(GetPropertyBeginning(buffer, OBJ_SPECULAR_POW_PROPERTY), NULL);
	SetObjectType(buffer, &(Scene_Objects[object_index]));
}
*/
/*
 * Loads the scene objects and its environment (window size, light sources, etc).
 *
 * scene_file_path: Path to the file that contains the scene configuration
 */
void load_scene(char* scene_file_path)
{
    config_t cfg;
    config_setting_t *setting;
    const char *str;
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
/********************************EXAMPLE***********************************************/
  /* Get the store name. */
  if(config_lookup_string(&cfg, "name", &str))
    printf("Store name: %s\n\n", str);
  else
    fprintf(stderr, "No 'name' setting in configuration file.\n");

  /* Output a list of all books in the inventory. */
  setting = config_lookup(&cfg, "inventory.books");
  if(setting != NULL)
  {
    int count = config_setting_length(setting);
    int i;

    printf("%-30s  %-30s   %-6s  %s\n", "TITLE", "AUTHOR", "PRICE", "QTY");

    for(i = 0; i < count; ++i)
    {
      config_setting_t *book = config_setting_get_elem(setting, i);

      /* Only output the record if all of the expected fields are present. */
      const char *title, *author;
      double price;
      int qty;

      if(!(config_setting_lookup_string(book, "title", &title)
           && config_setting_lookup_string(book, "author", &author)
           && config_setting_lookup_float(book, "price", &price)
           && config_setting_lookup_int(book, "qty", &qty)))
        continue;

      printf("%-30s  %-30s  $%6.2f  %3d\n", title, author, price, qty);
    }
    putchar('\n');
  }

  /* Output a list of all movies in the inventory. */
  setting = config_lookup(&cfg, "inventory.movies");
  if(setting != NULL)
  {
    unsigned int count = config_setting_length(setting);
    unsigned int i;

    printf("%-30s  %-10s   %-6s  %s\n", "TITLE", "MEDIA", "PRICE", "QTY");
    for(i = 0; i < count; ++i)
    {
      config_setting_t *movie = config_setting_get_elem(setting, i);

      /* Only output the record if all of the expected fields are present. */
      const char *title, *media;
      double price;
      int qty;

      if(!(config_setting_lookup_string(movie, "title", &title)
           && config_setting_lookup_string(movie, "media", &media)
           && config_setting_lookup_float(movie, "price", &price)
           && config_setting_lookup_int(movie, "qty", &qty)))
        continue;

      printf("%-30s  %-10s  $%6.2f  %3d\n", title, media, price, qty);
    }
    putchar('\n');
  }
/********************************EXAMPLE FINISHES***********************************************/
    //char *buffer = (char*) load_file(scene_file_path);
    load_eye(&cfg);
    load_scene_window(&cfg);
    load_background(&cfg);
    load_objects(&cfg);
    load_lights(&cfg);
    load_environment_light(&cfg);
    //free(buffer);

  config_destroy(&cfg);
}
