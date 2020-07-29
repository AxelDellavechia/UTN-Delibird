################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: broker

dependents:
<<<<<<< HEAD
	-cd /home/utnso/workspace/tp-2020-1c-Digimon/digiCommons/Debug && $(MAKE) clean all

# Tool invocations
broker: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2020-1c-Digimon/digiCommons/Debug/libdigiCommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc -L"/home/utnso/workspace/tp-2020-1c-Digimon/digiCommons/Debug" -o "broker" $(OBJS) $(USER_OBJS) $(LIBS)
=======
	-cd /home/utnso/workspace/digiCommons/Debug && $(MAKE) all

# Tool invocations
broker: $(OBJS) $(USER_OBJS) /home/utnso/workspace/digiCommons/Debug/libdigiCommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc -L"/home/utnso/workspace/digiCommons/Debug" -o "broker" $(OBJS) $(USER_OBJS) $(LIBS)
>>>>>>> origin/master
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) broker
	-@echo ' '

.PHONY: all clean dependents
<<<<<<< HEAD
/home/utnso/workspace/tp-2020-1c-Digimon/digiCommons/Debug/libdigiCommons.so:
=======
/home/utnso/workspace/digiCommons/Debug/libdigiCommons.so:
>>>>>>> origin/master

-include ../makefile.targets
