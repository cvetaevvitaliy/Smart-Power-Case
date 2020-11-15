/*******************************************************
* Created by V.Cvetaev on 2019-10-26.
********************************************************/
#ifndef STM32_LTE_PROJECT_BQ2589X_CHARGER_H
#define STM32_LTE_PROJECT_BQ2589X_CHARGER_H
#include "bq2589x_reg.h"
#include "stm32f1xx_hal.h"

typedef enum bq2589x_vbus_type {
    BQ2589X_VBUS_NONE,
    BQ2589X_VBUS_USB_SDP,
    BQ2589X_VBUS_USB_CDP,
    BQ2589X_VBUS_USB_DCP,
    BQ2589X_VBUS_MAXC,
    BQ2589X_VBUS_UNKNOWN,
    BQ2589X_VBUS_NONSTAND,
    BQ2589X_VBUS_OTG,
    BQ2589X_VBUS_TYPE_NUM,
}bq2589x_vbus_type;

typedef enum bq2589x_part_no {
    BQ25890 = 0x03,
    BQ25892 = 0x00,
    BQ25895 = 0x07,
}bq2589x_part_no;


bq2589x_vbus_type bq2589x_get_vbus_type(void);
uint16_t bq2589x_enable_otg(void);
uint16_t bq2589x_disable_otg(void);
uint16_t bq2589x_set_otg_volt(uint16_t volt);
uint16_t bq2589x_set_otg_current(int curr);
uint16_t bq2589x_enable_charger(void);
uint16_t bq2589x_disable_charger(void);
uint16_t bq2589x_adc_start(bool oneshot);
uint16_t bq2589x_adc_stop(void);
uint16_t bq2589x_adc_read_battery_volt(void);
uint16_t bq2589x_adc_read_sys_volt(void);
uint16_t bq2589x_adc_read_vbus_volt(void);
uint16_t bq2589x_adc_read_temperature(void);
uint16_t bq2589x_adc_read_charge_current(void);
uint16_t bq2589x_set_charge_current(int16_t curr);
uint16_t bq2589x_set_term_current(int curr);
uint16_t bq2589x_set_prechg_current(int curr);
uint16_t bq2589x_set_chargevoltage(int volt);
uint16_t bq2589x_set_input_volt_limit(int volt);
uint16_t bq2589x_set_input_current_limit(int curr);
uint16_t bq2589x_set_vindpm_offset(int offset);
uint16_t bq2589x_get_charging_status(void);
void bq2589x_set_otg(int enable);
uint16_t bq2589x_set_watchdog_timer(uint8_t timeout);
uint16_t bq2589x_disable_watchdog_timer(void);
uint16_t bq2589x_reset_watchdog_timer(void);
uint16_t bq2589x_force_dpdm(void);
uint16_t bq2589x_reset_chip(void);
uint16_t bq2589x_enter_ship_mode(void);
uint16_t bq2589x_exit_ship_mode(void);
uint16_t bq2589x_enter_hiz_mode(void);
uint16_t bq2589x_exit_hiz_mode(void);
bool bq2589x_get_hiz_mode(uint8_t *state);
uint16_t bq2589x_pumpx_enable(int enable);
uint16_t bq2589x_pumpx_increase_volt(void);
uint16_t bq2589x_pumpx_increase_volt_done(void);
uint16_t bq2589x_pumpx_decrease_volt(void);
uint16_t bq2589x_pumpx_decrease_volt_done(void);
uint16_t bq2589x_force_ico(void);
uint16_t bq2589x_enable_ico(bool enable);
uint16_t bq2589x_check_force_ico_done(void);
uint16_t bq2589x_enable_term(bool enable);
uint16_t bq2589x_enable_auto_dpdm(bool enable);
uint16_t bq2589x_use_absolute_vindpm(bool enable);

uint16_t bq2589x_read_idpm_limit(void);
bool bq2589x_is_charge_done(void);
uint16_t bq2589x_init_device(void);
uint16_t bq2589x_detect_device(bq2589x_part_no* part_no, int16_t * revision);


uint16_t bq2589x_enable_max_charge(bool enable);


uint16_t bq2589x_set_IR_compensation_resistor(int volt);
uint16_t bq2589x_set_IR_compensation_voltage(int volt);
uint16_t bq2589x_start_charging(bool charger);
uint16_t bq2589x_set_bat_limit(int limit);




#endif //STM32_LTE_PROJECT_BQ2589X_CHARGER_H
