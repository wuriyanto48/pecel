.PHONY : build

build:
	echo 'clean and build project...' \
	&& rm -rf build/ \
	&& mkdir build && cd build/ && cmake .. && make \
	&& echo 'finish clean and build project...'