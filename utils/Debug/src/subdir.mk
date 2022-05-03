################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/buffer.c \
../src/cliente.c \
../src/comunicacion.c \
../src/conexiones.c \
../src/errors.c \
../src/paquete.c \
../src/serializador.c \
../src/servidor.c \
../src/sockets.c 

OBJS += \
./src/buffer.o \
./src/cliente.o \
./src/comunicacion.o \
./src/conexiones.o \
./src/errors.o \
./src/paquete.o \
./src/serializador.o \
./src/servidor.o \
./src/sockets.o 

C_DEPS += \
./src/buffer.d \
./src/cliente.d \
./src/comunicacion.d \
./src/conexiones.d \
./src/errors.d \
./src/paquete.d \
./src/serializador.d \
./src/servidor.d \
./src/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


