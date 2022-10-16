echo "\n---LINUX BUILD:---\n"
gcc-12 main.c dataHandling2.c src/*.c -o sandwich -O3 -Wall -std=c99 -Wno-missing-braces -I include/4.0/ -I src/include/ -L lib/4.0-linux/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
echo "\n---WINDOWS BUILD:---\n"
x86_64-w64-mingw32-gcc main.c dataHandling2.c src/*.c info.res -o sandwich.exe -O3 -Wall -std=c99 -Wno-missing-braces -I include/4.0/ -I src/include/ -L lib/4.0/ -lraylib -lopengl32 -lgdi32 -lwinmm