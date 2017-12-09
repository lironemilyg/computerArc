################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tomasulo/Register.c \
../Tomasulo/main.c 

OBJS += \
./Tomasulo/Register.o \
./Tomasulo/main.o 

C_DEPS += \
./Tomasulo/Register.d \
./Tomasulo/main.d 


# Each subdirectory must supply rules for building sources it contributes
Tomasulo/%.o: ../Tomasulo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


