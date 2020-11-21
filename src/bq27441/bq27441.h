/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_BQ27441_H
#define FATSHARK_POWER_V1_BQ27441_H
#include "bq27441_definitions.h"
#include "main_app.h"

#define BQ72441_I2C_TIMEOUT 20000

// Parameters for the current() function, to specify which current to read
typedef enum {
    AVG,  // Average Current (DEFAULT)
    STBY, // Standby Current
    MAX   // Max Current
} current_measure;

// Parameters for the capacity() function, to specify which capacity to read
typedef enum {
    REMAIN,     // Remaining Capacity (DEFAULT)
    FULL,       // Full Capacity
    AVAIL,      // Available Capacity
    AVAIL_FULL, // Full Available Capacity
    REMAIN_F,   // Remaining Capacity Filtered
    REMAIN_UF,  // Remaining Capacity Unfiltered
    FULL_F,     // Full Capacity Filtered
    FULL_UF,    // Full Capacity Unfiltered
    DESIGN,     // Design Capacity
    TRUE_REMAIN //
} capacity_measure;

// Parameters for the soc() function
typedef enum {
    FILTERED,  // State of Charge Filtered (DEFAULT)
    UNFILTERED // State of Charge Unfiltered
} soc_measure;

// Parameters for the soh() function
typedef enum {
    PERCENT,  // State of Health Percentage (DEFAULT)
    SOH_STAT  // State of Health Status Bits
} soh_measure;

// Parameters for the temperature function
typedef enum {
    BATTERY,      // Battery Temperature (DEFAULT)
    INTERNAL_TEMP // Internal IC Temperature
} temp_measure;

// Parameters for the setGPOUTFunction() funciton
typedef enum {
    SOC_INT, // Set GPOUT to SOC_INT functionality
    BAT_LOW  // Set GPOUT to BAT_LOW functionality
} gpout_function;

/*****************************************************************************
 ************************** Initialization Functions *************************
 *****************************************************************************/

bool BQ27441_init (void);

bool BQ27441_setCapacity (uint16_t capacity);
bool BQ27441_setHibernateCurrent(uint16_t current_mA);
bool BQ27441_setDesignEnergy (uint16_t energy);
bool BQ27441_setTerminateVoltageMin (uint16_t voltage);
bool BQ27441_setChargeVChgTermination(uint16_t voltage);
bool BQ27441_setTaperRateTime (uint16_t rate);
bool BQ27441_setTaperRateVoltage(uint16_t voltage);

/*****************************************************************************
 ********************** Battery Characteristics Functions ********************
 *****************************************************************************/

uint16_t BQ27441_voltage (void);
int16_t BQ27441_current (current_measure type);
uint16_t BQ27441_capacity (capacity_measure type);
int16_t BQ27441_power (void);
uint16_t BQ27441_soc (soc_measure type);
uint8_t BQ27441_soh (soh_measure type);
uint16_t BQ27441_temperature (temp_measure type);

/*****************************************************************************
 ************************** GPOUT Control Functions **************************
 *****************************************************************************/

bool BQ27441_GPOUTPolarity (void);
bool BQ27441_setGPOUTPolarity (bool activeHigh);
bool BQ27441_GPOUTFunction (void);
bool BQ27441_setSLEEPenable(bool enable);
bool BQ27441_setGPOUTFunction (gpout_function function);
uint8_t BQ27441_SOC1SetThreshold (void);
uint8_t BQ27441_SOC1ClearThreshold (void);
bool BQ27441_set_BI_PU_EN(bool detect_bat_pin_enable);
bool BQ27441_setSOC1Thresholds (uint8_t set, uint8_t clear);
uint8_t BQ27441_SOCFSetThreshold (void);
uint8_t BQ27441_SOCFClearThreshold (void);
bool BQ27441_setSOCFThresholds (uint8_t set, uint8_t clear);
bool BQ27441_socFlag (void);
bool BQ27441_socfFlag (void);
bool BQ27441_itporFlag (void);
bool BQ27441_initComp(void);
bool BQ27441_fcFlag (void);
bool BQ27441_chgFlag (void);
bool BQ27441_dsgFlag (void);
uint8_t BQ27441_sociDelta (void);
bool BQ27441_setSOCIDelta (uint8_t delta);
bool BQ27441_pulseGPOUT (void);

/*****************************************************************************
 *************************** Control Sub-Commands ****************************
 *****************************************************************************/

uint16_t BQ27441_deviceType (void);
bool BQ27441_enterConfig (bool userControl);
bool BQ27441_exitConfig (bool resim);
uint16_t BQ27441_flags (void);
uint16_t BQ27441_status (void);

bool BQ27441_SET_HIBERNATE(void);
bool BQ27441_CLEAR_HIBERNATE(void);

void BQ27441_Full_Reset(void);

#endif //FATSHARK_POWER_V1_BQ27441_H