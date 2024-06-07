export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk

do: build
	cd build && make -j32

upload: do resetpico2
	./upload.sh

build: pico-extras
	mkdir build 
	cd build && cmake ..

clean:
	rm -rf build

resetpico2:
	-amidi -p $$(amidi -l | grep 'zeptocore\|zeptoboard\|ectocore\|zeemo' | awk '{print $$2}') -S "B00000"
	-curl localhost:7083 > /dev/null


pico-extras:
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git submodule update -i 

ignore:
	git status --porcelain | grep '^??' | cut -c4- >> .gitignore
	git commit -am "update gitignore"

midicom:
	cd dev/midicom && go build -v
	./dev/midicom/midicom

minicom:
	cd dev/minicom && go build -v
	./dev/minicom/minicom