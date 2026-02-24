/*
**  Copyright 2022 bitValence, Inc.
**  All Rights Reserved.
**
**  This program is free software; you can modify and/or redistribute it
**  under the terms of the GNU Affero General Public License
**  as published by the Free Software Foundation; version 3 with
**  attribution addendums as found in the LICENSE.txt
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU Affero General Public License for more details.
**
**  Purpose:
**    Define application configurations for the RPI LED Control Demo application
**
**  Notes:
**    1. These macros can only be built with the application and can't
**       have a platform scope because the same app_cfg.h filename is used for
**       all applications following the object-based application design.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "rpi_led_eds_typedefs.h"

#include "rpi_led_platform_cfg.h"
#include "app_c_fw.h"


/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0 - Initial release
** 3.0 - Compatible with Basecamp 4.0 and cFS 7.0
**
*/

#define  RPI_LED_MAJOR_VER   2
#define  RPI_LED_MINOR_VER   0


/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_APP_CFE_NAME     APP_CFE_NAME
#define CFG_APP_PERF_ID      APP_PERF_ID

#define CFG_CMD_PIPE_NAME    APP_CMD_PIPE_NAME
#define CFG_CMD_PIPE_DEPTH   APP_CMD_PIPE_DEPTH

#define CFG_RPI_LED_CMD_TOPICID        RPI_LED_CMD_TOPICID
#define CFG_BC_SCH_1_HZ_TOPICID        BC_SCH_1_HZ_TOPICID
#define CFG_RPI_LED_STATUS_TLM_TOPICID RPI_LED_STATUS_TLM_TOPICID

#define CFG_CHILD_NAME       CHILD_NAME
#define CFG_CHILD_PERF_ID    CHILD_PERF_ID
#define CFG_CHILD_STACK_SIZE CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY   CHILD_PRIORITY

#define CFG_CTRL_OUT_PIN     CTRL_OUT_PIN
#define CFG_CTRL_ON_TIME     CTRL_ON_TIME
#define CFG_CTRL_OFF_TIME    CTRL_OFF_TIME

#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_PERF_ID,uint32) \
   XX(APP_CMD_PIPE_NAME,char*) \
   XX(APP_CMD_PIPE_DEPTH,uint32) \
   XX(RPI_LED_CMD_TOPICID,uint32) \
   XX(BC_SCH_1_HZ_TOPICID,uint32) \
   XX(RPI_LED_STATUS_TLM_TOPICID,uint32) \
   XX(CHILD_NAME,char*) \
   XX(CHILD_PERF_ID,uint32) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32) \
   XX(CTRL_OUT_PIN,uint32) \
   XX(CTRL_ON_TIME,uint32) \
   XX(CTRL_OFF_TIME,uint32) \
   
DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define RPI_LED_BASE_EID    (APP_C_FW_APP_BASE_EID +  0)
#define LED_CTRL_BASE_EID   (APP_C_FW_APP_BASE_EID + 20)


#endif /* _app_cfg_ */
