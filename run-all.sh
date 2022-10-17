echo "\n---LINUX BUILD:---\n"
gcc-12 main.c dataHandling2.c src/*.c -o sandwich -O2 -Wall -std=c99 -Wno-missing-braces -I include/4.2/ -I src/include/ -L lib/4.2-linux/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
echo "\n---WINDOWS BUILD:---\n"
x86_64-w64-mingw32-gcc main.c dataHandling2.c src/*.c info.res -o sandwich.exe -O2 -Wall -std=c99 -Wno-missing-braces -I include/4.2/ -I src/include/ -L lib/4.2-windows/ -lraylib -lopengl32 -lgdi32 -lwinmm