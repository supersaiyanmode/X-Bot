#ifndef ARM_H
#define ARM_H

#include "Parallel.h"

struct Point2D {
    double x,y;
};

class StepperMotor{
    size_t curSteps;
    static const char array[] = {8,12,4,6,2,3,1,9};
public:
    StepperMotor();
    char stepRight();
    char stepLeft();
};

class Arm {
    double arm1Length, arm2Length;
    double theta1, theta2;
    Parallel p;
    Point2D basePoint,curPoint;
public:
    Arm(double, double, double, double);
    void moveTo(double, double);
};

#endif