#include "thermo.h"

int set_temp_from_ports(temp_t *temp){
// Uses the two global variables (ports) THERMO_SENSOR_PORT and
// THERMO_STATUS_PORT to set the fields of `temp`. If
// THERMO_SENSOR_PORT is negative or above its maximum trusted value
// (associated with +45.0 deg C), this function sets the
// tenths_degrees to 0 and the temp_mode to 3 for `temp` before
// returning 1.  Otherwise, converts the sensor value to deg C using
// shift operations.  Further converts to deg F if indicated from
// THERMO_STATUS_PORT. Sets the fields of `temp` to appropriate
// values. `temp_mode` is 1 for Celsius, and 2 for Fahrenheit. Returns
// 0 on success. This function DOES NOT modify any global variables
// but may access them.
//
// CONSTRAINTS: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU. Does
// not use any math functions such as abs().
    if(THERMO_SENSOR_PORT < 0 || THERMO_SENSOR_PORT > 28800 || THERMO_STATUS_PORT & (1 << 2)){ // if invalid input
        temp->tenths_degrees = 0;
        temp->temp_mode = 3;
        return 1;
    }
    temp->tenths_degrees = THERMO_SENSOR_PORT; // get the sensor's units
    if(temp->tenths_degrees & (1 << 4)){ // if the remainder >= 16, round up
        temp->tenths_degrees += 32 - (temp->tenths_degrees % 32);
    }
    temp->tenths_degrees = temp->tenths_degrees >> 5; // divide sensor units by 32 via shifting
    //printf("tenth degrees is %d\n", temp->tenths_degrees);
    temp->tenths_degrees = -450 + temp->tenths_degrees; // get the number of tenth degrees C above -45.0 C
    if(THERMO_STATUS_PORT & (1 << 5)){ // if port is F mode 
        temp->tenths_degrees = (temp->tenths_degrees * 9) / 5 + 320; // convert to F
        temp->temp_mode = 2; // set F mode
    }
    else{
        temp->temp_mode = 1; // set C mode
    }
    
    return 0;
}

int set_display_from_temp(temp_t temp, int *display){
// Alters the bits of integer pointed to by display to reflect the
// temperature in struct arg temp.  If temp has a temperature value
// that is below minimum or above maximum temperature allowable or if
// the temp_mode is not Celsius or Fahrenheit, sets the display to
// read "ERR" and returns 1. Otherwise, calculates each digit of the
// temperature and changes bits at display to show the temperature
// according to the pattern for each digit.  This function DOES NOT
// modify any global variables except if `display` points at one.
// 
// CONSTRAINTS: Uses only integer operations. No floating point
// operations are used as the target machine does not have a FPU. Does
// not use any math functions such as abs().

    *display = *display ^ *display; // deref-setting display to 0
    if((temp.tenths_degrees < -450 && temp.temp_mode == 1) || (temp.tenths_degrees > 450  && temp.temp_mode == 1) ||
    (temp.tenths_degrees < -490 && temp.temp_mode == 2) || (temp.tenths_degrees > 1130  && temp.temp_mode == 2) ||
    temp.temp_mode < 1 || temp.temp_mode > 2){ // if invalid input, set display with the ERR message and return 1
    *display = *display | (0b0110111 << 21); // set 'E'
    *display = *display | (0b1011111 << 14); // set 'R'
    *display = *display | (0b1011111 << 7); // set 'R'
    return 1;
    }

    int *masks = malloc(10 * sizeof(int)); // array of bit masks for 0-9
    masks[0] = 0b1111011;
    masks[1] = 0b1001000;
    masks[2] = 0b0111101;
    masks[3] = 0b1101101;
    masks[4] = 0b1001110;
    masks[5] = 0b1100111;
    masks[6] = 0b1110111;
    masks[7] = 0b1001001;
    masks[8] = 0b1111111;
    masks[9] = 0b1101111;

    int is_negative = 0; // determines if the temp is negative
    int temp_value = temp.tenths_degrees;
    if (temp_value < 0){ // if the temp is negative, make it positive
        is_negative = 1; // set value to true
        temp_value = temp_value * (-1);

    }
    
    // getting each digit from the temp
    int temp_tenths = temp_value % 10; // tenths digit
    int temp_ones = (temp_value % 100) / 10; // ones digit
    int temp_hundreds = temp_value / 100; // tens digit

    if(temp.temp_mode == 1){ // set C sign on
        *display = *display | 1 << 28;
    }
    else if(temp.temp_mode == 2){ // set F mode
        *display = *display | 1 << 29;
    }

    // set the tenth digit
    *display = *display | masks[temp_tenths]; // set first 7 bits of the display

    // set the ones digit
    *display = *display | (masks[temp_ones] << 7); // set next 7 bits of the display
    
    // set the tens digit
    if(temp_hundreds != 0){ // if the tens digit is not 0, set the tens digit
        if(temp_hundreds < 10){ // if the tens value is a single digit number
            *display = *display | (masks[temp_hundreds] << 14); // set the next bits of the display
            if(is_negative){ // if the degrees are negative, set the negative sign in the display
                *display = *display | 1 << 23;
            }
        }
        else{ // if the tens value is a double digit number
            *display = *display | (masks[1] << 21); // set the last display space to 1
            *display = *display | (masks[temp_hundreds - 10] << 14); // set the digit to the right of the one
        }
    }
    else{ // if the tens digit is 0
        if(is_negative){ // if the degrees is negative, set the negative sign in the display
        *display = *display | 1 << 16;
        }
    }
    
    free(masks); // free the masks array
    return 0;
}

int thermo_update(){
// Called to update the thermometer display.  Makes use of
// set_temp_from_ports() and set_display_from_temp() to access
// temperature sensor then set the display. Always sets the display
// even if the other functions returns an error. Returns 0 if both
// functions complete successfully and return 0. Returns 1 if either
// function or both return a non-zero values.
// 
// CONSTRAINT: Does not allocate any heap memory as malloc() is NOT
// available on the target microcontroller.  Uses stack and global
// memory only.
temp_t temp = {.tenths_degrees = THERMO_SENSOR_PORT, .temp_mode = THERMO_STATUS_PORT}; // create a temp struct
// check what each function returns
int func1 = set_temp_from_ports(&temp); 
int func2 = set_display_from_temp(temp, &THERMO_DISPLAY_PORT);
if(func1 || func2){ // if either one of the functions return a non zero, return 1
    return 1;
}
    return 0;
}