DIR=release_build &&
mkdir -p ${DIR} &&
cd ${DIR} &&
cmake -D CMAKE_BUILD_TYPE=Release .. &&
make 
