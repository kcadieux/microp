#include "geometry.h"

#ifndef max
  #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct 
{
	double x;
	double y;
} point;

typedef struct
{
	point p;
	double radius;
} circle;

float radii[MAX_RADII];
char radiiPresence[MAX_RADII];
int radii_count = 0;

int platformX[MAX_RADII] = { STATION0_X, STATION1_X, STATION2_X, STATION3_X };
int platformY[MAX_RADII] = { STATION0_Y, STATION1_Y, STATION2_Y, STATION3_Y };

float xPos;
float yPos;

double getDistance(point p1, point p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}



// returns how many intersections there are
// see http://stackoverflow.com/questions/3349125/circle-circle-intersection-points
int getIntersections(circle c1, circle c2, point * i1, point * i2)
{
	double d = getDistance(c1.p, c2.p);
	printf("%f", d);
	// too far apart
	if (d > c1.radius + c2.radius)
	{
		return 0;
	}
	
	// concentric circles
	if (d == 0)
	{
		return  0;
	}
	
	// a circle contain the other
	if (d + min(c1.radius, c2.radius) < max(c1.radius, c2.radius))
	{
		return 0;
	}
	
	double a = ((c1.radius*c1.radius) - (c2.radius*c2.radius) + (d*d))/(2.0f*d);
	double h = sqrt((c1.radius*c1.radius) - (a*a));
	
	point p2;
	p2.x = c1.p.x + (a*(c2.p.x - c1.p.x)) / d;
	p2.y = c1.p.y + (a*(c2.p.y - c2.p.y)) / d;
	
	i1->x = p2.x + (h*(c2.p.y - c1.p.y)/d);
	i1->y = p2.y - (h*(c2.p.x - c1.p.x)/d);
	
	i2->x = p2.x - (h*(c2.p.y - c1.p.y)/d);
	i2->y = p2.y + (h*(c2.p.x - c1.p.x)/d);
	
	// only one intersection
	if (d == c1.radius + c2.radius)
	{
		return 1;
	}
	
	return 2;
}

void UpdateRadius(float radius, int radiusIndex)
{
	if (radiusIndex >= MAX_RADII || radiusIndex < 0)
	{
		return;
	}
	
	radii[radiusIndex] = radius;
	if (!radiiPresence[radiusIndex])
	{
		radiiPresence[radiusIndex] = PRESENT;
		radii_count++;
	}
}

void ComputeCooordinates()
{
	int i, size, countX, countY;
	point p1, p2, i1, i2;
	circle c1, c2;
	
	float sum;
	for (i = 0; i < MAX_RADII && radiiPresence[i]; i++)
	{
		for (j = 0; i < MAX_RADII && i != j; j++)
		{
			p1.x = platformX[i];
			p1.y = platformY[i];
			
			p2.x = platformX[j];
			p2.y = platformY[j];
			
			c1.p = p1;
			c1.radius = radii[i];
			
			c2.p = p2;
			c2.radius = radii[j];
			
			size = getIntersections(c1, c2, &i1, &i2);
			
			while(size == 0)
			{
				c1.radius += RADIUS_OFFSET;
				c2.radius += RADIUS_OFFSET;
				size = getIntersections(c1, c2, &i1, &i2);
			}
			
			if (size == 1)
			{
				if (i1.x > 0 && i1.x <= MAX_X && i1.y > 0 && i1.y <= MAX_Y)
				{
					countX += i1.x;
					countY += i1.y;
					sum++;
				}
			}
			else if (size == 2)
			{
				if (i1.x > 0 && i1.x <= MAX_X && i1.y > 0 && i1.y <= MAX_Y
						&& i2.x > 0 && i2.x <= MAX_X && i2.y > 0 && i2.y <= MAX_Y)
				{
					return;
				}
				else if (i1.x > 0 && i1.x <= MAX_X && i1.y > 0 && i1.y <= MAX_Y)
				{
					countX += i1.x;
					countY += i1.y;
					sum++;
				}				
				else if (i2.x > 0 && i2.x <= MAX_X && i2.y > 0 && i2.y <= MAX_Y)
				{
					countX += i2.x;
					countY += i2.y;
					sum++;
				}
			}
		}
	}
}

void test()
{
	point p1,p2,i1,i2;
	circle c1,c2;
	p1.x = 1;
	p1.y = 1;
	c1.p = p1;
	c1.radius = 1;
	p2.x = 3;
	p2.y = 3;
	c2.p = p2;
	c2.radius = 3;
	
	int i = getIntersections(c1,c2, &i1,&i2);
}
