################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/gestionar_conexiones.c \
../src/memoria.c \
../src/memoria_config.c \
../src/memoria_principal.c \
../src/paginacion.c \
../src/swap.c 

OBJS += \
./src/gestionar_conexiones.o \
./src/memoria.o \
./src/memoria_config.o \
./src/memoria_principal.o \
./src/paginacion.o \
./src/swap.o 

C_DEPS += \
./src/gestionar_conexiones.d \
./src/memoria.d \
./src/memoria_config.d \
./src/memoria_principal.d \
./src/paginacion.d \
./src/swap.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2022-1c-lo-importante-es-aprobar/utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


