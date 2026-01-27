/**
 * @file main.c
 * @brief C application to read raw 10-bit data from an MCP3008 ADC via /dev/spidevX.Y.
 *
 * This program opens the specified SPI device, sets up the transfer parameters,
 * and executes an SPI transfer to read the raw ADC value from a selected channel.
 *
 * The compiled executable 'adc' will be run on the target RPi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// --- Configuration ---
#define SPI_SPEED_HZ 1000000            // 1 MHz maximum speed for MCP3008
#define BITS_PER_WORD 8                 // 8 bits per word

/**
 * @brief Reads the 10-bit raw ADC value from a specified MCP3008 channel.
 *
 * This function builds the 3-byte command required by the MCP3008, executes the 
 * SPI transaction using ioctl, and extracts the 10-bit result from the 
 * 3-byte response buffer.
 *
 * @param fd File descriptor of the opened SPI device (/dev/spidevX.Y).
 * @param channel The channel number to read (0-7, single-ended mode).
 * @return The 10-bit raw ADC value (0-1023) or -1 on error.
 */
int read_adc_channel(int fd, int channel) {
    // Buffers for transmission (tx_buffer) and reception (rx_buffer)
    uint8_t tx_buffer[3] = {0};
    uint8_t rx_buffer[3] = {0};

    // --- MCP3008 Command Protocol (3-byte sequence) ---
    // The MCP3008 expects 16 clocks to transfer the command, and 12 clocks 
    // to receive the result (2 null bits + 10 data bits). We use a 3-byte transfer (24 clocks).

    // Byte 0: Start Bit (Always 0x01)
    tx_buffer[0] = 0x01; 

    // Byte 1: Configuration Byte
    // Structure (MSB first): [SGL/DIF=1][D2][D1][D0][X][X][X][X]
    // SGL/DIF (Single/Differential): 1 for single-ended mode (default for moisture sensor).
    // D2, D1, D0: The 3-bit channel number.
    // The 0x08 bit sets the SGL/DIF bit (00001000). The channel is shifted left by 4 
    // to put D2, D1, D0 in the correct high-order positions.
    tx_buffer[1] = (0x08 | (channel << 4)); 

    // Byte 2: Dummy byte (0x00)
    // This byte is transmitted to clock out the last 8 bits of the ADC result.
    tx_buffer[2] = 0x00; 

    // --- Linux SPI Transfer Structure (struct spi_ioc_transfer) ---
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buffer,
        .rx_buf = (unsigned long)rx_buffer,
        .len = 3, // Transfer 3 bytes
        .speed_hz = SPI_SPEED_HZ,
        .bits_per_word = BITS_PER_WORD,
        .delay_usecs = 0,
    };

    // Execute the transfer using ioctl
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        perror("Error during SPI_IOC_MESSAGE_1");
        return -1;
    }

    // --- Result Processing (10-bit result) ---
    // The 10-bit result is spread across rx_buffer[1] and rx_buffer[2].
    // rx_buffer[1] contains the high 2 bits of the result (Bits 9 and 8)
    // rx_buffer[2] contains the low 8 bits of the result (Bits 7 down to 0)

    // 1. Get the high 2 bits (Bits 9-8): They are in the low 2 bits of rx_buffer[1].
    //    We mask with 0x03 (binary 00000011) and shift left by 8 bits.
    int adc_value = (rx_buffer[1] & 0x03) << 8;

    // 2. Add the low 8 bits (Bits 7-0): This is the entire contents of rx_buffer[2].
    adc_value |= rx_buffer[2];

    return adc_value;
}

int main(int argc, char *argv[]) {
    // Check for correct arguments: program name, device path, and channel number
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <spi_device> <channel_number>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/spidev0.0 0\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *device = argv[1];
    int channel = atoi(argv[2]);

    if (channel < 0 || channel > 7) {
        fprintf(stderr, "Error: Channel number must be between 0 and 7.\n");
        return EXIT_FAILURE;
    }

    // --- 1. Open and Configure SPI Device ---
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        // Output detailed error message
        fprintf(stderr, "Failed to open device %s: %s\n", device, strerror(errno));
        return EXIT_FAILURE;
    }

    // Setup required SPI parameters (Mode 0, 8 bits/word, Speed)
    uint8_t mode = 0;
    uint8_t bits = BITS_PER_WORD;
    uint32_t speed = SPI_SPEED_HZ;

    // Set Mode
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) { goto error_close; }
    // Set Bits per Word
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) { goto error_close; }
    // Set Speed
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) { goto error_close; }


    // --- 2. Read the Value and Output ---
    int raw_value = read_adc_channel(fd, channel);

    if (raw_value != -1) {
/*        printf("--- MCP3008 ADC Readout ---\n");
        printf("Device: %s\n", device);
        printf("Channel: %d\n", channel);
        printf("Raw 10-bit ADC Value: %d (Max 1023)\n", raw_value);

        // Assuming VREF=3.3V, convert raw reading to voltage
        float voltage = (float)raw_value * 3.3 / 1023.0;
        printf("Estimated Voltage (VREF=3.3V): %.3f V\n", voltage);

        // Simple application logic for a moisture sensor
        printf("\nMoisture Status:\n");
        printf("- High ADC value = Low Moisture (Dry)\n");
        printf("- Low ADC value = High Moisture (Wet)\n");
        printf("Current Status: %s\n",
               (raw_value > 700) ? "DRY (Needs Water)" : 
               (raw_value > 300) ? "MOIST (Optimal)" : 
               "WET (Saturated)");
               */
               
        printf("%d\n", raw_value);
        fflush(stdout);
    }

    // --- 3. Clean up ---
    close(fd);
    return (raw_value != -1) ? EXIT_SUCCESS : EXIT_FAILURE;

error_close:
    perror("SPI configuration error");
    close(fd);
    return EXIT_FAILURE;
}
