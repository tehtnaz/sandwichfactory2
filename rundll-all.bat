cd src
:: makes the object files
mkdir build
cd build
gcc -c ../*.c -I ../../include/4.0 -I ../include/ -L ../../lib/4.0/ -lraylib -lopengl32 -lgdi32 -lwinmm
cd ..

:: makes the dll using object files
gcc -shared -o sandwich.dll ./build/*.o -I ../include/4.0/ -I ./include/ -L ../lib/4.0/ -lraylib -lopengl32 -lgdi32 -lwinmm

move /Y sandwich.dll ../lib 


rmdir /S /Q build
cd ..

:: compiles the final exe using the dll and other.c file
gcc main.c info.res -o sandwichdll.exe -O1 -Wall -std=c99 -Wno-missing-braces -I include/4.0 -I src/include/ -L lib/4.0 -lsandwich -lraylib -lopengl32 -lgdi32 -lwinmm

move /Y .\lib\sandwich.dll .\