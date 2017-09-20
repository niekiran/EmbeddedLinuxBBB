################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/fastBit/courses/embedded_linux/EmbeddedLinuxBBB/Project_Src/Drivers/gpio/gpio_driver.c \
../src/jsmn.c \
E:/fastBit/courses/embedded_linux/EmbeddedLinuxBBB/Project_Src/Drivers/lcd/lcd_driver.c \
../src/lcd_news.c 

OBJS += \
./src/gpio_driver.o \
./src/jsmn.o \
./src/lcd_driver.o \
./src/lcd_news.o 

C_DEPS += \
./src/gpio_driver.d \
./src/jsmn.d \
./src/lcd_driver.d \
./src/lcd_news.d 


# Each subdirectory must supply rules for building sources it contributes
src/gpio_driver.o: E:/fastBit/courses/embedded_linux/EmbeddedLinuxBBB/Project_Src/Drivers/gpio/gpio_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\gpio" -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\lcd" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\gpio" -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\lcd" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/lcd_driver.o: E:/fastBit/courses/embedded_linux/EmbeddedLinuxBBB/Project_Src/Drivers/lcd/lcd_driver.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\gpio" -I"E:\fastBit\courses\embedded_linux\EmbeddedLinuxBBB\Project_Src\Drivers\lcd" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


