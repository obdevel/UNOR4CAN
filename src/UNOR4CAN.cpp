/*
 * Copyright (c) 2022 by Alexander Entinger <a.entinger@arduino.cc>
 * CAN library for Arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/


#include "UNOR4CAN.h"
#include <IRQManager.h>

#define CAN_DEFAULT_MASK                    (0x1FFFFFFFU)

extern "C" void can_callback2(can_callback_args_t *p_args);

///

#include "CanUtil.h"

///

UNOR4CAN::UNOR4CAN(int const can_tx_pin, int const can_rx_pin)
  : _can_tx_pin{can_tx_pin}
  , _can_rx_pin{can_rx_pin}
    // , _is_error{false}
    // , _err_code{0}
//  , _can_rx_buf{}
  , _can_bit_timing_cfg{}
  , _can_mailbox_mask {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
}
, _can_mailbox {
  /* Mailbox Group #0 */
  { .mailbox_id =  0, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  1, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  2, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  3, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #1 */
  { .mailbox_id =  4, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  5, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  6, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  7, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #2 */
  { .mailbox_id =  8, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  9, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 10, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 11, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #3 */
  { .mailbox_id = 12, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 13, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 14, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 15, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #4 */
  { .mailbox_id =  0, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  1, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  2, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  3, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #5 */
  { .mailbox_id =  4, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  5, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  6, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  7, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #6 */
  { .mailbox_id =  8, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  9, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 10, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 11, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #7 */
  { .mailbox_id = 12, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 13, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 14, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 15, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE }
}
, _can_extended_cfg {
  .clock_source   = CAN_CLOCK_SOURCE_PCLKB,
  .p_mailbox_mask = _can_mailbox_mask,
  .p_mailbox      = _can_mailbox,
  .global_id_mode = CAN_GLOBAL_ID_MODE_MIXED,
  .mailbox_count  = CAN_MAX_NO_MAILBOXES,
  .message_mode   = CAN_MESSAGE_MODE_OVERWRITE,
  .p_fifo_int_cfg = nullptr,
  .p_rx_fifo_cfg  = nullptr,
}
, _can_cfg {
  .channel        = 0,
  .p_bit_timing   = & _can_bit_timing_cfg,
  .p_callback     = can_callback2,
  .p_context      = this,
  .p_extend       = & _can_extended_cfg,
  .ipl            = (12),
  .error_irq      = FSP_INVALID_VECTOR,
  .rx_irq         = FSP_INVALID_VECTOR,
  .tx_irq         = FSP_INVALID_VECTOR,
}
{

}

bool UNOR4CAN::begin(void) {

  bool init_ok = true, r;

  // configure the TX and RX pins
  int const max_index = g_pin_cfg_size / sizeof(g_pin_cfg[0]);
  r = cfg_pins(max_index, _can_tx_pin, _can_rx_pin);

  // Serial.print("> cfg_pins returns ");
  // Serial.println(r);

  init_ok &= r;

  // configure interrupt
  CanIrqReq_t irq_req {
    .ctrl = &_can_ctrl,
    .cfg = &_can_cfg,
  };

  r =  IRQManager::getInstance().addPeripheral(IRQ_CAN, &irq_req);

  // Serial.print("> irq config returns ");
  // Serial.println(r);

  init_ok &= r;

  // calculate the CAN timing parameters
  static uint32_t const F_CAN_CLK_Hz = 24 * 1000 * 1000UL;
  static uint32_t const TQ_MIN     = 8;
  static uint32_t const TQ_MAX     = 25;
  static uint32_t const TSEG_1_MIN = 4;
  static uint32_t const TSEG_1_MAX = 16;
  static uint32_t const TSEG_2_MIN = 2;
  static uint32_t const TSEG_2_MAX = 8;

  auto [is_valid_baudrate, baud_rate_prescaler, time_segment_1, time_segment_2] =
    util::calc_can_bit_timing(can_bitrate, F_CAN_CLK_Hz, TQ_MIN, TQ_MAX, TSEG_1_MIN, TSEG_1_MAX, TSEG_2_MIN, TSEG_2_MAX);
  init_ok &= is_valid_baudrate;

  // Serial.print("> baud rate set returns ");
  // Serial.println(is_valid_baudrate);

  if (is_valid_baudrate) {
    _can_bit_timing_cfg.baud_rate_prescaler = baud_rate_prescaler;
    _can_bit_timing_cfg.time_segment_1 = time_segment_1;
    _can_bit_timing_cfg.time_segment_2 = time_segment_2;
    _can_bit_timing_cfg.synchronization_jump_width = 1;
  }

  // initialize the peripheral's FSP driver
  if (R_CAN_Open(&_can_ctrl, &_can_cfg) != FSP_SUCCESS) {
    init_ok = false;
    Serial.println("> R_CAN_Open fail");
  } else {
    // Serial.println("> R_CAN_Open ok");
  }

  // Serial.print("> begin returns ");
  // Serial.println(init_ok);

  return init_ok;
}

void UNOR4CAN::end() {

  R_CAN_Close(&_can_ctrl);
}

int UNOR4CAN::enableInternalLoopback() {

  if (fsp_err_t const rc = R_CAN_ModeTransition(&_can_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL); rc != FSP_SUCCESS)
    return -rc;

  return 1;
}

int UNOR4CAN::disableInternalLoopback() {

  if (fsp_err_t const rc = R_CAN_ModeTransition(&_can_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_DISABLED); rc != FSP_SUCCESS)
    return -rc;

  return 1;
}

int UNOR4CAN::send(can_frame_t *msg) {

  if (fsp_err_t const rc = R_CAN_Write(&_can_ctrl, CAN_MAILBOX_ID_0, msg); rc != FSP_SUCCESS)
    return -rc;

  return 1;
}

void UNOR4CAN::onCanCallback2(can_callback_args_t *p_args) {

  // Serial.println("> callback runs");

  if (user_callback) {
    // Serial.println("> calling user callback handler");
    (void)(*user_callback)(p_args);
  } else {
    Serial.println("> no user callback handler");
  }
}

void UNOR4CAN::set_can_bitrate(CanBitRate bitrate) {

  // Serial.println("> setting bitrate");
  can_bitrate = bitrate;
  return;
}

void UNOR4CAN::set_callback(void (*fptr)(can_callback_args_t *event)) {

  // Serial.println("> setting user callback handler");
  user_callback = fptr;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

bool UNOR4CAN::cfg_pins(int const max_index, int const can_tx_pin, int const can_rx_pin) {

  /* Verify if indices are good. */
  if (can_tx_pin < 0 || can_rx_pin < 0 || can_tx_pin >= max_index || can_rx_pin >= max_index) {
    return false;
  }

  /* Getting configuration from table. */
  // const uint16_t * cfg = nullptr;
  auto cfg_can_tx = getPinCfgs(can_tx_pin, PIN_CFG_REQ_CAN_TX);
  auto cfg_can_rx = getPinCfgs(can_rx_pin, PIN_CFG_REQ_CAN_RX);

  /* Verify if configurations are good. */
  if (cfg_can_tx[0] == 0 || cfg_can_rx[0] == 0) {
    return false;
  }

  /* Verify if channel is the same for all pins. */
  uint32_t const ch_can_tx = GET_CHANNEL(cfg_can_tx[0]);
  uint32_t const ch_can_rx = GET_CHANNEL(cfg_can_rx[0]);
  if (ch_can_tx != ch_can_rx) {
    return false;
  }

  /* Actually configure pin functions. */
  R_IOPORT_PinCfg(&g_ioport_ctrl, g_pin_cfg[can_tx_pin].pin, (uint32_t)(IOPORT_CFG_PERIPHERAL_PIN) | (uint32_t)(IOPORT_PERIPHERAL_CAN));
  R_IOPORT_PinCfg(&g_ioport_ctrl, g_pin_cfg[can_rx_pin].pin, (uint32_t)(IOPORT_CFG_PERIPHERAL_PIN) | (uint32_t)(IOPORT_PERIPHERAL_CAN));

  return true;
}

extern "C" void can_callback2(can_callback_args_t * p_args)
{
  UNOR4CAN * this_ptr = (UNOR4CAN *)(p_args->p_context);
  this_ptr->onCanCallback2(p_args);
}

