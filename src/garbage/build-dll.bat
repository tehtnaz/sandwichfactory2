:: makes the object files
mkdir build
cd build
gcc -c ../*.c -I ../../include/4.0/ -I ../include/ -L ../../lib/4.0/ -lraylib -lopengl32 -lgdi32 -lwinmm
cd ..

:: makes the dll using object files
gcc -shared -o sandwich.dll ./build/*.o -I ../include/4.0/ -I ./include/ -L ../lib/4.0/ -lraylib -lopengl32 -lgdi32 -lwinmm

move /Y sandwich.dll ../lib 