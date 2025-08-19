#pragma once

#include <cstdint>
#include <driver/spi_master.h>

class OdePixels
{
private:
    spi_device_handle_t spiDev;

    // WS2812 data buffer in GRB888 format
    uint32_t pixelCount;
    uint32_t *pixelData;

    static constexpr auto TAG = "OdePixels";
    static constexpr uint8_t WS2812_H = 0b11111000;
    static constexpr uint8_t WS2812_L = 0b11100000;
    static constexpr uint8_t WS2812_RESET = 0;

public:
    /**
     * @brief Construct a new OdePixels object
     *
     * @param count The count of pixels in series
     * @param buffer The data buffer for pixel colors,
     * could be `nullptr` to allocate a new buffer dynamically
     * or a pointer to an existing buffer with size of `count * 4`
     * like `uint32_t buffer[count]`
     */
    OdePixels(uint32_t count, void *buffer = nullptr)
        : pixelCount(count),
          pixelData(static_cast<uint32_t *>(buffer)) {}

    bool init(int pixelPin,
              spi_host_device_t spiHost = SPI2_HOST,
              int frequency = 8'000'000);

    static uint32_t rgb2u32(uint8_t r, uint8_t g, uint8_t b);
    static uint32_t hsv2u32(uint16_t hue, uint8_t sat, uint8_t val);

    // WS2812 control functions
    void setPixelColor(uint32_t n, uint32_t color);

    /**
     * @brief Set the color of one pixel
     *
     * @param n The index of the pixel
     * @param r The red component (0-255)
     * @param g The green component (0-255)
     * @param b The blue component (0-255)
     */
    inline void setPixelColor(uint32_t n, uint8_t r, uint8_t g, uint8_t b)
    {
        setPixelColor(n, rgb2u32(r, g, b));
    }

    /**
     * @brief Set the color of one pixel using HSV values
     *
     * @param n The index of the pixel
     * @param hue The hue component (0-360)
     * @param sat The saturation component (0-255)
     * @param val The value component (0-255)
     */
    inline void setPixelColorHSV(uint32_t n, uint16_t hue, uint8_t sat, uint8_t val)
    {
        setPixelColor(n, hsv2u32(hue, sat, val));
    }

    void fill(uint32_t color, uint32_t first = 0, uint32_t count = 0);
    void clear();
    void show();

    inline const uint32_t *getPixelData() const { return pixelData; }
};
