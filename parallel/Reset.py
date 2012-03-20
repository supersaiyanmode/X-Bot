import Server
import sys
 
r = Server.Robot()
print "Args", "\n".join(sys.argv)
if len(sys.argv) > 1 and sys.argv[1] == "test":
    r.moveXY(135,10)
    r.motorDown()
    raw_input("So .. On it?")
    r.motorUp()
 
print "Please bring the arm to Top right cell...\n"
try:
    while True:
        r.moveArm1(int(raw_input("Arm1 Steps: ")))
        r.moveArm2(int(raw_input("Arm2 Steps: ")))
except:
    r.curAngle1,r.curAngle2 = 1.280498333175863, 0.7284740625539476
    r.moveXY(0,380)