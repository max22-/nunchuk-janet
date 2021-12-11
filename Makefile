all: nunchuk

nunchuk: nunchuk.c
	gcc nunchuk.c -o nunchuk

clean:
	rm -f nunchuk
