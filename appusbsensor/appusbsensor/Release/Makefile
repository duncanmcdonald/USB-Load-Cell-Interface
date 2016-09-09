################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += 


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS += 

OBJS_AS_ARGS += 

C_DEPS += 

C_DEPS_AS_ARGS += 

OUTPUT_FILE_PATH +=appusbsensor.elf

OUTPUT_FILE_PATH_AS_ARGS +=appusbsensor.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 0.0.0
	$(QUOTE)C:\cygwin64\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="appusbsensor.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=at90usb1286  
	@echo Finished building target: $@
	"C:\cygwin64\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "appusbsensor.elf" "appusbsensor.hex"
	"C:\cygwin64\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "appusbsensor.elf" "appusbsensor.eep" || exit 0
	"C:\cygwin64\bin\avr-objdump.exe" -h -S "appusbsensor.elf" > "appusbsensor.lss"
	"C:\cygwin64\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "appusbsensor.elf" "appusbsensor.srec"
	"C:\cygwin64\bin\avr-size.exe" "appusbsensor.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "appusbsensor.elf" "appusbsensor.a" "appusbsensor.hex" "appusbsensor.lss" "appusbsensor.eep" "appusbsensor.map" "appusbsensor.srec" "appusbsensor.usersignatures"
	