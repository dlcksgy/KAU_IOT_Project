/****************************************************************************
*
* Copyright 2018 baruncorechips All Rights Reserved.
*
* Filename: A053BasicKit.h
* Author: sj.yang
* Release date: 2018/09/18
* Version: 1.2
*
****************************************************************************/

#ifndef A053BASICKIT_H_
#define A053BASICKIT_H_

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <tinyara/gpio.h>	// for GPIO control
#include <tinyara/pwm.h>	// for PWM control
#include <tinyara/analog/adc.h>	// for ADC control
#include <tinyara/analog/ioctl.h>	// for ADC control
#include <tinyara/config.h>
#include <apps/shell/tash.h>
#include <errno.h>	// for ADC control

// Hexagon GPIO Pin number
#define PIN_D2 46
#define PIN_D4 47
#define PIN_D7 48
#define PIN_D8 50

// Hexagon ADC Pin number
#define A0 0
#define A1 1
#define A2 2
#define A3 3

// Hexagon PWM Pin number
#define PWM0 0
#define PWM1 1
#define PWM2 2
#define PWM4 4

// octave_chord period[us] // frequency[hz]
#define O5_DO 1911 // 523.251[hz]
#define O5_RE 1703 // 587.330[hz]
#define O5_MI 1517 // 659.254[hz]
#define O5_FA 1432 // 698.456[hz]
#define O5_SO 1276 // 783.990[hz]
#define O5_LA 1136 // 880.000[hz]
#define O5_TI 1012 // 987.766[hz]

#define O6_DO 956 // 1046.502[hz]
#define O6_RE 851 // 1174.659[hz]
#define O6_MI 758 // 1318.510[hz]
#define O6_FA 716 // 1396.913[hz]
#define O6_SO 638 // 1567.982[hz]
#define O6_LA 568 // 1760.000[hz]
#define O6_TI 506 // 1975.533[hz]

#define	HIGH	1
#define	LOW		0
#define ENABLE	1
#define DISABLE	0
#define S5J_ADC_MAX_CHANNELS	4

//GPIO function
void gpio_write(int port, int value);
int gpio_read(int port);

//PWM function
int pwm_open (int port);
void pwm_write(int fd, int period, int duty_cycle);
void pwm_close (int fd);
void ServoAngle(int fd, int PERIOD, int angle);

//ADC function
int read_adc(int channel);

#endif A053BASICKIT_H_
