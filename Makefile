export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk

do: build
	cd build && make -j32

upload: do
	./upload.sh

build: pico-extras
	mkdir build 
	cd build && cmake ..

clean:
	rm -rf build

pico-extras:
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git submodule update -i 

ignore:
	git status --porcelain | grep '^??' | cut -c4- >> .gitignore
	git commit -am "update gitignore"
