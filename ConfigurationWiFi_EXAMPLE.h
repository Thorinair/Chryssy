/*******
 * WiFi Configuration
 *******/

/* RENAME THIS FILE TO "ConfigurationWiFi.h" TO GET STARTED.

/* Name of the device, as visible on the WiFi network. */
#define WIFI_HOST "TwiFi_Example"

/* WiFi network definitions. The device will attempt to connect to them in the order as defined here.
 * The first one that works will become the default one once successfully connected.
 * Up to 8 WiFi networks can be defined. Unused ones should be commented out. 
 * Second WiFi here is an example of a static IP. Use the | symbol to separate values:
 *   ip|gateway|subnet|dns1|dns2
 */
#define WIFI_0_SSID "ExampleWiFiName"
#define WIFI_0_PASS "password"
#define WIFI_0_CONF "DHCP"
#define WIFI_0_OPEN false

#define WIFI_1_SSID "AnotherWiFi"
#define WIFI_1_PASS "password"
#define WIFI_1_CONF "192.168.0.201|192.168.0.1|255.255.255.0|8.8.8.8|8.8.4.4"
#define WIFI_1_OPEN false

//#define WIFI_2_SSID ""
//#define WIFI_2_PASS ""
//#define WIFI_2_CONF ""
//#define WIFI_2_OPEN false

//#define WIFI_3_SSID ""
//#define WIFI_3_PASS ""
//#define WIFI_3_CONF ""
//#define WIFI_3_OPEN false

//#define WIFI_4_SSID ""
//#define WIFI_4_PASS ""
//#define WIFI_4_CONF ""
//#define WIFI_4_OPEN false

//#define WIFI_5_SSID ""
//#define WIFI_5_PASS ""
//#define WIFI_5_CONF ""
//#define WIFI_5_OPEN false

//#define WIFI_6_SSID ""
//#define WIFI_6_PASS ""
//#define WIFI_6_CONF ""
//#define WIFI_6_OPEN false

//#define WIFI_7_SSID ""
//#define WIFI_7_PASS ""
//#define WIFI_7_CONF ""
//#define WIFI_7_OPEN false

/* WiFi Timeout, time in seconds until WiFi connection is cancelled if unavailable. */
#define WIFI_TIMEOUT 10

/* EEPROM Addresses. Change these only if needed. */
#define EEPROM_SAVED 0
#define EEPROM_WIFI  1

/* Whether the WiFi connection should be debugged. */
#define WIFI_DEBUG true
