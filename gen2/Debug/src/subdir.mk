################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/analysis.c \
../src/gen2.c \
../src/id-list.c \
../src/prettyprinter.c \
../src/scan.c 

OBJS += \
./src/analysis.o \
./src/gen2.o \
./src/id-list.o \
./src/prettyprinter.o \
./src/scan.o 

C_DEPS += \
./src/analysis.d \
./src/gen2.d \
./src/id-list.d \
./src/prettyprinter.d \
./src/scan.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


