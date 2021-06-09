import pyb, sensor, image, time, math

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

def degrees(radians):
   return (180 * radians) / math.pi

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
   clock.tick()
   img = sensor.snapshot()
   i = 0;


   if (uart.readchar() == 115):
    print("start Read")
    for l in img.find_line_segments(merge_distance = 1, max_theta_diff = 5):

      if ((l.magnitude() > 5) & (l.y1() < 30) & (l.y2() < 30) & (i == 0)):
         i = 1
         img.draw_line(l.line(), color = (255, 0, 0))
         #print(l)
         print_args = (l.x1(), l.x2(), l.y1(), l.y2())
      # Translation units are unknown. Rotation units are in degrees.

      # select need data
         uart.write(("%d,%d,%d,%d\n" % print_args).encode())
         print("%d,%d,%d,%d\n" % print_args)

      #uart.write(("Tx: %f, Ty %f, Tz %f, Rx %f, Ry %f, Rz %f" % print_args).encode())
   #uart.write(("FPS %f\r\n" % clock.fps()).encode())
