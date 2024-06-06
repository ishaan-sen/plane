import sys
import pygame
import serial
import time

ser = serial.Serial('/dev/ttyACM0')

pygame.init()
pygame.joystick.init()

# Check if a joystick is connected
if pygame.joystick.get_count() == 0:
    print("No joystick connected", file=sys.stderr)
    exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()


axes = [0, 0, 0, 0, 0, 0]  # lx, ly, lt, rx, ry, rt


last_out = time.time()

while True:
    event = pygame.event.wait()
    if event.type == pygame.JOYAXISMOTION:
        # print("Axis:", event.axis, "Value:", event.value)
        axes[event.axis] = event.value * [1, -1, 1, 1, -1, 1][event.axis]

    out = bytes([int((x + 1) * 127) for x in axes] + [255, 255])
    # print(["%0.2f" % i for i in axes])
    now = time.time()
    if now - last_out > 0.1:
        last_out = now
        print(list(out))
        print(out)
        ser.write(out)
