/**
  * @brief
  */
#include <stdint.h>
#include <stdio.h>
#include "system_management.h"
#include "cv.h"
Navigator::Navigator(uint8_t THRESHOLD)
{
    /*
    direction = 1500;
	detected_flag = 0;
    */
    threshold = THRESHOLD;
    /* for drawPath */
    lastRoadCenter.x = (VPE_OUTPUT_W/2)*3;
    lastRoadCenter.y = VPE_OUTPUT_H;
}
/* for drawPath */
void Navigator::drawPath(uint8_t (*src)[VPE_OUTPUT_W*3], uint8_t (*des)[VPE_OUTPUT_W*3])
{
    uint16_t y;
    Point roadCenter;
    for(y=0; y < VPE_OUTPUT_H; y++) {
        roadCenter = getRoadCenter(src, y);
        drawDot(des, roadCenter);
        drawDot(des, getRightPosition(src, y));
        drawDot(des, getLeftPosition(src, y));
    }
}
Navigator::Point Navigator::getRoadCenter(uint8_t (*src)[VPE_OUTPUT_W*3], uint16_t y)
{
    detected_flag = 0;

    Point roadCenter = lastRoadCenter;
    Point right_point = getRightPosition(src,y);
    Point left_point = getLeftPosition(src,y);
    
    if(isRightDetected() & isLeftDetected()) {
        roadCenter.x = (right_point.x + left_point.x)/2;
        roadCenter.y = (right_point.y + left_point.y)/2;
        sideSlope = (float)(roadCenter.y - lastRoadCenter.y)/(roadCenter.x - lastRoadCenter.x);
        lastRoadCenter = roadCenter;
    }
    else if( isRightDetected() ) {
        roadCenter.x = right_point.x / 2;
        roadCenter.y = right_point.y;
        sideSlope = (float)(roadCenter.y - lastRoadCenter.y)/(roadCenter.x - lastRoadCenter.x);
        lastRoadCenter = roadCenter;
    }
    else if( isLeftDetected() ) {
        roadCenter.x = left_point.x + (VPE_OUTPUT_W - left_point.x)/ 2;
        roadCenter.y = left_point.y;
        sideSlope = (float)(roadCenter.y - lastRoadCenter.y)/(roadCenter.x - lastRoadCenter.x);
        lastRoadCenter = roadCenter;
    }
    return roadCenter;
}
Navigator::Point Navigator::getRightPosition(uint8_t (*src)[VPE_OUTPUT_W*3], uint16_t y)
{
    // detect direction from Right
    temp = 0;
    Point point = {0,};
    for(i = VPE_OUTPUT_W / 2; i < VPE_OUTPUT_W; i++) {
        j = 3*i;
        if( src[y][j] )
        {
            for(k=1; k<11; k++) {
                if( src[y][j+3*k] )    temp++;
            }
            if(temp > threshold) {
                RightDetected();
                point = {i, y};
                return point;
            }
        }
    }
}
Navigator::Point Navigator::getLeftPosition(uint8_t (*src)[VPE_OUTPUT_W*3], uint16_t y)
{
    // detect direction from Left
    temp = 0;
    Point point = {0,};
    for(i = VPE_OUTPUT_W / 2; i > 0; i--) {
        j = 3*i;
        if( src[y][j] )
        {
            for(k=1; k<11; k++) {
                if( src[y][j-3*k] )    temp++;
            }
            if(temp > threshold) {
                LeftDetected();
                point = {i, y};
                return point;
            }
        }
    }
}
void Navigator::RightDetected() { detected_flag += 1; }
void Navigator::LeftDetected() { detected_flag += 2; }
bool Navigator::isRightDetected() {
    if(detected_flag & 1) return true;
    else                    return false;
}
bool Navigator::isLeftDetected() {
    if(detected_flag & 2) return true;
    else                    return false;
}
void Navigator::drawDot(uint8_t (*des)[VPE_OUTPUT_W*3], Point point)
{
    uint16_t x,y;
    x = point.x;
    y = point.y;
    #ifdef bgr24
        des[y][3*x+2] = 255;
        des[y][3*x] = des[y][3*x+1] = 0;
    #endif
}
void Navigator::drawBigdot(uint8_t (*des)[VPE_OUTPUT_W*3], Point point)
{
    int32_t i,j;
    uint16_t x,y;
    x = point.x;
    y = point.y;
    #ifdef bgr24
        for(i=-1; i<2; i++) {
            for(j=-1; j<2; j++) {
                des[y+j][3*(x+i)+1] = 255;
                des[y+j][3*(x+i)] = des[y+j][3*(x+i)+2] = 0;
            }
        }
    #endif
}
/*
uint16_t Navigator::getDirection(uint8_t (*src)[VPE_OUTPUT_W*3])
{
    getUpperRightPosition(src);
    getLowerRightPosition(src);
    getUpperLeftPosition(src);
    getLowerLeftPosition(src);

    calculateDirection();
    
	detected_flag = 0;
    printf("direction : %d\n", direction);
	return direction;
}
void Navigator::getUpperRightPosition(uint8_t (*src)[VPE_OUTPUT_W*3])
{
	// detect direction from Right UPPER_LINE
    temp = 0;
    for(i = VPE_OUTPUT_W / 2; i < VPE_OUTPUT_W; i++)
    {
        j = 3*i;
        if( src[UPPER_LINE][j] )
        {
            for(k=1; k<11; k++) {
                if( src[UPPER_LINE][j+3*k] )    temp++;
            }
            if(temp > threshold) {
                right_up = i;
                drawBigdot(src, i, UPPER_LINE);
                UpperRightDetected();
                break;
            }
        }
    }
}
void Navigator::getLowerRightPosition(uint8_t (*src)[VPE_OUTPUT_W*3])
{
	// detect direction from Right LOWER_LINE
    temp = 0;
    for(i = VPE_OUTPUT_W / 2; i < VPE_OUTPUT_W; i++)
    {
        j = 3*i;
        if( src[LOWER_LINE][j] )
        {
            for(k=1; k<11; k++) {
                if( src[LOWER_LINE][j+3*k] )    temp++;
            }
            if(temp > threshold) {
                right_low = i;
                drawBigdot(src, i, LOWER_LINE);
                LowerRightDetected();
                break;
            }
        }
    }
}
void Navigator::getUpperLeftPosition(uint8_t (*src)[VPE_OUTPUT_W*3])
{
	// detect direction from Left UPPER_LINE
    temp = 0;
    for(i = VPE_OUTPUT_W / 2; i > 0; i--)
    {
        j = 3*i;
        if( src[UPPER_LINE][j] )
        {
            for(k=1; k<11; k++) {
                if( src[UPPER_LINE][j-3*k] )    temp++;
            }
            if(temp > threshold) {
                left_up = i;
                drawBigdot(src, i, UPPER_LINE);
                UpperLeftDetected();
                break;
            }
        }
    }
}
void Navigator::getLowerLeftPosition(uint8_t (*src)[VPE_OUTPUT_W*3])
{
    temp = 0;
    for(i = VPE_OUTPUT_W / 2; i > 0; i--)
    {
        j = 3*i;
        if( src[LOWER_LINE][j] )
        {
            for(k=1; k<11; k++) {
                if( src[LOWER_LINE][j-3*k] )    temp++;
            }
            if(temp > threshold) {
                left_low = i;
                drawBigdot(src, i, LOWER_LINE);
                LowerLeftDetected();
                break;
            }
        }
    }
}
void Navigator::UpperRightDetected() { detected_flag += 1; }
void Navigator::LowerRightDetected() { detected_flag += 2; }
void Navigator::UpperLeftDetected()  { detected_flag += 4; }
void Navigator::LowerLeftDetected()  { detected_flag += 8; }
bool Navigator::isRightDetected() {
	if((detected_flag & 1) && (detected_flag & 2)) return true;
	else                                               return false;
}
bool Navigator::isLeftDetected() {
    if((detected_flag & 4) && (detected_flag & 8)) return true;
    else                                               return false;
}
void Navigator::calculateDirection()
{
    float vector;
    if( isRightDetected() & isLeftDetected() )
        vector = (float)(right_up - right_low) / (LOWER_LINE - UPPER_LINE) + (float)(left_up - left_low) / (LOWER_LINE - UPPER_LINE);
    else if( isRightDetected() )
        vector = (float)(right_up - right_low) / (LOWER_LINE - UPPER_LINE);
    else if( isLeftDetected() )
        vector = (float)(left_up - left_low) / (LOWER_LINE - UPPER_LINE);
    else return;

    if(vector > 1.11)         direction = 1000;
    else if (vector < -1.11) direction = 2000;
    else                       direction = (uint16_t)(1500 - 450 * vector);
}
<<<<<<< HEAD
void Navigator::drawDot(uint8_t (*des)[VPE_OUTPUT_W*3], uint16_t x, uint16_t y)
{
    #ifdef bgr24
        des[y][3*x+1] = 255;
        des[y][3*x] = des[y][3*x+2] = 0;
    #endif
}
void Navigator::drawBigdot(uint8_t (*des)[VPE_OUTPUT_W*3], uint16_t x, uint16_t y)
{
    #ifdef bgr24
        for(i=-1; i<2; i++) {
            for(j=-1; j<2; j++) {
                des[y+j][3*(x+i)+1] = 255;
                des[y+j][3*(x+i)] = des[y+j][3*(x+i)+2] = 0;
            }
        }
    #endif
}
/**
  * @ Traffic Lights
  */
bool Navigator::checkTrafficLights(uint8_t (*src)[VPE_OUTPUT_H][VPE_OUTPUT_W*3])
{
    
}
=======
*/
>>>>>>> calibration2
