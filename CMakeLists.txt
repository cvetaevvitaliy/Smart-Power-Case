SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

set(VERSION_RELEASE 1)
set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)

set(VERSION_FW "${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}")
set(FIRMWARE_NAME firmware)
add_definitions(-DMINOR=\"${VERSION_RELEASE}\")
add_definitions(-DMAJOR=\"${VERSION_MAJOR}\")
add_definitions(-DPATCH=\"${VERSION_MINOR}\")

execute_process(COMMAND git rev-parse --abbrev-ref HEAD COMMAND cut -c1-10 OUTPUT_VARIABLE GIT_BRANCH)
string(STRIP ${GIT_BRANCH} GIT_BRANCH)
message(STATUS "GIT_BRANCH=${GIT_BRANCH}")
add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
#GIT_HASH=$(shell git rev-parse --short=8 HEAD)
execute_process(COMMAND git rev-parse --short=8 HEAD OUTPUT_VARIABLE GIT_HASH)
string(STRIP ${GIT_HASH} GIT_HASH)
message(STATUS "GIT_HASH=${GIT_HASH}")
add_definitions(-DGIT_HASH=\"${GIT_HASH}\")


add_definitions(-DSOFTWARE_VERSION=\"${VERSION_FW}\")
if (DEBUG_USB STREQUAL ON)
    add_definitions(-DUSE_USB_DEBUG_PRINTF)
    set(OPTIMIZATION_LEVEL -g)
else()
    add_definitions(-DUSE_USB_DEBUG)
    set(OPTIMIZATION_LEVEL -O2)
endif()

if (DEBUG STREQUAL ON)
    add_definitions(-DDEBUG=1)
endif()

# Set type STM32 STM32F103T8U - 64kb flash, STM32F103TBU - 128kb flash (add cmake build flag TYPE_MCU=F103TBU or TYPE_MCU=F103T8U)
set(TYPE_MCU_STM F103TBU F103T8U)
foreach(MCU ${TYPE_MCU_STM})
    if (TYPE_MCU STREQUAL F103T8U)
        SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103T8Ux_FLASH.ld)
        add_definitions(-DSTM32F103T8U)
        set(SEEK_DATA 64512)
        message(STM32F103T8U)
        set(MCU STM32F103T8U)
        set(EEPROM 0x800FC00)
        set(FLASH_SIZE 0x8001000)
        set(FLASH_SIZE_DEC 64K)
        add_definitions(-DMCU=\"STM32F103T8U\")
    else()
        add_definitions(-DSTM32F103TBU)
        set(SEEK_DATA 130048)
        message(STM32F103TBU)
        set(MCU STM32F103TBU)
        set(EEPROM 0x801FC00)
        set(FLASH_SIZE 0x8010000)
        set(FLASH_SIZE_DEC 128K)
        SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103TBUx_FLASH.ld)
        add_definitions(-DMCU=\"STM32F103TBU\")
    endif()
endforeach()

#SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
#SET(CMAKE_CXX_COMPILER_WORKS 1)
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
        "drivers/*.*"
        "src/*.*")


include_directories(src
        src/device
        src/ssd1306
        src/ssd1306/Fonts
        src/ssd1306/Image
        src/cli
        src/cli/lib
        src/main_app
        src/bq25895
        src/bq27441
        drivers/STM32F1xx_HAL_Driver/Inc
        drivers/STM32F1xx_HAL_Driver/Inc/Legacy
        drivers/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
        drivers/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
        drivers/CMSIS/Device/ST/STM32F1xx/Include
        drivers/CMSIS/Include)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")


set(BIN_FILE ${PROJECT_BINARY_DIR}/${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}_${TYPE_MCU}.bin)

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
endif()

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${SIZE} --format=berkeley ${PROJECT_NAME}.elf
        COMMAND arm-none-eabi-objcopy -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND echo
        #COMMAND ${CMAKE_OBJDUMP} -x $<TARGET_FILE:${PROJECT_NAME}.elf>
        COMMAND echo "${Yellow}Start create FullFW.bin ${FLASH_SIZE_DEC} firmware for flash via SWD ${ColourReset}"
        COMMAND echo \"${Green}*********************************************************${ColourReset}\"
        COMMAND echo "${Green}*     0x8000000 to 0x8000800 bootloader.bin             *${ColourReset}"
        COMMAND echo "${Green}*     0x8000800 to ${EEPROM} ${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}.bin         *${ColourReset}"
        COMMAND echo "${Green}*     ${EEPROM} to ${FLASH_SIZE} EEPROM data                *${ColourReset}"
        COMMAND echo \"${Green}*********************************************************${ColourReset}\"
        COMMAND dd if=${PROJECT_SOURCE_DIR}/bootloader.bin of=FullFW_${TYPE_MCU}.bin bs=1 count=2048
        COMMAND dd if=${PROJECT_BINARY_DIR}/${FIRMWARE_NAME}-${VERSION_RELEASE}.${VERSION_MAJOR}.${VERSION_MINOR}_${TYPE_MCU}.bin of=FullFW_${TYPE_MCU}.bin bs=1 seek=2048
        COMMAND dd if=${PROJECT_SOURCE_DIR}/eeprom.bin of=FullFW_${TYPE_MCU}.bin bs=1 seek=${SEEK_DATA}
        COMMAND echo "${Yellow}File FullFW.bin created ${ColourReset}"
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
