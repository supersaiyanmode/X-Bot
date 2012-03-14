#include "Arm.h"
#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>

//########STEPPER MOTOR CONTROL!!###############
static const char array[] = {8,12,4,6,2,3,1,9};

StepperMotor::StepperMotor():curSteps(0){

}
char StepperMotor::stepRight(){
    return array[curSteps=((curSteps+1)%8)];
}
char StepperMotor::stepLeft(){
    return array[curSteps=((8+curSteps-1)%8)];
}


//#########MAIN ARM!!#######################
Arm::Arm():
        armLength(std::atof(CONFIG["ARM_LENGTH"].c_str())),
        angle1(std::atoi(CONFIG["ARM1_ANGLE"].c_str())),
        angle2(std::atoi(CONFIG["ARM2_ANGLE"].c_str())),
        steps1(std::atoi(CONFIG["ARM1_STEP"].c_str())),
        steps2(std::atoi(CONFIG["ARM2_STEP"].c_str()))
        {
    
}
    
void Arm::moveTo(double x, double y){
    /*
    armLength*armLength + lenSq - armLength*armLength
    -------------------------------------------------     || a^2 + b^2 +2abcos(Q) = c^2
          2 * armLength * sqrt(lenSq)
    */
    static double prevAngle1=angle1, prevAngle2=angle2;
    
    //assume base is at (0,0)
    double baseX = 0, baseY = 0;
    double p = x-baseX, q = y-baseY;
    double lenSq = p*p + q*q;
    double len = std::sqrt(lenSq);
    
    //theta is the angle of first arm from horizontal
    double theta = std::atan2(q,p) + std::acos(lenSq/(2.0*armLength*len));
    double endPointX=baseX + armLength*std::cos(theta), endPointY=baseY+armLength*std::sin(theta);
    
    //phi is the second arm angle with the *horizontal*
    double phi = std::atan2(endPointY-p,endPointX-q);
    
    double phi_wrt_theta = phi-theta;
    
    //CRAP! I hate these many variables! x-(
}
void Arm::destroy(){
    std::stringstream ss1,ss2;
    ss1<<steps1;
    CONFIG["ARM1_STEP"] = ss1.str();
    
    ss2<<steps2;
    CONFIG["ARM2_STEP"] = ss2.str();
}