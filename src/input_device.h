// Copyright (C) 2025 Joshua Vandaële
// SPDX-License-Identifier: MIT

#pragma once

struct input_device {
  char *handler;
  char *name;
};

struct input_device *get_input_devices();
void free_input_devices(struct input_device *devices);