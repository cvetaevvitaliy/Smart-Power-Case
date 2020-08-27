#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

set(VERSION_RELEASE 1)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 7)
set(VERSION_FW "${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}")
set(FIRMWARE_NAME firmware)

add_definitions(-DSOFTWARE_VERSION=\"${VERSION_FW}\")
if (DEBUG STREQUAL ON)
    add_definitions(-DUSE_USB_DEBUG_PRINTF)
    set(OPTIMIZATION_LEVEL -g)
    else()
    set(OPTIMIZATION_LEVEL -O2)
endif()

# Set type STM32 STM32F103T8U - 64kb flash, STM32F103TBU - 128kb flash (add cmake build flag TYPE_MCU=F103TBU or TYPE_MCU=F103T8U)
set(TYPE_MCU_STM F103TBU F103T8U)
foreach(MCU ${TYPE_MCU_STM})
    if (TYPE_MCU STREQUAL F103T8U)
        SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103T8Ux_FLASH.ld)
        add_definitions(-DSTM32F103T8U)
        message(STM32F103T8U)
    else()
        add_definitions(-DSTM32F103TBU)
        message(STM32F103TBU)
        SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103TBUx_FLASH.ld)
    endif()
endforeach()

SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

#SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103TBUx_FLASH.ld)

SET(COMMON_FLAGS
    "-mcpu=cortex-m3 ${FPU_FLAGS} -mthumb -mthumb-interwork -ffunction-sections -fdata-sections \
    ${OPTIMIZATION_LEVEL} -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")

SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

PROJECT(FatShark_Power_V1 C CXX ASM)
set(CMAKE_CXX_STANDARD 11)

add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\) -DUSE_HAL_DRIVER -DSTM32F103xB)

# todo need implement multilingual UI
set(GUI_LANGUAGE EN DE FR RU)
foreach(LANGUAGE ${GUI_LANGUAGE})
    if (TRANSLATIONS STREQUAL ${LANGUAGE})
        add_definitions(-DTRANSLATIONS_${LANGUAGE})
    endif()
endforeach()

file(GLOB_RECURSE SOURCES "startup/*.*"
        "Middlewares/*.*"
        "Drivers/*.*" "Src/*.*"
        "Inc/*.*"
        "ssd1306/*.*"
        "ssd1306/Image/*.*"
        "ssd1306/Fonts/*.*"
        "Main_App/*.*")

include_directories(Inc
        Drivers/STM32F1xx_HAL_Driver/Inc
        Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
        Middlewares/ST/STM32_USB_Device_Library/Core/Inc
        Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
        Drivers/CMSIS/Device/ST/STM32F1xx/Include Drivers/CMSIS/Include
        ssd1306 ssd1306/Fonts ssd1306/Image Main_App)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-u,_printf_float,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        #COMMAND ${SIZE} --format=SysV ${PROJECT_NAME}.elf
        COMMAND ${SIZE} --format=berkeley ${PROJECT_NAME}.elf
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        #COMMAND ${CMAKE_OBJDUMP} -x $<TARGET_FILE:${PROJECT_NAME}.elf>
        COMMAND echo \"*****************************************************************\"
        COMMAND echo Start create Full firmware for flash via SWD
        COMMAND echo In Flash memory 0x8000000 bootloader.bin
        COMMAND echo In Flash memory 0x8000800 ${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}.bin
        COMMAND dd if=${PROJECT_SOURCE_DIR}/bootloader.bin of=FullFW.bin bs=1 count=2048
        COMMAND dd if=${PROJECT_BINARY_DIR}/${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}.bin of=FullFW.bin bs=1 seek=2048
        COMMAND echo File FullFW.bin created complete
        COMMAND echo \"*****************************************************************\"
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
