################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Broker.c \
<<<<<<< HEAD
../src/generales.c \
=======
../src/generales_broker.c \
>>>>>>> origin/master
../src/memoria.c 

OBJS += \
./src/Broker.o \
<<<<<<< HEAD
./src/generales.o \
=======
./src/generales_broker.o \
>>>>>>> origin/master
./src/memoria.o 

C_DEPS += \
./src/Broker.d \
<<<<<<< HEAD
./src/generales.d \
=======
./src/generales_broker.d \
>>>>>>> origin/master
./src/memoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
<<<<<<< HEAD
	gcc -I"/home/utnso/workspace/tp-2020-1c-Digimon/digiCommons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
=======
	gcc -I"/home/utnso/workspace/digiCommons" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
>>>>>>> origin/master
	@echo 'Finished building: $<'
	@echo ' '


