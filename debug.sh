gcc-12 main.c src/*.c -o sandwich -Wall -D DEBUG_DATA_HANDLING -g -std=c99 -Wno-missing-braces -I include/4.2/ -I src/include/ -L lib/4.2-linux/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11