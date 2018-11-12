/***************************************************************************
 *   Copyright (C) 2010 Serge Vakulenko                                    *
 *   serge@vak.ru                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if IS_CYGWIN == 1
#include "windows.h"
#undef LOG_ERROR
#endif

/* project specific includes */
#include <jtag/interface.h>
#include <jtag/commands.h>
#include <helper/time_support.h>
#include "libusb1_common.h"

/* system includes */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

/*
 *   Bit 7 (0x80, pin 6, RI ): unused.
 *   Bit 6 (0x40, pin 10,DCD): /SYSRST output.
 *   Bit 5 (0x20, pin 9, DSR): unused.
 *   Bit 4 (0x10, pin 2, DTR): /TRST output.
 *   Bit 3 (0x08, pin 11,CTS): TMS output.
 *   Bit 2 (0x04, pin 3, RTS): TDO input.
 *   Bit 1 (0x02, pin 5, RXD): TDI output.
 *   Bit 0 (0x01, pin 1, TXD): TCK output.
 *
 * Sync bit bang mode is implemented as described in FTDI Application
 * Note AN232R-01: "Bit Bang Modes for the FT232R and FT245R".
 */
#define TCK			(1 << 0)
#define TDI			(1 << 1)
#define READ_TDO		(1 << 2)
#define TMS			(1 << 3)
#define NTRST			(1 << 4)
#define NSYSRST			(1 << 6)

/*
 * USB endpoints.
 */
#define IN_EP			0x02
#define OUT_EP			0x81

/* Requests */
#define SIO_RESET		0 /* Reset the port */
#define SIO_MODEM_CTRL		1 /* Set the modem control register */
#define SIO_SET_FLOW_CTRL	2 /* Set flow control register */
#define SIO_SET_BAUD_RATE	3 /* Set baud rate */
#define SIO_SET_DATA		4 /* Set the data characteristics of the port */
#define SIO_POLL_MODEM_STATUS	5
#define SIO_SET_EVENT_CHAR	6
#define SIO_SET_ERROR_CHAR	7
#define SIO_SET_LATENCY_TIMER	9
#define SIO_GET_LATENCY_TIMER	10
#define SIO_SET_BITMODE		11
#define SIO_READ_PINS		12
#define SIO_READ_EEPROM		0x90
#define SIO_WRITE_EEPROM	0x91
#define SIO_ERASE_EEPROM	0x92

#define FT232R_BUF_SIZE		4000

static char *ft232r_serial_desc;
static uint16_t ft232r_vid = 0x0403; /* FTDI */
static uint16_t ft232r_pid = 0x6001; /* FT232R */
static jtag_libusb_device_handle *adapter;

static uint8_t *ft232r_output;
static size_t ft232r_output_len;

/**
 * Perform sync bitbang output/input transaction.
 * Before call, an array ft232r_output[] should be filled with data to send.
 * Counter ft232r_output_len contains the number of bytes to send.
 * On return, received data is put back to array ft232r_output[].
 */
static int ft232r_send_recv(void)
{
	/* FIFO TX buffer has 128 bytes.
	 * FIFO RX buffer has 256 bytes.
	 * First two bytes of received packet contain contain modem
	 * and line status and are ignored.
	 * Unfortunately, transfer sizes bigger than 64 bytes
	 * frequently cause hang ups. */
	assert(ft232r_output_len > 0);

	size_t total_written = 0;
	size_t total_read = 0;
	int rxfifo_free = 128;

	while (total_read < ft232r_output_len) {
		/* Write */
		int bytes_to_write = ft232r_output_len - total_written;
		if (bytes_to_write > 64)
			bytes_to_write = 64;
		if (bytes_to_write > rxfifo_free)
			bytes_to_write = rxfifo_free;

		if (bytes_to_write) {
			int n = jtag_libusb_bulk_write(adapter, IN_EP,
				(char *) ft232r_output + total_written,
				bytes_to_write, 1000);

			if (n == 0) {
				LOG_ERROR("usb bulk write failed");
				return ERROR_JTAG_DEVICE_ERROR;
			}

			total_written += n;
			rxfifo_free -= n;
		}

		/* Read */
		uint8_t reply[64];

		int n = jtag_libusb_bulk_read(adapter, OUT_EP,
			(char *) reply,
			sizeof(reply), 1000);

		if (n == 0) {
			LOG_ERROR("usb bulk read failed");
			return ERROR_JTAG_DEVICE_ERROR;
		}
		if (n > 2) {
			/* Copy data, ignoring first 2 bytes. */
			memcpy(ft232r_output + total_read, reply + 2, n - 2);
			int bytes_read = n - 2;
			total_read += bytes_read;
			rxfifo_free += bytes_read;
			if (total_read > total_written) {
				LOG_ERROR("read more bytes than wrote");
				return ERROR_JTAG_DEVICE_ERROR;
			}
		}
	}
	ft232r_output_len = 0;
	return ERROR_OK;
}

/**
 * Add one TCK/TMS/TDI sample to send buffer.
 */
static void ft232r_write(int tck, int tms, int tdi)
{
	unsigned out_value = NTRST | NSYSRST;
	if (tck)
		out_value |= TCK;
	if (tms)
		out_value |= TMS;
	if (tdi)
		out_value |= TDI;

	if (ft232r_output_len >= FT232R_BUF_SIZE) {
		/* FIXME: should we just execute queue here? */
		LOG_ERROR("ft232r_write: buffer overflow");
		return;
	}
	ft232r_output[ft232r_output_len++] = out_value;
}

/**
 * Control /TRST and /SYSRST pins.
 * Perform immediate bitbang transaction.
 */
static void ft232r_reset(int trst, int srst)
{
	unsigned out_value = NTRST | NSYSRST;
	LOG_DEBUG("ft232r_reset(%d,%d)", trst, srst);

	if (trst == 1)
		out_value &= ~NTRST;		/* switch /TRST low */
	else if (trst == 0)
		out_value |= NTRST;			/* switch /TRST high */

	if (srst == 1)
		out_value &= ~NSYSRST;		/* switch /SYSRST low */
	else if (srst == 0)
		out_value |= NSYSRST;		/* switch /SYSRST high */

	if (ft232r_output_len >= FT232R_BUF_SIZE) {
		/* FIXME: should we just execute queue here? */
		LOG_ERROR("ft232r_write: buffer overflow");
		return;
	}

	ft232r_output[ft232r_output_len++] = out_value;
	ft232r_send_recv();
}

static int ft232r_speed(int divisor)
{
	int baud = (divisor == 0) ? 3000000 :
		(divisor == 1) ? 2000000 :
		3000000 / divisor;
	LOG_DEBUG("ft232r_speed(%d) rate %d bits/sec", divisor, baud);

	if (jtag_libusb_control_transfer(adapter,
		LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
		SIO_SET_BAUD_RATE, divisor, 0, 0, 0, 1000) != 0) {
		LOG_ERROR("cannot set baud rate");
		return ERROR_JTAG_DEVICE_ERROR;
	}
	return ERROR_OK;
}

static int ft232r_init(void)
{
	uint16_t avids[] = {ft232r_vid, 0};
	uint16_t apids[] = {ft232r_pid, 0};
	if (jtag_libusb_open(avids, apids, ft232r_serial_desc, &adapter)) {
		LOG_ERROR("ft232r not found: vid=%04x, pid=%04x, serial=%s\n",
			ft232r_vid, ft232r_pid, (ft232r_serial_desc == NULL) ? "[any]" : ft232r_serial_desc);
		return ERROR_JTAG_INIT_FAILED;
	}

	libusb_detach_kernel_driver(adapter, 0);

	if (jtag_libusb_claim_interface(adapter, 0)) {
		LOG_ERROR("unable to claim interface");
		return ERROR_JTAG_INIT_FAILED;
	}

	/* Reset the device. */
	if (jtag_libusb_control_transfer(adapter,
		LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
		SIO_RESET, 0, 0, 0, 0, 1000) != 0) {
		LOG_ERROR("unable to reset device");
		return ERROR_JTAG_INIT_FAILED;
	}

	/* Sync bit bang mode. */
	if (jtag_libusb_control_transfer(adapter,
		LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
		SIO_SET_BITMODE, TCK | TDI | TMS | NTRST | NSYSRST | 0x400,
		0, 0, 0, 1000) != 0) {
		LOG_ERROR("cannot set sync bitbang mode");
		return ERROR_JTAG_INIT_FAILED;
	}

	/* Exactly 500 nsec between updates. */
	unsigned divisor = 1;
	unsigned char latency_timer = 1;

	/* Frequency divisor is 14-bit non-zero value. */
	if (jtag_libusb_control_transfer(adapter,
		LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
		SIO_SET_BAUD_RATE, divisor,
		0, 0, 0, 1000) != 0) {
		LOG_ERROR("cannot set baud rate");
		return ERROR_JTAG_INIT_FAILED;
	}
	if (jtag_libusb_control_transfer(adapter,
		LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT,
		SIO_SET_LATENCY_TIMER, latency_timer, 0, 0, 0, 1000) != 0) {
		LOG_ERROR("unable to set latency timer");
		return ERROR_JTAG_INIT_FAILED;
	}

	ft232r_output = malloc(FT232R_BUF_SIZE);
	if (ft232r_output == NULL) {
		LOG_ERROR("Unable to allocate memory for the buffer");
		return ERROR_JTAG_INIT_FAILED;
	}

	return ERROR_OK;
}

static int ft232r_quit(void)
{
	if (jtag_libusb_release_interface(adapter, 0) != 0)
		LOG_ERROR("usb release interface failed");

	jtag_libusb_close(adapter);
	free(ft232r_output);

	return ERROR_OK;
}

static int ft232r_speed_div(int divisor, int *khz)
{
	/* Maximum 3 Mbaud for bit bang mode. */
	if (divisor == 0)
		*khz = 3000;
	else if (divisor == 1)
		*khz = 2000;
	else
		*khz = 3000 / divisor;
	return ERROR_OK;
}

static int ft232r_khz(int khz, int *divisor)
{
	if (khz == 0) {
		LOG_DEBUG("RCLK not supported");
		return ERROR_FAIL;
	}

	/* Calculate frequency divisor. */
	if (khz > 2500)
		*divisor = 0;		/* Special case: 3 MHz */
	else if (khz > 1700)
		*divisor = 1;		/* Special case: 2 MHz */
	else {
		*divisor = (2*3000 / khz + 1) / 2;
		if (*divisor > 0x3FFF)
			*divisor = 0x3FFF;
	}
	return ERROR_OK;
}

COMMAND_HANDLER(ft232r_handle_serial_desc_command)
{
	if (CMD_ARGC == 1)
		ft232r_serial_desc = strdup(CMD_ARGV[0]);
	else
		LOG_ERROR("require exactly one argument to "
				  "ft232r_serial_desc <serial>");
	return ERROR_OK;
}

COMMAND_HANDLER(ft232r_handle_vid_pid_command)
{
	if (CMD_ARGC > 2) {
		LOG_WARNING("ignoring extra IDs in ft232r_vid_pid "
					"(maximum is 1 pair)");
		CMD_ARGC = 2;
	}
	if (CMD_ARGC == 2) {
		COMMAND_PARSE_NUMBER(u16, CMD_ARGV[0], ft232r_vid);
		COMMAND_PARSE_NUMBER(u16, CMD_ARGV[1], ft232r_pid);
	} else
		LOG_WARNING("incomplete ft232r_vid_pid configuration");

	return ERROR_OK;
}

static const struct command_registration ft232r_command_handlers[] = {
	{
		.name = "ft232r_serial_desc",
		.handler = ft232r_handle_serial_desc_command,
		.mode = COMMAND_CONFIG,
		.help = "USB serial descriptor of the adapter",
		.usage = "serial string",
	},
	{
		.name = "ft232r_vid_pid",
		.handler = ft232r_handle_vid_pid_command,
		.mode = COMMAND_CONFIG,
		.help = "USB VID and PID of the adapter",
		.usage = "vid pid",
	},
	COMMAND_REGISTRATION_DONE
};

/*
 * Synchronous bitbang protocol implementation.
 */

static void syncbb_end_state(tap_state_t state)
{
	if (tap_is_state_stable(state))
		tap_set_end_state(state);
	else {
		LOG_ERROR("BUG: %i is not a valid end state", state);
		exit(-1);
	}
}

static void syncbb_state_move(int skip)
{
	int i = 0, tms = 0;
	uint8_t tms_scan = tap_get_tms_path(tap_get_state(), tap_get_end_state());
	int tms_count = tap_get_tms_path_len(tap_get_state(), tap_get_end_state());

	for (i = skip; i < tms_count; i++) {
		tms = (tms_scan >> i) & 1;
		ft232r_write(0, tms, 0);
		ft232r_write(1, tms, 0);
	}
	ft232r_write(0, tms, 0);

	tap_set_state(tap_get_end_state());
}

/**
 * Clock a bunch of TMS (or SWDIO) transitions, to change the JTAG
 * (or SWD) state machine.
 */
static int syncbb_execute_tms(struct jtag_command *cmd)
{
	unsigned num_bits = cmd->cmd.tms->num_bits;
	const uint8_t *bits = cmd->cmd.tms->bits;

	DEBUG_JTAG_IO("TMS: %d bits", num_bits);

	int tms = 0;
	for (unsigned i = 0; i < num_bits; i++) {
		tms = ((bits[i/8] >> (i % 8)) & 1);
		ft232r_write(0, tms, 0);
		ft232r_write(1, tms, 0);
	}
	ft232r_write(0, tms, 0);

	return ERROR_OK;
}

static void syncbb_path_move(struct pathmove_command *cmd)
{
	int num_states = cmd->num_states;
	int state_count;
	int tms = 0;

	state_count = 0;
	while (num_states) {
		if (tap_state_transition(tap_get_state(), false) == cmd->path[state_count]) {
			tms = 0;
		} else if (tap_state_transition(tap_get_state(), true) == cmd->path[state_count]) {
			tms = 1;
		} else {
			LOG_ERROR("BUG: %s -> %s isn't a valid TAP transition",
				tap_state_name(tap_get_state()),
				tap_state_name(cmd->path[state_count]));
			exit(-1);
		}

		ft232r_write(0, tms, 0);
		ft232r_write(1, tms, 0);

		tap_set_state(cmd->path[state_count]);
		state_count++;
		num_states--;
	}

	ft232r_write(0, tms, 0);

	tap_set_end_state(tap_get_state());
}

static void syncbb_runtest(int num_cycles)
{
	int i;

	tap_state_t saved_end_state = tap_get_end_state();

	/* only do a state_move when we're not already in IDLE */
	if (tap_get_state() != TAP_IDLE) {
		syncbb_end_state(TAP_IDLE);
		syncbb_state_move(0);
	}

	/* execute num_cycles */
	for (i = 0; i < num_cycles; i++) {
		ft232r_write(0, 0, 0);
		ft232r_write(1, 0, 0);
	}
	ft232r_write(0, 0, 0);

	/* finish in end_state */
	syncbb_end_state(saved_end_state);
	if (tap_get_state() != tap_get_end_state())
		syncbb_state_move(0);
}

/**
 * Function syncbb_stableclocks
 * issues a number of clock cycles while staying in a stable state.
 * Because the TMS value required to stay in the RESET state is a 1, whereas
 * the TMS value required to stay in any of the other stable states is a 0,
 * this function checks the current stable state to decide on the value of TMS
 * to use.
 */
static void syncbb_stableclocks(int num_cycles)
{
	int tms = (tap_get_state() == TAP_RESET ? 1 : 0);
	int i;

	/* send num_cycles clocks onto the cable */
	for (i = 0; i < num_cycles; i++) {
		ft232r_write(1, tms, 0);
		ft232r_write(0, tms, 0);
	}
}

static void syncbb_scan(bool ir_scan, enum scan_type type, uint8_t *buffer, int scan_size)
{
	tap_state_t saved_end_state = tap_get_end_state();
	int bit_cnt, bit0_index;

	if (!((!ir_scan && (tap_get_state() == TAP_DRSHIFT)) || (ir_scan && (tap_get_state() == TAP_IRSHIFT)))) {
		if (ir_scan)
			syncbb_end_state(TAP_IRSHIFT);
		else
			syncbb_end_state(TAP_DRSHIFT);

		syncbb_state_move(0);
		syncbb_end_state(saved_end_state);
	}

	bit0_index = ft232r_output_len;
	for (bit_cnt = 0; bit_cnt < scan_size; bit_cnt++) {
		int tms = (bit_cnt == scan_size-1) ? 1 : 0;
		int tdi;
		int bytec = bit_cnt/8;
		int bcval = 1 << (bit_cnt % 8);

		/* if we're just reading the scan, but don't care about the output
		 * default to outputting 'low', this also makes valgrind traces more readable,
		 * as it removes the dependency on an uninitialised value
		 */
		tdi = 0;
		if ((type != SCAN_IN) && (buffer[bytec] & bcval))
			tdi = 1;

		ft232r_write(0, tms, tdi);
		ft232r_write(1, tms, tdi);
	}

	if (tap_get_state() != tap_get_end_state()) {
		/* we *KNOW* the above loop transitioned out of
		 * the shift state, so we skip the first state
		 * and move directly to the end state.
		 */
		syncbb_state_move(1);
	}
	ft232r_send_recv();

	if (type != SCAN_OUT)
		for (bit_cnt = 0; bit_cnt < scan_size; bit_cnt++) {
			int bytec = bit_cnt/8;
			int bcval = 1 << (bit_cnt % 8);
			int val = ft232r_output[bit0_index + bit_cnt*2 + 1];

			if (val & READ_TDO)
				buffer[bytec] |= bcval;
			else
				buffer[bytec] &= ~bcval;
		}
}

static int syncbb_execute_queue(void)
{
	struct jtag_command *cmd = jtag_command_queue; /* currently processed command */
	int scan_size;
	enum scan_type type;
	uint8_t *buffer;
	int retval;

	/* return ERROR_OK, unless a jtag_read_buffer returns a failed check
	 * that wasn't handled by a caller-provided error handler
	 */
	retval = ERROR_OK;

/*	ft232r_blink(1);*/

	while (cmd) {
		switch (cmd->type) {
			case JTAG_RESET:
				LOG_DEBUG_IO("reset trst: %i srst %i", cmd->cmd.reset->trst, cmd->cmd.reset->srst);

				if ((cmd->cmd.reset->trst == 1) ||
					(cmd->cmd.reset->srst &&
					(jtag_get_reset_config() & RESET_SRST_PULLS_TRST))) {
					tap_set_state(TAP_RESET);
				}
				ft232r_reset(cmd->cmd.reset->trst, cmd->cmd.reset->srst);
				break;

			case JTAG_RUNTEST:
				LOG_DEBUG_IO("runtest %i cycles, end in %s", cmd->cmd.runtest->num_cycles,
					tap_state_name(cmd->cmd.runtest->end_state));

				syncbb_end_state(cmd->cmd.runtest->end_state);
				syncbb_runtest(cmd->cmd.runtest->num_cycles);
				break;

			case JTAG_STABLECLOCKS:
				/* this is only allowed while in a stable state.  A check for a stable
				 * state was done in jtag_add_clocks()
				 */
				syncbb_stableclocks(cmd->cmd.stableclocks->num_cycles);
				break;

			case JTAG_TLR_RESET: /* renamed from JTAG_STATEMOVE */
				LOG_DEBUG_IO("statemove end in %s", tap_state_name(cmd->cmd.statemove->end_state));

				syncbb_end_state(cmd->cmd.statemove->end_state);
				syncbb_state_move(0);
				break;

			case JTAG_PATHMOVE:
				LOG_DEBUG_IO("pathmove: %i states, end in %s", cmd->cmd.pathmove->num_states,
					tap_state_name(cmd->cmd.pathmove->path[cmd->cmd.pathmove->num_states - 1]));

				syncbb_path_move(cmd->cmd.pathmove);
				break;

			case JTAG_SCAN:
				LOG_DEBUG_IO("%s scan end in %s",  (cmd->cmd.scan->ir_scan) ? "IR" : "DR",
					tap_state_name(cmd->cmd.scan->end_state));

				syncbb_end_state(cmd->cmd.scan->end_state);
				scan_size = jtag_build_buffer(cmd->cmd.scan, &buffer);
				type = jtag_scan_type(cmd->cmd.scan);
				syncbb_scan(cmd->cmd.scan->ir_scan, type, buffer, scan_size);
				if (jtag_read_buffer(buffer, cmd->cmd.scan) != ERROR_OK)
					retval = ERROR_JTAG_QUEUE_FAILED;
				if (buffer)
					free(buffer);
				break;

			case JTAG_SLEEP:
				LOG_DEBUG_IO("sleep %" PRIi32, cmd->cmd.sleep->us);

				jtag_sleep(cmd->cmd.sleep->us);
				break;

			case JTAG_TMS:
				retval = syncbb_execute_tms(cmd);
				break;
			default:
				LOG_ERROR("BUG: unknown JTAG command type encountered");
				exit(-1);
		}
		if (ft232r_output_len > 0)
			ft232r_send_recv();
		cmd = cmd->next;
	}
/*	ft232r_blink(0);*/

	return retval;
}

struct jtag_interface ft232r_interface = {
	.name = "ft232r",
	.commands = ft232r_command_handlers,
	.transports = jtag_only,
	.supported = DEBUG_CAP_TMS_SEQ,

	.execute_queue = syncbb_execute_queue,

	.speed = ft232r_speed,
	.init = ft232r_init,
	.quit = ft232r_quit,
	.speed_div = ft232r_speed_div,
	.khz = ft232r_khz,
};
