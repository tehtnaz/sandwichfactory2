:: makes the object files
mkdir build
cd build
gcc -c ../*.c -I ../../include/ -I ../include/ -L ../../lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
cd ..

:: makes the dll using object files
gcc -shared -o sandwich.dll ./build/*.o -I ../include/ -I ./include/ -L ../lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

move /Y sandwich.dll ../lib 