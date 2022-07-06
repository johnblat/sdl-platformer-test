#pragma once

#include <cmath>
#include <cinttypes>
#include <cassert>

#define PI 3.14159265358979323846


struct v2d {
    float x, y;

    v2d(float X, float Y) : x(X), y(Y) {}
    v2d() : x(0), y(0) {}

    float &operator[](const uint32_t i){
        return (&x)[i];
    }

    const float &operator[](const uint32_t i) const {
        return (&x)[i];
    }

};



v2d v2d_add(v2d a, v2d b);
v2d v2d_sub(v2d a, v2d b);
v2d v2d_scale(float val, v2d a);
float v2d_dot( v2d a, v2d b );
float v2d_magnitude( v2d v );
float v2d_magnitude_no_root(v2d v );
v2d v2d_unit( v2d v );
v2d v2d_perp(v2d v);
v2d v2d_orthogonal_projection_point_to_line(v2d p, v2d line_p1, v2d line_p2);
float v2d_shortest_distance_from_point_to_line_segment(v2d p, v2d line_p1, v2d line_p2);
float v2d_angle_between(v2d v, v2d u);
v2d v2d_rotate(v2d v, v2d o, float rads);
v2d v2d_rotate_90_degrees_counter_clockwise(v2d v, v2d o);
v2d v2d_rotate_90_degrees_clockwise(v2d v, v2d o);
v2d v2d_rotate_180_degrees(v2d v, v2d o);
bool v2d_equal(v2d v1, v2d v2);


#define v2d_normal v2d_unit


inline v2d operator+(const v2d lhs, const v2d rhs){
     return v2d_add(lhs, rhs);
}

inline v2d operator-(v2d lhs, v2d rhs){
    return v2d_sub(lhs, rhs);
}

inline v2d operator*(double lhs, v2d rhs){
    return v2d_scale(lhs, rhs);
}
   
inline v2d operator*(v2d lhs, double rhs){
    return v2d_scale(rhs, lhs);
}

inline float operator*(v2d lhs, v2d rhs){
    return v2d_dot(lhs, rhs);
}