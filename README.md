# Pathtracer

Pathtracer implementation

## Config file

Path to input file:
```
input C:\models\file.obj
```
Path to output file (png only):
```
output C:\result.png
```
Number of bounces per path:
```
k 0
```
Height of rendered image:
```
yres 500
```
Width of rendered image:
```
xres 500
```
Position of camera:
```
VPx 0.0
VPy 0.0
VPz 0.0
```
Look at:
```
LAx 0.0
LAy 1.0
LAz 0.0
```
Up direction:
```
UPx 0.0
UPy 1.0
UPz 0.0
```

## Batch mode
Executing program in batch mode consist of adding 'batch' as last argument ie.
```
pathtracer.exe config.rtc batch
```

##Gui mode

Executing program without 'batch' argument makes it start in interactive mode with interface.
In this mode you can move camera with WASD buttons and rotate it with mouse while holding SPACE.
Pressing T starts rendering scene from current camera perspective.

## Dependencies

GLFW3
ASSIMP
OPENGL


