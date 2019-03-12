#ifndef TWIFI
#define TWIFI

/* WiFI Connection Results */
#define WIFI_RESULT_FAIL 0
#define WIFI_RESULT_DONE 1

/* Connects to a WiFi network based on the configuration. */
int connectWiFi(bool repeatAttempts);

#endif
