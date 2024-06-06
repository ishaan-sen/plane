import os
import sys
import pygame
import threading
import time

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

    out = [int((x + 1) * 127) for x in axes]
    # print(["%0.2f" % i for i in axes])
    now = time.time()
    if now - last_out > 0.1:
        last_out = now
        for val in out:
            os.write(1, val.to_bytes(1))
            # print(val.to_bytes(1), end='\t')
        os.write(1, (255).to_bytes(1))
        os.write(1, (255).to_bytes(1))
