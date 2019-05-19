#include <time.h>
#include <color.h>
#include <stdlib.h>
#include <stdbool.h>
RGBColor color_init(float red, float green, float blue){
    return (RGBColor) {red, green, blue};
}

RGBColor get_new_color(){
  float c1 = ((float)rand()/(float)RAND_MAX);
  float c2 = ((float)rand()/(float)RAND_MAX);
  float c3 = ((float)rand()/(float)RAND_MAX);
  return (RGBColor) {c1, c2, c3};
}

bool color_equal(RGBColor one, RGBColor two)
{
  if((one.r == two.r) && (one.g == two.g) && (one.b == two.b))
  {
    return true;
  }
  return false;
}
