#pragma once
#include "velocity.h"
#include "position.h"
#include "ints.h"

#define swapValues(a, b, T){ \
    T temp = a; \
    a = b; \
    b = temp; \
}


#define MAX(a, b)(a > b ? a : b)
#define MIN(a, b)(a < b ? a : b)
#define sign(x)(x >= 0 ? 1 : -1)


u32 
util_index_2d_to_1d(u32 row, u32 col, u32 numRows);

void 
util_index_1d_to_2d(u32 i, u32 numRows, u32 *row, u32 *col);

v2d 
util_v2d_local_to_world(v2d l, v2d o);

bool 
util_is_in_range(float begin, float end, float x);

float 
util_get_y_for_x_on_line(Position p1, Position p2, float x);

float 
util_get_x_for_y_on_line(Position p1, Position p2, float y);

float 
util_rads_to_degrees(float rads);

GroundMode 
util_rads_to_ground_mode(float angleInRads);

bool 
util_in_range(float val, float start, float end);

void 
util_break_on_condition(bool condition);

template<typename T> void 
util_dynamic_array_to_vector(T *arr, size_t size, std::vector<T> &vec){
    for(int i = 0; i < size; i++){
        vec.push_back(arr[i]);
    }
}

