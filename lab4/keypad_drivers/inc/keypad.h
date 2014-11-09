/// <summary>
/// This header contains every declarations needed by the keypad
/// </summary>
/// <authors>
/// Maxime Grégoire
/// Vincent Arabian
/// </authors>

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/**
	*	GPIO ports
	*/
#define row_GPIO GPIOA
#define col_GPIO GPIOC

/**
	*	GPIO pins
	*/

#define col_bit_useless GPIO_Pin_9

#define col_bit_0 GPIO_Pin_1
#define col_bit_1 GPIO_Pin_2
#define col_bit_2 GPIO_Pin_4

#define row_bit_0 GPIO_Pin_1
#define row_bit_1 GPIO_Pin_2
#define row_bit_2 GPIO_Pin_3
#define row_bit_3 GPIO_Pin_8

#define STAR -1
#define POUND -2

#define NO_KEY_PRESSED -3

static int map[4][3] = {
	{1,			2,			3},
	{4,			5,			6},
	{7,			8,			9},
	{STAR,	0,	POUND}
};

extern int keypad_read(void);
extern void set_row_read(void);
extern void set_col_read(void);
extern int read_sequence(int* values, char selected);
extern void whileNotReleased(void);

extern void keypad_trans(void);
extern void keypad_init(void);
extern void keypad_timer_init(void);
