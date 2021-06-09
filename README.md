# hw4
mbed_HW4

1.Xbee
send Xbee signal via car_control.py. it use Xbee to control PRC function.
i wrote a 'park' PRC function. it can reveive speed, d1, d2, direction(west), use to control bbcar.
first use encoder to count how further the car went(d2), then it turn 90 degree, then finish park.

2.Line detect
it use OpenMV to detect line segment.
linefinish.py: contorl OpenMV when a signal send from mbed via uart. it return the coordinates of line to mbed via uart 'one time'.

main.cpp:
it use counter to count timeout or read data. if time out, recall openMV to send Data again. 
when it reecive data from OpenMV, it can control the car. when x1, x1 lies is left half of picture (x1 or x2 < 80) steer bbcar to left and go forward in order to close the line.

3.April tag:
Same method as line detect in openMV. it return 2 values, including Tx and Ry. when mbed read arguments, it can control the car to face the tag.

control method: since we know the rotation angle of car and position of tag in the screen. we can contorl the car close to the perpendicular line of the tag then face to the tag. besides, when the angle is small we just need to turn the car sightly until to face to tag.
