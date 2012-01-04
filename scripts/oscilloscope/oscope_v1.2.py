#!/usr/bin/env python
# encoding: utf-8
# BPscope v 1.2
# Author: hwmayer
# Site: hwmayer.blogspot.com
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# From: http://dangerousprototypes.com/forum/index.php?topic=976
# More: http://dangerousprototypes.com/docs/Bus_Pirate:_Python_Oscilloscope
# USAGE:
# f - trigger on falling slope
# r - trigger on rising slope
# s - trigger off
# key_up 		- trigger level++
# key_down 	- trigger level--
# 9 - time scale++ (zoom out)
# 0 - time scale-- (zoom in)
# q - QUIT
#
#"pygame" lib is needed to run this script.
#
#To install in Ubuntu use this command:
# sudo apt-get install python-pygame
import os, sys
import Image
import serial
import pygame
import time

from pyBusPirateLite.UART import *
from pyBusPirateLite.BitBang import *

NO_SYNC = 0
RISING_SLOPE = 1
FALLING_SLOPE = 2

#change this path
BUS_PIRATE_DEV = "/dev/ttyUSB0"

RES_X = 640
RES_Y = 480
MAX_VOLTAGE = 6
OFFSET = 10
TRIGGER_LEV_RES = 0.05
TRIG_CAL = 0.99
DEFAULT_TIME_DIV = 1
DEFAULT_TRIGGER_LEV = 1.0
DEFAULT_TRIGGER_MODE = 0



DATA_RATE = 5720.0 #measures/second (estimated experimenticaly)

DEFAULT_TIME_SCALE = RES_X / DATA_RATE #default time in seconds to make one window fill

pygame.init() 

bp = UART(BUS_PIRATE_DEV,115200)

print "Entering binmode: ",
if bp.BBmode():
	print "OK."
else:
	print "failed."
	sys.exit()
	
window = pygame.display.set_mode((RES_X, RES_Y)) 
background = (0,0,0)
line = (0,255,0)
trig_color = (100,100,0)

time_div = DEFAULT_TIME_DIV
trigger_level = DEFAULT_TRIGGER_LEV
trig_mode = DEFAULT_TRIGGER_MODE

bp.port.write("\x15")
while 1:
	plot = {}
	voltage = {}
	maxv = 0
	minv = 100
	time_scale = DEFAULT_TIME_SCALE * time_div
	prev_voltage = 0
	measure = 0;
	
	if(trig_mode != NO_SYNC):
		for k in range(1,2000):
			prev_voltage = voltage
			measure = bp.response(2, True)
			voltage = ord(measure[0]) << 8
			voltage = voltage + ord(measure[1])
			voltage = (voltage/1024.0) * 6.6
			#rising slope
			if((voltage >= trigger_level) and (prev_voltage < (voltage * TRIG_CAL)) and (trig_mode == RISING_SLOPE)):
				break
			if((voltage < trigger_level) and (voltage > 0.01) and (prev_voltage > voltage/TRIG_CAL) and (trig_mode == FALLING_SLOPE)):
				break
		
	for i in range(RES_X):
		
		for k in range(time_div - 1):
			#ignoring (time_div-1) samples to achieve proper time resolution
			bp.response(2, True)
		measure = bp.response(2, True)
		voltage = ord(measure[0]) << 8
		voltage = voltage + ord(measure[1])
		voltage = (voltage/1024.0) * 6.6
		plot[i] = voltage	
		

	for i in range(1,RES_X):
			if plot[i] > maxv:
				maxv = plot[i]
			if plot[i] < minv:
				minv = plot[i]
				
			y = (RES_Y) - plot[i]*(RES_Y/MAX_VOLTAGE) - OFFSET
			x = i
			px = i-1;
			py = (RES_Y) - plot[i-1]*(RES_Y/MAX_VOLTAGE) - OFFSET
			pygame.draw.line(window, line, (px, py), (x, y))	
			trig_y = RES_Y - trigger_level * (RES_Y/MAX_VOLTAGE)
			pygame.draw.line(window, trig_color, (0, trig_y), (RES_X, trig_y))
		
	##GUI)
	font = pygame.font.Font(None, 19)
	text_max_voltage = font.render("Max: %f V" % maxv, 1, (255, 255, 255))
	text_min_voltage = font.render("Min: %f V" % minv, 1, (255, 255, 255))
	text_time_scale = font.render("Timescale: %f s" % time_scale, 1, (255, 255, 255))
	text_maxv_Rect = text_max_voltage.get_rect()
	text_minv_Rect = text_min_voltage.get_rect()
	text_time_scale_Rect = text_time_scale.get_rect()
	text_maxv_Rect.x = 10
	text_maxv_Rect.y = 10
	text_minv_Rect.x = 10 
	text_minv_Rect.y = 30
	text_time_scale_Rect.x = 10
	text_time_scale_Rect.y = 50
	window.blit(text_max_voltage, text_maxv_Rect)
	window.blit(text_min_voltage, text_minv_Rect)
	window.blit(text_time_scale, text_time_scale_Rect)

	pygame.display.flip() 
			
	for event in pygame.event.get(): 
		if event.type == pygame.QUIT: 
			sys.exit(0) 	
		elif event.type == pygame.KEYDOWN:
			if event.key == pygame.K_0:
				print "timescale x 2"
				time_div = time_div * 2
			elif event.key == pygame.K_9:
				if (time_div >= 2):
					print "timescale / 2"
					time_div = time_div / 2
			elif event.key == pygame.K_s:
				print "Trigger of, no sync"
				trig_mode = NO_SYNC
			elif event.key == pygame.K_f:
				print "Trigger set to falling slope"
				trig_mode = FALLING_SLOPE
			elif event.key == pygame.K_r:
				print "Trigger set to rising slope"
				trig_mode = RISING_SLOPE
			elif event.key == pygame.K_UP:
				trigger_level += TRIGGER_LEV_RES
				print "Trigger level: %f" % trigger_level
			elif event.key == pygame.K_DOWN:
				trigger_level -= TRIGGER_LEV_RES
				print "Trigger levelL: %f" % trigger_level
			elif event.key == pygame.K_q:
				sys.exit(0)
			

	window.fill(background)

#END


