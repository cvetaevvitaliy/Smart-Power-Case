//
// Created by Vitaliy on 11/1/20.
//
#include "CLI_comands.h"
#include "Power.h"

static uint8_t get_bat(void);
static uint8_t get_eeprom(void);
static uint8_t get_charger(void);
static uint8_t set_out(void);

static uint8_t get(void);


void CLI_cmd_init(void) {
    /*** @brief CLI_AddCmd (name, fcn, argc, mode, descr)
     * * @param name - input name (string)
     * * @param fcn - callback function
     * * @param argc - min count arguments (if no arguments are required = 0)
     * * @param mode - execute mode:  TMC_None, TMC_PrintStartTime, TMC_PrintStopTime, TMC_PrintDiffTime
     * * @param descr - description command (string)
     * 	for example @mode - TMC_PrintStartTime | TMC_PrintStopTime - to see the command execution time
     * */

    CLI_AddCmd("bat", get_bat, 0, CLI_PrintNone, "get battery info");
    CLI_AddCmd("eeprom", get_eeprom, 0, CLI_PrintNone, "get eeprom data");
    CLI_AddCmd("charger", get_charger, 0, CLI_PrintNone, "get charger chip info");
    CLI_AddCmd("out", set_out, 0, CLI_PrintNone, "enable or disable power out");
}

// ***************** implementation commands ****************

static uint8_t get(void){

    if (CLI_GetArgStr(0, "bat")) {
        get_bat();
    } else if (CLI_GetArgStr(0, "eeprom")) {
        get_eeprom();
    } else if (CLI_GetArgStr(0, "charger")) {
        get_charger();
    } else {
        CLI_Printf("\r\nError: get: bat, eeprom, charger");
        //return TE_ArgErr;
    }

    return CLI_OK;
}

uint8_t get_bat(void) {

    Device_Status_t* device =  Get_Device_Status();

    CLI_Printf("\r\n Voltage:\t%.2fV", device->Battery_Info.Vbat);
    CLI_Printf("\r\n Current:\t%dmA", device->Battery_Info.current);
    CLI_Printf("\r\n Power:\t\t%dmW", device->Battery_Info.power);
    CLI_Printf("\r\n Health:\t%d%%", device->Battery_Info.health);
    CLI_Printf("\r\n Full capacity:\t%dmAh", device->Battery_Info.capacity_full);
    CLI_Printf("\r\n Capacity:\t%dmAh", device->Battery_Info.capacity);

    return CLI_OK;
}

uint8_t get_eeprom(void) {

    Device_Status_t* device =  Get_Device_Status();

    CLI_Printf("\r\n Buzzer:\t%s", device->Device_Settings.buzzer_enable ? "enable" : "disable");
    CLI_Printf("\r\n AutoOff:\t%dm", device->Device_Settings.time_auto_off);
    CLI_Printf("\r\n Output V:\t%s", device->Device_Settings.Boost_mode == Boost_8V ? "8V" : "12V" );
    CLI_Printf("\r\n MAX Current:\t%dmA", device->Device_Settings.current_max );
    CLI_Printf("\r\n Min Vbat:\t%.2fV", (float )(device->Device_Settings.low_volt / 100.0) );

    return CLI_OK;
}

uint8_t get_charger(void){

    Device_Status_t* device =  Get_Device_Status();

    CLI_Printf("\r\n USB in:\t%.2fV", (float)(device->ChargeChip.Vbus / 1000.0));
    CLI_Printf("\r\n Power:\t\t%.2fW", (float)(device->Battery_Info.power / 1000.0));

    /**
    * Charging Status
    * 00 – Not Charging
    * 01 – Pre-charge ( < VBATLOWV)
    * 10 – Charging
    * 11 – Charge Termination Done
    * */
    CLI_Printf("\r\n CH status :\t");
    if (device->ChargeChip.charging_status == 0) {
        CLI_Printf("Not Charging");
    } else if (device->ChargeChip.charging_status == 1) {
        CLI_Printf("Pre-charge");
    } else if (device->ChargeChip.charging_status == 2) {
        CLI_Printf("Charging");
    } else if (device->ChargeChip.charging_status == 3) {
        CLI_Printf("Charge Done");
    }

    CLI_Printf("\r\n Charge done:\t%s", device->ChargeChip.charge_done ? "Yes" : "No");

    return CLI_OK;
}

uint8_t set_out(void){

    Power_DCDC();

    return CLI_OK;
}