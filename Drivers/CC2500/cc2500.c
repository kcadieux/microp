#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "cc2500.h"

//SPI pin configuration provided externally because it may vary.
extern SPI_TypeDef* 		CC2500_SPI;
extern uint32_t					CC2500_SPI_CLK;
extern uint8_t					CC2500_SPI_AF;

extern uint32_t 				CC2500_SPI_SCLK_GPIO_CLK;
extern GPIO_TypeDef* 		CC2500_SPI_SCLK_GPIO_PORT;
extern uint16_t			 		CC2500_SPI_SCLK_GPIO_PIN;
extern uint8_t 					CC2500_SPI_SCLK_AF_PIN_SOURCE;

extern uint32_t 				CC2500_SPI_CSN_GPIO_CLK;
extern GPIO_TypeDef*		CC2500_SPI_CSN_GPIO_PORT;
extern uint16_t 				CC2500_SPI_CSN_GPIO_PIN;
extern uint8_t 					CC2500_SPI_CSN_AF_PIN_SOURCE;

extern uint32_t 				CC2500_SPI_MISO_GPIO_CLK;
extern GPIO_TypeDef* 		CC2500_SPI_MISO_GPIO_PORT;
extern uint16_t 				CC2500_SPI_MISO_GPIO_PIN;
extern uint8_t 					CC2500_SPI_MISO_AF_PIN_SOURCE;

extern uint32_t 				CC2500_SPI_MOSI_GPIO_CLK;
extern GPIO_TypeDef* 		CC2500_SPI_MOSI_GPIO_PORT;
extern uint16_t 				CC2500_SPI_MOSI_GPIO_PIN;
extern uint8_t 					CC2500_SPI_MOSI_AF_PIN_SOURCE;

#define CC2500_ADDR_MASK 		0x3F
#define CC2500_PATABLE_ADDR 0x3E
#define CC2500_FIFO_ADDR		0x3F

static const uint32_t CC2500_BR_BURST_MODE = 		6500000;

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
static uint8_t SingleTransfer(uint8_t addr, uint8_t byte_in, uint8_t* byte_out);
static uint8_t BurstTransfer(uint8_t addr, uint8_t* data_in, uint8_t* data_out, uint8_t nbBytes);

static void LowLevelInit(void);
static void CSN_LO(void);
static void CSN_HI(void);
static void WaitForChipReady(void);

static CC2500_StatusTypeDef RegisterToStatus(uint8_t statusRegister);

void CC2500_Init()
{
	LowLevelInit();
	CC2500_SendCommandStrobe(CC2500_SRES_R);
}

void CC2500_WriteConfig(CC2500_InitTypeDef* init_s)
{
	//Macro-powered code generation: initialize every register with the value found in the init structure 
#define CC2500_CONFIG_REGISTER(name, addr, def_value) CC2500_WriteConfigRegister(addr, init_s->name);
	CC2500_CONFIG_REGISTERS
#undef	CC2500_CONFIG_REGISTER
}


void CC2500_StructInit(CC2500_InitTypeDef* init_s)
{
//Expand the config registers to fill in the struct fields with their default value
#define CC2500_CONFIG_REGISTER(name, addr, def_val)	init_s->name = def_val;
	CC2500_CONFIG_REGISTERS
#undef CC2500_CONFIG_REGISTER
}

CC2500_StatusTypeDef CC2500_WriteConfigRegister(uint8_t addr, uint8_t byte)
{
	//Clear top 2 bits
	addr &= CC2500_ADDR_MASK;
	
	return RegisterToStatus(SingleTransfer(addr, byte, &byte));
}

CC2500_StatusTypeDef CC2500_ReadConfigRegister(uint8_t addr, uint8_t* register_data)
{
	//Clear top 2 bits and set read bit to 1
	addr &= CC2500_ADDR_MASK;
	addr |= CC2500_RW_READ;
	
	return RegisterToStatus(SingleTransfer(addr, 0, register_data));
}

CC2500_StatusTypeDef CC2500_ReadStatusRegister(uint8_t addr, uint8_t* register_data)
{	
	return RegisterToStatus(SingleTransfer(addr, 0, register_data));
}

CC2500_StatusTypeDef CC2500_SendCommandStrobe(uint8_t strobe)
{
	uint8_t status;
	CSN_LO();
	status = SendByte(strobe);
	CSN_HI();
	return RegisterToStatus(status);
}

CC2500_StatusTypeDef CC2500_WritePATABLE(CC2500_PATABLETypeDef* patable)
{
	return RegisterToStatus(BurstTransfer(CC2500_PATABLE_ADDR, patable->data, patable->data, CC2500_PATABLE_SIZE));
}

CC2500_StatusTypeDef CC2500_ReadPATABLE(CC2500_PATABLETypeDef* patable)
{
	return RegisterToStatus(BurstTransfer(CC2500_PATABLE_ADDR | CC2500_RW_READ, patable->data, patable->data, CC2500_PATABLE_SIZE));
}

CC2500_StatusTypeDef CC2500_WriteTxFIFO(uint8_t* fifo_data, uint8_t nbBytes)
{
	return RegisterToStatus(BurstTransfer(CC2500_FIFO_ADDR, fifo_data, fifo_data, nbBytes));
}

CC2500_StatusTypeDef CC2500_ReadRxFIFO(uint8_t* fifo_data, uint8_t nbBytes)
{
	return RegisterToStatus(BurstTransfer(CC2500_FIFO_ADDR | CC2500_RW_READ, fifo_data, fifo_data, nbBytes));
}

CC2500_StatusTypeDef CC2500_ReadConfiguration(CC2500_InitTypeDef* init_s)
{
	CC2500_StatusTypeDef status;
	
#define CC2500_CONFIG_REGISTER(name, addr, def_value) status = CC2500_ReadConfigRegister(addr, &init_s->name);
	CC2500_CONFIG_REGISTERS
#undef	CC2500_CONFIG_REGISTER
	
	return status;
}

static uint8_t SingleTransfer(uint8_t addr, uint8_t byte_in, uint8_t* byte_out)
{
	uint8_t status;
	
	CSN_LO();
	status = SendByte(addr);
	*byte_out = SendByte(byte_in);
	CSN_HI();
	
	return status;
}

static uint8_t BurstTransfer(uint8_t addr, uint8_t* data_in, uint8_t* data_out, uint8_t nbBytes)
{
	uint8_t status;
	int i;
	
	CSN_LO();
	status = SendByte(addr | CC2500_BURST);
	
	for (i=0; i < nbBytes; ++i) {
		*data_out++ = SendByte(*data_in++);
	}
	CSN_HI();
	
	return status;
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
	SPI_I2S_DeInit(CC2500_SPI);
	
	RCC_GetClocksFreq(&clockInfo);
	
	//Automatic detection of required baud rate prescaler based on peripheral clock
	if (CC2500_SPI == SPI2 || CC2500_SPI == SPI3) {
		RCC_APB1PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
		for (; (clockInfo.PCLK1_Frequency >>= 1) > CC2500_BR_BURST_MODE && baudRateIndex < NB_BAUD_RATE_PRESCALERS; ++baudRateIndex);
	} else {
		RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
		for (; (clockInfo.PCLK2_Frequency >>= 1) > CC2500_BR_BURST_MODE && baudRateIndex < NB_BAUD_RATE_PRESCALERS; ++baudRateIndex);
	}
	
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

static CC2500_StatusTypeDef RegisterToStatus(uint8_t statusRegister)
{
	CC2500_StatusTypeDef status;
	status.chipReady = statusRegister & CC2500_STATUS_CHIPREADY;
	status.state = (CC2500_StatusStateTypeDef)(statusRegister & CC2500_STATUS_STATE);
	status.fifoBytesAvailable = statusRegister & CC2500_STATUS_FIFOBYTES;
	return status;
}

