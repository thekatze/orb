#pragma once

#include "../core/types.h"

typedef union orb_vec2_u {
    f32 elements[2];
    struct {
        union {
            f32 x, s, u;
        };
        union {
            f32 y, t, v;
        };
    };
} orb_vec2;

typedef union orb_vec3_u {
    f32 elements[3];
    struct {
        union {
            f32 x, s, u, r;
        };
        union {
            f32 y, t, v, g;
        };
        union {
            f32 z, p, w, b;
        };
    };
} orb_vec3;

typedef union orb_vec4_u {
    f32 elements[4];
    struct {
        union {
            f32 x, s, r;
        };
        union {
            f32 y, t, g;
        };
        union {
            f32 z, p, b;
        };
        union {
            f32 w, q, a;
        };
    };
} orb_vec4;

typedef orb_vec4 orb_quat;

typedef union orb_mat4_u {
    f32 elements[16];
    orb_vec4 rows[4];
} orb_mat4;

typedef struct orb_vertex_3d {
    orb_vec3 position;
} orb_vertex_3d;
