#ifndef REFMATH_H
#define REFMATH_H
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

struct point {
    float x, y, z;
};

struct material {
    float am[4];
    float diff[4];
    float spec[4];
    float shine[1];
};

struct surface {
    struct point coords[4];
    struct point n;
    unsigned int mtl_id;
};

struct ray {
    struct point o;
    struct point v;
};


struct spotlight_info {
    struct ray r;
    bool hit;
    float dist;
    unsigned int mtl_id;
};

#define WHITE_WALL 0
#define RED_WALL 1
#define GREEN_WALL 2
#define FLOOR 3
#define TEAPOT 4

static inline struct point v_normalize(struct point v)
{
    float sum = sqrt(pow(v.x,2) + pow(v.y,2) + pow(v.z,2));
    if(sum != 0){
        v.x /= sum;
        v.y /= sum;
        v.z /= sum;
    }
    return v;
}

static inline struct point v_sub(struct point v1, struct point v2)
{
    return (struct point) {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

static inline float v_dot(struct point v1, struct point v2)
{
    return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

static inline struct point v_cross(struct point v1, struct point v2)
{
    return((struct point) {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x)});
}

static inline void set_normal(struct surface *s)
{
    struct point v1 = v_sub(s->coords[1], s->coords[0]);
    struct point v2 = v_sub(s->coords[2], s->coords[0]);
    s->n = v_normalize(v_cross(v1,v2));
}

static inline bool inside_tri(struct point v0, struct point v1, struct point v2, struct point p, struct point n)
{
    struct point e0 = v_sub(v1, v0);
    struct point e1 = v_sub(v2, v1);
    struct point e2 = v_sub(v0, v2);
    struct point c0 = v_sub(p, v0);
    struct point c1 = v_sub(p, v1);
    struct point c2 = v_sub(p, v2);
    return (v_dot(n, v_cross(e0, c0)) <= 0 && v_dot(n, v_cross(e1,c1)) <= 0 && v_dot(n, v_cross(e2,c2)) <= 0);
}

static inline bool inside_quad(struct surface s, struct point p)
{
    return(inside_tri(s.coords[0], s.coords[1], s.coords[3], p, s.n) || inside_tri(s.coords[1], s.coords[2], s.coords[3], p, s.n));
}

static inline struct spotlight_info find_intersect(struct surface s, struct ray r)
{
    struct spotlight_info spot;
    float D = v_dot(s.n, s.coords[0]);
    float t = -(v_dot(s.n, r.o) - D) / v_dot(s.n, r.v);
    float dot = v_dot(r.v, s.n);
    spot.r = (struct ray) { .o = {r.o.x + r.v.x * t, r.o.y + r.v.y * t, r.o.z + r.v.z * t},
    .v = {r.v.x - 2 * dot * s.n.x, r.v.y - 2 * dot * s.n.y, r.v.z - 2 * dot * s.n.z}};

    spot.hit = inside_quad(s, spot.r.o);
    if(t <= 0) spot.hit = false;
    spot.dist = sqrt(pow(spot.r.o.x - r.o.x, 2) + pow(spot.r.o.y - r.o.y, 2) + pow(spot.r.o.z - r.o.z, 2));
    spot.mtl_id = s.mtl_id;
    return spot;
}

static inline float r01()
{
    return (float)rand() / (float)RAND_MAX;
}

static inline struct point hemisphere_point(float s, float t, float scale)
{
    float u = 2 * M_PI * s;
    float v = sqrt(1 - pow(t, 2));

    struct point p = {v * cos(u), scale * t, v * sin(u)};
    return p;
}

static inline struct point sphere_point(float s, float t)
{
    float u = 2 * M_PI * s;
    float v = sqrt(t * (1 - t));

    struct point p = {2 * v * cos(u), 1 - 2 * t, 2 * v * sin(u)};
    return p;
}

static inline float halton_sequence(int i, int b)
{
    float f = 1;
    float val = 0;

    while(i > 0) {
        f = f / (float)b;
        val = val + f * (float)(i%b);
        i = i/b;
    }
    return val;
}
#endif
