INO=rocket/rocket.ino

verify:
	arduino --board arduino:avr:nano --verbose-build --preserve-temp-files --verify $(INO)

upload:
	arduino --board arduino:avr:nano --verbose-upload --upload $(INO)

clean:
