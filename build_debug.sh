#rm -rf build
#mkdir build
#cd build
#cmake -G "Unix Makefiles" ..
#cmake --build . --config Debug --target netapp

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make