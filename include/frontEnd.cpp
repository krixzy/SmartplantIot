#include <Arduino_MKRIoTCarrier.h>
#include <WiFiNINA.h>
#include <secrets.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include "functions.h"
#include "visuals.h"
#include "classes.h"


MKRIoTCarrier carrier;
bool ledOn = true;
void optionsText();

void onStartUp(bool wifiConnected)
{
    carrier.display.fillScreen(0x0);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(40, 60);
    carrier.display.setTextWrap(true);

    carrier.display.print("er det foerste");
    carrier.display.setCursor(30, 80);
    carrier.display.print("gang du bruger");
    carrier.display.setCursor(20, 100);
    carrier.display.print(" denne arduino?");
    carrier.display.setCursor(50, 170);
    carrier.display.print("NEJ");
    carrier.display.setCursor(160, 170);
    carrier.display.print("JA");
    carrier.display.drawBitmap(35, 180, arrow_left, 61, 61, 0xffff);
    carrier.display.drawBitmap(140, 180, arrow_right, 61, 61, 0xffff);
    carrier.leds.setBrightness(25);
    carrier.leds.setPixelColor(1, 0, 255, 0);
    carrier.leds.setPixelColor(3, 255, 0, 0);
    carrier.leds.show();
    
    bool waitingForInput = true;
    while (waitingForInput)
    {
        carrier.Buttons.update();
        if (carrier.Buttons.onTouchDown(TOUCH1))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitingForInput = false;
            connectAp();
            calibratedMoistureSensor();
            showQrCode();

           
        }
        if (carrier.Buttons.onTouchDown(TOUCH3))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitingForInput = false;
            updateUserInterface(wifiConnected);


            
           
        }
    }


}

void arduinoApConnectionText()
{
    carrier.display.setTextSize(2);
    carrier.display.fillScreen(0x0);
    carrier.display.setCursor(30, 60);
    carrier.display.print("aaben dine wifi");
    carrier.display.setCursor(20, 80);
    carrier.display.print("neetvaerks og find");
    carrier.display.setCursor(10, 100);
    carrier.display.print("netvaerket Arduino");
    carrier.display.setCursor(30, 120);
    carrier.display.print("WiFi og connect");
    carrier.display.setCursor(10, 140);
    carrier.display.print("aaben en browser");
    carrier.display.setCursor(10, 160);
    carrier.display.print("skriv 192.168.4.1");


}

void arduinoApInstruction()
{
carrier.display.setTextSize(2);
carrier.display.fillScreen(0x0);
carrier.display.setCursor(20, 60);
carrier.display.print("nu skal du skrive");
carrier.display.setCursor(20, 80);
carrier.display.print("dit wifi navn");
carrier.display.setCursor(20, 100);
carrier.display.print("og kodeord ind");
carrier.display.setCursor(20, 120);
carrier.display.print("og saa tryk paa");
carrier.display.setCursor(20, 140);
carrier.display.print("Connect knappen");
}


void drawWifiOn()
{

    carrier.display.fillRect(105, 25, 5, 5, 0xffff);
    carrier.display.fillRect(115, 20, 5, 10, 0xffff);
    carrier.display.fillRect(125, 15, 5, 15, 0xffff);
    carrier.display.fillRect(135, 10, 5, 20, 0xffff);

}
void drawWifiOff()
{
    carrier.display.drawRect(105, 25, 5, 5, 0xffff);
    carrier.display.drawRect(115, 20, 5, 10, 0xffff);
    carrier.display.drawRect(125, 15, 5, 15, 0xffff);
    carrier.display.drawRect(135, 10, 5, 20, 0xffff);

}

void optionsLight()
{
    carrier.leds.setPixelColor(4, 0, 255, 0);
    carrier.leds.setPixelColor(0, 0, 255, 0);
    carrier.leds.setPixelColor(2, 0, 0, 255);
    carrier.leds.setPixelColor(3, 0, 255, 0);

}









void optionInputListener(bool waitforInput)
{
    while (waitforInput)
    {
        carrier.Buttons.update();
        if (carrier.Buttons.onTouchDown(TOUCH0))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitforInput = false;
            calibratedMoistureSensor();

        }
        if (carrier.Buttons.onTouchDown(TOUCH4))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitforInput = false;
            connectAp();

        }
        if (carrier.Buttons.onTouchDown(TOUCH2))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitforInput = false;
            updateUserInterface(true);
        }
        if (carrier.Buttons.onTouchDown(TOUCH3))
        {
            ledOn = !ledOn;
            if (ledOn)
            {
                carrier.display.fillScreen(0);
                optionsText();
                carrier.display.print("Sluk led");
                optionsLight();
                carrier.leds.show();


            }else
            {
                carrier.display.fillScreen(0);
                optionsText();
                carrier.display.print("tænd led");
                carrier.leds.clear();
                carrier.leds.show();

            }
            delay(100);
           
        }
       
        
    }

}

void optionsText()
{
    carrier.display.setTextSize(1,2);
    carrier.display.setCursor(30, 80);
    carrier.display.print("nyt wifi id");
    carrier.display.drawBitmap(20, 30, arrow_left, 61, 61, 0xffff);
    carrier.display.setCursor(130, 80);
    carrier.display.print("kalibrer soil");
    carrier.display.drawBitmap(160, 30, arrow_right, 61, 61, 0xffff);
    carrier.display.setCursor(100, 200);
    carrier.display.print("tilbage");
    carrier.display.drawBitmap(88, 200, arrow_down, 61, 61, 0xffff);
    carrier.display.setCursor(30, 170);
    carrier.display.drawBitmap(30, 180, arrow_left, 61, 61, 0xffff);
}








void showOptions()
{
    
    delay(200);
    carrier.display.fillScreen(0);
    carrier.leds.clear();
    carrier.leds.show();    
    optionsText();
    carrier.leds.setBrightness(25);
    if (ledOn)
    {
        carrier.display.print("Sluk led");
        optionsLight();
        carrier.leds.show();

    }
    else
    {
        carrier.display.print("taend led");
        
    }
    
    carrier.display.print("");
    


    bool waitforInput = true;
    optionInputListener(waitforInput);

    


}



void dataText()
{

    carrier.leds.setBrightness(25);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(80, 117);
    carrier.display.print("Loading");
    DynamicJsonDocument value = updateValues();
    // DynamicJsonDocument temperature = readTemperature();
    // DynamicJsonDocument humidity = readHumidity();
    // DynamicJsonDocument soilMoisture = readMoistureSensor();
    // DynamicJsonDocument light = readLight();  
    carrier.display.fillScreen(0);
    carrier.display.setTextSize(1,2);






    carrier.display.setCursor(30, 80);
    carrier.display.print("Luft fugtighed:");
    carrier.display.setCursor(170, 80);
    serializeJson(value["humidity"]["value"], carrier.display);
    carrier.display.setCursor(30, 100);
    carrier.display.print("Jord fugtighed:");
    carrier.display.setCursor(170, 100);
    serializeJson(value["soilMoisture"]["value"], carrier.display);
    carrier.display.setCursor(30, 120);
    carrier.display.print("Temperatur:");
    carrier.display.setCursor(170, 120);
    serializeJson(value["temperature"]["value"], carrier.display);
    carrier.display.setCursor(30, 140);
    carrier.display.print("Lys:");
    carrier.display.setCursor(170, 140);
    serializeJson(value["light"]["value"], carrier.display);
    carrier.display.setCursor(100, 200);
    carrier.display.print("Tilbage");
    carrier.display.drawBitmap(88, 200, arrow_down, 61, 61, 0xffff);
}


void dataInputListener(bool waitForInput, long startMillis)
 {
   while (waitForInput)
   {
        carrier.Buttons.update();
        if (carrier.Buttons.onTouchDown(TOUCH2))
        {
            carrier.leds.clear();
            carrier.leds.show();
            carrier.display.fillScreen(0);
            waitForInput = false;
            updateUserInterface(true);
        }
        if (millis() - startMillis > 1000 * 60 * 5)
        {
            waitForInput = false;
            updateUserInterface(true);  
        }
        
   }
    
 }






void showData()
{
    unsigned long startMillis = millis();
    delay(200);
    carrier.display.fillScreen(0);
    carrier.leds.clear();
    carrier.leds.show();
    dataText();
   




    if (ledOn)
    {
        carrier.leds.setPixelColor(2, 0, 0, 255);
        carrier.leds.show();
    }
    
    dataInputListener(true, startMillis);

}




void smilySettings(DeviceValues values)
{

    if(values.soilMoisture < 15 && values.soilMoisture > 5 || values.light < 10 )
    {
        carrier.display.drawBitmap(80, 60, sadSmily, 90, 90, 0xF800);
    }
    else if (values.soilMoisture < 40 && values.soilMoisture >= 15 )
    {
        carrier.display.drawBitmap(80, 60, neutralSmily, 90, 90, 0xFFE0);
    }
    else if(values.soilMoisture < 90 && values.soilMoisture >= 40)
    {
        carrier.display.drawBitmap(80, 60, happySmily, 90, 90, 0x07E0);
    }
    else
    {

        carrier.display.drawBitmap(80, 60, deadSmily, 90, 90, 0xffff);
    }
}



void updateUserInterface( bool wifiConnected)
{
    carrier.display.fillScreen(0);
    carrier.display.setTextSize(2);
    carrier.display.setCursor(80, 117);
    carrier.display.print("Loading");
    DynamicJsonDocument values = updateValues();
    DeviceValues deviceValues(values["humidity"]["value"].as<int>(), values["soilMoisture"]["value"].as<int>(), values["temperature"]["value"].as<int>(), values["light"]["value"].as<int>());





    delay(200);
    carrier.display.fillScreen(0);
    smilySettings(deviceValues);

    if (wifiConnected)
    {
        drawWifiOn();
    }
    else
    {
        drawWifiOff();
    }

    carrier.display.setTextSize(1,2);
    carrier.display.setCursor(85, 205);
    carrier.display.print("Indstillinger");
    carrier.display.drawBitmap(88, 200, arrow_down, 61, 61, 0xffff);
    carrier.leds.setBrightness(25);
    carrier.display.setCursor(30, 170);
    carrier.display.print("Vis data");
    carrier.display.drawBitmap(20, 170, arrow_left, 61, 61, 0xffff);


    if (ledOn)
    {
    carrier.leds.setPixelColor(2, 0, 0, 255);
    carrier.leds.setPixelColor(3, 0, 220, 40);
    carrier.leds.show();
    }
    else
    {
        carrier.leds.clear();
        carrier.leds.show();
    }
    
     
    
}