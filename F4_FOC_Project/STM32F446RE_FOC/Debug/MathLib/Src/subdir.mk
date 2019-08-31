################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MathLib/Src/MathLib.cpp \
../MathLib/Src/Matrix.cpp \
../MathLib/Src/PID.cpp \
../MathLib/Src/Trigonometric.cpp \
../MathLib/Src/ZIntegrate.cpp 

OBJS += \
./MathLib/Src/MathLib.o \
./MathLib/Src/Matrix.o \
./MathLib/Src/PID.o \
./MathLib/Src/Trigonometric.o \
./MathLib/Src/ZIntegrate.o 

CPP_DEPS += \
./MathLib/Src/MathLib.d \
./MathLib/Src/Matrix.d \
./MathLib/Src/PID.d \
./MathLib/Src/Trigonometric.d \
./MathLib/Src/ZIntegrate.d 


# Each subdirectory must supply rules for building sources it contributes
MathLib/Src/MathLib.o: ../MathLib/Src/MathLib.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -D__FPU_PRESENT -DSTM32F446xx -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../CMSIS_DSP -I../Inc -I../SystemLib/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Observer/Inc -I../MotorLib/Inc -I../MathLib/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MathLib/Src/MathLib.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
MathLib/Src/Matrix.o: ../MathLib/Src/Matrix.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -D__FPU_PRESENT -DSTM32F446xx -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../CMSIS_DSP -I../Inc -I../SystemLib/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Observer/Inc -I../MotorLib/Inc -I../MathLib/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MathLib/Src/Matrix.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
MathLib/Src/PID.o: ../MathLib/Src/PID.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -D__FPU_PRESENT -DSTM32F446xx -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../CMSIS_DSP -I../Inc -I../SystemLib/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Observer/Inc -I../MotorLib/Inc -I../MathLib/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MathLib/Src/PID.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
MathLib/Src/Trigonometric.o: ../MathLib/Src/Trigonometric.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -D__FPU_PRESENT -DSTM32F446xx -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../CMSIS_DSP -I../Inc -I../SystemLib/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Observer/Inc -I../MotorLib/Inc -I../MathLib/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MathLib/Src/Trigonometric.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
MathLib/Src/ZIntegrate.o: ../MathLib/Src/ZIntegrate.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -D__FPU_PRESENT -DSTM32F446xx -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../CMSIS_DSP -I../Inc -I../SystemLib/Inc -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Observer/Inc -I../MotorLib/Inc -I../MathLib/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MathLib/Src/ZIntegrate.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

