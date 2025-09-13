/*
 * Copyright (c) 2022 by Alexander Entinger <a.entinger@arduino.cc>
 * CAN library for Arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#pragma once

///

#include <Arduino.h>

#include <cstdint>
#include <tuple>

#include "bsp_api.h"
#include "r_can.h"

///

namespace util {

std::tuple<bool,     /* valid result found */
    uint32_t, /* baud_rate_prescaler */
    uint32_t, /* time_segment_1 */
    uint32_t> /* time_segment_2 */
    calc_can_bit_timing(uint32_t const can_bitrate, uint32_t const can_clock_Hz, uint32_t const tq_min, uint32_t const tq_max,
                        uint32_t const tseg1_min, uint32_t const tseg1_max, uint32_t const tseg2_min, uint32_t const tseg2_max);
}

/// CAN driver class for Arduino Uno and Nano R4

class UNOR4CAN {
public:

  UNOR4CAN();
  ~UNOR4CAN();

  bool begin(void);
  void end(void);

  // void set_can_bitrate(const CanBitRate bitrate);
  void set_can_bitrate(const uint32_t bitrate);
  void set_callback(void (*fptr)(can_callback_args_t *args));

  int send(can_frame_t *msg);

  int enableInternalLoopback();
  int disableInternalLoopback();

  void set_debug(const bool state);
  bool get_info(can_info_t *pstats);

  // this function is used by the library and should NOT be called by the user
  void onCanCallback2(can_callback_args_t *p_args);

private:

  static size_t constexpr CAN_MAX_NO_MAILBOXES = 32U;

  uint32_t can_bitrate;

  can_instance_ctrl_t _can_ctrl;
  can_bit_timing_cfg_t _can_bit_timing_cfg;
  uint32_t _can_mailbox_mask[CAN_MAX_NO_MAILBOXES / 4];
  can_mailbox_t _can_mailbox[CAN_MAX_NO_MAILBOXES];
  can_extended_cfg_t _can_extended_cfg;
  can_cfg_t _can_cfg;

  void (*user_callback)(can_callback_args_t *args) = nullptr;
  bool debug = false;

  static bool cfg_pins(int const max_index, int const can_tx_pin, int const can_rx_pin);
};


