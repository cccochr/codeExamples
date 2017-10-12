#include "reflectance_math.h"

int main(){

  //struct ray r = {{0,0,-1},{0.1,-0.1,1}};
  //struct surface s = {{{-1, 1, 0},{1,1,0},{1,-1,0},{-1,-1,0}}, {0,0,0}, {0,0,0}};
  //set_normal(&s);

  //struct spotlight_info spot = find_intersect(s, r);

  //printf("Intersect: [%f, %f, %f] is %s with reflection: [%f, %f, %f]\n", spot.r.o.x, spot.r.o.y, spot.r.o.z, spot.hit ? "true" : "false",
  //  spot.r.v.x, spot.r.v.y, spot.r.v.z);

  int i;
  for(i=0;i<100;i++){
    float r = halton_sequence(i,17);
    printf("%f\n", r);
  }
  for(i=0;i<100;i++){
    struct point p = sphere_point(r01(), r01());
    printf("%f, %f, %f\n", p.x, p.y, p.z);
  }

  return 0;
}
