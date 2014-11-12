#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

//SPI pin configuration for the CC2500 driver

SPI_TypeDef* 		CC2500_SPI = 										SPI4;
uint32_t				CC2500_SPI_CLK = 								RCC_APB2Periph_SPI4;
uint8_t					CC2500_SPI_AF = 								GPIO_AF_SPI4;

uint32_t 				CC2500_SPI_SCLK_GPIO_CLK =			RCC_AHB1Periph_GPIOE;
GPIO_TypeDef* 	CC2500_SPI_SCLK_GPIO_PORT = 		GPIOE;
uint16_t			 	CC2500_SPI_SCLK_GPIO_PIN =			GPIO_Pin_2;
uint8_t 				CC2500_SPI_SCLK_AF_PIN_SOURCE	= GPIO_PinSource2;

uint32_t 				CC2500_SPI_CSN_GPIO_CLK =				RCC_AHB1Periph_GPIOE;
GPIO_TypeDef*		CC2500_SPI_CSN_GPIO_PORT =			GPIOE;
uint16_t 				CC2500_SPI_CSN_GPIO_PIN =				GPIO_Pin_4;
uint8_t 				CC2500_SPI_CSN_AF_PIN_SOURCE =	GPIO_PinSource4;

uint32_t 				CC2500_SPI_MISO_GPIO_CLK =			RCC_AHB1Periph_GPIOE;
GPIO_TypeDef* 	CC2500_SPI_MISO_GPIO_PORT =			GPIOE;
uint16_t 				CC2500_SPI_MISO_GPIO_PIN =			GPIO_Pin_5;
uint8_t 				CC2500_SPI_MISO_AF_PIN_SOURCE =	GPIO_PinSource5;

uint32_t 				CC2500_SPI_MOSI_GPIO_CLK =			RCC_AHB1Periph_GPIOE;
GPIO_TypeDef* 	CC2500_SPI_MOSI_GPIO_PORT =			GPIOE;
uint16_t 				CC2500_SPI_MOSI_GPIO_PIN =			GPIO_Pin_6;
uint8_t 				CC2500_SPI_MOSI_AF_PIN_SOURCE =	GPIO_PinSource6;
