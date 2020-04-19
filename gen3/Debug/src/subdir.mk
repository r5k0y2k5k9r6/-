################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/analysis.c \
../src/cross-reference.c \
../src/gen3.c \
../src/scan.c 

OBJS += \
./src/analysis.o \
./src/cross-reference.o \
./src/gen3.o \
./src/scan.o 

C_DEPS += \
./src/analysis.d \
./src/cross-reference.d \
./src/gen3.d \
./src/scan.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


