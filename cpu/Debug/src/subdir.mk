################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cpu.c \
../src/cpu_config.c \
../src/servidor_dispatch.c \
../src/servidor_interrupt.c 

OBJS += \
./src/cpu.o \
./src/cpu_config.o \
./src/servidor_dispatch.o \
./src/servidor_interrupt.o 

C_DEPS += \
./src/cpu.d \
./src/cpu_config.d \
./src/servidor_dispatch.d \
./src/servidor_interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-lo-importante-es-aprobar/utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


