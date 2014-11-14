#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"

#include <stdio.h>
#include <string.h>

#define CLEARING_COLOR							LCD_COLOR_BLACK

#define CENTERED_TEXT_COLOR					LCD_COLOR_CYAN
#define	CENTERED_BACKGROUND_COLOR		LCD_COLOR_BLACK

#define SCROLLING_TEXT_COLOR				LCD_COLOR_YELLOW
#define	SCROLLING_BACKGROUND_COLOR	LCD_COLOR_BLACK
#define PIXEL_BETWEEN_TEXT_LINES 		2

#define LONG_TIME_AGO_LINES 				4
extern const char *aLongTimeAgo[LONG_TIME_AGO_LINES];

#define A_NEW_HOPE_TITLE_LINES 				4
#define A_NEW_HOPE_LINES 				23
extern const char *aNewHopeTitle[A_NEW_HOPE_TITLE_LINES];
extern const char *aNewHope[A_NEW_HOPE_LINES];
#define A_NEW_HOPE_ROLL_TIME			10 


#define COORDS_TITLE							"  Coordinates  "
#define COORDS_TITLE_OFFSET				5
#define COORDS_BACKGROUND_COLOR		LCD_COLOR_BLACK
#define COORDS_TEXT_COLOR					LCD_COLOR_GREEN
#define COORDS_HEIGHT							100 - 2
#define COORDS_WIDTH							LCD_PIXEL_WIDTH

#define MAP_BACKGROUND_COLOR			LCD_COLOR_GREY
#define MAP_TEXT_COLOR						LCD_COLOR_GREEN
#define MAP_HEIGHT								LCD_PIXEL_HEIGHT - COORDS_HEIGHT - 2
#define MAP_WIDTH									LCD_PIXEL_WIDTH - 1

/// Font choices are
/// - Font16x24
/// - Font12x12
/// - Font8x12
/// - Font8x8
void displayScrollingText(const char** lines, int numberOfLines, int rollTime, sFONT font);
void displayCenteredText(const char** lines, int numberOfLines, sFONT font);
void displayScrollingTextAndTitle(const char** titleLines, int titleNumberOfLines,const char** lines, int numberOfLines, int rollTime);

void starWarsIntro(void);
void initCoords(void);
void initMap(void);
void clearBackground(void);
