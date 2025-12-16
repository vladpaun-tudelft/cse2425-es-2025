#!/bin/bash

cmake --build build
picotool load build/robot.uf2
picotool reboot