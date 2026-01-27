#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define ADC_DEVICE   "/dev/spidev0.0"
#define ADC_CHANNEL  0

#define DRY_THRESH   400
#define WET_THRESH   450

#define INTERVAL_S         5
#define TRIAL_INTERVAL_S   1

static int pump_on = 0;

static void pump_on_cmd(void)
{
    if (!pump_on) {
        system("/usr/bin/pump on");
        pump_on = 1;
        printf("[sws] Pump ON\n");
    }
}

static void pump_off_cmd(void)
{
    if (pump_on) {
        system("/usr/bin/pump off");
        pump_on = 0;
        printf("[sws] Pump OFF\n");
    }
}

static void sig_handler(int sig)
{
    (void)sig;
    pump_off_cmd();
    printf("\n[sws] Controller stopped safely\n");
    exit(0);
}

int read_adc(void)
{
    FILE *fp;
    int value = -1;
    int ret;

    fp = popen("/usr/bin/adc /dev/spidev0.0 0", "r");
    if (!fp) {
        perror("popen adc");
        return -1;
    }

    ret = fscanf(fp, "%d", &value);
    pclose(fp);

    if (ret != 1) {
        printf("ADC output invalid or empty\n");
        return -1;
    }

    printf("adc read value : %d\n", value);
    return value;
}

int main(void)
{
    int moisture;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    printf("[sws] Controller started\n");
    printf("[sws] ADC: %s channel %d\n",
           ADC_DEVICE, ADC_CHANNEL);

    while (1) {
        moisture = read_adc();
        if (moisture < 0) {
            printf("[sws] ADC read failed\n");
            sleep(INTERVAL_S);
            continue;
        }
        if (moisture < 10) {
            printf("[sws] Skip the noise\n");
            sleep(TRIAL_INTERVAL_S);
            continue;
        }

        printf("[sws] Moisture = %d\n", moisture);

        if (!pump_on && moisture < DRY_THRESH)
            pump_on_cmd();
            sleep(TRIAL_INTERVAL_S);
            pump_off_cmd();

        if (pump_on && moisture > WET_THRESH)
            pump_off_cmd();

        sleep(INTERVAL_S);
    }

    return 0;
}

