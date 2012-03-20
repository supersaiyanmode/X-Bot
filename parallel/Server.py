import socket 
from parallel import *
import time
import math
import thread
 
toRad = lambda x: x*math.pi/180.0
toDeg = lambda x: x*180.0/math.pi
def toSteps(x):
    print type(x)
    return int(x*2044/math.pi)
 
STEPS =[1,3,2,6,4,12,8,9]
 
class Connection:
    def __init__(self, r,s):
        self.robot = r
        self.sock = s
        self.process()
        
    def process(self):
        while True:
            try:
                line = ""
                while True:
                    cur = self.sock.recv(1)
                    if cur == "\n":
                        break;
                    line += cur
                if line == "":
                    continue
                print "Received:",line, "(%d)"%len(line)
                if line.startswith("UP"):
                    self.robot.motorUp()
                elif line.startswith("DOWN"):
                    self.robot.motorDown()
                elif line.startswith("MAGNETON"):
                    self.robot.magnetOn()
                elif line.startswith("MAGNETOFF"):
                    self.robot.magnetOff()
                elif line.startswith("STEP"):
                    line = line.split(" ")
                    (r.moveArm1 if line[1] == "1" else r.moveArm2)(int(line[2]))
                else:
                    [x,y] = map(int,line.split(' '))
                    print "moving..",
                    self.robot.moveXY(x,y)
                    print "..Done!"
                self.sock.send("DONE\n")
            except Exception,e:
                self.sock.send("ERROR!\n" + str(e))
 
class Server:
    def __init__(self,r):
        self.s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
        self.s.bind(("192.168.1.100",21739))
        self.s.listen(1)
        self.robot = r
        
    def start(self):
        print "SERVER STARTED"
        while True:
            sock,details=self.s.accept()
            thread.start_new_thread(Connection,(self.robot,sock))
 
class Robot:
    def __init__(self):
        self.p = Parallel()
        self._setData = self.p.setData
        self.p.setData = lambda x: self._setData(255 - x)
        self.magnetOff()
        self.motorStop()
        self.p.setData(0)
        
        self.armLength = 190
        self.curAngle1 = math.pi/2
        self.curAngle2 = math.pi
        self.curStep1 = 0
        self.curStep2 = 0
  
    def getAngles(self,x,y):
        lenSq = x*x + y*y
        length = math.sqrt(lenSq)
        
        #theta is the angle of first arm from horizontal
        theta = math.acos(lenSq/(2.0*self.armLength*length))
        
        #second angle is pi/2 - 2*theta from perpendicular of first arm..
        return [math.atan2(y,x) + theta, math.pi - 2*theta]
    
    def getMoveSteps(self,x,y):
        [ang1, ang2] = self.getAngles(x,y)
        print "Position:", x,y
        print "Angles:", toDeg(ang1), toDeg(ang2)
        print "CurAngles:",toDeg(self.curAngle1),toDeg(self.curAngle2)
        toAngle1,toAngle2 = ang1-self.curAngle1, ang2 - self.curAngle2
        
        #clockwise is negative, anti -- posreitive!!
        toAngle1,toAngle2 = -toAngle1,-toAngle2
        
        print "DeltaAngles:",toDeg(toAngle1),toDeg(toAngle2)
        toStep1,toStep2 = toSteps(toAngle1), toSteps(toAngle2)
        print "Steps:",toStep1, toStep2
        raw_input();
        return toStep1,toStep2
    
    def moveXY(self, x,y):
        toStep1,toStep2 = self.getMoveSteps(x,y)
        self.moveArm1(steps=toStep1)
        self.moveArm2(steps=toStep2)
        self.curAngle1,self.curAngle2 = self.getAngles(x,y)
    
    def parallelMoveXY(self,x,y):
        toStep1,toStep2 = self.getMoveSteps(x,y)
        step1 = 1 if toStep1>0 else 7
        step2 = 1 if toStep2>0 else 7
        reduceStep1 = 1 if toStep1>0 else -1
        reduceStep2 = 1 if toStep2>0 else -1
        
        for i in xrange(min(abs(toStep1),abs(toStep2))):
            self.curStep1 = (self.curStep1+step1)%8
            self.curStep2 = (self.curStep2+step2)%8
            step1 -= reduceStep1
            step2 -= reduceStep2
            self.p.setData(STEPS[self.curStep1] | (STEPS[self.curStep2]<<4))
            time.sleep(0.01)
            print ".",
        if step1:
            print "Moving extra step1"
            self.moveArm1(step1)
        if step2:
            print "Moving extra step2"
            self.moveArm2(step2)
            
    def motorDown(self,t=1.8):
        self.p.setAutoFeed(1)
        self.p.setSelect(0)
        time.sleep(t)
        self.p.setAutoFeed(0)
        self.p.setSelect(0)
        
    def motorUp(self,t=1.8):
        self.p.setAutoFeed(0)
        self.p.setSelect(1)
        time.sleep(t)
        self.p.setAutoFeed(0)
        self.p.setSelect(0)
        
    def motorStop(self):
        self.p.setAutoFeed(0)
        self.p.setSelect(0)
        
    def magnetOn(self):
        self.p.setDataStrobe(1)
        #self.p.setInitOut(1)
    
    def magnetOff(self):
        self.p.setDataStrobe(0)
        #self.p.setInitOut(0)
 
    def moveArm1(self, steps=400):
        step = 1 if steps>0 else 7
        for i in xrange(abs(steps)):
            self.curStep1 = (self.curStep1+step)%8
            self.p.setData(STEPS[self.curStep1])
            time.sleep(0.01)
    
    def moveArm2(self, steps=400):
        steps = -steps #stupid 2nd arm moves in the opposite direction!
        
        step = 1 if steps>0 else 7
        for i in xrange(abs(steps)):
            self.curStep2 = (self.curStep2+step)%8
            self.p.setData(STEPS[self.curStep2]<<4)
            time.sleep(0.01)
            
    def test(self):
        self.motorDown()
        self.magnetOn()
        time.sleep(0.1)
        self.motorUp()
        self.moveArm1()
        self.moveArm2()
        self.motorDown()
        self.magnetOff()
        self.motorUp()
        
def main():
    r = Robot()
    r.motorStop()
    s = Server(r)
    s.start()
    
    
if __name__ == '__main__':
    main()