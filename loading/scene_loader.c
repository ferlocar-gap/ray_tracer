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

// Constants

// General Scene Properties
#define false 0;
#define g_eye_PROPERTY "g_eye:"
#define WINDOW_PROPERTY "WINDOW:"
#define BACKGROUND_PROPERTY "BACKGROUND:"
#define ENVIRONMENT_LIGHT_PROPERTY "ENVIRONMENT_LIGHT:"
#define X_MIN_PROPERY "X_MIN:"
#define X_MAX_PROPERY "X_MAX:"
#define Y_MIN_PROPERY "Y_MIN:"
#define Y_MAX_PROPERY "Y_MAX:"
#define Z_ANCHOR_PROPERY "Z_ANCHOR:"

// General Object Properties
#define OBJECTS_PROPERTY "OBJECTS:"
#define OBJECT_PROPERTY "OBJECT:"
#define OBJECT_AMOUNT_PROPERTY "OBJECT_AMOUNT:"
#define OBJ_TYPE_PROPERTY "TYPE:"
#define OBJ_LIGHT_MATERIAL_PROPERTY "LIGHT_MATERIAL:"
#define OBJ_LIGHT_AMBIENTAL_PROPERTY "LIGHT_AMBIENTAL:"
#define OBJ_SPECULAR_MATERIAL_PROPERTY "SPECULAR_MATERIAL:"
#define OBJ_SPECULAR_POW_PROPERTY "SPECULAR_POW:"

#define SPHERE_PROPERTY "SPHERE "
#define RADIO_PROPERTY "RADIO:"
#define SPHERE_CENTER_PROPERTY "SPHERE_CENTER:"

#define PLANE_PROPERTY "PLANE "
#define POLYGON_PROPERTY "POLYGON "
#define POINTS_PROPERTY "POINTS:"
#define POINT_PROPERTY "POINT:"

#define CYLINDER_PROPERTY "CYLINDER "
#define CYLINDER_ANCHOR_PROPERTY "ANCHOR:"
#define CYLINDER_DIRECTION_PROPERTY "DIR_VECTOR:"
#define IS_FINITE_PROPERTY "IS_FINITE:"
#define FRONT_LENGTH_PROPERTY "FRONT_LENGTH:"
#define BACK_LENGTH_PROPERTY "BACKWARD_LENGTH:"

#define CONE_PROPERTY "CONE "
#define WIDTH_PER_LENGTH_PROPERTY "WIDTH_PER_LENGTH:"

#define ELIPSIS_PROPERTY "ELIPSIS "
#define ELIPSIS_FOCUS1_PROPERTY "FOCUS1:"
#define ELIPSIS_FOCUS2_PROPERTY "FOCUS2:"
#define CIRCULAR_DISTANCE_PROPERTY "CIRCULAR_DISTANCE:"

#define CIRCULAR_DISC_PROPERTY "DISC "
#define DISC_CENTER_PROPERTY "CENTER:"
#define INTERIOR_RADIO_PROPERTY "INTERIOR_RADIO:"
#define EXTERIOR_RADIO_PROPERTY "EXTERIOR_RADIO:"

// Light properties
#define LIGHTS_PROPERTY "LIGHTS:"
#define LIGHT_PROPERTY "LIGHT:"
#define LIGHT_AMOUNT_PROPERTY "LIGHT_AMOUNT:"
#define INTENSITY_PROPERTY "INTENSITY:"
#define LIGHT_ANCHOR_PROPERTY "LIGHT_ANCHOR:"
#define CONST_ATT_FACTOR "CONSTANTS_ATT_FACTOR:"
#define LIN_ATT_FACTOR "LINEAR_ATT_FACTOR:"
#define EXP_ATT_FACTOR "EXPONENTIAL_ATT_FACTOR:"

// General properties
#define X_PROPERTY "X:"
#define Y_PROPERTY "Y:"
#define Z_PROPERTY "Z:"
#define RED_PROPERTY "RED:"
#define GREEN_PROPERTY "GREEN:"
#define BLUE_PROPERTY "BLUE:"
#define TRUE_PROPERTY "TRUE "
#define NORMAL_VECTOR_PROPERTY "NORMAL_VECTOR:"

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
 * Loads the eye position in the 'g_eye' variable.
 */
void load_eye(char *buffer)
{
    g_eye.x = 250;
    g_eye.y = 250;
    g_eye.z = -1000;
	/*char *g_eye_str = GetPropertyBeginning(buffer, g_eye_PROPERTY);
	g_eye.x = strtold(GetPropertyBeginning(g_eye_str, X_PROPERTY), NULL);
	g_eye.y = strtold(GetPropertyBeginning(g_eye_str, Y_PROPERTY), NULL);
	g_eye.z = strtold(GetPropertyBeginning(g_eye_str, Z_PROPERTY), NULL);*/
}

/*
 * Loads the window position in the 'g_window' variable.
 */
void load_scene_window(char* buffer)
{
    g_window.x_max = 750;
    g_window.x_min = 0;
    g_window.y_max = 750;
    g_window.y_min = 0;
    g_window.z_anchor = 0;
/*	char* window_str = GetPropertyBeginning(buffer, WINDOW_PROPERTY);
	Scene_Window.x_min = strtold(GetPropertyBeginning(window_str, X_MIN_PROPERY), NULL);
	Scene_Window.y_min = strtold(GetPropertyBeginning(window_str, Y_MIN_PROPERY), NULL);
	Scene_Window.x_max = strtold(GetPropertyBeginning(window_str, X_MAX_PROPERY), NULL);
	Scene_Window.y_max = strtold(GetPropertyBeginning(window_str, Y_MAX_PROPERY), NULL);
	Scene_Window.z_anchor = strtold(GetPropertyBeginning(window_str, Z_ANCHOR_PROPERY), NULL);*/
}

/*
 * Loads the background color in the 'g_background' variable.
 */
void load_background(char* buffer)
{
    g_background.red = 0.7;
    g_background.green = 0.7;
    g_background.blue = 0.7;
	/*char* background_str = GetPropertyBeginning(buffer, BACKGROUND_PROPERTY);
	Background.red = strtold(GetPropertyBeginning(background_str, RED_PROPERTY), NULL);
	Background.green = strtold(GetPropertyBeginning(background_str, GREEN_PROPERTY), NULL);
	Background.blue = strtold(GetPropertyBeginning(background_str, BLUE_PROPERTY), NULL);*/
}

/*
 * Loads all the scene objects and stores them in the 'g_objs' variable. The
 * size of the array will be stored in the 'g_objs_length' variable.
 */
void load_objects(char* buffer)
{
    g_objs_length = 7;
    g_objs = (Object*) get_memory(sizeof(Object)*g_objs_length, NULL);
    g_objs->color.green = 1;
    g_objs->color.red = 0;
    g_objs->color.blue = 0;
    g_objs->light_material = 1;
    g_objs->light_ambiental = 1;
    g_objs->specular_material = 1;
    g_objs->specular_pow = 10;
    g_objs->get_intersections = &get_sphere_intersection;
    g_objs->get_normal_vector = &get_sphere_normal_vector;

    Plane *cut_plane = (Plane*) get_memory(sizeof(Plane), NULL);
	cut_plane->offset = 20;
	cut_plane->direction.x = 0;
	cut_plane->direction.y = 0;
	cut_plane->direction.z = -1;
	g_objs->cutting_planes = cut_plane;
	g_objs->cutting_planes_length = 1;

    Sphere *sphere = (Sphere*) get_memory(sizeof(Sphere), NULL);
	sphere->radius = 100;
	sphere->center.x = 400;
	sphere->center.y = 300;
	sphere->center.z = 100;
    g_objs->figure = sphere;




    g_objs[1].color.green = 0;
    g_objs[1].color.red = 0;
    g_objs[1].color.blue = 1;
    g_objs[1].light_material = 1;
    g_objs[1].light_ambiental = 0;
    g_objs[1].specular_material = 1;
    g_objs[1].specular_pow = 10;
    g_objs[1].cutting_planes_length = 0;
    g_objs[1].get_intersections = &get_sphere_intersection;
    g_objs[1].get_normal_vector = &get_sphere_normal_vector;

    sphere = (Sphere*) get_memory(sizeof(Sphere), NULL);
	sphere->radius = 30;
	sphere->center.x = 100;
	sphere->center.y = 100;
	sphere->center.z = 50;
    g_objs[1].figure = sphere;





    g_objs[2].color.green = 0;
    g_objs[2].color.red = 0;
    g_objs[2].color.blue = 1;
    g_objs[2].light_material = 1;
    g_objs[2].light_ambiental = 0;
    g_objs[2].specular_material = 1;
    g_objs[2].specular_pow = 10;
    g_objs[2].cutting_planes_length = 0;
    g_objs[2].get_intersections = &get_plane_intersection;
    g_objs[2].get_normal_vector = &get_plane_normal_vector;

    Plane *plane = (Plane*) get_memory(sizeof(Plane), NULL);
	plane->offset = 0;
	plane->direction.x = 0;
	plane->direction.y = 1;
	plane->direction.z = 0;
    g_objs[2].figure = plane;




    g_objs[3].color.green = 1;
    g_objs[3].color.red = 0;
    g_objs[3].color.blue = 1;
    g_objs[3].light_material = 1;
    g_objs[3].light_ambiental = 0;
    g_objs[3].specular_material = 1;
    g_objs[3].specular_pow = 10;
    g_objs[3].cutting_planes_length = 0;
    g_objs[3].get_intersections = &get_polygon_intersection;
    g_objs[3].get_normal_vector = &get_polygon_normal_vector;

    plane = (Plane*) get_memory(sizeof(Plane), NULL);
	plane->offset = -10;
	plane->direction.x = 0;
	plane->direction.y = 0;
	plane->direction.z = 1;
	Coord2D *vertex = (Coord2D*) get_memory(sizeof(Coord2D) * 3, NULL);
	vertex[0].u = 250;
	vertex[0].v = 250;
	vertex[1].u = 350;
	vertex[1].v = 250;
	vertex[2].u = 320;
	vertex[2].v = 450;
    Polygon *poly = (Polygon*) get_memory(sizeof(Polygon), NULL);
    poly->plane = *plane;
    poly->vertex = vertex;
    poly->vertex_amount = 3;
    g_objs[3].figure = poly;




    g_objs[4].color.green = 0;
    g_objs[4].color.red = 1;
    g_objs[4].color.blue = 0;
    g_objs[4].light_material = 1;
    g_objs[4].light_ambiental = 0.25;
    g_objs[4].specular_material = 1;
    g_objs[4].specular_pow = 10;
    g_objs[4].cutting_planes_length = 0;
    g_objs[4].get_intersections = &get_cylinder_intersection;
    g_objs[4].get_normal_vector = &get_cylinder_normal_vector;

    Cylinder *cyl = (Cylinder*) get_memory(sizeof(Cylinder), NULL);
	cyl->direction.x = 0.57735;
	cyl->direction.y = 0.57735;
	cyl->direction.z = 0.57735;
	cyl->anchor.x = 250;
	cyl->anchor.y = 300;
	cyl->anchor.z = 250;
	cyl->radio = 100;
	cyl->is_finite = 1;
	cyl->front_length = 200;
	cyl->back_length = -200;
    g_objs[4].figure = cyl;





    g_objs[5].color.green = 0;
    g_objs[5].color.red = 1;
    g_objs[5].color.blue = 1;
    g_objs[5].light_material = 1;
    g_objs[5].light_ambiental = 0.25;
    g_objs[5].specular_material = 1;
    g_objs[5].specular_pow = 10;
    g_objs[5].cutting_planes_length = 0;
    g_objs[5].get_intersections = &get_cone_intersection;
    g_objs[5].get_normal_vector = &get_cone_normal_vector;

    Cone *cone = (Cone*) get_memory(sizeof(Cone), NULL);
	cone->direction.x = 0.57735;
	cone->direction.y = -0.57735;
	cone->direction.z = -0.57735;
	cone->anchor.x = 300;
	cone->anchor.y = 200;
	cone->anchor.z = 100;
	cone->radio = 0.5;
	cone->is_finite = 1;
	cone->front_length = 200;
	cone->back_length = 0;
    g_objs[5].figure = cone;




    g_objs[6].color.green = 1;
    g_objs[6].color.red = 1;
    g_objs[6].color.blue = 1;
    g_objs[6].light_material = 0.2;
    g_objs[6].light_ambiental = 0;
    g_objs[6].specular_material = 1;
    g_objs[6].specular_pow = 10;
    g_objs[6].cutting_planes_length = 0;
    g_objs[6].get_intersections = &get_disc_intersection;
    g_objs[6].get_normal_vector = &get_disc_normal_vector;

    plane = (Plane*) get_memory(sizeof(Plane), NULL);
	plane->offset = -10;
	plane->direction.x = 0;
	plane->direction.y = 0;
	plane->direction.z = 1;
    Disc *disc = (Disc*) get_memory(sizeof(Disc), NULL);
    disc->plane = *plane;
    disc->inner_focus1.x = 590;
    disc->inner_focus1.y = 250;
    disc->inner_focus1.z = -20;
    disc->inner_focus2.x = 610;
    disc->inner_focus2.y = 250;
    disc->inner_focus2.z = -20;
    disc->ext_focus1 = disc->inner_focus1;
    disc->ext_focus2 = disc->inner_focus2;
    disc->ext_focus1.x = 550;
    disc->ext_focus2.x = 650;
    disc->ext_focus2.y = 350;
    disc->inner_dist = 100;
    disc->ext_dist = 300;
    g_objs[6].figure = disc;

	/*char* objects_str = GetPropertyBeginning(buffer, OBJECTS_PROPERTY);
	Object_Amount = strtold(GetPropertyBeginning(objects_str, OBJECT_AMOUNT_PROPERTY), NULL);
	Scene_Objects = GetMemory(sizeof(OBJECT) * Object_Amount, NULL);
	int object_index;
	char* obj_str = objects_str;
	for(object_index = 0; object_index < Object_Amount; object_index++)
	{
		obj_str = GetPropertyBeginning(obj_str, OBJECT_PROPERTY);
		LoadObject(obj_str, object_index);
	}*/
}

/*
 * Loads all the scene light sources and stores them in the 'g_lights' variable.
 * The size of the array will be stored in the 'g_lights_length' variable.
 */
void load_lights(char* buffer)
{
    g_lights_length = 2;
    g_lights = (Light*) get_memory(sizeof(Light) * 2, NULL);
    g_lights->intensity = 1;
    g_lights->color.green = 1;
    g_lights->color.red = 1;
    g_lights->color.blue = 1;
    g_lights->const_att_factor = 1;
    g_lights->lin_att_factor = 0;
    g_lights->expo_att_factor = 0;
    g_lights->anchor.x = 750;
	g_lights->anchor.y = 750;
	g_lights->anchor.z = -1000;

	g_lights[1].intensity = 0.5;
    g_lights[1].color.green = 1;
    g_lights[1].color.red = 1;
    g_lights[1].color.blue = 1;
    g_lights[1].const_att_factor = 1;
    g_lights[1].lin_att_factor = 0;
    g_lights[1].expo_att_factor = 0;
    g_lights[1].anchor.x = 250;
	g_lights[1].anchor.y = 300;
	g_lights[1].anchor.z = 250;
}

/*
 * Loads the enviornmental light factor in the variable 'g_environment_light'
 */
void load_environment_light(char* buffer)
{
	g_environment_light = 0.2;
}

/*

// Description:
// Loads the enviornmental light factor in the variable 'Environment_Light'
void LoadEnvironmentLight(char* buffer)
{
	char* environment_str = GetPropertyBeginning(buffer, ENVIRONMENT_LIGHT_PROPERTY);
	Environment_Light = strtold(environment_str, NULL);
}

// Description:
// Loads the general information of the next light in the 'buffer' in
// the index 'light_index' on the 'Scene_Lights' array.
void LoadLight(char* buffer, int light_index)
{
	LIGHT light;
	light.color.red = strtold(GetPropertyBeginning(buffer, RED_PROPERTY), NULL);
	light.color.green = strtold(GetPropertyBeginning(buffer, GREEN_PROPERTY), NULL);
	light.color.blue = strtold(GetPropertyBeginning(buffer, BLUE_PROPERTY), NULL);
	char* anchor_str = GetPropertyBeginning(buffer, LIGHT_ANCHOR_PROPERTY);
	light.anchor.x = strtold(GetPropertyBeginning(anchor_str, X_PROPERTY), NULL);
	light.anchor.y = strtold(GetPropertyBeginning(anchor_str, Y_PROPERTY), NULL);
	light.anchor.z = strtold(GetPropertyBeginning(anchor_str, Z_PROPERTY), NULL);
	light.intensity = strtold(GetPropertyBeginning(buffer, INTENSITY_PROPERTY), NULL);
	light.exponential_att_factor = strtold(GetPropertyBeginning(buffer, EXP_ATT_FACTOR), NULL);
	light.lineal_att_factor = strtold(GetPropertyBeginning(buffer, LIN_ATT_FACTOR), NULL);
	light.const_att_factor = strtold(GetPropertyBeginning(buffer, CONST_ATT_FACTOR), NULL);
	Scene_Lights[light_index] = light;
}

// Description:
// Loads all the lights that appear in the scene and stores them in the
// 'Scene_Lights' variable. The array size is stored in the 'Light_Amount'
// variable.
void LoadLights(char* buffer)
{
	char* lights_str = GetPropertyBeginning(buffer, LIGHTS_PROPERTY);
	Light_Amount = strtold(GetPropertyBeginning(lights_str, LIGHT_AMOUNT_PROPERTY), NULL);
	Scene_Lights = GetMemory(sizeof(LIGHT) * Light_Amount, NULL);
	int light_index;
	char* light_str = lights_str;
	for(light_index = 0; light_index < Light_Amount; light_index++)
	{
		light_str = GetPropertyBeginning(light_str, LIGHT_PROPERTY);
		LoadLight(light_str, light_index);
	}
}

// Description:
// Returns if the beginning of the buffer contains a given object type.
int IsType(char* buffer, char* type)
{
	while(!(isspace(*buffer) || isspace(*type) || !type))
		if(*(buffer++) != *(type++))
			return false;
	return (isspace(*buffer) && (isspace(*type) || !type));
}

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
	//char *buffer = (char*) load_file(scene_file_path);
	/*load_eye(NULL);
	load_scene_window(NULL);
	load_background(NULL);
	load_objects(NULL);
	load_lights(NULL);
	load_environment_light(NULL);*/
	//free(buffer);
	config_t cfg;
  config_setting_t *setting;
  const char *str;

  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "scene.cfg"))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

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

  config_destroy(&cfg);
  return(EXIT_SUCCESS);
}
