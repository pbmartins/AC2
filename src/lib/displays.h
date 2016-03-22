#ifndef __DISPLAYS__
#define __DISPLAYS__

/***************************************************/
/* Declaration of the codes of 7 seg displays      */
/***************************************************/
static const unsigned char display7Scodes[] = {
        0b01110111,                                // 0
        0b01000001,                                // 1
        0b00111011,                                // 2
        0b01101011,                                // 3
        0b01001101,                                // 4
        0b01101110,                                // 5
        0b01111110,                                // 6
        0b01000011,                                // 7
        0b01111111,                                // 8
        0b01101111,                                // 9
        0b01011111,                                // a
        0b01111100,                                // b
        0b00110110,                                // c
        0b01111001,                                // d
        0b00111110,                                // e
        0b00011110,                                // f
        0b00000000                                 // null
};

/***************************************************/
/* Convert char to binary coded value              */
/***************************************************/
unsigned char toBcd(unsigned char value);

/***************************************************/
/* Turn on the dots on 7seg displays               */
/* Accepts a char that indicates which point is on */
/* 0x0011 -> both on                               */
/* 0x0010 -> just high on                          */
/* 0x0001 -> just low on                           */
/***************************************************/
void send2dots(unsigned char value);

/***************************************************/
/* Displays a char on 7 seg display                */
/***************************************************/
void send2displays(unsigned char value);

#endif /* __DISPLAYS__ */