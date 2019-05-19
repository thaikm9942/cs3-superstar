#ifndef __COLOR_H__
#define __COLOR_H__
#include <stdbool.h>
/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} RGBColor;

//Added functionality

// Color initializer (not needed, but kind of nice)
RGBColor color_init(float red, float green, float blue);

// Random color generator
RGBColor get_new_color();

bool color_equal(RGBColor one, RGBColor two);

#endif // #ifndef __COLOR_H__
