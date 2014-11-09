#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "cc2500.h"

#define CC2500_SPI										SPI4
#define CC2500_SPI_CLK								RCC_APB2Periph_SPI4
#define CC2500_SPI_AF									GPIO_AF_SPI4

#define CC2500_SPI_SCLK_GPIO_CLK			RCC_AHB1Periph_GPIOE
#define CC2500_SPI_SCLK_GPIO_PORT 		GPIOE
#define CC2500_SPI_SCLK_GPIO_PIN			GPIO_Pin_2
#define CC2500_SPI_SCLK_AF_PIN_SOURCE	GPIO_PinSource2

#define CC2500_SPI_CSN_GPIO_CLK				RCC_AHB1Periph_GPIOE
#define CC2500_SPI_CSN_GPIO_PORT			GPIOE
#define CC2500_SPI_CSN_GPIO_PIN				GPIO_Pin_4
#define CC2500_SPI_CSN_AF_PIN_SOURCE	GPIO_PinSource4

#define CC2500_SPI_MISO_GPIO_CLK			RCC_AHB1Periph_GPIOE
#define CC2500_SPI_MISO_GPIO_PORT			GPIOE
#define CC2500_SPI_MISO_GPIO_PIN			GPIO_Pin_5
#define CC2500_SPI_MISO_AF_PIN_SOURCE	GPIO_PinSource5

#define CC2500_SPI_MOSI_GPIO_CLK			RCC_AHB1Periph_GPIOE
#define CC2500_SPI_MOSI_GPIO_PORT			GPIOE
#define CC2500_SPI_MOSI_GPIO_PIN			GPIO_Pin_6
#define CC2500_SPI_MOSI_AF_PIN_SOURCE	GPIO_PinSource6


#define CC2500_ADDR_MASK 0x3F

static const uint32_t CC2500_BR_BURST_MODE = 		6500000;
static const uint32_t CC2500_BR_SINGLE_MODE = 	9000000;

static uint8_t ZEROBYTE[] = {0};

static const uint16_t baudRatePrescalers[] = {
	SPI_BaudRatePrescaler_2,
	SPI_BaudRatePrescaler_4,
	SPI_BaudRatePrescaler_8,
	SPI_BaudRatePrescaler_16,
	SPI_BaudRatePrescaler_32,
	SPI_BaudRatePrescaler_64,
	SPI_BaudRatePrescaler_128,
	SPI_BaudRatePrescaler_256
};

static const uint16_t NB_BAUD_RATE_PRESCALERS = sizeof(baudRatePrescalers) / sizeof(baudRatePrescalers[0]);

extern void HardFault_Handler(void);

static uint8_t SendByte(uint8_t byte);
static uint8_t RegisterInteraction(uint8_t addr, uint8_t* bytes, uint8_t nbBytes);
static void LowLevelInit(void);
static void CSN_LO(void);
static void CSN_HI(void);
static void WaitForChipReady(void);

void CC2500_Init()
{
	LowLevelInit();
}


void CC2500_StructInit(CC2500_InitTypeDef* init_s)
{
//Expand the config registers to fill in the struct fields with their default value
#define CC2500_CONFIG_REGISTER(name, addr, def_val)	init_s->name = def_val;
	CC2500_CONFIG_REGISTERS
#undef CC2500_CONFIG_REGISTER
}

uint8_t CC2500_WriteConfigRegister(uint8_t addr, uint8_t byte)
{
	uint8_t bytes[1];
	bytes[0] = byte;
	//Set R/W bit and Burst bit to 0
	addr &= CC2500_ADDR_MASK;
	
	return RegisterInteraction(addr, bytes, 1);
}

uint8_t CC2500_ReadConfigRegister(uint8_t addr)
{
	//Set R/W bit to 1 and Burst bit to 0
	addr &= CC2500_ADDR_MASK;
	addr |= CC2500_RW_READ;
	
	return RegisterInteraction(addr, ZEROBYTE, 1);
}


uint8_t CC2500_ReadStatusRegister(uint8_t addr)
{	
	return RegisterInteraction(addr, ZEROBYTE, 1);
}

uint8_t CC2500_SendCommandStrobe(uint8_t strobe)
{
	uint8_t result;
	CSN_LO();
	result = SendByte(strobe);
	CSN_HI();
	return result;
}

static uint8_t RegisterInteraction(uint8_t addr, uint8_t* bytes, uint8_t nbBytes)
{
	uint8_t result;
	
	CSN_LO();
	result = SendByte(addr);
	result = SendByte(*bytes);
	CSN_HI();
	
	return result;
}

static void LowLevelInit(void)
{
	GPIO_InitTypeDef	gpio_init_s;
	SPI_InitTypeDef  	spi_init_s;
	RCC_ClocksTypeDef clockInfo;
	uint16_t 					baudRateIndex = 0;
	
	//1. Initialize GPIO pins for SPI alternate functions
	RCC_AHB1PeriphClockCmd(CC2500_SPI_SCLK_GPIO_CLK | CC2500_SPI_CSN_GPIO_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(CC2500_SPI_MISO_GPIO_CLK | CC2500_SPI_MOSI_GPIO_CLK, ENABLE);
	
	gpio_init_s.GPIO_Mode = GPIO_Mode_AF;
	gpio_init_s.GPIO_OType = GPIO_OType_PP;
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_DOWN;
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;
	
	gpio_init_s.GPIO_Pin = CC2500_SPI_SCLK_GPIO_PIN;
	GPIO_Init(CC2500_SPI_SCLK_GPIO_PORT, &gpio_init_s);
	GPIO_PinAFConfig(CC2500_SPI_SCLK_GPIO_PORT, CC2500_SPI_SCLK_AF_PIN_SOURCE, CC2500_SPI_AF);
	
	gpio_init_s.GPIO_Pin = CC2500_SPI_MISO_GPIO_PIN;
	GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &gpio_init_s);
	GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_AF_PIN_SOURCE, CC2500_SPI_AF);
	
	gpio_init_s.GPIO_Pin = CC2500_SPI_MOSI_GPIO_PIN;
	GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &gpio_init_s);
	GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_AF_PIN_SOURCE, CC2500_SPI_AF);
	
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT;
	gpio_init_s.GPIO_Pin = CC2500_SPI_CSN_GPIO_PIN;
	GPIO_Init(CC2500_SPI_CSN_GPIO_PORT, &gpio_init_s);
	CSN_HI();
	
	//2. Initialize SPI
	RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
	SPI_I2S_DeInit(CC2500_SPI);
	
	//Automatic detection of required baud rate prescaler based on APB2 clock
	RCC_GetClocksFreq(&clockInfo);
	for (; (clockInfo.PCLK2_Frequency >>= 1) > CC2500_BR_BURST_MODE && baudRateIndex < NB_BAUD_RATE_PRESCALERS; ++baudRateIndex);
	
	//If no satisfactory baud rate prescaler was found, stop execution (extremely unlikely to happen).
	if (baudRateIndex == NB_BAUD_RATE_PRESCALERS) HardFault_Handler();
	
	spi_init_s.SPI_Direction = 					SPI_Direction_2Lines_FullDuplex;
  spi_init_s.SPI_DataSize = 					SPI_DataSize_8b;
  spi_init_s.SPI_CPOL = 							SPI_CPOL_Low;
  spi_init_s.SPI_CPHA = 							SPI_CPHA_1Edge;
  spi_init_s.SPI_NSS = 								SPI_NSS_Soft;
  spi_init_s.SPI_BaudRatePrescaler = 	baudRatePrescalers[baudRateIndex];
  spi_init_s.SPI_FirstBit = 					SPI_FirstBit_MSB;
  spi_init_s.SPI_CRCPolynomial = 			7;
  spi_init_s.SPI_Mode = 							SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &spi_init_s);
	
	SPI_Cmd(CC2500_SPI, ENABLE);
}

static uint8_t SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET);
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(CC2500_SPI, byte);
  
  /* Wait to receive a Byte */
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}

static void CSN_LO()
{
	GPIO_ResetBits(CC2500_SPI_CSN_GPIO_PORT, CC2500_SPI_CSN_GPIO_PIN);
	WaitForChipReady();
}

static void CSN_HI()
{
	GPIO_SetBits(CC2500_SPI_CSN_GPIO_PORT, CC2500_SPI_CSN_GPIO_PIN);
}

static void WaitForChipReady()
{
	//The MISO pin will be high as long as CC2500 power and crystal have not stabilized
	//Therefore, wait for the MISO pin to go low
	while (GPIO_ReadInputDataBit(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_GPIO_PIN));
}

