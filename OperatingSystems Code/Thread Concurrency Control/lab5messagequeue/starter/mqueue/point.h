/**
 * @file:   code/lab4/mqueue/point.h
 * @brief:  header file for modeling a point in a 2D space
 * @date:   2014/06/25
 */

#ifndef POINT_H_
#define POINT_H_

struct point {
	int x;	/* x coordinate */
};

inline void set_position(int x, struct point *buf);
inline int get_x_coord (struct point p);

#endif /* POINT_H_ */
