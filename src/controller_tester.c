// Copyright (C) 2025 Joshua Vandaële
// SPDX-License-Identifier: MIT

#include "input_device.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
  struct input_device *devices = get_input_devices();
  if (!devices)
    return EXIT_FAILURE;

  for (int i = 0; devices[i].handler != nullptr; i++) {
    printf("Device %d: %s (%s)\n", i, devices[i].name, devices[i].handler);
  }

  free_input_devices(devices);

  return EXIT_SUCCESS;
}