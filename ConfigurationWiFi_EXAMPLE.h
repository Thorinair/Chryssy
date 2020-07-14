/*******
 * WiFi Configuration
 *******/

/* RENAME THIS FILE TO "ConfigurationWiFi.h" TO GET STARTED.

/* Name of the device, as visible on the WiFi network. */
#define WIFI_HOST "Example_Device"

/* Number of WiFi networks. */
#define WIFI_COUNT 2

/* Individual WiFis. */
TwiFiEntry wifis[WIFI_COUNT] = {
    {"ExampleWiFiName", "password", "DHCP", false},
    {"AnotherWiFi",     "password", "192.168.0.201|192.168.0.1|255.255.255.0|8.8.8.8|8.8.4.4", false}
};

/* WiFi Timeout, time in seconds until WiFi connection is cancelled if unavailable. */
#define WIFI_TIMEOUT 10

/* Whether the WiFi connection should be debugged. */
#define WIFI_DEBUG true
