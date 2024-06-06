import pygame
# import time

pygame.init()
pygame.joystick.init()

# Check if a joystick is connected
if pygame.joystick.get_count() == 0:
    print("No joystick connected")
    exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()

print("Joystick Name:", joystick.get_name())

axes = [0, 0, 0, 0, 0, 0]  # lx, ly, lt, rx, ry, rt

while True:
    for event in pygame.event.get():
        if event.type == pygame.JOYAXISMOTION:
            print("Axis:", event.axis, "Value:", event.value)
            axes[event.axis] = event.value * [1, -1, 1, 1, -1, 1][event.axis]

    out = [int((x + 1.0) * 128.0) for x in axes]
    # print(["%0.2f" % i for i in axes])
    print(["%0.2f" % i for i in out])
 
