#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include <libusb-1.0/libusb.h>

#include "../config.h"

const char *errorMsgs[] = {
	"no error",
	"write protection is enabled",
	"string to long",
	"invalid slot",
	"unknown"
};

const char *writableStates[] = {
	"no protection",
	"true",
	"false"
};

char * getPass(char *buffer, size_t size) {
	char *r, *n;
	struct termios oldt, newt;
	int fd = fileno(stdin);

	tcgetattr(fd, &oldt);

	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);

	tcsetattr(fd, TCSANOW, &newt);

	r = fgets(buffer, size, stdin);
	if (n = strrchr(buffer, '\n')) {
		*n = '\0';
	}

	tcsetattr(fd, TCSANOW, &oldt);

	return r;
}

char * getUser(char *buffer, size_t size) {
	char *r, *n;

	r = fgets(buffer, size, stdin);
	if (n = strrchr(buffer, '\n')) {
		*n = '\0';
	}

	return r;
}

int setSlot(struct libusb_device_handle *handle, uint16_t slotIndex, struct slot slot) {
	return libusb_control_transfer(
		handle,
		LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, /* bmRequestType */
		USBRQ_VENDOR_SET_SLOT,	/* bRequest */
		0,			/* wValue */
		slotIndex,		/* wIndex */
		(uint8_t *) &slot,	/* data */
		sizeof(struct slot),	/* wLength */
		500			/* timeout */
	);
}

struct status getStatus(struct libusb_device_handle *handle) {
	struct status stat;
	libusb_control_transfer(
		handle,
		LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
		USBRQ_VENDOR_GET_STATUS,
		0,
		0,
		(uint8_t *) &stat,
		sizeof(stat),
		500
	);

	return stat;
}

int checkStatus(struct libusb_device_handle *handle) {
	struct status stat = getStatus(handle);

	if (stat.error != ERROR_SUCCESS) {
		fprintf(stderr, "Error orccured: %s (%u)\n", errorMsgs[stat.error], stat.error);
		return stat.error;
	}

	return EXIT_SUCCESS;
}

double decodeUsbBcd(uint16_t bcd) {
	double result = 0;

	result += ((bcd & 0x0f00) >> 8) * 1e-2;
	result += ((bcd & 0xf000) >> 12) * 1e-1;
	result += ((bcd & 0x000f) >> 0) * 1e0;
	result += ((bcd & 0x00f0) >> 4) * 1e1;

	return result;
}

int main(int argc, char *argv[]) {
	struct libusb_device_handle *handle;
	struct libusb_device_descriptor desc;

	int r;

	r = libusb_init(NULL);
	if (r < 0) {
		return EXIT_FAILURE;
	}

	/* search device */
	handle = libusb_open_device_with_vid_pid(NULL, DEVICE_VID, DEVICE_PID);
	if (handle == NULL) {
		fprintf(stderr, "failed to open the device\n");
		return EXIT_FAILURE;
	}

	/* get status */
	struct status stat = getStatus(handle);

	r = libusb_get_device_descriptor(libusb_get_device(handle), &desc);
	if (r < 0) {
		fprintf(stderr, "failed to get device descriptor\n");
		return EXIT_FAILURE;
	}

	if (argc == 1) {
		printf("usage: hidekey COMMAND [OPTIONS]\n");
		printf(" available COMMANDs and their specific OPTIONS:\n\n");
		printf("   info\n\tshow information about plugged in keys\n\n");
		printf("   set SLOT TYPE USER [PASSWORD]\n\tstore new user/password combination on stick\n\n");
		printf("   clear SLOT\n\tdelete password from SLOT\n\n");
		printf("   reset\n\tprune all passwords and reset startup counter\n\n");
		printf("   serial SERIAL\tset new serial (expert option)\n\n");
		printf("hidekey 0.1 - HIDeKey setup utility\n");
		printf("by Steffen Vogel <stv0g@0l.de>\n");
		printf("please send bugreports to http://bugs.0l.de\n");
	}
	else if (argc == 2 && strcmp(argv[1], "info") == 0) {
			/* get product string */
			char productString[255];
			libusb_get_string_descriptor_ascii(handle, desc.iProduct, productString, 255);
			printf("name: %s\n", productString);

			/* get product version */
			printf("version: %.2f\n", decodeUsbBcd(desc.bcdDevice)); /* decode bcd */

			/* get serial no */
			char serialNumber[255];
			libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serialNumber, 255);
			printf("serial: %s\n", serialNumber);

			/* get status struct */
			printf("startups: %u\n", stat.counter);
			printf("writable: %s\n", writableStates[stat.writable]);// ? "true" : "false");
			printf("slots: %u\n", stat.maxSlots);
			printf("max user length: %u\n", stat.maxUserLen);
			printf("max password length: %u\n", stat.maxPassLen);
			printf("max serial length: %u\n", stat.maxSerialLen);
	}
	else if (argc == 2 && strcmp(argv[1], "reset") == 0) {
		libusb_control_transfer(
			handle,
			LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
			USBRQ_VENDOR_RESET,
			0,
			0,
			NULL,
			0,
			500
		);
	}
	else if (argc == 3 && strcmp(argv[1], "serial") == 0) {
		size_t len = strlen(argv[2]);

		libusb_control_transfer(
			handle,
			LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, /* bmRequestType */
			USBRQ_VENDOR_SET_SERIAL,/* bRequest */
			0,			/* wValue */
			0,			/* wIndex */
			(uint8_t *) argv[2],	/* data */
			len,			/* wLength (string is not \0-terminated!) */
			500			/* timeout */
		);
	}
	else if (argc == 3 && strcmp(argv[1], "clear") == 0) {
		uint16_t slotIndex = atoi(argv[2]);

		struct slot slot = { SLOT_EMPTY };
		setSlot(handle, slotIndex, slot);
	}
	else if (argc >= 4 && strcmp(argv[1], "set") == 0) {
		uint16_t slotIndex = atoi(argv[2]);
		uint8_t type = atoi(argv[3]);

		struct slot slot = {
			.type = type,
			.options = SKIP_USER | SKIP_DELIMITER | SKIP_RETURN,
			.delimiter = '\t'
		}; // TODO make configurable

		if (argc >= 5) {
			strncpy(slot.user, argv[4], MAX_USER_LEN);
		}
		else {
			printf("User: ");
			getUser(slot.user, MAX_PASS_LEN);
		}

		if (type == SLOT_STRING) {
			if (argc == 6) {
				strncpy(slot.pass.string, argv[5], MAX_PASS_LEN);
			}
			else {
				char tmpPass[MAX_PASS_LEN];

				printf("Password: ");
				getPass(slot.pass.string, MAX_PASS_LEN);

				printf("\nRepeat password: ");
				getPass(tmpPass, MAX_PASS_LEN);
				printf("\n");

				if (strcmp(slot.pass.string, tmpPass) != 0) {
					fprintf(stderr, "Passwords didn't match!\n");
					return EXIT_FAILURE;
				}
			}
		}

		setSlot(handle, slotIndex, slot);
	}
	else {
		fprintf(stderr, "Unknown command\n");
		return EXIT_FAILURE;
	}

	return checkStatus(handle);
}
