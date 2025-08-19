# OdePixels

A library for controlling WS2812 RGB LEDs with SPI for ESP32.  
Some libraries control WS2812 LEDs using RMT (Remote Control) peripheral,
which is not available on a few ESP32 chips like ESP8684.
So this library uses SPI to control the LEDs instead.

This library uses a buffer to hold the pixel data,
which could be allocated at runtime or specified at compile time.  
The pixel data won't be sent to the LEDs until `show()` is called.  
When using a static buffer, make sure it is not destroyed before the `OdePixels` object is done using it.  

## Compatibility

All ESP32 chips are expected to be compatible, but I've not fully tested.  
The following table summarizes the compatibility status:

| Chip | ESP-IDF (5.0+) | Platform IO |
|------|---------|---------|
| ESP32 |   ✅    |   ✅    |
| ESP32-C2 |   ✅    |   ❔    |

*ESP32-C2 seems not supported by PlatformIO for now*

***✅ is supported, ❔ is not tested***

## Installation

### For ESP-IDF projects

Just clone this repo into the `components` directory of your ESP-IDF project.

### For PlatformIO projects

1. Add the following to your `platformio.ini` file:

    ```ini
    [env:your_env] ;Replace `your_env` with the name of your environment.
    lib_deps =
        https://github.com/redstonee/OdePixels.git
    ```

2. Update the project by either running `pio update` in the terminal
or using the VS Code PlatformIO extension to install the new library.

3. Enjoy it!

## Usage

0. Include the library in your project:

    ```cpp
    #include "OdePixels.h"
    ```

1. Create an `OdePixels` object:

    ```cpp
    // 24 pixels, with data buffer dynamically allocated at runtime
    OdePixels pixels(24); 
    ```

    or

    ```cpp
    constexpr auto numPixels = 24;

    // Data buffer for the pixels,
    // with a size of at least 4 * numPixels bytes
    uint32_t buffer[numPixels];
    
    // 24 pixels, with a specified data buffer
    OdePixels pixels(numPixels, buffer); 
    ```

2. Call the `init()` method to initialize the SPI controller:

    ```cpp
    pixels.init(6); // GPIO 6
    ```

3. Set pixel colors using `setPixelColor()`, `setPixelColorHSV()` or `fill()`:

    ```cpp
    pixels.setPixelColor(0, 255, 0, 0); // Set pixel 0 to red

    pixels.setPixelColor(1, 0x00ff00); // Set pixel 1 to green

    pixels.setPixelColorHSV(2, 120, 255, 255); // Set pixel 2 to green

    pixels.fill(0x0000ff, 3, 5); // Set pixels 3, 4, 5, 6, 7 to blue

    pixels.fill(OdePixels::rgb2u32(255, 255, 255)); // Set all pixels to white
    ```

4. Call `show()` to send the pixel data to the LEDs:

    ```cpp
    pixels.show(); // Send the pixel data to the LEDs
    ```