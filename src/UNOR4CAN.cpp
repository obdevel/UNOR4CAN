/*
 * Copyright (c) 2022 by Alexander Entinger <a.entinger@arduino.cc>
 * CAN library for Arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

/// CAN error codes

// FSP_ERR_CAN_DATA_UNAVAILABLE   = 60000,          ///< No data available.
// FSP_ERR_CAN_MODE_SWITCH_FAILED = 60001,          ///< Switching operation modes failed.
// FSP_ERR_CAN_INIT_FAILED        = 60002,          ///< Hardware initialization failed.
// FSP_ERR_CAN_TRANSMIT_NOT_READY = 60003,          ///< Transmit in progress.
// FSP_ERR_CAN_RECEIVE_MAILBOX    = 60004,          ///< Mailbox is setup as a receive mailbox.
// FSP_ERR_CAN_TRANSMIT_MAILBOX   = 60005,          ///< Mailbox is setup as a transmit mailbox.
// FSP_ERR_CAN_MESSAGE_LOST       = 60006,          ///< Receive message has been overwritten or overrun.
// FSP_ERR_CAN_TRANSMIT_FIFO_FULL = 60007,          ///< Transmit FIFO is full.

///

#include "UNOR4CAN.h"
#include <IRQManager.h>

#include "CanUtil.h"

///

extern "C" void can_callback2(can_callback_args_t *p_args);

/// constructor

UNOR4CAN::UNOR4CAN()
  :
  // , _is_error{false}
  // , _err_code{0}
  // , _can_rx_buf{}
  _can_bit_timing_cfg {}
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
  { .mailbox_id =  4, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  5, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  6, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  7, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #2 */
  { .mailbox_id =  8, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id =  9, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 10, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 11, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #3 */
  { .mailbox_id = 12, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 13, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 14, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  { .mailbox_id = 15, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_TRANSMIT},
  /* Mailbox Group #4 */
  { .mailbox_id =  0, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  1, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  2, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  3, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #5 */
  { .mailbox_id =  4, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  5, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  6, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  7, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #6 */
  { .mailbox_id =  8, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id =  9, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 10, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 11, .id_mode = CAN_ID_MODE_STANDARD, .frame_type = CAN_FRAME_TYPE_REMOTE, .mailbox_type = CAN_MAILBOX_RECEIVE },
  /* Mailbox Group #7 */
  { .mailbox_id = 12, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 13, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 14, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE },
  { .mailbox_id = 15, .id_mode = CAN_ID_MODE_EXTENDED, .frame_type = CAN_FRAME_TYPE_DATA, .mailbox_type = CAN_MAILBOX_RECEIVE }
}
, _can_extended_cfg {
  .clock_source   = CAN_CLOCK_SOURCE_PCLKB,
  .p_mailbox_mask = _can_mailbox_mask,
  .p_mailbox      = _can_mailbox,
  .global_id_mode = CAN_GLOBAL_ID_MODE_MIXED,
  .mailbox_count  = CAN_MAX_NO_MAILBOXES,
  // .message_mode   = CAN_MESSAGE_MODE_OVERWRITE,
  .message_mode   = CAN_MESSAGE_MODE_OVERRUN,
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

/// destructor

UNOR4CAN::~UNOR4CAN() {

}

/// begin

bool UNOR4CAN::begin(void) {

  bool init_ok = true, r;

  if (debug) {
    Serial.println("DRVR: begin");
    Serial.println("DRVR: configuring pins");
    Serial.flush();
  }

  // configure the TX and RX pins
  // pins are fixed: tx = 4, rx = 5

  int const max_index = PINS_COUNT;
  init_ok &= cfg_pins(max_index, PIN_CAN0_TX, PIN_CAN0_RX);

  if (debug) {
    Serial.print("DRVR: cfg_pins returns ");
    Serial.println(init_ok);
    Serial.print("DRVR: tx = ");
    Serial.print(PIN_CAN0_TX);
    Serial.print(", rx = ");
    Serial.println(PIN_CAN0_RX);
    Serial.flush();
  }

  // install interrupt

  CanIrqReq_t irq_req {
    .ctrl = &_can_ctrl,
    .cfg = &_can_cfg,
  };

  r = IRQManager::getInstance().addPeripheral(IRQ_CAN, &irq_req);

  if (debug) {
    Serial.print("DRVR: irq config returns ");
    Serial.println(r);
    Serial.flush();
  }

  init_ok &= r;

  // calculate the CAN timing parameters

  if (debug) {
    Serial.print("DRVR: calculating timing parameters for baudrate = ");
    Serial.println(can_bitrate);
    Serial.flush();
  }

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

  if (debug) {
    Serial.print("DRVR: baud rate set returns ");
    Serial.println(is_valid_baudrate);
    Serial.flush();
  }

  if (is_valid_baudrate) {
    _can_bit_timing_cfg.baud_rate_prescaler = baud_rate_prescaler;
    _can_bit_timing_cfg.time_segment_1 = time_segment_1;
    _can_bit_timing_cfg.time_segment_2 = time_segment_2;
    _can_bit_timing_cfg.synchronization_jump_width = 1;
  }

  if (debug) {
    Serial.println("DRVR: calling R_CAN_Open");
    Serial.flush();
  }

  fsp_err_t open_r;

  // initialize the peripheral's FSP driver
  if ((open_r = R_CAN_Open(&_can_ctrl, &_can_cfg)) != FSP_SUCCESS) {
    init_ok = false;
  }

  if (debug) {
    Serial.print("DRVR: R_CAN_Open returns ");
    Serial.println(open_r);
    Serial.print("DRVR: begin returning ");
    Serial.println(init_ok);
    Serial.flush();
  }

  return init_ok;
}

/// end

void UNOR4CAN::end() {

  R_CAN_Close(&_can_ctrl);
}

/// enable/disable loopback mode

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

/// send a CAN message

int UNOR4CAN::send(can_frame_t *msg) {

  // transmit mailboxes
  // 0-7   - standard
  // 8-11  - remote
  // 12-15 - extended

  static uint32_t next_extended_mbx = 12, next_rtr_mbx = 8, next_standard_mbx = 0;

  uint32_t selected_mbx;
  fsp_err_t rc;

  // select the transmit mailbox to use
  // round-robin over the set of allocated mailboxs, according to message type

  if (msg->id_mode == CAN_ID_MODE_EXTENDED) {                                            // extended messages x4, 12-15
    selected_mbx = next_extended_mbx;
    next_extended_mbx = (next_extended_mbx == 15 ? 12 : next_extended_mbx + 1);

    if (debug) {
      Serial.print("DRVR: using extended mailbox = ");
      Serial.println(selected_mbx);
    }

  } else if (msg->type == CAN_FRAME_TYPE_REMOTE) {                                       // remote (RTR) messages x4, 8-11
    selected_mbx = next_rtr_mbx;
    next_rtr_mbx = (next_rtr_mbx == 11 ? 8 : next_rtr_mbx + 1);

    if (debug) {
      Serial.print("DRVR: using remote mailbox = ");
      Serial.println(selected_mbx);
    }

  } else {                                                                               // standard messages x8, 0-7
    selected_mbx = next_standard_mbx;
    next_standard_mbx = (next_standard_mbx == 7 ? 0 : next_standard_mbx + 1);

    if (debug) {
      Serial.print("DRVR: using standard mailbox = ");
      Serial.println(selected_mbx);
    }
  }

  rc = R_CAN_Write(&_can_ctrl, selected_mbx, msg);

  if (debug) {
    Serial.print("DRVR: R_CAN_Write returns ");
    Serial.println(rc);
    Serial.flush();
  }

  return (rc == FSP_SUCCESS);
}

/// called from CAN interrupt to surface events

void UNOR4CAN::onCanCallback2(can_callback_args_t *p_args) {

  if (user_callback) {
    (void)(*user_callback)(p_args);
  }

  return;
}

/// set the CAN bitrate

void UNOR4CAN::set_can_bitrate(const uint32_t bitrate) {

  can_bitrate = bitrate;

  if (debug) {
    Serial.print("DRVR: set bitrate = ");
    Serial.println(bitrate);
    Serial.flush();
  }

  return;
}

/// set the CAN callback

void UNOR4CAN::set_callback(void (*fptr)(can_callback_args_t *event)) {

  user_callback = fptr;
  return;
}

/// set debug output state

void UNOR4CAN::set_debug(const bool state) {

  debug = state;

  if (debug) {
    Serial.println("DRVR: debug is on");
    Serial.flush();
  }

  return;
}

/// get CAN stats
/// fsp_err_t R_CAN_InfoGet(can_ctrl_t * const p_api_ctrl, can_info_t * const p_info);

bool UNOR4CAN::get_stats(can_info_t *pstats) {

  fsp_err_t ok;

  if (debug) {
    Serial.print("DRVR: getting CAN driver stats ... ");
    Serial.flush();
  }

  ok = R_CAN_InfoGet(&_can_ctrl, pstats);

  if (debug) {
    Serial.println((ok == FSP_SUCCESS ? "ok" : "fail"));
    Serial.flush();
  }

  return (ok == FSP_SUCCESS);
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

extern "C" void can_callback2(can_callback_args_t *p_args)
{
  UNOR4CAN *this_ptr = (UNOR4CAN *)(p_args->p_context);
  this_ptr->onCanCallback2(p_args);
}

