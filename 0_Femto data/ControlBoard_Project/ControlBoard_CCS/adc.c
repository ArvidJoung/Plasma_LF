/*
 * adc.c
 *
 *  Created on: 2014. 4. 25.
 *      Author: lastdkht
 */

#include "DSP280x_Device.h"     // Headerfile Include File
#include "DSP280x_Examples.h"   // Examples Include File


#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 12.5MHz/(1*2)   = 6.25MHz
                         //    for 60 MHz devices: ADC module clk = 7.5MHz/(1*2)     = 3.75MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                          = 16 ADC clocks

#define MAX_ADC_BUF_SIZE	20

static Uint16 g_Adc_Val[8][MAX_ADC_BUF_SIZE];
static Uint16 g_nAdc_Val_Index = 0;

void Init_Adc_280x ()
{
    InitAdc();  // For this example, init the ADC

    // Specific ADC setup for this example:
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x7;  // convert and store in 8 results registers
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;
    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;
    AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x7;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run

    DELAY_US (10);

    // Start SEQ1
    AdcRegs.ADCTRL2.all = 0x2000;
}

void Run_Adc_280x ()
{
    while (AdcRegs.ADCST.bit.INT_SEQ1== 0) {} // Wait for interrupt
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;

    g_Adc_Val[0][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT0 >> 4);
    g_Adc_Val[1][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT1 >> 4);
    g_Adc_Val[2][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT2 >> 4);
    g_Adc_Val[3][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT3 >> 4);
    g_Adc_Val[4][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT4 >> 4);
    g_Adc_Val[5][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT5 >> 4);
    g_Adc_Val[6][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT6 >> 4);
    g_Adc_Val[7][g_nAdc_Val_Index] = (AdcRegs.ADCRESULT7 >> 4);

    if (++g_nAdc_Val_Index >= MAX_ADC_BUF_SIZE)
    	g_nAdc_Val_Index = 0;
}

Uint16 Get_Adc_Val_280x (int id)
{
	Uint16 n;
	Uint32 sum = 0;

	for (n = 0; n < MAX_ADC_BUF_SIZE; n++)
		sum += g_Adc_Val[id][n];

	return sum / MAX_ADC_BUF_SIZE;
}

