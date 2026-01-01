// Typedef for the colors of the LEDs.
typedef struct
{
    uint16_t hue;
    uint8_t sat;
    uint8_t val;
}hsvColorTypedef;

// LED colors - I tried to emulate the old xmas incandescent lights with added new cool and warm white.
static hsvColorTypedef xmasColors[] =
{
        {       // Red color
                .hue = (uint16_t)(0.0 / 360.0 * 65535),
                .sat = (uint8_t)(94.0 / 100.0 * 255.0),
                .val = 13
        },
        {       // Green color
                .hue = (uint16_t)(105.0 / 360.0 * 65535),
                .sat = (uint8_t)(82.0 / 100.0 * 255.0),
                .val = 5
        },
        {       // Yellow color
                .hue = (uint16_t)(35.0 / 360.0 * 65535),
                .sat = (uint8_t)(98.0 / 100.0 * 255.0),
                .val = 15
        },
        {       // Blue color
                .hue = (uint16_t)(202.0 / 360.0 * 65535),
                .sat = (uint8_t)(94.0 / 100.0 * 255.0),
                .val = 4
        },
        {       // Pink color
                .hue = (uint16_t)(359.0 / 360.0 * 65535),
                .sat = (uint8_t)(80.0 / 100.0 * 255.0),
                .val = 13
        },
        {       // White color
                .hue = (uint16_t)(200.0 / 360.0 * 65535),
                .sat = (uint8_t)(20.0 / 100.0 * 255.0),
                .val = 5
        },
        {       // Warm white color
                .hue = (uint16_t)(30.0 / 360.0 * 65535),
                .sat = (uint8_t)(81.0 / 100.0 * 255.0),
                .val = 10
        }
};
