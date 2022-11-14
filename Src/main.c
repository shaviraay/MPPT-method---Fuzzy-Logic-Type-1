/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"   																													// 1 logger via bluetooth (cek 1-7)
#include "lcd_character.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#define SampleData 1000
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
 
 /*Inisialisasi jumlah ADC*/
__IO uint16_t ADC_value[5];



/*Inisialisasi PWM*/
float pwm =2099,pwm1; //735;945;1155duty trying before 50%duty 1050 (max psc 2099) how? psc x duty%
//float pwm =1155,pwm1; //945 1155
float D;
char bufferD[200];
char buffer_defuzzy[200];



// int tes=10;
float A0, Esq0[SampleData], Esum0, Eadc0;
float A1, Vsq1[SampleData], Vsum1, Vadc1;
float A2, Isq2[SampleData], Isum2, Iadc2;
float A3, Vsq3[SampleData], Vsum3, Vadc3;
float A4, Isq4[SampleData], Isum4, Iadc4;
float E0, V1, I2, V3, I4, Epv, Vin, Iin, Vout, Iout, Pin, Pout, Vi, Ii, Vo, Io;

int 	k=0, Z=0,i=0;
int		recordE0, recordV1, recordI2, recordV3, recordI4;
char	buffer_ADC_E0[200];
char  buffer_ADC_V1[200];
char  buffer_ADC_I2[200];
char  buffer_ADC_V3[200];
char  buffer_ADC_I4[200];
char	buffer_Epv[200];
char  buffer_Vin[200];
char  buffer_Iin[200];
char  buffer_Vout[200];
char  buffer_Iout[200];
char  buffer_Pin[200];
char  buffer_Pout[200];


//Deklarasi MPPT Fuzzy 1//
float eNB,eNK,eZ,ePK,ePB ; //Deklarasi nilai error fuzzy
float dNB,dNK,dZ,dPK,dPB; // Deklarasi nilai delta error fuzzy
float oNB = (-0.005), oNK = (-0.0025) , oZ = (0) , oPK = (0.0025) , oPB = (0.005); //fuzzyfikasi output fuzzy sugeno
float a=-200, b=-100, c=0, d=100, e=200;						
float f=-200, g=-100, hh=0, ii=100, j=200;

float r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25; //Inferensi
float esblm=0, defuzzy, e, d;
float vin, iin, vin2, iin2, vout, iout, step, Duty, vv, ii, pp, vs, is, ps, pv, pout,sp;
int t=0, s=2;
double error, derror;
float hasil, dppp, dvvv;
static int status=0,MPP;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM13_Init(void);
static void MX_TIM14_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

//SENSOR//
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	if (htim->Instance==TIM14) 																										
	   { 
				HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADC_value,5);	
			  //Sensor Tegangan Input
				Vadc1= ADC_value[4];
				Vsum1-=Vsq1[k];
				Vsq1[k]=Vadc1;
				Vsum1+=Vsq1[k];
				A1=Vsum1/SampleData;
//				V1=A1/4095*5;                                         	 /*Rumus untuk mendapatkan Vadc*/
				Vin = (0.0042*A1) - 0.21;											   		 	
			
			 
			 //Sensor Tegangan Output//
				Vadc3= ADC_value[0];
				Vsum3-=Vsq3[k];
				Vsq3[k]=Vadc3;
				Vsum3+=Vsq3[k];
				A3=Vsum3/SampleData;
//			V3=A3/4095*5;                                         		 /*Rumus untuk mendapatkan Vadc*/
				Vout = (0.0036*A3) - 0.1692;
			 	
				//Sensor Arus Input
				Iadc4= ADC_value[2];
				Isum4-=Isq4[k];
				Isq4[k]=Iadc4;
				Isum4+=Isq4[k];
				A4=Isum4/SampleData;
//				I4=A4/4095*5;																			    /*Rumus untuk mendapatkan Iadc*/
				Iin = (0.0125*A4) - 25.39;

			 	//Sensor Arus Output
				Iadc2= ADC_value[3];
				Isum2-=Isq2[k];
				Isq2[k]=Iadc2;
				Isum2+=Isq2[k];
				A2=Isum2/SampleData;
//			I1=A2/4095*5;																			    /*Rumus untuk mendapatkan Iadc*/
				Iout = (0.0152*A2 - 30.6);													  /*Rumus untuk mendapatkan Iout*/
			
			//Menghitung Nilai Daya
//				Pin = Vin*Iin;					/*Rumus Daya Input*/
//				Pout = Vout*Iout;				/*Rumus Daya Output*/

				k++;
				if(k>=SampleData)
				k=0;					
 			
				//SENSOR TEGANGAN INPUT ==> Mikro ADC_Value[4] PIN PA4 PCB ADC0
				//SENSOR ARUS INPUT ==> Mikro ADC_Value[2] PIN PA2 PCB ADC2
				//SENSOR TEGANGAN OUTPUT ==> Mikro ADC_Value[0] PIN PA4 PCB ADC0
				//SENSOR ARUS OUTPUT ==> Mikro ADC_Value[3] PIN PA2 PCB ADC2
			

	  }


							//------------------MPPT FLC TYPE 1 BY SHAVIRA AISYAH------------------//
		
	if (htim->Instance==TIM9) 																										
	{
		status = ~status;
		MPP = status;
		if(MPP == 0) // saat ini
		{
			is = Iin;
			vs = Vin;
			ps = is * vs;
		}
		else if(MPP == -1) //sebelumnya
		{
			ii = Iin;
			vv = Vin;
			pp = ii * vv;	
		}
		
		dppp = ps - pp;
		dvvv = vs - vv;
		pout = Vout * Iout;
		
		ps = pp;
		vs=vv;
		
		error = dppp/dvvv;
		derror = error - esblm;
		esblm = error;
		
					//------------------FUZZIFIKASI ERROR------------------//
	if(ps<200)
	{
		if (error<=a)
	{ 	
		eNB=1;
		eNK=eZ=ePK=ePB=0;
	}
	else if (error>=a && error<=b)
	{
		eNB=(b-error)/(b-a);	
		eNK=(error-a)/(b-a);
		eZ=ePK=ePB=0;
	}
	else if (error>=b && error<=c)
	{ 
		eNK=(c-error)/(c-b);
		eZ=(error-b)/(c-b);
		eNB=ePK=ePB=0;
	}
	else if (error>=c && error<=d)
	{ 
		eZ=(d-error)/(d-c);
		ePK=(error-c)/(d-c);
		eNB=eNK=ePB=0;
	}
	else if (error>=d && error<=e)
	{
		ePK=(e-error)/(e-d);
		ePB=(error-d)/(e-d);
		eNB=eNK=eZ=0;
	}
		else if (error>=e)
	{ 
		ePB=1; 
		eNB=eNK=eZ=ePK=0;
	}
	
						//------------------FUZZIFIKASI DELTA ERROR------------------//
	if (derror<=f)
	{ 	
		dNB=1;
		dNK=dZ=dPK=dPB=0;
	}
	else if (derror>=f && derror<=g)
	{
		dNB=(g-derror)/(g-f);
		dNK=(derror-f)/(g-f);
		dZ=dPK=dPB=0;
	}
	else if (derror>=g && derror<=hh)
	{ 
		dNK=(hh-derror)/(hh-g);
		dZ=(derror-g)/(hh-g);
		dPK=dNB=dPB=0;
	}
	else if (derror>=hh && derror<=i)
	{
		dZ=(ii-derror)/(ii-hh);
		dPK=(derror-hh)/(ii-hh);
		dNB=dNK=dPB=0;
	}
	else if (derror>=i && derror<=j)
	{
		dPK=(j-derror)/(j-ii);
		dPB=(derror-ii)/(j-ii);
		dNB=dNK=dZ=0;
	}
	else if (derror>=j)
	{ 
		dPB=1;
		dPK=dZ=dNK=dNB=0;
	}
	
				//------------------FUZZY INFERENCE------------------//
r1=dPB;	if (eNB>dPB)r1=eNB;
r2=dNB;	if (eNK>dPB)r2=eNK;
r3=dPB;	if (eZ>dPB)r3=eZ;
r4=dPB;	if (ePK>dPB)r4=ePK;
r5=dPB;	if (ePB>dPB)r5=ePB;
	
r6=dPK;	if (eNB>dPK)r6=eNB;
r7=dPK;	if (eNK>dPK)r7=eNK;
r8=dPK;	if (eZ>dPK)r8=eZ;
r9=dPK;	if (ePK>dPK)r9=ePK;
r10=dPK; if (ePB>dPK)r10=ePB;
	
r11=dZ;	if (eNB>dZ)r11=eNB;
r12=dZ;	if (eNK>dZ)r12=eNK;
r13=dZ;	if (eZ>dZ)r13=eZ;
r14=dZ;	if (ePK>dZ)r14=ePK;
r15=dZ;	if (ePB>dZ)r15=ePB;
	
r16=dNK;	if (eNB>dNK)r16=eNB;
r17=dNK;	if (eNK>dNK)r17=eNK;
r18=dNK;	if (eZ>dNK)r18=eZ;
r19=dNK;	if (ePK>dNK)r19=ePK;
r20=dNK;	if (ePB>dNK)r20=ePB;

r21=dNB;	if (eNB>dNB)r21=eNB;
r22=dNB;	if (eNK>dNB)r22=eNK;
r23=dNB;	if (eZ>dNB)r23=eZ;
r24=dNB;	if (ePK>dNB)r24=ePK;
r25=dNB;	if (ePB>dNB)r25=ePB;

 				//------------------DEFUZZYFIKASI------------------//
hasil=((r1*oPB)+(r2*oPK)+(r3*oNK)+(r4*oZ)+(r5*oZ)+(r6*oPB)+(r7*oPK)+(r8*oZ)+(r9*oZ)+(r10*oZ)+(r11*oPB)+(r12*oPK)+(r13*oZ)+(r14*oNK)+(r15*oNK)+(r16*oZ)+(r17*oZ)+(r18*oZ)+(r19*oNK)+(r20*oNK)+(r21*oZ)+(r22*oZ)+(r23*oPK)+(r24*oNK)+(r25*oNB))/(r1+r2+r3+r4+r5+r6+r7+r8+r9+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24+r25);	

	if(dppp == 0)
	{
		hasil = hasil;
	}
	
defuzzy = defuzzy + hasil;

hasil=(r1*oZ+r2*oZ+r3*oZ+r4*oZ+r5*oZ+r6*oZ+r7*oZ+r8*oZ+r9*oZ+r10*oZ+r11*oZ+r12*oZ+r13*oZ+r14*oZ+r15*oZ+r16*oZ+r17*oZ+r18*oZ+r19*oZ+r20*oZ+r21*oZ+r22*oZ+r23*oZ+r24*oZ+r25*oZ)/(r1+r2+r3+r4+r5+r6+r7+r8+r9+r10+r11+r12+r13+r14+r15+r16+r17+r18+r19+r20+r21+r22+r23+r24+r25);
defuzzy = defuzzy + hasil;

pwm = 945 + (defuzzy);
D = pwm*100/2100;
TIM4->CCR1 = pwm;	

		pp = ps;
		vv = vs;
}		

		if (D>60)
		{	
		D=60;
		}
		if (D<0)
		{
		D=0;
		}

	}
	}
/* USER CODE END PFP */




/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
__IO uint16_t Nilai_ADC[5];
float pwm1;
/* USER CODE END 0 */







/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM4_Init();
  MX_TIM9_Init();
  MX_TIM13_Init();
  MX_TIM14_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &ADC_value,5);
	HAL_TIM_Base_Start_IT(&htim14); 																									// interrupt ADC
	HAL_TIM_Base_Start_IT(&htim13);  																									// interrupt PWM
	HAL_TIM_Base_Start_IT(&htim9);   																								
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);

//TAMPILAN AWAL//
	TIM4->CCR1 = 0;
	lcd_init();

	lcd_gotoxy(0,0);	
	lcd_puts("   MPPT - FUZZY1   ");
	lcd_gotoxy(1,1);
	lcd_puts(" SHAVIRA AISYAH ");
	lcd_gotoxy(5,2);
	lcd_puts("2320640021");
	HAL_Delay(3000);
	lcd_clear();
	
//	lcd_gotoxy(0,2);
//	lcd_puts ("adcV:     ");

//	lcd_gotoxy(10,2);
//	lcd_puts ("adcI:");

	lcd_gotoxy(0,1);
	lcd_puts ("Vin:    ");

	lcd_gotoxy(0,2);
	lcd_puts ("Vo :    ");

	lcd_gotoxy(10,1);
	lcd_puts ("Iin:     ");

	lcd_gotoxy(10,2);
	lcd_puts ("Io :     ");

	lcd_gotoxy(0,0);
	lcd_puts ("PWM :     ");
	
	lcd_gotoxy(0,3);
	lcd_puts ("Pin:     ");

	lcd_gotoxy(10,3);
	lcd_puts ("Po :     ");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
			HAL_Delay(100);

		//Menampilkan PWM //
	TIM4->CCR1 = pwm;
	D = pwm/2099*100;
	lcd_gotoxy(5,0);																													
	lcd_puts(bufferD);

	
		//Menampilkan adcV dan adcI //
//	HAL_Delay(200);
//	lcd_gotoxy(5,2);																													
//	sprintf(buffer_ADC_V1,"%4.0f",A1);
//	lcd_puts(buffer_ADC_V1);
//	HAL_Delay(50);

//	HAL_Delay(200); 		//NEW
//	lcd_gotoxy(5,2);																													
//	sprintf(buffer_ADC_V3,"%4.0f",A3);
//	lcd_puts(buffer_ADC_V3);
//	HAL_Delay(50);

//	lcd_gotoxy(15,2);		//NEW																											
//	sprintf(buffer_ADC_I4,"%4.0f",A4);
//	lcd_puts(buffer_ADC_I4);
//	HAL_Delay(50);
		
//	lcd_gotoxy(15,2);																													
//	sprintf(buffer_ADC_I1,"%4.0f",A2);
//	lcd_puts(buffer_ADC_I1);
//	HAL_Delay(50);
	
		//Menampilkan Vin dan Iin //
	lcd_gotoxy(5,1);																													
	sprintf(buffer_Vin,"%2.2f",Vin);
	lcd_puts(buffer_Vin);

	HAL_Delay(100);
	lcd_gotoxy(14,1);																													
	sprintf(buffer_Iin,"%2.1f",Iin);
	lcd_puts(buffer_Iin);
	HAL_Delay(50);

		//Menampilkan Vout dan Iout //
	lcd_gotoxy(5,2);																													
	sprintf(buffer_Vout,"%2.2f",Vout);
	lcd_puts(buffer_Vout);

	HAL_Delay(100);
	lcd_gotoxy(14,2);																													
	sprintf(buffer_Iout,"%2.1f",Iout);
	lcd_puts(buffer_Iout);
	HAL_Delay(50);
	
	////		//Menampilkan Pin dan Pout //
	
	//Menghitung Nilai Daya
	Pin = Vin*Iin;					/*Rumus Daya Input*/
	Pout = Vout*Iout;				/*Rumus Daya Output*/
	lcd_gotoxy(5,3);																													
	sprintf(buffer_Pin,"%3.1f",Pin);
	lcd_puts(buffer_Pin);
	HAL_Delay(100);

	lcd_gotoxy(13,3);																													
	sprintf(buffer_Pout,"%3.1f",Pout);
	lcd_puts(buffer_Pout);
	HAL_Delay(100);
	
  }
  /* USER CODE END 3 */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------//



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 5;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable Calibrartion
  */
  if (HAL_RTCEx_SetCalibrationOutPut(&hrtc, RTC_CALIBOUTPUT_1HZ) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 2099;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 5000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 42041;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 0;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 2099;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 0;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 16801;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
