#ifndef v2d_h
#define v2d_h

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
 float v2d_angle_between(v2d v, v2d u);
 v2d v2d_rotate(v2d v, v2d o, float rads);

#define v2d_normal v2d_unit

#endif // v2d_h

#ifdef V2D_IMPLEMENTATION

v2d v2d_add(v2d a, v2d b){
    return (v2d){a.x+b.x, a.y+b.y};
}


v2d v2d_sub(v2d a, v2d b){
    return (v2d){a.x-b.x, a.y-b.y};
}


v2d v2d_scale(float val, v2d a){
    return (v2d){a.x*val, a.y*val};
}


float v2d_dot( v2d a, v2d b ){
    return (float)((a.x*b.x)+(a.y*b.y));
}


float v2d_magnitude( v2d v ) {
    return sqrtf(v2d_dot(v, v));
}


float v2d_magnitude_no_root( v2d v ){
    return v2d_dot(v, v);

}


v2d v2d_unit( v2d v ) {
    v2d unit_vector;
    float magnitude = v2d_magnitude( v );

    unit_vector.x = (float)v.x / magnitude;
    unit_vector.y = (float)v.y / magnitude;


    return unit_vector;
}


v2d v2d_rotate(v2d v, v2d o, float rads){
    // v2d rotatedV;
    // float s = sin(rads);
    // float c = cos(rads);
    // v2d subtracted = v2d_sub(v, o);
    // rotatedV.x = subtracted.x * c - subtracted.y * s;
    // rotatedV.y = subtracted.x * s + subtracted.y * c;
    // return rotatedV;
    v2d rotatedV;
    rotatedV.x =      ((v.x - o.x) * cos(rads)) - ((v.y - o.y) * sin(rads)) + o.x;
    rotatedV.y = o.y -((o.y - v.y) * cos(rads)) + ((v.x - o.x) * sin(rads));
    return rotatedV;
}
    
v2d v2d_perp(v2d v){
    v2d perpV = v2d_rotate(v, (v2d){0,0}, -(PI * 0.5));
    return perpV;
}


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


#endif // V2D_IMPLEMENTATION
