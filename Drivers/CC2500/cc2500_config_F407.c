#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

//SPI pin configuration for the CC2500 driver

SPI_TypeDef* 		CC2500_SPI = 										SPI2;
uint32_t				CC2500_SPI_CLK = 								RCC_APB1Periph_SPI2;
uint8_t					CC2500_SPI_AF = 								GPIO_AF_SPI2;

uint32_t 				CC2500_SPI_SCLK_GPIO_CLK =			RCC_AHB1Periph_GPIOB;
GPIO_TypeDef* 	CC2500_SPI_SCLK_GPIO_PORT = 		GPIOB;
uint16_t			 	CC2500_SPI_SCLK_GPIO_PIN =			GPIO_Pin_13;				//ez430 Pin 16
uint8_t 				CC2500_SPI_SCLK_AF_PIN_SOURCE	= GPIO_PinSource13;

uint32_t 				CC2500_SPI_CSN_GPIO_CLK =				RCC_AHB1Periph_GPIOB;
GPIO_TypeDef*		CC2500_SPI_CSN_GPIO_PORT =			GPIOB;
uint16_t 				CC2500_SPI_CSN_GPIO_PIN =				GPIO_Pin_12;				//ez430 Pin 17
uint8_t 				CC2500_SPI_CSN_AF_PIN_SOURCE =	GPIO_PinSource12;

uint32_t 				CC2500_SPI_MISO_GPIO_CLK =			RCC_AHB1Periph_GPIOB;
GPIO_TypeDef* 	CC2500_SPI_MISO_GPIO_PORT =			GPIOB;
uint16_t 				CC2500_SPI_MISO_GPIO_PIN =			GPIO_Pin_14;				//ez430 Pin 15
uint8_t 				CC2500_SPI_MISO_AF_PIN_SOURCE =	GPIO_PinSource14;

uint32_t 				CC2500_SPI_MOSI_GPIO_CLK =			RCC_AHB1Periph_GPIOB;
GPIO_TypeDef* 	CC2500_SPI_MOSI_GPIO_PORT =			GPIOB;
uint16_t 				CC2500_SPI_MOSI_GPIO_PIN =			GPIO_Pin_15;				//ez430 Pin 18
uint8_t 				CC2500_SPI_MOSI_AF_PIN_SOURCE =	GPIO_PinSource15;
