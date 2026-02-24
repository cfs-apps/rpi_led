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
**    Define GPIO Controller class
**
**  Notes:
**    TODO - Consider adding a map command if it fails during init. 
**
*/

#ifndef _led_ctrl_
#define _led_ctrl_

/*
** Includes
*/

#include "app_cfg.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Event Message IDs
*/

#define LED_CTRL_CONSTRUCTOR_EID  (LED_CTRL_BASE_EID + 0)
#define LED_CTRL_SET_ON_TIME_EID  (LED_CTRL_BASE_EID + 1)
#define LED_CTRL_SET_OFF_TIME_EID (LED_CTRL_BASE_EID + 2)
#define LED_CTRL_CHILD_TASK_EID   (LED_CTRL_BASE_EID + 3)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
** - See EDS command definitions in rpi_led.xml
*/

/******************************************************************************
** Telmetery Packets
*/


/******************************************************************************
** GPIO_CTRL_Class
*/

typedef struct
{

   /*
   ** Framework References
   */
   
   INITBL_Class_t  *IniTbl;


   /*
   ** Class State Data
   */

   bool    IsMapped;
   bool    LedOn;
   uint8   OutPin;
   uint16  OnTime;    /* Time in Milliseconds */
   uint16  OffTime;   /* Time in Milliseconds */ 
   
   /*
   ** Telemetry Packets
   */
   
   
} LED_CTRL_Class_t;



/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: LED_CTRL_Constructor
**
** Initialize the GPIO Controller object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void LED_CTRL_Constructor(LED_CTRL_Class_t *LedCtrlPtr, INITBL_Class_t *IniTbl);


/******************************************************************************
** Function: LED_CTRL_ChildTask
**
** Notes:
**   1.  This is a CHILDMGR callback function and the function signature must
**       match CHILDMGR_TaskCallback_t.
**
*/
bool LED_CTRL_ChildTask(CHILDMGR_Class_t *ChildMgr);


/******************************************************************************
** Function: LED_CTRL_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in status telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void LED_CTRL_ResetStatus(void);


/******************************************************************************
** Function: LED_CTRL_SetOffTimeCmd
**
*/
bool LED_CTRL_SetOffTimeCmd(void *DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


/******************************************************************************
** Function: LED_CTRL_SetOnTimeCmd
**
*/
bool LED_CTRL_SetOnTimeCmd(void *DataObjPtr, const CFE_MSG_Message_t *MsgPtr);


#endif /* _led_ctrl_ */
