#include <stdbool.h>
#include "bq2589x_charger.h"


extern I2C_HandleTypeDef hi2c1;

static uint16_t bq2589x_read_byte(uint8_t *data, uint8_t reg)
{
  
  if(HAL_I2C_Mem_Read(&hi2c1,BQ25895_ADDR,reg,1,data,1,25)==HAL_OK)
  {
    return BQ25895_OK;//TI lib uses 1 as failed
  }
  else
  {
    return BQ25895_ERR;//TI lib uses 1 as failed
  }
  
}


static uint16_t bq2589x_write_byte(uint8_t reg, uint8_t data)
{
  
  if(HAL_I2C_Mem_Write(&hi2c1,BQ25895_ADDR,reg,1,&data,1,25)==HAL_OK)
  {
    return BQ25895_OK;//TI lib uses 1 as failed
  }
  else
  {
    return BQ25895_ERR;//TI lib uses 1 as failed
  }
}


static uint16_t bq2589x_update_bits(uint8_t reg, uint8_t mask, uint8_t data)
{
  uint16_t ret;
  uint8_t tmp;
  
  ret = bq2589x_read_byte(&tmp, reg);
  
  if (ret)
    return ret;
  
  tmp &= ~mask;
  tmp |= data & mask;
  
  return bq2589x_write_byte(reg, tmp);
}



/********************************************************************************************************************
 * 0: No Input
 * 1: USB Host SDP
 * 2: USB CDP (1.5A)
 * 3: USB DCP (3.25A)
 * 4: Adjustable High Voltage DCP (MaxCharge) (1.5A)
 * 5: Unknown Adapter (500mA)
 * 6: Non-Standard Adapter (1A/2A/2.1A/2.4A)
 * 7: OTG
 *
 * Note: Software current limit is reported in IINLIM register
 * *****************************************************************************************************************/
bq2589x_vbus_type bq2589x_get_vbus_type()
{
  uint8_t val = 0;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_0B);
  if (ret)
    return (BQ2589X_VBUS_UNKNOWN);
  val &= BQ2589X_VBUS_STAT_MASK;
  val >>= BQ2589X_VBUS_STAT_SHIFT;
  
  return (bq2589x_vbus_type)val;
}



/********************************************************************************************************************
 * Boost (OTG) Mode Configuration
 * 1 – OTG Enable (default)
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_otg()
{
  uint8_t val = BQ2589X_OTG_ENABLE << BQ2589X_OTG_CONFIG_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_03,
                             BQ2589X_OTG_CONFIG_MASK, val);
  
}

/********************************************************************************************************************
 * Boost (OTG) Mode Configuration
 * 0 – OTG Disable
 * *****************************************************************************************************************/
uint16_t bq2589x_disable_otg()
{
  uint8_t val = BQ2589X_OTG_DISABLE << BQ2589X_OTG_CONFIG_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_03,
                             BQ2589X_OTG_CONFIG_MASK, val);
  
}


/********************************************************************************************************************
 * Boost Mode Voltage Regulation
 * Offset: 4.55V
 * Range: 4.55V – 5.51V
 * Default: 5.126V (1001)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_otg_volt(uint16_t volt)
{
  uint8_t val = 0;
  
  if (volt < BQ2589X_BOOSTV_BASE)
    volt = BQ2589X_BOOSTV_BASE;
  if (volt > BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB)
    volt = BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB;
  
  val = ((volt - BQ2589X_BOOSTV_BASE) / BQ2589X_BOOSTV_LSB) << BQ2589X_BOOSTV_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_0A, BQ2589X_BOOSTV_MASK, val);
  
}

/********************************************************************************************************************
 * Boost Mode Voltage Regulation
 * Step: 200mA
 * Range: 500mA – 2400mA
 * *****************************************************************************************************************/
uint16_t bq2589x_set_otg_current(int curr)
{
  uint8_t temp;
  
  if (curr == 500)
    temp = BQ2589X_BOOST_LIM_500MA;
  else if (curr == 700)
    temp = BQ2589X_BOOST_LIM_700MA;
  else if (curr == 1100)
    temp = BQ2589X_BOOST_LIM_1100MA;
  else if (curr == 1600)
    temp = BQ2589X_BOOST_LIM_1600MA;
  else if (curr == 1800)
    temp = BQ2589X_BOOST_LIM_1800MA;
  else if (curr == 2100)
    temp = BQ2589X_BOOST_LIM_2100MA;
  else if (curr == 2400)
    temp = BQ2589X_BOOST_LIM_2400MA;
  else
    temp = BQ2589X_BOOST_LIM_1300MA;
  
  return bq2589x_update_bits(BQ2589X_REG_0A, BQ2589X_BOOST_LIM_MASK, temp << BQ2589X_BOOST_LIM_SHIFT);
}


/********************************************************************************************************************
 * Charge Enable Configuration
 * 0 - Charge Disable
 * 1- Charge Enable (default)
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_charger()
{
  uint8_t val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;
  return bq2589x_update_bits(BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
}

uint16_t bq2589x_disable_charger()
{
  uint8_t val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;
  return bq2589x_update_bits(BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
}

uint16_t bq2589x_start_charging(bool charger)
{
  uint8_t val;
  if (charger)
    val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;
  else
    val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;

  return bq2589x_update_bits(BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
}



/********************************************************************************************************************
 * Minimum System Voltage Limit    example: set lim 3500  >> val = 3500 - 3000 / 100 = 3470 Voltage
 * Range 3.0V-3.7V
 * Default: 3.5V (101)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_bat_limit(int limit)
{
  uint8_t val = (limit - BQ2589X_SYS_MINV_BASE) / BQ2589X_SYS_MINV_LSB;
  return bq2589x_update_bits(BQ2589X_REG_03, BQ2589X_SYS_MINV_MASK, val << BQ2589X_SYS_MINV_SHIFT);
}



/********************************************************************************************************************
 * ADC Conversion Start Control
 * 0 – ADC conversion not active (default).
 * 1 – Start ADC Conversion
 * This bit is read-only when CONV_RATE = 1. The bit stays high during
 * ADC conversion and during input source detection.
 *
 * ADC Conversion Rate Selection
 * 0 – Oneshot ADC conversion (default)
 * 1 – Start 1s Continuous Conversion
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_start(bool oneshot)
{
  uint8_t val;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_02);
  if (ret) 
  {
    return ret;
  }
  
  if (((val & BQ2589X_CONV_RATE_MASK) >> BQ2589X_CONV_RATE_SHIFT) == BQ2589X_ADC_CONTINUE_ENABLE)
    return BQ25895_OK; /*is doing continuous scan*/
  if (oneshot)
    ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_CONV_START_MASK, BQ2589X_CONV_START << BQ2589X_CONV_START_SHIFT);
  else
    ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK,  BQ2589X_ADC_CONTINUE_ENABLE << BQ2589X_CONV_RATE_SHIFT);
  return ret;
}



/********************************************************************************************************************
 * ADC Conversion Stop Control
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_stop()
{
  return bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_DISABLE << BQ2589X_CONV_RATE_SHIFT);
}



/********************************************************************************************************************
 * ADC conversion of Battery Voltage (VBAT)
 * Offset: 2.304V
 * Range: 2.304V (0000000) – 4.848V (1111111)
 * Default: 2.304V (0000000)
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_read_battery_volt()
{
  uint8_t val;
  uint16_t volt;
  uint16_t ret;
  ret = bq2589x_read_byte(&val, BQ2589X_REG_0E);
  if (ret) {
    return ret;
  } else{
    volt = (BQ2589X_BATV_BASE + ((val & BQ2589X_BATV_MASK) >> BQ2589X_BATV_SHIFT) * BQ2589X_BATV_LSB) ;
    return volt;
  }
}


/********************************************************************************************************************
 * ADDC conversion of System Voltage (VSYS)
 * Offset: 2.304V
 * Range: 2.304V (0000000) – 4.848V (1111111)
 * Default: 2.304V (0000000)
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_read_sys_volt()
{
  uint8_t val;
  uint16_t volt;
  uint16_t ret;
  ret = bq2589x_read_byte(&val, BQ2589X_REG_0F);
  if (ret) {
    return ret;
  } else{
    volt = BQ2589X_SYSV_BASE + ((val & BQ2589X_SYSV_MASK) >> BQ2589X_SYSV_SHIFT) * BQ2589X_SYSV_LSB ;
    return volt;
  }
}


/********************************************************************************************************************
 * ADC conversion of VBUS voltage (VBUS)
 * Offset: 2.6V
 * Range 2.6V (0000000) – 15.3V (1111111)
 * Default: 2.6V (0000000)
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_read_vbus_volt()
{
  uint8_t val;
  uint16_t volt;
  uint16_t ret;
  ret = bq2589x_read_byte(&val, BQ2589X_REG_11);
  if (ret) {
    return ret;
  } else{
    volt = BQ2589X_VBUSV_BASE + ((val & BQ2589X_VBUSV_MASK) >> BQ2589X_VBUSV_SHIFT) * BQ2589X_VBUSV_LSB ;
    return volt;
  }
}


/********************************************************************************************************************
 * ADC conversion of TS Voltage (TS) as percentage of REGN
 *
 * Temperature Profile Cold/Hot
 *
 * Offset: 21%
 * Range 21% (0000000) – 80% (1111111)
 * Default: 21% (0000000)
 * *****************************************************************************************************************/
uint16_t bq2589x_adc_read_temperature()
{
  uint8_t val;
  uint16_t temp;
  uint16_t ret;
  ret = bq2589x_read_byte(&val, BQ2589X_REG_10);
  if (ret) {
    return ret;
  } else{
    temp = BQ2589X_TSPCT_BASE + ((val & BQ2589X_TSPCT_MASK) >> BQ2589X_TSPCT_SHIFT) * BQ2589X_TSPCT_LSB ;
    return temp;
  }
}


/********************************************************************************************************************
 * ADC conversion of Charge Current (IBAT) when VBAT >
 * VBATSHORT
 * Offset: 0mA
 * Range 0mA (0000000) – 6350mA (1111111)
 * Default: 0mA (0000000)
 * Note:
 * This register returns 0000000 for VBAT < VBATSHORT
* *****************************************************************************************************************/
uint16_t bq2589x_adc_read_charge_current()
{
  uint8_t val;
  uint16_t volt;
  uint16_t ret;
  ret = bq2589x_read_byte(&val, BQ2589X_REG_12);
  if (ret) {
    return ret;
  } else{
    volt = (int)(BQ2589X_ICHGR_BASE + ((val & BQ2589X_ICHGR_MASK) >> BQ2589X_ICHGR_SHIFT) * BQ2589X_ICHGR_LSB) ;
    return volt;
  }
}



/********************************************************************************************************************
 * Fast Charge Current Limit
 * Offset: 0mA
 * Range: 0mA (0000000) – 5056mA (1001111)
 * Default: 2048mA (0100000)
 * Note:
 * ICHG=000000 (0mA) disables charge
 * ICHG > 1001111 (5056mA) is clamped to register value
 * 1001111 (5056mA)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_charge_current(int16_t curr)
{
  uint8_t ichg;
  ichg = (curr - BQ2589X_ICHG_BASE)/BQ2589X_ICHG_LSB;
  return bq2589x_update_bits(BQ2589X_REG_04, BQ2589X_ICHG_MASK, ichg << BQ2589X_ICHG_SHIFT);
  
}


/********************************************************************************************************************
 * Current pulse control Enable
 * 0 - Disable Current pulse control (default)
 * 1- Enable Current pulse control (PUMPX_UP and PUMPX_DN)
 *
 * 8.2.11 Current Pulse Control Protocol
The device provides the control to generate the VBUS current pulse protocol to communicate with adjustable
high voltage adapter in order to signal adapter to increase or decrease output voltage. To enable the interface,
the EN_PUMPX bit must be set. Then the host can select the increase/decrease voltage pulse by setting one of
the PUMPX_UP or PUMPX_DN bit (but not both) to start the VBUS current pulse sequence. During the current
pulse sequence, the PUMPX_UP and PUMPX_DN bits are set to indicate pulse sequence is in progress and the
device pulses the input current limit between current limit set forth by IINLIM or IDPM_LIM register and the
100mA current limit (IINDPM100_ACC). When the pulse sequence is completed, the input current limit is returned to
value set by IINLIM or IDPM_LIM register and the PUMPX_UP or PUMPX_DN bit is cleared. In addition, the
EN_PUMPX can be cleared during the current pulse sequence to terminate the sequence and force charger to
return to input current limit as set forth by the IINLIM or IDPM_LIM register immediately. When EN_PUMPX bit is
low, write to PUMPX_UP and PUMPX_DN bit would be ignored and have no effect on VBUS current limit.
 * *****************************************************************************************************************/
uint16_t bq2589x_pumpx_enable(int enable)
{
  uint8_t val;
  uint16_t ret;

  if (enable)
    val = BQ2589X_PUMPX_ENABLE << BQ2589X_EN_PUMPX_SHIFT;
  else
    val = BQ2589X_PUMPX_DISABLE << BQ2589X_EN_PUMPX_SHIFT;

  ret = bq2589x_update_bits(BQ2589X_REG_04, BQ2589X_EN_PUMPX_MASK, val);

  return ret;
}



/********************************************************************************************************************
 *Termination Current Limit
 * Offset: 64mA
 * Range: 64mA – 1024mA
 * Default: 256mA (0011)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_term_current(int curr)
{
  uint8_t iterm;
  
  iterm = (curr - BQ2589X_ITERM_BASE) / BQ2589X_ITERM_LSB;
  
  return bq2589x_update_bits(BQ2589X_REG_05, BQ2589X_ITERM_MASK, iterm << BQ2589X_ITERM_SHIFT);
}



/********************************************************************************************************************
 * Precharge Current Limit
 * Offset: 64mA
 * Range: 64mA – 1024mA
 * Default: 128mA (0001)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_prechg_current(int curr)
{
  uint8_t iprechg;
  
  iprechg = (curr - BQ2589X_IPRECHG_BASE) / BQ2589X_IPRECHG_LSB;
  
  return bq2589x_update_bits(BQ2589X_REG_05, BQ2589X_IPRECHG_MASK, iprechg << BQ2589X_IPRECHG_SHIFT);
}





/********************************************************************************************************************
 * Charge Voltage Limit
 * Offset: 3.840V
 * Range: 3.840V – 4.608V (110000)
 * Default: 4.208V (010111)
 * Note:
 * VREG > 110000 (4.608V) is clamped to register value
 * 110000 (4.608V)
 * *****************************************************************************************************************/
uint16_t bq2589x_set_chargevoltage(int volt)
{
  uint8_t val;

  val = (volt - BQ2589X_VREG_BASE)/BQ2589X_VREG_LSB;
  return bq2589x_update_bits(BQ2589X_REG_06, BQ2589X_VREG_MASK, val << BQ2589X_VREG_SHIFT);
}



/********************************************************************************************************************
 * Absolute VINDPM Threshold
 * Offset: 2.6V
 * Range: 3.9V (0001101) – 15.3V (1111111)
 * Default: 4.4V (0010010)
 * Note:
 * Value < 0001101 is clamped to 3.9V (0001101)
 * Register is read only when FORCE_VINDPM=0 and can
 * be written by internal control based on relative VINDPM
 * threshold setting
 * Register can be read/write when FORCE_VINDPM = 1
 * *****************************************************************************************************************/
uint16_t bq2589x_set_input_volt_limit(int volt)
{
  uint8_t val;
  val = (volt - BQ2589X_VINDPM_BASE) / BQ2589X_VINDPM_LSB;
  return bq2589x_update_bits(BQ2589X_REG_0D, BQ2589X_VINDPM_MASK, val << BQ2589X_VINDPM_SHIFT);
}


/********************************************************************************************************************
 * Input Current Limit
 * Offset: 100mA
 * Range: 100mA (000000) – 3.25A (111111)
 * Default:0001000 (500mA)
 *
 *
 *
 * (Actual input current limit is the lower of I2C or ILIM pin)
 * IINLIM bits are changed automaticallly after input source
 * type detection is completed
 * USB Host SDP w/ OTG=Hi (USB500) = 500mA
 * USB Host SDP w/ OTG=Lo (USB100) = 500mA
 * USB CDP = 1.5A
 * USB DCP = 3.25A
 * Adjustable High Voltage (MaxCharge) DCP = 1.5A
 * Unknown Adapter = 500mA
 * Non-Standard Adapter = 1A/2A/2.1A/2.4A
 * *****************************************************************************************************************/
uint16_t bq2589x_set_input_current_limit(int curr)
{
  uint8_t val;
  
  val = (curr - BQ2589X_IINLIM_BASE) / BQ2589X_IINLIM_LSB;
  return bq2589x_update_bits(BQ2589X_REG_00, BQ2589X_IINLIM_MASK, val << BQ2589X_IINLIM_SHIFT);
}


/********************************************************************************************************************
 * Input Voltage Limit Offset
 * Default: 600mV (00110)
 * Range: 0mV – 3100mV
 * Minimum VINDPM threshold is clamped at 3.9V
 * Maximum VINDPM threshold is clamped at 15.3V
 * When VBUS at noLoad is ≤ 6V, the VINDPM_OS is used
 * to calculate VINDPM threhold
 * When VBUS at noLoad is > 6V, the VINDPM_OS multiple
 * by 2 is used to calculate VINDPM threshold.
 * *****************************************************************************************************************/
uint16_t bq2589x_set_vindpm_offset(int offset)
{
  uint8_t val;
  
  val = (offset - BQ2589X_VINDPMOS_BASE)/BQ2589X_VINDPMOS_LSB;
  return bq2589x_update_bits(BQ2589X_REG_01, BQ2589X_VINDPMOS_MASK, val << BQ2589X_VINDPMOS_SHIFT);
}



/********************************************************************************************************************
 * Charging Status
 * 00 – Not Charging
 * 01 – Pre-charge ( < VBATLOWV)
 * 10 – Fast Charging
 * 11 – Charge Termination Done
 * *****************************************************************************************************************/
uint16_t bq2589x_get_charging_status()
{
  uint8_t val = 0;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_0B);
  if (ret) {
    return 0x04; //Error
  }
  val &= BQ2589X_CHRG_STAT_MASK;
  val >>= BQ2589X_CHRG_STAT_SHIFT;
  return val;
}



void bq2589x_set_otg(int enable)
{
  uint16_t ret;
  
  if (enable) {
    ret = bq2589x_enable_otg();
    if (ret) {
      return;
    }
  } else{
    ret = bq2589x_disable_otg();
  }
}




/********************************************************************************************************************
 * I2C Watchdog Timer Setting
 * 40s (default)
 * 80s
 * 160s
 * *****************************************************************************************************************/
uint16_t bq2589x_set_watchdog_timer(uint8_t timeout)
{
  return bq2589x_update_bits(BQ2589X_REG_07, BQ2589X_WDT_MASK, (uint8_t)((timeout - BQ2589X_WDT_BASE) / BQ2589X_WDT_LSB) << BQ2589X_WDT_SHIFT);
}


/********************************************************************************************************************
 * I2C Watchdog Timer Setting
 * Disable watchdog timer
 * *****************************************************************************************************************/
uint16_t bq2589x_disable_watchdog_timer()
{
  uint8_t val = BQ2589X_WDT_DISABLE << BQ2589X_WDT_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_07, BQ2589X_WDT_MASK, val);
}



/********************************************************************************************************************
 * I2C Watchdog Timer Reset
 * Reset (Back to 0 after timer reset)
 * *****************************************************************************************************************/
uint16_t bq2589x_reset_watchdog_timer()
{
  uint8_t val = BQ2589X_WDT_RESET << BQ2589X_WDT_RESET_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_03, BQ2589X_WDT_RESET_MASK, val);
}



/********************************************************************************************************************
 * Force D+/D- Detection
 * 0 – Not in D+/D- or PSEL detection (default)
 * 1 – Force D+/D- detection
 * 1 or 0 see define BQ2589X_FORCE_DPDM
 * *****************************************************************************************************************/
uint16_t bq2589x_force_dpdm()
{
  uint16_t ret;
  uint8_t val = BQ2589X_FORCE_DPDM << BQ2589X_FORCE_DPDM_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_FORCE_DPDM_MASK, val);
  if (ret)
    return ret;
  HAL_Delay (20); /*TODO: how much time needed to finish dpdm detect*/
  return BQ25895_OK;
  
}



/********************************************************************************************************************
 * Register Reset
 * 0 – Keep current register setting (default)
 * 1 – Reset to default register value and reset safety timer
 * Note:
 * Reset to 0 after register reset is completed
 *
 * 1 or 0 see define BQ2589X_RESET
 * *****************************************************************************************************************/
uint16_t bq2589x_reset_chip()
{
  uint16_t ret;
  uint8_t val = BQ2589X_RESET << BQ2589X_RESET_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_14, BQ2589X_RESET_MASK, val);
  return ret;
}



/********************************************************************************************************************
 * Force BATFET off to enable ship mode
 * 0 – Allow BATFET turn on (default)   define BQ2589X_BATFET_ON
 * 1 – Force BATFET off                 define BQ2589X_BATFET_OFF
 *
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_enter_ship_mode()
{
  uint16_t ret;
  uint8_t val = BQ2589X_BATFET_OFF << BQ2589X_BATFET_DIS_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
  return ret;
  
}



/********************************************************************************************************************
 * Force BATFET off to enable ship mode
 * 0 – Allow BATFET turn on (default)       define BQ2589X_BATFET_ON
 * 1 – Force BATFET off                     define BQ2589X_BATFET_OFF
 *
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_exit_ship_mode()
{
  uint16_t ret;
  uint8_t val = BQ2589X_BATFET_ON << BQ2589X_BATFET_DIS_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
  return ret;
  
}



/********************************************************************************************************************
 * Enable HIZ Mode
 * 0 – Disable (default)
 * 1 – Enable
 *
 *
 * define BQ2589X_HIZ_ENABLE        1
 * define BQ2589X_HIZ_DISABLE       0
 * *****************************************************************************************************************/
uint16_t bq2589x_enter_hiz_mode()
{
  uint8_t val = BQ2589X_HIZ_ENABLE << BQ2589X_ENHIZ_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);
  
}

/********************************************************************************************************************
 * Disable HIZ Mode
 * 0 – Disable (default)
 * 1 – Enable
 *
 *
 * define BQ2589X_HIZ_ENABLE        1
 * define BQ2589X_HIZ_DISABLE       0
 * *****************************************************************************************************************/
uint16_t bq2589x_exit_hiz_mode()
{
  
  uint8_t val = BQ2589X_HIZ_DISABLE << BQ2589X_ENHIZ_SHIFT;
  
  return bq2589x_update_bits(BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);
  
}



/********************************************************************************************************************
 * Get  HIZ Mode
 * return 0 – Disable
 * return 1 – Enable
 * *****************************************************************************************************************/
bool bq2589x_get_hiz_mode(uint8_t *state)
{
  uint8_t val;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_00);
  if (ret)
    return ret;
  *state = (val & BQ2589X_ENHIZ_MASK) >> BQ2589X_ENHIZ_SHIFT;
  
  return BQ25895_OK;
}



/********************************************************************************************************************
 * Current pulse control voltage up enable
 * 0 – Disable (default)
 * 1 – Enable
 *
 *
 * define BQ2589X_PUMPX_UP 1
 * Note:
 * This bit is can only be set when EN_PUMPX bit is set and returns to 0
 * after current pulse control sequence is completed
 *
 * 1 or 0 see define BQ2589X_PUMPX_UP
 * *****************************************************************************************************************/
uint16_t bq2589x_pumpx_increase_volt()
{
  uint8_t val;
  uint16_t ret;

  val = BQ2589X_PUMPX_UP << BQ2589X_PUMPX_UP_SHIFT;

  ret = bq2589x_update_bits(BQ2589X_REG_09, BQ2589X_PUMPX_UP_MASK, val);

  return ret;

}


/********************************************************************************************************************
 * Current pulse control voltage up done
 *
 * return  - 1          (not finished)
 * return  - 0          (pumpx up finished)
 *
 * *****************************************************************************************************************/

uint16_t bq2589x_pumpx_increase_volt_done()
{
  uint8_t val;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_09);
  if (ret)
    return ret;
  
  if (val & BQ2589X_PUMPX_UP_MASK)
    return BQ25895_ERR;   /* not finished*/
  else
    return BQ25895_OK;   /* pumpx up finished*/
  
}


/********************************************************************************************************************
 * Current pulse control voltage down enable
 * 0 – Disable (default)
 * 1 – Enable
 *
 * define  BQ2589X_PUMPX_DOWN 1
 * Note:
 * This bit is can only be set when EN_PUMPX bit is set and returns to 0
 * after current pulse control sequence is completed
 * *****************************************************************************************************************/
uint16_t bq2589x_pumpx_decrease_volt()
{
  uint8_t val;
    uint16_t ret;
  
  val = BQ2589X_PUMPX_DOWN << BQ2589X_PUMPX_DOWN_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_09, BQ2589X_PUMPX_DOWN_MASK, val);
  
  return ret;
  
}



/********************************************************************************************************************
 * Current pulse control voltage down done
 *
 * return  - 1      (not finished)
 * return  - 0      (pumpx up finished)
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_pumpx_decrease_volt_done()
{
  uint8_t val;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_09);
  if (ret)
    return ret;
  
  if (val & BQ2589X_PUMPX_DOWN_MASK)
    return BQ25895_ERR;   /* not finished*/
  else
    return BQ25895_OK;   /* pumpx down finished*/
  
}




/********************************************************************************************************************
 * Force Start Input Current Optimizer (ICO)
 * 0 – Do not force ICO (default)
 * 1 – Force ICO
 *
 * define BQ2589X_FORCE_ICO  =  1
 * Note:
 * This bit is can only be set only and always returns to 0 after ICO starts
 * *****************************************************************************************************************/
uint16_t bq2589x_force_ico()
{
  uint8_t val;
  uint16_t ret;
  
  val = BQ2589X_FORCE_ICO << BQ2589X_FORCE_ICO_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_09, BQ2589X_FORCE_ICO_MASK, val);
  
  return ret;
}



/********************************************************************************************************************
 * Check Force Input Current Optimizer (ICO)
 *
 * return  - 1  (finished)
 * return  - 0  (in progress)
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_check_force_ico_done()
{
  uint8_t val;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_14);
  if (ret)
    return ret;
  
  if (val & BQ2589X_ICO_OPTIMIZED_MASK)
    return BQ25895_ERR;  /*finished*/
  else
    return BQ25895_OK;   /* in progress*/
}



/********************************************************************************************************************
 * Charging Termination Enable
 * 0 – Disable
 * 1 – Enable (default)
 *
 * define BQ2589X_TERM_ENABLE            1
 * define BQ2589X_TERM_DISABLE           0
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_term(bool enable)
{
  uint8_t val;
  uint16_t ret;
  
  if (enable)
    val = BQ2589X_TERM_ENABLE << BQ2589X_EN_TERM_SHIFT;
  else
    val = BQ2589X_TERM_DISABLE << BQ2589X_EN_TERM_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_07, BQ2589X_EN_TERM_MASK, val);
  
  return ret;
}



/********************************************************************************************************************
 * Automatic D+/D- Detection Enable
 * 0 –Disable D+/D- or PSEL detection when VBUS is plugged-in
 * 1 –Enable D+/D- or PEL detection when VBUS is plugged-in (default)
 *
 * define BQ2589X_AUTO_DPDM_ENABLE          1
 * define BQ2589X_AUTO_DPDM_DISABLE         0
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_auto_dpdm(bool enable)
{
  uint8_t val;
  uint16_t ret;
  
  if (enable)
    val = BQ2589X_AUTO_DPDM_ENABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;
  else
    val = BQ2589X_AUTO_DPDM_DISABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_AUTO_DPDM_EN_MASK, val);
  
  return ret;
  
}

/********************************************************************************************************************
 * Absolute VINDPM Threshold
 * Offset: 2.6V
 * Return r ange: 3.9V (0001101) – 15.3V (1111111)
 * Default: 4.4V (0010010)
 * Note:
 * Value < 0001101 is clamped to 3.9V (0001101)
 * Register is read only when FORCE_VINDPM=0 and can
 * be written by internal control based on relative VINDPM
 * threshold setting
 * Register can be read/write when FORCE_VINDPM = 1
 *
 *
 * VINDPM Threshold Setting Method
 * 0 – Run Relative VINDPM Threshold (default)
 * 1 – Run Absolute VINDPM Threshold
 *
 * define BQ2589X_FORCE_VINDPM_ENABLE               1
 * define BQ2589X_FORCE_VINDPM_DISABLE              0
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_use_absolute_vindpm(bool enable)
{
  uint8_t val;
  uint16_t ret;
  
  if (enable)
    val = BQ2589X_FORCE_VINDPM_ENABLE << BQ2589X_FORCE_VINDPM_SHIFT;
  else
    val = BQ2589X_FORCE_VINDPM_DISABLE << BQ2589X_FORCE_VINDPM_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_0D, BQ2589X_FORCE_VINDPM_MASK, val);
  
  return ret;
  
}


/********************************************************************************************************************
 * Input Current Optimizer (ICO) Enable
 * 0 – Disable ICO Algorithm
 * 1 – Enable ICO Algorithm (default)
 *
 * define BQ2589X_ICO_ENABLE        1
 * define BQ2589X_ICO_DISABLE       0
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_ico(bool enable)
{
  uint8_t val;
  uint16_t ret;
  
  if (enable)
    val = BQ2589X_ICO_ENABLE << BQ2589X_ICOEN_SHIFT;
  else
    val = BQ2589X_ICO_DISABLE << BQ2589X_ICOEN_SHIFT;
  
  ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_ICOEN_MASK, val);
  
  return ret;
  
}




/********************************************************************************************************************
 * Input Current Limit in effect while Input Current Optimizer
 * (ICO) is enabled
 * Offset: 100mA (default)
 * Range 100mA (0000000) – 3.25mA (1111111)
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_read_idpm_limit()
{
  uint8_t val;
  uint16_t curr;
  uint16_t ret;
  
  ret = bq2589x_read_byte(&val, BQ2589X_REG_13);
  if (ret) {
    return ret;
  } else{
    curr = BQ2589X_IDPM_LIM_BASE + ((val & BQ2589X_IDPM_LIM_MASK) >> BQ2589X_IDPM_LIM_SHIFT) * BQ2589X_IDPM_LIM_LSB ;
    return curr;
  }
}



/********************************************************************************************************************
 * Charging Status
 * 0 – Not Charging
 * 1 – Pre-charge ( < VBATLOWV)
 * 2 – Fast Charging
 * 3 – Charge Termination Done
 * *****************************************************************************************************************/
bool bq2589x_is_charge_done()
{
  uint8_t val;
  
  bq2589x_read_byte(&val, BQ2589X_REG_0B);
  val &= BQ2589X_CHRG_STAT_MASK;
  val >>= BQ2589X_CHRG_STAT_SHIFT;
  
  return (bool)(val == BQ2589X_CHRG_STAT_CHGDONE);
}



/********************************************************************************************************************
 *                                              Common initialization
 *
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_init_device()
{
    bq2589x_part_no BQ25895_type;
    int16_t rev;

    if (bq2589x_detect_device(&BQ25895_type, &rev) == BQ25895_OK) {
        uint16_t ret;
        /*common initialization*/

        bq2589x_disable_watchdog_timer();

        ret = bq2589x_set_charge_current(2560); //2.5A

        return true;
    } else
        return false;

}

/********************************************************************************************************************
 *                                          Detect device and part number
 *   BQ25890 = 0x03
 *   BQ25892 = 0x00
 *   BQ25895 = 0x07
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_detect_device(bq2589x_part_no* part_no, int16_t * revision)
{
  uint8_t data;
  if (bq2589x_read_byte(&data, BQ2589X_REG_14) == 0) 
  {
    *part_no  = (bq2589x_part_no)((data & BQ2589X_PN_MASK) >> BQ2589X_PN_SHIFT);
    *revision = (data & BQ2589X_DEV_REV_MASK) >> BQ2589X_DEV_REV_SHIFT;
    return BQ25895_OK;
  }
  return BQ25895_ERR;
}



/********************************************************************************************************************
 * High Voltage DCP Enable
 * 0 – Disable HVDCP handshake
 * 1 – Enable HVDCP handshake (default)
 *
 * MaxCharge Adapter Enable
 * 0 – Disable MaxCharge handshake
 * 1 – Enable MaxCharge handshake (default)
 *
 * define  BQ2589X_HVDCP_ENABLE             1
 * define  BQ2589X_MAXC_ENABLE              1
 * define  BQ2589X_MAXC_DISABLE             0
 * define  BQ2589X_MAXC_DISABLE             0
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_enable_max_charge(bool enable)
{
  uint8_t val;
  uint8_t val1;
  uint16_t ret;
  
  if (enable)
  {
    val =  BQ2589X_HVDCP_ENABLE  << BQ2589X_HVDCPEN_SHIFT;
    val1 = BQ2589X_MAXC_ENABLE   << BQ2589X_MAXCEN_SHIFT;
  }
  else
  {
    val =  BQ2589X_HVDCP_DISABLE  << BQ2589X_HVDCPEN_SHIFT;
    val1 = BQ2589X_MAXC_DISABLE << BQ2589X_MAXCEN_SHIFT;
  }
  
  ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_HVDCPEN_MASK, val);
  ret = bq2589x_update_bits(BQ2589X_REG_02, BQ2589X_MAXCEN_MASK, val1);
  
  return ret;
}

/********************************************************************************************************************
 * IR Compensation Resistor Setting
 * Range: 0 – 140mΩ
 * Default: 0Ω (000) (i.e. Disable IRComp)
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_set_IR_compensation_resistor(int milliohm)
{
  uint8_t val;

  val = (milliohm - BQ2589X_BAT_COMP_BASE)/BQ2589X_BAT_COMP_LSB;
  return bq2589x_update_bits(BQ2589X_REG_08, BQ2589X_BAT_COMP_MASK, val << BQ2589X_BAT_COMP_SHIFT);
}

/********************************************************************************************************************
 * IR Compensation Voltage Clamp
 * above VREG (REG06[7:2])
 * Offset: 0mV
 * Range: 0-224mV
 * Default: 0mV (000)
 *
 * *****************************************************************************************************************/
uint16_t bq2589x_set_IR_compensation_voltage(int millivolt)
{
  uint8_t val;

  val = (millivolt - BQ2589X_VCLAMP_BASE)/BQ2589X_VCLAMP_LSB;
  return bq2589x_update_bits(BQ2589X_REG_08, BQ2589X_VCLAMP_MASK, val << BQ2589X_VCLAMP_SHIFT);

}