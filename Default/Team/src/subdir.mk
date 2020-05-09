################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Team/src/Team.c \
../Team/src/utils.c 

OBJS += \
./Team/src/Team.o \
./Team/src/utils.o 

C_DEPS += \
./Team/src/Team.d \
./Team/src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Team/src/%.o: ../Team/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


