/*
 * Copyright (c) 2020 Nordic Semiconuctor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr.h>
#include <drivers/adc.h>

/* identify node with path: /my,node */
#define NODE DT_PATH(my_node)

#define RESOLUTION 14

struct io_channel {
	const char *label;
	uint8_t input;
};

#define IO_INIT(name) { \
	.label = DT_IO_CHANNELS_LABEL_BY_NAME(NODE, name), \
	.input = DT_IO_CHANNELS_INPUT_BY_NAME(NODE, name), \
}

/* initialize for the IO channel named OP-AMP (op_amp) */
static struct io_channel const op_amp_cfg = IO_INIT(op_amp);
/* initialize for the IO channel named RC-OUT (rc_out) */
static struct io_channel const rc_out_cfg = IO_INIT(rc_out);
static uint32_t const rc_Ohm = DT_PROP(NODE, rc_ohm);
static uint32_t const rc_uF = DT_PROP(NODE, rc_ufarad);

static const struct device *adc;
static uint16_t adc_raw;
static struct adc_channel_cfg adc_cfg;
static struct adc_sequence adc_seq;

void main(void)
{
	int rc;

	printf("OP-AMP is %s/%u\n", op_amp_cfg.label, op_amp_cfg.input);
	printf("RC-OUT is %s/%u\n", rc_out_cfg.label, rc_out_cfg.input);
	printf("RC is %u Ohm / %u uF\n", rc_Ohm, rc_uF);

	/* Assume op-amp and rc-out on same ADC */
	adc = device_get_binding(op_amp_cfg.label);
	if (adc == NULL) {
		printf("Failed to get ADC: %s\n", op_amp_cfg.label);
		return;
	}

	adc_seq = (struct adc_sequence){
		.channels = BIT(op_amp_cfg.input),
		.buffer = &adc_raw,
		.buffer_size = sizeof(adc_raw),
		.oversampling = 4,
		.calibrate = true,
		.resolution = RESOLUTION,
	};

	adc_cfg = (struct adc_channel_cfg) {
		.gain = ADC_GAIN_1_6,
		.reference = ADC_REF_INTERNAL,
		.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40),
		.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0
			+ op_amp_cfg.input,
	};

	rc = adc_channel_setup(adc, &adc_cfg);
	printk("Channel setup: %d\n", rc);
	if (rc != 0) {
		return;
	}

	while (true) {
		int rc;
		int32_t mV;

		rc = adc_read(adc, &adc_seq);
		adc_seq.calibrate = false;
		if (rc != 0) {
			printk("ADC read failed: %d\n", rc);
			break;
		}

		mV = (int16_t)adc_raw;
		rc = adc_raw_to_millivolts(adc_ref_internal(adc), adc_cfg.gain,
					   adc_seq.resolution, &mV);
		if (rc != 0) {
			printk("ADC conversion failed: %d\n", rc);
			break;
		}
		printf("ADC raw %d => %d mV\n", (int16_t)adc_raw, mV);
		k_sleep(K_SECONDS(3));
	}

}
