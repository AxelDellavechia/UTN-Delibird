################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gameboy/src/GameBoy.c \
../gameboy/src/Generales.c 

OBJS += \
./gameboy/src/GameBoy.o \
./gameboy/src/Generales.o 

C_DEPS += \
./gameboy/src/GameBoy.d \
./gameboy/src/Generales.d 


# Each subdirectory must supply rules for building sources it contributes
gameboy/src/%.o: ../gameboy/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


