#ifndef WINDOW_H
#define WINDOW_H

/*
 * Represents the window that peeks at the tridimensional scene
 *
 * x_max: Maximum X position of the window.
 * x_min: Minimum X position of the window.
 * y_max: Maximum Y position of the window.
 * y_min: Minimum Y position of the window.
 * z_anchor: Z position of the window.
 */
typedef struct
{
	long double x_max;
	long double x_min;
	long double y_max;
	long double y_min;
	long double z_anchor;
} Window;

#endif
