/****************************************************************************
*
* Copyright 2018 baruncorechips All Rights Reserved.
*
* Filename: A053BasicKit.c
* Author: sj.yang
* Release date: 2018/09/18
* Version: 1.2
*
****************************************************************************/

#include "A053BasicKit.h"

/***************************************************************************
 *
 *	GPIO function
 *	- void gpio_write(int port, int value);
 *	- int gpio_read(int port);
 *
 ***************************************************************************/
/****************************************************************************

*

* Copyright 2018 Barun Corechips All Rights Reserved.

*

* Filename: buzzer_main.c

* Author: bm.huh, sj.yang

* Release date: 2018/10/16

* Version: 1.2

*

****************************************************************************/

#include "A053BasicKit.h"

static void show_help_HEXT(FAR char *argv[]);

int buzzer_main(int argc, FAR char *argv[]);

static void show_help_HEXT(FAR char *argv[])

{

	printf("********************************************************\n");

	printf("* ARTIK053 BASIC KIT - BUZZER board Example \n");

	printf("* Please connect BUZZER board to HEXT UNO Interface board\n");

	printf("* %s | pwm[#] |  [#]TURNS \n", argv[0]);

	printf("* ex) HEXT_buzzer 0 3\n");

	printf("********************************************************\n");

}

int buzzer_main(int argc, FAR char *argv[])

{
	int i;
	int fd;
	int pin_buzzer=0;
	int max = 1;
	char *pwm0 = "0";
	char *pwm1 = "1";
	int dutycycle=100;


	//max = atoi(argv[2]);

	/*if (strcmp(argv[1], pwm0)==0 )
	{
		pin_buzzer = PWM0; // 부저센서가 pwm0에 연결된 경우
	}

	else if (strcmp(argv[1], pwm1)==0 )
	{
		pin_buzzer = PWM2; // 부저가 pwm1에 연겨된 경우
	}
	else
	{
		show_help_HEXT(argv);
		return 0;
	}*/

	printf("*********** HEXT BUZZER BOARD TEST START ***********\n");

	fd = pwm_open(pin_buzzer);

		for(i=0 ; i<max ; i++)
		{
			printf("|--- (%2d/%d) Do Re Mi Fa So La Ti Do ~ \n", i+1, max);
			pwm_write(fd, O5_DO, dutycycle);
			up_mdelay(250);
			pwm_write(fd, O5_RE, dutycycle); up_mdelay(250);
			pwm_write(fd, O5_MI, dutycycle); up_mdelay(250);
			pwm_write(fd, O5_FA, dutycycle); up_mdelay(250);
			pwm_write(fd, O5_SO, dutycycle); up_mdelay(250);
			pwm_write(fd, O5_LA, dutycycle); up_mdelay(250);
			pwm_write(fd, O5_TI, dutycycle); up_mdelay(250);
			pwm_write(fd, O6_DO, dutycycle); up_mdelay(250);
		}

	printf("************ HEXT BUZZER BOARD TEST END ************\n");
	pwm_write(fd, O5_DO, 0);
	pwm_close(fd);

	return 0;
}

void gpio_write(int port, int value)
{
	char str[4];
	static char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);

	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_OUT);
	write(fd, str, snprintf(str, 4, "%d", value != 0) + 1);

	close(fd);
}

int gpio_read(int port)
{
	char buf[4];
	char devpath[16];
	snprintf(devpath, 16, "/dev/gpio%d", port);
	int fd = open(devpath, O_RDWR);

	ioctl(fd, GPIOIOC_SET_DIRECTION, GPIO_DIRECTION_IN);
	read(fd, buf, sizeof(buf));
	close(fd);

	return buf[0] == '1';
}

/***************************************************************************
 *
 * PWM function
 * - int pwm_open (int port);
 * - void pwm_write(int fd, int period, int duty_cycle);
 * - void pwm_close (int fd);
 * - void ServoAngle(int fd, int PERIOD, int angle);
 *
 ***************************************************************************/

int pwm_open (int port)
{
	int fd;

	switch (port){
		case 0 : fd = open("/dev/pwm0", O_RDWR); break;
		case 1 : fd = open("/dev/pwm1", O_RDWR); break;
		case 2 : fd = open("/dev/pwm2", O_RDWR); break;
		case 3 : fd = open("/dev/pwm3", O_RDWR); break;
		case 4 : fd = open("/dev/pwm4", O_RDWR); break;
		case 5 : fd = open("/dev/pwm5", O_RDWR); break;
		default : break;
	}
	return fd;
}

void pwm_write(int fd, int period, int duty_cycle)
{
	int frequency;
	ub16_t duty;
	struct pwm_info_s pwm_info;

	// set pwo_info parameter
	frequency = 1000000 / period;
	duty = duty_cycle * 65536 / period;
	pwm_info.frequency = frequency;
	pwm_info.duty = duty;

	ioctl(fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)((uintptr_t)&pwm_info));
	ioctl(fd, PWMIOC_START);
}

void pwm_close (int fd)
{
	ioctl(fd, PWMIOC_STOP);
	close(fd);
}

void ServoAngle(int fd, int PERIOD, int angle)
{
	int duty_cycle;
	duty_cycle = 1500 + angle * 8.9;
	if(duty_cycle >2300) {duty_cycle = 2300;}
	else if(duty_cycle<700) {duty_cycle = 700;}
	else {}
	pwm_write(fd, PERIOD, duty_cycle);
}
/***************************************************************************
 *
 * ADC function
 * - int read_adc(int channel);
 *
 ***************************************************************************/

int read_adc(int channel)
{
	int fd, ret;
	struct adc_msg_s sample[S5J_ADC_MAX_CHANNELS];
	int32_t data;
	size_t readsize;
	ssize_t nbytes;

	fd = open("/dev/adc0", O_RDONLY);
	if (fd < 0)
	{
		printf("%s: open failed: %d\n", __func__, errno);
		return -1;
	}

	for (;;)
	{
		ret = ioctl(fd, ANIOC_TRIGGER, 0);
		if (ret < 0)
		{
			printf("%s: ioctl failed: %d\n", __func__, errno);
			close(fd);
			return -1;
		}

		readsize = S5J_ADC_MAX_CHANNELS * sizeof(struct adc_msg_s);
		nbytes = read(fd, sample, readsize);
		if (nbytes < 0)
		{
			if (errno != EINTR) {
				printf("%s: read failed: %d\n", __func__, errno);
				close(fd);
				return -1;
			}
		}
		else if (nbytes == 0) {	printf("%s: No data read, Ignoring\n", __func__); }
		else
		{
			int nsamples = nbytes / sizeof(struct adc_msg_s);
			if (nsamples * sizeof(struct adc_msg_s) != nbytes)
			{
				printf("%s: read size=%ld is not a multiple of sample size=%d, Ignoring\n", __func__, (long)nbytes, sizeof(struct adc_msg_s));
			}
			else
			{
				int i;
				for (i = 0; i < nsamples; i++)
				{
					if (sample[i].am_channel == channel)
					{
						data = sample[i].am_data;
						//printf("Sample %d: channel: %d, value: %d\n", i + 1, sample[i].am_channel, sample[i].am_data);
						close(fd);
						return data;
					}
				}
			}
		}
	}

	return 0;
}
