// Copyright (C) 2025 Joshua Vandaële
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input_device.h"

#include <libevdev/libevdev.h>

#define LINE_BUFFER_SIZE 256
#define INPUT_DEVICES_FILE "/proc/bus/input/devices"
#define INPUT_DEVICES_PATH "/dev/input/"

/**
 * Extract the name of the device from a line in the input devices file.
 * The line is expected to be in the format: `N: Name="Device Name"`
 */
static char *extract_name_from_line(const char *line) {
  char *name = nullptr;
  char *opening_quote;
  char *closing_quote;

  if ((opening_quote = strchr(line, '"'))) {
    if ((closing_quote = strrchr(line, '"'))) {
      size_t gap = closing_quote - opening_quote;
      name = malloc(gap + 1);
      memcpy(name, opening_quote + 1, gap - 1);
      name[gap - 1] = '\0';
    }
  }
  return name;
}

/**
 * Extract the handler from a line in the input devices file.
 * The line is expected to be in the format: `H: Handlers=eventX`
 */
static char *extract_handler_from_line(const char *line) {
  char *handler = nullptr;
  const char EVENT_LITERAL[6] = "event\0";
  char *event_str_start = nullptr;

  if ((event_str_start = strstr(line, EVENT_LITERAL))) {
    char *event_str_end = nullptr;
    event_str_end = strpbrk(event_str_start, " \n");

    if (event_str_end) {
      size_t gap = event_str_end - event_str_start;
      const size_t path_len = strlen(INPUT_DEVICES_PATH);
      handler = malloc(path_len + gap + 1);
      strcpy(handler, INPUT_DEVICES_PATH);
      strncat(handler, event_str_start, gap);
      handler[path_len + gap] = '\0';
    }
  }
  return handler;
}

/**
 * Count the number of devices in the input devices file.
 */
static int count_devices_in_file(FILE *file) {
  int device_count = 0;
  char line[LINE_BUFFER_SIZE];
  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '\n') {
      device_count++;
    }
  }
  return device_count;
}

/**
 * Get the list of available input devices from the input devices file.
 */
struct input_device *get_input_devices() {
  FILE *file = fopen(INPUT_DEVICES_FILE, "r");
  if (!file) {
    perror("Failed to open input devices file");
    return nullptr;
  }

  int device_count = count_devices_in_file(file);

  struct input_device *devices =
      malloc(sizeof(struct input_device) * (device_count + 1));

  rewind(file);
  device_count = 0;
  char line[LINE_BUFFER_SIZE];
  char *handler = nullptr;
  char *name = nullptr;
  while (fgets(line, sizeof(line), file)) {
    switch (line[0]) {
    case 'N':
      name = extract_name_from_line(line);
      break;
    case 'H':
      handler = extract_handler_from_line(line);
      break;
    case '\n':
      // End of a device
      if (handler && name) {
        devices[device_count] =
            (struct input_device){.handler = handler, .name = name};
        device_count++;
      }

      handler = nullptr;
      name = nullptr;
      break;
    }
  }
  fclose(file);

  devices[device_count] =
      (struct input_device){.handler = nullptr, .name = nullptr};

  return devices;
}

/**
 * Free the memory allocated for the input devices.
 */
void free_input_devices(struct input_device *devices) {
  for (int i = 0; devices[i].handler != nullptr; i++) {
    free(devices[i].handler);
    free(devices[i].name);
  }
  free(devices);
}