DEVICE     = atmega328p
CLOCK      = 16000000
ARDUINO    = 100

AVRDUDE = /home/dronische/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino9/bin/avrdude -C/home/dronische/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino9/etc/avrdude.conf -carduino -P/dev/ttyUSB0 -b57600 -D -p atmega328p -v

OBJECTS = main.o

INC_LIB = /home/dronische/Arduino/libraries
INC_ARD = /home/dronische/.arduino15/packages/arduino/hardware/avr/1.6.18
INC     = 	$(INC_LIB)/Adafruit-GFX-Library \
			$(INC_LIB)/Adafruit_INA219 \
			$(INC_LIB)/Adafruit_SSD1306 \
			$(INC_ARD)/cores/arduino \
			$(INC_ARD)/variants/standard \
			$(INC_ARD)/libraries/SPI/src \
			$(INC_ARD)/libraries/Wire/src
INC_PARAMS = $(INC:%=-I%)

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) 
COMPILE_EX = $(COMPILE) $(INC_PARAMS) -DARDUINO=$(ARDUINO)

all:	main.hex

.c.o:
	$(COMPILE_EX) -c $< -o ./bin/$@

.cpp.o:
	$(COMPILE_EX) -c $< -o ./bin/$@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:./bin/main.hex:i

clean:
	rm -f ./bin/*

main.elf: $(OBJECTS)
	mkdir -p ./bin
	$(COMPILE) -o bin/main.elf ./bin/$(OBJECTS)

main.hex: main.elf
	rm -f ./bin/main.hex
	avr-objcopy -j .text -j .data -O ihex ./bin/main.elf ./bin/main.hex
	avr-size --format=avr --mcu=$(DEVICE) ./bin/main.elf
