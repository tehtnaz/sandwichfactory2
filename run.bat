:: main and all of src
gcc main.c src/*.c info.res -o sandwich.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/ -I src/include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm