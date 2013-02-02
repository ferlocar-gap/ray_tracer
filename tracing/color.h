#ifndef COLOR_H
#define COLOR_H

/*
 * Represents an RGB color.
 *
 * red: Red value of the color, must be between 0 and 1.
 * green: Green value of the color, must be between 0 and 1.
 * blue: Blue value of the color, must be between 0 and 1.
 */
typedef struct
{
	long double red;
	long double green;
	long double blue;
} Color;

#endif
