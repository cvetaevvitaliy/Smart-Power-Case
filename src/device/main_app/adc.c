/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "adc.h"

#define V_25                        (1.43f)
#define SLOPE                       (0.0043f)
#define VREF                        (2.25f)
#define ADC_MAX_OUTPUT_VALUE        (4095.0f)

#define VBUS_R1                     (97600.0f)
#define VBUS_R2                     (33200.0f)
#define VOUT_R1                     (13300.0f)
#define VOUT_R2                     (13700.0f)

extern ADC_HandleTypeDef hadc1;
static uint32_t adc_raw_data[3]; /***  0 - Vbus , 1 - Vout, 2 - Temperature ***/

bool ADC_Init(void){
    HAL_ADCEx_Calibration_Start(&hadc1);
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &adc_raw_data, sizeof(adc_raw_data)/sizeof(uint32_t)) == HAL_OK)
        return true;
    else
        return false;
}

void ADC_Task(ADC_Voltage_Data_t *Data){

    static uint32_t adc_time = 0;
    if (HAL_GetTick() - adc_time > 350) {
        Data->Vout = ((float) adc_raw_data[0] * VREF / ADC_MAX_OUTPUT_VALUE) / (VBUS_R2 / (VBUS_R2 + VBUS_R1));
        Data->Vbus = ((float) adc_raw_data[1] * VREF / ADC_MAX_OUTPUT_VALUE) / (VOUT_R2 / (VOUT_R2 + VOUT_R1));
        Data->internal_temperature_stm = ((V_25 - ((float) adc_raw_data[2] / ADC_MAX_OUTPUT_VALUE * VREF)) / SLOPE) + 25;
        adc_time = HAL_GetTick();
    }

}

