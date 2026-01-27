#include <gpiod.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17   /* BCM numbering */

int main(int argc, char *argv[])
{
    struct gpiod_chip *chip;
    struct gpiod_line_settings *settings;
    struct gpiod_line_config *line_cfg;
    struct gpiod_request_config *req_cfg;
    struct gpiod_line_request *request;

    unsigned int line = GPIO_LINE;
    int value;

    if (argc != 2) {
        fprintf(stderr, "Usage: pump <on|off>\n");
        return 1;
    }

    if (strcmp(argv[1], "on") == 0)
        value = 1;
    else if (strcmp(argv[1], "off") == 0)
        value = 0;
    else {
        fprintf(stderr, "Invalid command: %s\n", argv[1]);
        return 1;
    }

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("gpiod_chip_open");
        return 1;
    }

    /* Allocate objects */
    settings = gpiod_line_settings_new();
    line_cfg = gpiod_line_config_new();
    req_cfg  = gpiod_request_config_new();

    if (!settings || !line_cfg || !req_cfg) {
        fprintf(stderr, "Failed to allocate gpiod objects\n");
        goto cleanup;
    }

    /* Configure GPIO line */
    gpiod_line_settings_set_direction(
        settings, GPIOD_LINE_DIRECTION_OUTPUT);

    gpiod_line_settings_set_output_value(
        settings,
        value ? GPIOD_LINE_VALUE_ACTIVE
              : GPIOD_LINE_VALUE_INACTIVE);

    gpiod_line_config_add_line_settings(
        line_cfg, &line, 1, settings);

    /* Request configuration */
    gpiod_request_config_set_consumer(req_cfg, "pump");

    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if (!request) {
        perror("gpiod_chip_request_lines");
        goto cleanup;
    }

    printf("Pump %s\n", value ? "ON" : "OFF");

    sleep(1);

    gpiod_line_request_release(request);

cleanup:
    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);

    return 0;
}

