INO=the_box.ino
BOARD=arduino:avr:mega:cpu=atmega2560

all: verify

upload:
	arduino --board $(BOARD) --verbose-upload --upload $(INO)

verify:
	arduino --board $(BOARD) --verbose-build --preserve-temp-files --verify $(INO)

clean:
