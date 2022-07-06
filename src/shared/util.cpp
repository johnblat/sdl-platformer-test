#include "util.h"
#include "velocity.h"
#include "position.h"
#include <assert.h>
#include "ray2d.h"


u32 util_index_2d_to_1d(u32 row, u32 col, u32 numRows){
    return (row * numRows) + col;
}


void util_index_1d_to_2d(u32 i, u32 numRows, u32 *row, u32 *col){
    *row = i / (numRows);
    *col = i % (numRows);
}

v2d util_v2d_local_to_world(v2d l, v2d o){
    v2d g = l + o;
    return g;
}

bool util_is_in_range(float begin, float end, float x){
    if(begin > end){
        swapValues(begin, end, float);
    }
    if(x <= begin || x >= end){
        return false;
    }
    return true;
}

bool util_in_range(float val, float start, float end){
    if(end < start){
        swapValues(end, start, float);
    }

    if(val >= start && val <= end){
        return true;
    }

    return false;

}


float util_get_y_for_x_on_line(Position p1, Position p2, float x){
    if(p2.x < p1.x){
        swapValues(p2, p1, Position);
    }
    if(x < p1.x || x > p2.x){
        assert("util_get_y_for_x_on_line: X passed must be in the range p1.x to p2.x" && 0);
    }
    if(p1.x == p2.x){
        //printf("p1.x == p2.x");
        return p1.y;
    }
    float m = (p2.y - p1.y)/(p2.x - p1.x);
    float b = p1.y - m * p1.x;
    float y = m * x + b;
    return y;
}

float util_get_x_for_y_on_line(Position p1, Position p2, float y){
    if(p2.y < p1.y){
        swapValues(p2, p1, Position);
    }
    if(y < p1.y || y > p2.y){
        assert("getYForyOnLine: y passed must be in the range p1.y to p2.y" && 0);
    }
    if(p1.y == p2.y){
        //printf("p1.y == p2.y");
        return p1.x;
    }
    float m = (p2.x - p1.x)/(p2.y - p1.y);
    float b = p1.x - m * p1.y;
    float x = m * y + b;
    return x;
}

inline float util_rads_to_degrees(float rads){
    return rads * 180 / 3.14;
}

GroundMode util_rads_to_ground_mode(float angleInRads){
    float angleInDegrees = util_rads_to_degrees(angleInRads);

    if(util_in_range(angleInDegrees, 46.0f, 134.0f)){
        return GROUND_MODE_RIGHT_WALL;
    }
    if(util_in_range(angleInDegrees, 135.0f, 225.0f)){
        return GROUND_MODE_CEILING;
    }
    if(util_in_range(angleInDegrees, 226.0f, 314.0f)){
        return GROUND_MODE_LEFT_WALL;
    }

    return GROUND_MODE_FLOOR;
    
}





void util_break_on_condition(bool condition){
    if(condition == true){
        int x = 0;
    }
}


bool util_lines_equal(v2d line_start_1, v2d line_end_1, v2d line_start_2, v2d line_end_2){
    if(v2d_equal(line_start_1, line_start_2) && v2d_equal(line_end_1, line_end_2)){
        return true;
    }
    return false;
}


Ray2d ray2d_local_to_world(Position world_position, Ray2d ray2d_local){
    Ray2d ray2d_world;
    ray2d_world.distance = ray2d_local.distance;
    ray2d_world.position_start = v2d_add(world_position, ray2d_local.position_start);

    return ray2d_world;
}


float util_make_angle_between_0_and_2PI_rads(float angle_in_rads){
    const float max_rads = 3.14 * 2;

    if(angle_in_rads < 0 || angle_in_rads > max_rads){
        
        angle_in_rads = max_rads + angle_in_rads;
    }

    return angle_in_rads;
}