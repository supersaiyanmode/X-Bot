#ifndef ARM_H
#define ARM_H

#include "Parallel.h"
#include "Config.h"

struct Point2D {
    double x,y;
};

class StepperMotor{
    size_t curSteps;
public:
    StepperMotor();
    char stepRight();
    char stepLeft();
};

class Arm {
    double armLength;
    double theta1, theta2;
    Parallel p;
    Point2D basePoint,curPoint;
public:
    Arm(double, double, double);
    void moveTo(double, double);
};

#endif