:: main and all of src - includes icon (info.res)
gcc main.c src/*.c info.res -o sandwich -Wall -O2 -std=c99 -Wno-missing-braces -I include/4.2/ -I src/include/ -L lib/4.2-windows/ -lraylib -lopengl32 -lgdi32 -lwinmm