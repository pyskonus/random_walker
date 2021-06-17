import pygame
from pygame.locals import *
import cv2
import os
from math import floor

from pygame.constants import MOUSEBUTTONDOWN, MOUSEBUTTONUP

img = cv2.imread("../images/text2.png")

[height, width] = img.shape[0], img.shape[1]
scale = 10  # TODO: auto scaling, for huge images as well
width *= scale
height *= scale

resized = cv2.resize(img, (width, height), interpolation=cv2.INTER_AREA)

cv2.imwrite("resized_text.png", resized)
cv2.destroyAllWindows()

pygame.init()
pygame.display.set_caption("Selector")
screen = pygame.display.set_mode((resized.shape[1], resized.shape[0]))
bg = pygame.image.load("resized_text.png")
clicked = False
run = True

seeds = dict()  # seed_pos|tuple(int, int) -> seed_type|int
seed_pos = (0,0)
seed_type = 0
while (run):
    screen.blit(bg, (0,0))
    
    if clicked:
        seed_pos = pygame.mouse.get_pos()
        seed_pos = (floor(seed_pos[0]/scale), floor(seed_pos[1]/scale))
        # if seed_pos not in seeds:
        #     print(seed_pos, '->', seed_type)
        seeds[seed_pos] = seed_type
        
    
    pygame.display.update()

    for event in pygame.event.get():
        if event.type == MOUSEBUTTONDOWN:
            clicked = True
        if event.type == MOUSEBUTTONUP:
            clicked = False
        if event.type == KEYDOWN:
            if event.key == K_TAB:
                seed_type += 1
                # print("SEED TYPE UPDATE TO", seed_type)
        if event.type == pygame.QUIT:
            run = False

pygame.quit()

with open("../seeds.dat", 'w') as result:
    result.write(str(seed_type+1))
    for coord in seeds:
        result.write('\n'+str(coord[1])+' '+str(coord[0])+' '+str(seeds[coord]))

os.remove("resized_text.png")
