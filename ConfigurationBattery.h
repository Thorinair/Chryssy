/********
 * Battery Status
 *******/

 /* 
 * To calibrate the battery, enable the BATTERY_VOLTAGE display.
 * Let the battery completely drain and monitor the values on the display. Set the 
 * BATTERY_V_MIN to the lowest measured value when Chryssy has shut down.
 * Then, allow Chryssy to completely charge up and enter the measured value (when not
 * plugged in) as BATTERY_V_MAX.
 * Once the calibration is over, disable the BATTERY_VOLTAGE again.
 */

/* Use voltage instead of percentage. */
#define BATTERY_VOLTAGE false
 
/* Battery voltage when empty. */
#define BATTERY_V_MIN 2.38

/* Battery voltage when full. */
#define BATTERY_V_MAX 3.94
