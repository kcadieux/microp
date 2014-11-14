#include "lcd.h"

//at font 16, max 15 char
const char *aLongTimeAgo[LONG_TIME_AGO_LINES] = 
{
	"  A long time  ",
	"   ago in a    ",
	" building far  ",
	"  far away...  "
};

const char *aNewHopeTitle[A_NEW_HOPE_TITLE_LINES] = 
{
	"  Episode IV  ",
	"  A NEW HOPE  ",
	"              ",
	"              "
};

//at font 12, max 20 char
const char *aNewHope[A_NEW_HOPE_LINES] = 
{
	" It is a period of  ",
	"civil war. The rebel",
	"patients striking at",
	"   Royal-Victoria   ",
	" Hospital, have won ",
	" their first battle ",
	"  against the evil  ",
	"  Hospital Empire.  ",
	"                    ",
	"                    ",
	" During the battle, ",
	" patients managed to",
	" trick doctors and  ",
	"and nurses. However,",
	" the Empire striked ",
	" back and installed ",
	"the ultimate weapon:",
	" the DETECT STAR, a ",
	"localization device ",
	" powerful enough to ",
	"   track down any   ",
	"   rebel patients   ",
	"                    "
};

void starWarsIntro()
{
	displayCenteredText(aLongTimeAgo, LONG_TIME_AGO_LINES, Font16x24);
	osDelay(3000);
	displayScrollingTextAndTitle(aNewHopeTitle, A_NEW_HOPE_TITLE_LINES, aNewHope, A_NEW_HOPE_LINES, A_NEW_HOPE_ROLL_TIME);
}

void displayScrollingText(const char** lines, int numberOfLines, int rollTime, sFONT font)
{
	uint16_t line;
	int xpos;
	LCD_SetFont(&font);
	LCD_Clear(SCROLLING_BACKGROUND_COLOR);
	LCD_SetBackColor(SCROLLING_BACKGROUND_COLOR);
	
	for (xpos = LCD_PIXEL_HEIGHT; xpos> -(LCD_GetFont()->Height * numberOfLines); xpos--)
	{
		LCD_SetTextColor(SCROLLING_TEXT_COLOR);
		for (line = 0; line < numberOfLines; line++)
		{
			LCD_DisplayStringLineNegative(xpos + (line*(LCD_GetFont()->Height+PIXEL_BETWEEN_TEXT_LINES)), (uint8_t*)lines[line]);
		}
		
		LCD_SetTextColor(SCROLLING_BACKGROUND_COLOR);
		LCD_DrawLine(0, xpos + (numberOfLines * LCD_GetFont()->Height + PIXEL_BETWEEN_TEXT_LINES) +1, LCD_PIXEL_WIDTH, LCD_DIR_HORIZONTAL);
				
		osDelay(rollTime);		
	}
	
	LCD_Clear(LCD_COLOR_BLACK);
}

void displayScrollingTextAndTitle(const char** titleLines, int titleNumberOfLines,const char** lines, int numberOfLines, int rollTime)
{
	uint16_t line;
	int xpos;
	LCD_Clear(SCROLLING_BACKGROUND_COLOR);
	LCD_SetBackColor(SCROLLING_BACKGROUND_COLOR);
	
	for (xpos = LCD_PIXEL_HEIGHT; xpos> -(((Font12x12.Height + PIXEL_BETWEEN_TEXT_LINES) * numberOfLines) + ((Font16x24.Height + PIXEL_BETWEEN_TEXT_LINES) * titleNumberOfLines)); xpos--)
	{
		LCD_SetTextColor(SCROLLING_TEXT_COLOR);		
		for (line = 0; line < titleNumberOfLines + numberOfLines; line++)
		{
			if (line < titleNumberOfLines)
			{
				LCD_SetFont(&Font16x24);
				LCD_DisplayStringLineNegative(xpos + (line*(LCD_GetFont()->Height+PIXEL_BETWEEN_TEXT_LINES)), (uint8_t*)titleLines[line]);
				
				if (line == titleNumberOfLines - 1)
				{
					LCD_SetFont(&Font12x12);
				}
			}
			else
			{			
				LCD_DisplayStringLineNegative(xpos + (titleNumberOfLines *(Font16x24.Height + PIXEL_BETWEEN_TEXT_LINES)) + (line-titleNumberOfLines)*(LCD_GetFont()->Height+PIXEL_BETWEEN_TEXT_LINES), (uint8_t*)lines[line - titleNumberOfLines]);
			}
		}

		osDelay(rollTime);		
	}
	
	LCD_Clear(SCROLLING_BACKGROUND_COLOR);
}

void displayCenteredText(const char** lines, int numberOfLines, sFONT font)
{
	uint16_t heightStart, heightNeeded, line;
	
	LCD_SetColors(CENTERED_TEXT_COLOR, CENTERED_BACKGROUND_COLOR);
	LCD_Clear(CENTERED_BACKGROUND_COLOR);
	
	LCD_SetFont(&font);
	
	heightNeeded = numberOfLines * (LCD_GetFont()->Height + PIXEL_BETWEEN_TEXT_LINES);
	heightStart = (LCD_PIXEL_HEIGHT / 2) - (heightNeeded / 2);
		
	for (line = 0; line < numberOfLines; line++)
	{
		LCD_DisplayStringLine(heightStart + (line * (LCD_GetFont()->Height + PIXEL_BETWEEN_TEXT_LINES)), (uint8_t*)lines[line]);
	}	
}

void clearBackground()
{
	LCD_Clear(CLEARING_COLOR);
}

void initMap()
{	
	LCD_SetColors(MAP_TEXT_COLOR, MAP_BACKGROUND_COLOR);
	LCD_DrawRect(0, COORDS_HEIGHT, MAP_HEIGHT, MAP_WIDTH - 1);
	int32_t lineIndex;
	
	// Create the grid
	for (lineIndex = 1; lineIndex < MAP_GRID_LINES; lineIndex++)
	{
		LCD_DrawLine(0, COORDS_HEIGHT + (lineIndex * MAP_LINES_HORIZONTAL_OFFSET), MAP_WIDTH, LCD_DIR_HORIZONTAL);
		LCD_DrawLine(lineIndex * MAP_LINES_VERTICAL_OFFSET, COORDS_HEIGHT, MAP_HEIGHT, LCD_DIR_VERTICAL);
	}
	
}

void updateCoordsString(uint8_t* xCoord, uint8_t* yCoord)
{
	LCD_SetColors(COORDS_BACKGROUND_COLOR, COORDS_TEXT_COLOR);
	LCD_DisplayStringLineAtY(LINE(3), 3 * Font12x12.Width, xCoord);
	LCD_DisplayStringLineAtY(LINE(5), 3 * Font12x12.Width, yCoord);
}

void initCoords()
{
	LCD_SetColors(COORDS_TEXT_COLOR, COORDS_BACKGROUND_COLOR);
	LCD_DrawFullRect(0, 0, COORDS_WIDTH, COORDS_HEIGHT); 
	
	// Title
	LCD_SetFont(&Font16x24);
	LCD_SetColors(COORDS_BACKGROUND_COLOR, COORDS_TEXT_COLOR);
	LCD_DisplayStringLine(COORDS_TITLE_OFFSET, (uint8_t*)COORDS_TITLE);
	
	// X and Y
	LCD_SetFont(&Font12x12);
	LCD_DisplayStringLine(LINE(3), (uint8_t*)COORDS_X);
	LCD_DisplayStringLine(LINE(5), (uint8_t*)COORDS_Y);	
}

void updateCoordsMap(int32_t xCoord, int32_t yCoord)
{
	float fractionX = (float)xCoord/(float)MAXIMUM_COORDINATES_POINT;
	float fractionY = (float)yCoord/(float)MAXIMUM_COORDINATES_POINT;
	
	LCD_SetColors(BEACON_COLOR, BEACON_COLOR);
	
	LCD_DrawFullCircle(fractionX * MAP_WIDTH, COORDS_HEIGHT + (fractionY*MAP_HEIGHT), BEACON_RADIUS);
}

void updateCoords(int32_t xCoord, int32_t yCoord)
{
	// Check if the coordinates are within the valid range
	if (xCoord < 0 || yCoord < 0 || xCoord > MAXIMUM_COORDINATES_POINT || yCoord > MAXIMUM_COORDINATES_POINT)
	{
		return;
	}
	
	initMap();
	char x[4];
	char y[4];
	sprintf(x, "%d", xCoord);
	sprintf(y, "%d", yCoord);
	
	updateCoordsMap(xCoord, yCoord);
	updateCoordsString((uint8_t*)x, (uint8_t*)y);
}
