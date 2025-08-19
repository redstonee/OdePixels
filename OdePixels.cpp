#include <cstring>
#include "esp_log.h"
#include "OdePixels.h"

/**
 * @brief Initialize the SPI bus for WS2812 communication
 *  (and maybe allocate pixel data buffer)
 *
 * @param pixelPin The GPIO pin connected to the WS2812 data line
 * @param spiHost The SPI host to use, default is SPI2_HOST
 * @param frequency The SPI clock frequency in Hz, default is 8e6
 */
bool OdePixels::init(int pixelPin, spi_host_device_t spiHost, int frequency)
{
    // Try dynamic allocation if the buffer is set to nullptr
    if (!pixelData)
    {
        pixelData = new uint32_t[pixelCount];
    }

    // Check if pixelData was allocated successfully
    if (!pixelData)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for pixel data");
        return false;
    }

    spi_bus_config_t buscfg = {
        .mosi_io_num = pixelPin,
        .miso_io_num = -1,
        .sclk_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10000};
    spi_device_interface_config_t devcfg = {
        .mode = 0,
        .clock_speed_hz = frequency,
        .spics_io_num = -1,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(spiHost, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(spiHost, &devcfg, &spiDev));

    // Turn off all pixels
    clear();
    show();
    return true;
}

/**
 * @brief Send the pixel data to the WS2812 LEDs
 *
 */
void OdePixels::show()
{
    uint8_t data[pixelCount * 24 + 1];
    uint8_t *p = data;
    *p++ = WS2812_RESET; // reset the data line
    for (uint8_t i = 0; i < pixelCount; i++)
    {
        uint32_t pixel = pixelData[i];
        for (uint8_t j = 0; j < 24; j++)
        {
            *p++ = (pixel & 0x800000) ? WS2812_H : WS2812_L;
            pixel <<= 1;
        }
    }

    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    trans.length = sizeof(data) * 8; // length in bits
    trans.tx_buffer = data;
    ESP_ERROR_CHECK(spi_device_polling_transmit(spiDev, &trans));
}

/**
 * @brief Create color from RGB values
 *
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return uint32_t RGB color value
 */
uint32_t OdePixels::rgb2u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

/**
 * @brief Create color from HSV values
 *
 * @param hue Hue component (0-360)
 * @param sat Saturation component (0-255)
 * @param val Value component (0-255)
 * @return uint32_t RGB color value
 */
uint32_t OdePixels::hsv2u32(uint16_t hue, uint8_t sat, uint8_t val)
{
    uint8_t r, g, b;

    // Convert HSV to RGB
    uint8_t region = hue / 43;
    uint8_t remainder = (hue - (region * 43)) * 6;

    uint8_t p = (val * (255 - sat)) >> 8;
    uint8_t q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
    uint8_t t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        r = val;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = val;
        b = p;
        break;
    case 2:
        r = p;
        g = val;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = val;
        break;
    case 4:
        r = t;
        g = p;
        b = val;
        break;
    default:
        r = val;
        g = p;
        b = q;
        break;
    }

    return rgb2u32(r, g, b);
}

/**
 * @brief Set the color of one pixel
 *
 * @param n The index of the pixel
 * @param color The color value as uint32_t
 */
void OdePixels::setPixelColor(uint32_t n, uint32_t color)
{
    if (n >= pixelCount)
    {
        ESP_LOGE(TAG, "Invalid pixel index: %lu", n);
        return;
    }
    pixelData[n] = color;
}

/**
 * @brief Fill the specified range of pixels with a color
 *
 * @param color The color to fill the pixels with
 * @param first The index of the first pixel to fill, default is 0
 * @param count The number of pixels to fill, default is 0, for all remaining pixels
 */
void OdePixels::fill(uint32_t color, uint32_t first, uint32_t count)
{
    auto end = first + count;
    if (end > pixelCount || count == 0)
        end = pixelCount;

    for (uint32_t i = first; i < end; i++)
    {
        pixelData[i] = color;
    }
}

/**
 * @brief Clear all pixels
 *
 */
void OdePixels::clear()
{
    for (uint32_t i = 0; i < pixelCount; i++)
    {
        pixelData[i] = 0;
    }
}
