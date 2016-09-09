###############################################################################
# Makefile 
###############################################################################

## General Flags
PROJECT = appusbsensor
ROOT_DIR = ..
ATMEL_DIR = $(ROOT_DIR)/atmel
ARCH_DIR = $(ATMEL_DIR)/at90usb128
COMMON_DIR = $(ATMEL_DIR)/common
USB_DIR = $(ROOT_DIR)/usbstack
UTIL_DIR = $(ROOT_DIR)/util
TEDS_DIR = $(ROOT_DIR)/teds

APP_DIR = .
CMP_SUPPORT_DIR = $(ROOT_DIR)/avrgcc
MCU = at90usb1287
TARGET = $(PROJECT).elf
CC = avr-gcc

# Boot loader starts at 0xf000 (large boot loader) word address
# Byte address=0x1e000
BOOT_SECTION_START = 0x1e000

F_CPU=8000000UL

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -g
CFLAGS += -DBOOT_SECTION_START=$(BOOT_SECTION_START)
# PRG_HIMEM==1 builds mem access routines to access upper prg memory (>64kb)
# CFLAGS += -DPRG_HIMEM=1
CFLAGS += -D AVRGCC -Wall -gdwarf-2  -Os -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += -DF_CPU=$(F_CPU)

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -T$(CMP_SUPPORT_DIR)/at90usbxxx.x
LDFLAGS += -Wl,-Map=$(PROJECT).map,--cref

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Include Directories
INCLUDES = -I"$(APP_DIR)" -I"$(APP_DIR)/conf" -I"$(ARCH_DIR)" -I"$(COMMON_DIR)" -I"$(USB_DIR)" -I"$(UTIL_DIR)" -I"$(TEDS_DIR)"

## Make directory for debugging files


## Objects that must be built in order to link
OBJECTS = appusbsensormain.o usb_stack.o usb_stack_std.o usb_drv_wrap.o hid_handler.o serial_debug.o teds.o

#usb_specific_request.o device_template_task.o
## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all:	$(TARGET)	$(PROJECT).hex	$(PROJECT).eep	$(PROJECT).epp	size
		
##	cp $(TARGET) ../Debug
##	cp $(PROJECT).hex ../Debug
##	cp $(PROJECT).eep ../Debug
##	cp $(PROJECT).epp ../Debug
##	cp $(PROJECT).map ../Debug
	

## Compile
appusbsensormain.o: appusbsensormain.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

serial_debug.o: $(UTIL_DIR)/serial_debug.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

hid_handler.o: $(USB_DIR)/hid_handler.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $<
   
usb_stack.o: $(USB_DIR)/usb_stack.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_stack_std.o: $(USB_DIR)/usb_stack_std.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usb_drv_wrap.o: $(USB_DIR)/usb_drv_wrap.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

teds.o: $(TEDS_DIR)/teds.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@  || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target 
.PHONY: clean
clean:
	-rm	$(OBJECTS) $(PROJECT).elf	$(PROJECT).hex	$(PROJECT).eep	$(PROJECT).map



