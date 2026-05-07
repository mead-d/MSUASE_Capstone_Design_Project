#!/bin/bash

SIMNAME="LCANsim_v1-3"
ITERATION="-1"
# Creates a video of recorded QT frames
# ffmpeg -framerate 30 -i Recording/frame_%05d.png -c:v libx264 -pix_fmt yuv420p $SIMNAME$ITERATION.mp4

# Creates a GIF from recorded QT frames
ffmpeg -framerate 30 -i Recording/frame_%05d.png -i palette.png -filter_complex "paletteuse" $SIMNAME$ITERATION.gif
# Either OR GIF depending on scale
# ffmpeg -framerate 30 -i Recording/frame_%05d.png -i palette.png -filter_complex "[0:v]scale=800:-1[v];[v][1:v]paletteuse" $SIMNAME$ITERATION.gif