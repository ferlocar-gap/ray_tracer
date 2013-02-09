/* intersection.c
 * 		Copyright 2012 Carlos Fernandez
 *
 * Contains the functions for calculating intersections.
 */

// Headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../scene_config.h"
#include "../utilities/memory_handler.h"
#include "vector.h"
#include "intersection.h"
#include "object.h"

// Constants
#define INTER_EPSILON 0.001

// Attributes
extern SceneConfig g_conf;

// Methods

/*
 * Swaps the values of intersection pointers.
 */
void swap_intersections(Intersection* inter1, Intersection* inter2)
{
	Intersection temp = *inter1;
	*inter1 = *inter2;
	*inter2 = temp;
}

/*
 * Orders an intersection array (using Quicksort) according to the distance
 * that each intersection has from the eye. The intersections with the shortest
 * distance from the eye go first.
 */
void sort_intersections(Intersection arr[], int beg, int end)
{
	if (end > beg + 1)
	{
		int piv = arr[beg].distance, l = beg + 1, r = end;
		while (l < r)
		{
			if (arr[l].distance <= piv)
				l++;
			else
				swap_intersections(&arr[l], &arr[--r]);
		}
		swap_intersections(&arr[--l], &arr[beg]);
		sort_intersections(arr, beg, l);
		sort_intersections(arr, r, end);
  	}
}

/*
 * Returns the intersection with 'object' and a ray thrown from 'eye' position
 * towards 'dir_vec' direction. If there is not any intersection, it returns
 * NULL.
 *
 * eye: Position from which the intersection ray is thrown.
 * dir_vec: Direction to which the ray travels. Must be normalized.
 * obj: Object with which the intersection with the ray is calculated.
 * inter_amount: Output parameter to indicate how many intersections were returned.
 */
Intersection* get_object_intersection(Vector eye, Vector dir_vec, Object obj, int *inter_amount)
{
    Intersection* inter_list;
    Intersection inter;
    int cut_plane_i, inter_i;
	inter_list = (Intersection*) obj.get_intersections(eye, dir_vec, &obj, inter_amount);
	if(inter_list)
    {   // Check each cutting plane
        for(cut_plane_i = 0; cut_plane_i < obj.cutting_planes_length; cut_plane_i++)
        {
            Plane cutting_plane = obj.cutting_planes[cut_plane_i];
            for(inter_i = 0; inter_i < *inter_amount; inter_i++)
            {   // Check each intersection with the object
                inter = inter_list[inter_i];
                if(inter.is_valid && is_up_the_plane(inter.posn, cutting_plane))
                {   // Intersecion is cut
                    inter_list[inter_i].is_valid = 0;
                }
            }
        }
    }
	return inter_list;
}

/*
 * Obtains the intersections found from the 'eye' position towards the
 * 'dir_vec' direction. The intersection list is ordered from the nearest
 * intersection to the farthest one. It also receives a 'length' output
 * parameter to indicate the intersection list length. If there are not any
 * intersections, NULL is returned.
 *
 * eye: Anchor of the ray that is used to find intersections
 * dir_vec: Direction of the ray. This vector must be normalized.
 * length: Output parameter to indicate how many intersections were returned.
 */
Intersection* get_intersections(Vector eye, Vector dir_vec, int* length)
{
    Intersection *inter_list, *obj_inter_list;
    Intersection obj_inter;
    int obj_index, inter_index, obj_inter_amount, obj_inter_i;
	// Create an intersection list with the maximum of intersections that can be found.
	inter_list = get_memory(sizeof(Intersection) * g_conf.objs_length * 2, NULL);
	inter_index = 0;
	for(obj_index = 0; obj_index < g_conf.objs_length; obj_index++)
	{
		// For each object in the scene we look for an intersection
		obj_inter_list = get_object_intersection(eye, dir_vec, g_conf.objs[obj_index], &obj_inter_amount);
		if(obj_inter_list)
		{
			for(obj_inter_i = 0; obj_inter_i < obj_inter_amount; obj_inter_i++)
            {
                obj_inter = obj_inter_list[obj_inter_i];
                // Special condition for shadows, to check for a distance larger than 0 (INTER_EPSILON)
                if(obj_inter.is_valid && obj_inter.distance > INTER_EPSILON)
                {
                    inter_list[inter_index++] = obj_inter;
                }
            }
			free(obj_inter_list);
		}
	}
	// We return the list only if we found at least one intersection, otherwise we return NULL
	if (inter_index > 0)
	{
		sort_intersections(inter_list, 0, inter_index);
		if(length) *length = inter_index;
		return inter_list;
	}
	else
	{
		if(length) *length = 0;
		free(inter_list);
		return NULL;
	}
}
