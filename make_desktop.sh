
# Stop and exit on error
set -e

# Setup compiler build flags
CC="g++"
CFLAGS="-O3 -g -std=c++14 -lGL -lGLEW -lSDL2 -lSDL2_mixer -lSDL2_image -lpthread -DDESKTOP=true"


# Build the exe file
echo Building exe ...
$CC main.cpp $CFLAGS -o main
