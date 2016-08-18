.PHONY: all release debug clean install clean-debug clean-release

CMAKE_PREFIX_PATH=~/Qt56/5.6/gcc_64/lib/cmake/

PREFIX=..
all: release
release:
	@mkdir -p ${PREFIX}/Release
	@cd ${PREFIX}/Release && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_INSTALL_PREFIX=~/bin ${PREFIX}/sqt && make

debug:
	@mkdir -p ${PREFIX}/Debug
	@cd ${PREFIX}/Debug && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} ${PREFIX}/sqt && make

install:
	@if [ -d ${PREFIX}/Release ]; then cd ${PREFIX}/Release && make install; fi

clean: clean-debug clean-release
clean-release:
	@if [ -f ${PREFIX}/Release/Makefile ]; then cd ${PREFIX}/Release && make clean; fi
	@if [ -d ${PREFIX}/Release ]; then rm -rf ${PREFIX}/Release; fi

clean-debug:
	@if [ -f ${PREFIX}/Debug/Makefile ]; then cd ${PREFIX}/Debug && make clean; fi
	@if [ -d ${PREFIX}/Debug ]; then rm -rf ${PREFIX}/Debug; fi

