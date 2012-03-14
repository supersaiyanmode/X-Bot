#include "Arm.h"
#include <sstream>
#include <string>

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
Arm::Arm(double len, int s1, int s2):
        armLength(len), steps1(s1), steps2(s2){
    
}
    
void Arm::moveTo(double, double){
    
}

void Arm::destroy(){
    std::stringstream ss1,ss2;
    ss1<<steps1;
    CONFIG["ARM1_STEP"] = ss1.str();
    
    ss2<<steps2;
    CONFIG["ARM2_STEP"] = ss2.str();
}