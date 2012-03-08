#include "Arm.h"

//########STEPPER MOTOR CONTROL!!###############
static const char array[] = {8,12,4,6,2,3,1,9};

StepperMotor::StepperMotor():curSteps(0){

}
char StepperMotor::stepRight(){
    return array[curSteps=((curSteps+1)%8)]
}
char StepperMotor::stepLeft(){
    return array[curSteps=((8+curSteps-1)%8)]
}


//#########MAIN ARM!!#######################
Arm::Arm(double len):armLength(len), theta1(0.0), theta2(0.0){
    
}
    
void moveTo(double, double){
    
};
