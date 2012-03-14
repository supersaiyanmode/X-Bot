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
    double armLength,angle1, angle2;
    int steps1, steps2;
    Parallel p;
    Point2D basePoint,curPoint;
public:
    Arm();
    void moveTo(double, double);
    void destroy();
};

#endif