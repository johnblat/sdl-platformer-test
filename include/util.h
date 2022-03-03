#ifndef utilh
#define utilh

#define swapValues(a, b, T){ \
    T temp = a; \
    a = b; \
    b = temp; \
}


#define MAX(a, b)(a > b ? a : b)
#define MIN(a, b)(a < b ? a : b)

#define rads2deg(rads)(rads * 180 / 3.14)

#endif