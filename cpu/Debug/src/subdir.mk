################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ciclo_instruccion.c \
../src/cpu.c \
../src/cpu_config.c \
../src/mmu.c \
../src/peticiones.c \
../src/tlb.c 

OBJS += \
./src/ciclo_instruccion.o \
./src/cpu.o \
./src/cpu_config.o \
./src/mmu.o \
./src/peticiones.o \
./src/tlb.o 

C_DEPS += \
./src/ciclo_instruccion.d \
./src/cpu.d \
./src/cpu_config.d \
./src/mmu.d \
./src/peticiones.d \
./src/tlb.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-lo-importante-es-aprobar/utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


