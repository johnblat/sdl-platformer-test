
#include "v2d.h"


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





v2d v2dRotate90DegreesClockWise(v2d v, v2d o){
    // v2d t = v - o;
    // v2d result = {t.y, -t.x};
    // return result;
    v2d result = {
        o.x + (v.y - o.y),
        o.y - (v.x - o.x)
    };
    return result;
}

v2d v2dRotate90DegreesCounterClockWise(v2d v, v2d o){
    // v2d t = v - o;
    // v2d result = {-t.y, t.x};
    // result = result + o;
    // return result;
    v2d result = {
        o.x - (v.y - o.y),
        o.y + (v.x - o.x)
    };
    return result;
}

    
v2d v2d_perp(v2d v){
    v2d perpV = v2d_rotate(v, (v2d){0,0}, -(PI * 0.5));
    return perpV;
}


