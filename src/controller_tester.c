// Copyright (C) 2025 Joshua Vandaële
// SPDX-License-Identifier: MIT

#include "input_device.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <libevdev/libevdev.h>

int main() {
  struct input_device *devices = get_input_devices();
  if (!devices)
    return EXIT_FAILURE;

  int device_count = 0;
  for (int i = 0; devices[i].handler != nullptr; i++) {
    printf("Device %d: %s (%s)\n", i, devices[i].name, devices[i].handler);
    device_count++;
  }

  printf("Which device do you want to test? (0-%d): ", device_count - 1);
  int device_index = -1;
  while (device_index < 0 || device_index >= device_count) {
    scanf("%d", &device_index);
    if (device_index < 0 || device_index >= device_count) {
      printf("Invalid choice. Please enter a number between 0 and %d: ",
             device_count - 1);
    }
  }

  struct libevdev *dev = NULL;
  int fd = open(devices[device_index].handler, O_RDONLY | O_NONBLOCK);
  libevdev_new_from_fd(fd, &dev);
  while (1) {
    struct input_event ev;

    if (libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) ==
        LIBEVDEV_READ_STATUS_SUCCESS) {
      switch (ev.type) {
      case EV_KEY:
        printf("%s %s\n", libevdev_event_code_get_name(ev.type, ev.code),
               ev.value ? "pressed" : "released");
        break;
      case EV_ABS:
        printf("%s: %d\n", libevdev_event_code_get_name(ev.type, ev.code),
               ev.value);
        break;
      case EV_SYN:
        printf("== Sync ==\n");
        break;
      default:
        printf("Event: type=%d, code=%d, value=%d\n", ev.type, ev.code,
               ev.value);
        break;
      }
    }
  }

  free_input_devices(devices);

  return EXIT_SUCCESS;
}