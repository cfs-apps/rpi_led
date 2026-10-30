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
**    Implement the RPI LED Control Demo application
**
**  Notes:
**    1. See rpi_led_app.h for details.
**
*/

/*
** Includes
*/

#include <string.h>
#include "rpi_led_app.h"
#include "rpi_led_eds_cc.h"

/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ    (&(RpiLed.IniTbl))
#define  CMDMGR_OBJ    (&(RpiLed.CmdMgr))
#define  CHILDMGR_OBJ  (&(RpiLed.ChildMgr))
#define  LED_CTRL_OBJ  (&(RpiLed.LedCtrl))


/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(void);
static int32 ProcessCommands(void);
static void SendStatusTlm(void);


/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  


static CFE_EVS_BinFilter_t  EventFilters[] =
{  
   /* Event ID                  Mask */
   {LED_CTRL_CHILD_TASK_EID,   CFE_EVS_FIRST_4_STOP}  // Use CFE_EVS_NO_FILTER to see all events

};


/*****************/
/** Global Data **/
/*****************/

RPI_LED_Class_t  RpiLed;


/******************************************************************************
** Function: RPI_LED_AppMain
**
*/
void RPI_LED_AppMain(void)
{

   uint32 RunStatus = CFE_ES_RunStatus_APP_ERROR;


   CFE_EVS_Register(EventFilters, sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                    CFE_EVS_EventFilter_BINARY);

   if (InitApp() == CFE_SUCCESS) /* Performs initial CFE_ES_PerfLogEntry() call */
   {  
   
      RunStatus = CFE_ES_RunStatus_APP_RUN;
      
   }
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      RunStatus = ProcessCommands(); /* Pends indefinitely & manages CFE_ES_PerfLogEntry() calls */

   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("RPI_LED App terminating, err = 0x%08X\n", RunStatus);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(RPI_LED_EXIT_EID, CFE_EVS_EventType_CRITICAL, "RPI_LED App terminating, err = 0x%08X", RunStatus);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of RPI_LED_AppMain() */


/******************************************************************************
** Function: RPI_LED_NoOpCmd
**
*/

bool RPI_LED_NoOpCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_SendEvent (RPI_LED_NOOP_EID, CFE_EVS_EventType_INFORMATION,
                      "No operation command received for RPI_LED App version %d.%d.%d",
                      RPI_LED_MAJOR_VER, RPI_LED_MINOR_VER, RPI_LED_PLATFORM_REV);

   return true;


} /* End RPI_LED_NoOpCmd() */


/******************************************************************************
** Function: RPI_LED_ResetAppCmd
**
** Notes:
**   1. No need to pass an object reference to contained objects because they
**      already have a reference from when they were constructed
**
*/

bool RPI_LED_ResetAppCmd(void *ObjDataPtr, const CFE_MSG_Message_t *MsgPtr)
{

   CFE_EVS_ResetAllFilters();
   
   CMDMGR_ResetStatus(CMDMGR_OBJ);
   CHILDMGR_ResetStatus(CHILDMGR_OBJ);
   
   LED_CTRL_ResetStatus();
	  
   return true;

} /* End RPI_LED_ResetAppCmd() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{

   int32 Status = APP_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit_t ChildTaskInit;
   
   /*
   ** Initialize objects 
   */

   if (INITBL_Constructor(&RpiLed.IniTbl, RPI_LED_INI_FILENAME, &IniCfgEnum))
   {
   
      RpiLed.PerfId           = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_PERF_ID);
      RpiLed.CmdMid           = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_RPI_LED_CMD_TOPICID));
      RpiLed.SendStatusTlmMid = CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_RPI_LED_SEND_STATUS_TLM_TOPICID));
      
      CFE_ES_PerfLogEntry(RpiLed.PerfId);

      /* Constructor sends error events */    
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PERF_ID);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, ChildMgr_TaskMainCallback,
                                    LED_CTRL_ChildTask, &ChildTaskInit); 
  
   } /* End if INITBL Constructed */
  
   if (Status == CFE_SUCCESS)
   {

      LED_CTRL_Constructor(LED_CTRL_OBJ, &RpiLed.IniTbl);

      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&RpiLed.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  
      CFE_SB_Subscribe(RpiLed.CmdMid, RpiLed.CmdPipe);
      CFE_SB_Subscribe(RpiLed.SendStatusTlmMid, RpiLed.CmdPipe);

      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, EDS_CONTAINER_RPI_LED_NOOP_CC,   NULL, RPI_LED_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, EDS_CONTAINER_RPI_LED_RESET_CC,  NULL, RPI_LED_ResetAppCmd, 0);

      CMDMGR_RegisterFunc(CMDMGR_OBJ, EDS_CONTAINER_RPI_LED_SET_ON_TIME_CC,  LED_CTRL_OBJ, LED_CTRL_SetOnTimeCmd,  sizeof(RPI_LED_SetOnTime_Payload_t));
      CMDMGR_RegisterFunc(CMDMGR_OBJ, EDS_CONTAINER_RPI_LED_SET_OFF_TIME_CC, LED_CTRL_OBJ, LED_CTRL_SetOffTimeCmd, sizeof(RPI_LED_SetOffTime_Payload_t));
      
      CFE_MSG_Init(CFE_MSG_PTR(RpiLed.StatusTlm.TelemetryHeader), CFE_SB_ValueToMsgId(INITBL_GetIntConfig(INITBL_OBJ, CFG_RPI_LED_STATUS_TLM_TOPICID)), sizeof(RPI_LED_StatusTlm_t));
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(RPI_LED_INIT_APP_EID, CFE_EVS_EventType_INFORMATION,
                        "RPI_LED App Initialized. Version %d.%d.%d",
                        RPI_LED_MAJOR_VER, RPI_LED_MINOR_VER, RPI_LED_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static int32 ProcessCommands(void)
{

   int32  RetStatus = CFE_ES_RunStatus_APP_RUN;
   int32  SysStatus;

   CFE_SB_Buffer_t *SbBufPtr;
   CFE_SB_MsgId_t   MsgId = CFE_SB_INVALID_MSG_ID;
   

   CFE_ES_PerfLogExit(RpiLed.PerfId);
   SysStatus = CFE_SB_ReceiveBuffer(&SbBufPtr, RpiLed.CmdPipe, CFE_SB_PEND_FOREVER);
   CFE_ES_PerfLogEntry(RpiLed.PerfId);

   if (SysStatus == CFE_SUCCESS)
   {
      
      SysStatus = CFE_MSG_GetMsgId(&SbBufPtr->Msg, &MsgId);
   
      if (SysStatus == CFE_SUCCESS)
      {
  
         if (CFE_SB_MsgId_Equal(MsgId, RpiLed.CmdMid)) 
         {
            
            CMDMGR_DispatchFunc(CMDMGR_OBJ, &SbBufPtr->Msg);
         
         } 
         else if (CFE_SB_MsgId_Equal(MsgId, RpiLed.SendStatusTlmMid))
         {

            SendStatusTlm();
            
         }
         else
         {
            
            CFE_EVS_SendEvent(RPI_LED_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                              "Received invalid command packet, MID = 0x%04X",
                              CFE_SB_MsgIdToValue(MsgId));
         } 

      }
      else
      {
         
         CFE_EVS_SendEvent(RPI_LED_INVALID_MID_EID, CFE_EVS_EventType_ERROR,
                           "CFE couldn't retrieve message ID from the message, Status = %d", SysStatus);
      }
      
   } /* Valid SB receive */ 
   else 
   {
   
         CFE_ES_WriteToSysLog("RPI_LED software bus error. Status = 0x%08X\n", SysStatus);   /* Use SysLog, events may not be working */
         RetStatus = CFE_ES_RunStatus_APP_ERROR;
   }  
      
   return RetStatus;

} /* End ProcessCommands() */


/******************************************************************************
** Function: SendStatusTlm
**
*/
static void SendStatusTlm(void)
{
   
   RPI_LED_StatusTlm_Payload_t *StatusTlmPayload = &RpiLed.StatusTlm.Payload;
   
   StatusTlmPayload->ValidCmdCnt   = RpiLed.CmdMgr.ValidCmdCnt;
   StatusTlmPayload->InvalidCmdCnt = RpiLed.CmdMgr.InvalidCmdCnt;

   /*
   ** Controller 
   */ 
   
   StatusTlmPayload->CtrlIsMapped = RpiLed.LedCtrl.IsMapped;
   StatusTlmPayload->CtrlOutPin   = RpiLed.LedCtrl.OutPin;
   
   StatusTlmPayload->CtrlLedOn    = RpiLed.LedCtrl.LedOn;
   StatusTlmPayload->CtrlSpare    = 0;
         
   StatusTlmPayload->CtrlOnTime   = RpiLed.LedCtrl.OnTime;
   StatusTlmPayload->CtrlOffTime  = RpiLed.LedCtrl.OffTime;
   
   CFE_SB_TimeStampMsg(CFE_MSG_PTR(RpiLed.StatusTlm.TelemetryHeader));
   CFE_SB_TransmitMsg(CFE_MSG_PTR(RpiLed.StatusTlm.TelemetryHeader), true);
   
} /* End SendStatusTlm() */


