all: build/nunchuk.so

build/nunchuk.so: nunchuk.c project.janet
	jpm build

clean:
	rm -rf build/
