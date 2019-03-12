/*
 * The main configuration file.
*/

/********
 * Battery Status
 *******/

/* Use voltage instead of percentage. */
#define BATTERY_VOLTAGE false

/* Voltage conversion multiplier. Should be adjusted so that the reported voltage is same as measured on battery. 
 * You will need to enable battery voltage above to tune this.
 * Use the following formula:  NewMultiplier = OldMultiplier * (MeasuredVoltage / ReportedVoltage) */
#define BATTERY_MULTIPLIER 5.622
 
/* Battery voltage when empty. */
#define BATTERY_V_MIN 2.56

/* Battery voltage when full. */
#define BATTERY_V_MAX 4.18
