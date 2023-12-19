class DeviceValues {
public:
    int humidity;
    int soilMoisture;
    int temperature;
    int light;

    DeviceValues(int hum, int n2, int n3, int n4) {
        humidity = hum;
        soilMoisture = n2;
        temperature = n3;
        light = n4;
    }
};
struct WifiData {
    char ssid[64];
    char password[64];
};
