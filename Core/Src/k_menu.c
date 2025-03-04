/**  ******************************************************************************  * @file         k_menu.c  * @modified by  Werner Wolfrum  * @date         09.03.2019  * @modification Insert some text boxes for IMU test on I2C (a HACK!!!).
                  Insert some comments about function parameter assignment.

                  Original file name, author, description, copyright below.  ******************************************************************************  *//**
  ******************************************************************************
  * @file    k_menu.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel menu functions 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/

#include "main.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_MENU
  * @brief Kernel menu routines
  * @{
  */

/* Private defines -----------------------------------------------------------*/
#define ID_ICONVIEW_MENU               (GUI_ID_USER + 0x00)
#define ID_FEATURES_FPU                (GUI_ID_USER + 0x44)
#define ID_FEATURES_IC                 (GUI_ID_USER + 0x45)
#define ID_FEATURES_DC                 (GUI_ID_USER + 0x46)
#define ID_FEATURES_CPU                (GUI_ID_USER + 0x47)
#define ID_FEATURES_FREQ               (GUI_ID_USER + 0x48)

#define ID_FEATURES_X_ACCEL            (GUI_ID_USER + 0x80)
#define ID_FEATURES_Y_ACCEL            (GUI_ID_USER + 0x81)
#define ID_FEATURES_Z_ACCEL            (GUI_ID_USER + 0x82)
#define ID_FEATURES_X_GYRO             (GUI_ID_USER + 0x83)
#define ID_FEATURES_Y_GYRO             (GUI_ID_USER + 0x84)
#define ID_FEATURES_Z_GYRO             (GUI_ID_USER + 0x85)
#define ID_FEATURES_TEMP               (GUI_ID_USER + 0x86)

ICONVIEW_Handle hIcon ;
WM_HWIN         hCPULoad;
extern GUI_CONST_STORAGE GUI_BITMAP bmF7Logo;

volatile int16_t mpu_data[7];


/**
  * @brief  Callback routine of desktop window.
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */
static void _cbBk(WM_MESSAGE * pMsg) {
  
  uint32_t NCode, Id;
  static uint8_t sel;
  
  switch (pMsg->MsgId) 
  {
  case WM_PAINT:
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();

    /*H/W Information */
    GUI_SetColor(0x007C18CC);
    GUI_AA_FillRoundedRect(400, 31, 780, 238, 15);
    GUI_DrawBitmap(&bmF7Logo, 400, 37); // WW: Shift upward

    break;
  
  
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);     
    NCode = pMsg->Data.v;  
    
    switch (NCode) 
    {  
      
    case WM_NOTIFICATION_RELEASED:
      if (Id == ID_ICONVIEW_MENU)
      {
        sel = ST_AnimatedIconView_GetSel(pMsg->hWinSrc);
        
        if(sel < k_ModuleGetNumber())
        {
          ST_AnimatedIconView_SetSel(pMsg->hWinSrc, -1);
          if(module_prop[sel].in_use == 0)
          {
            module_prop[sel].in_use = 1;
            module_prop[sel].module->startup(pMsg->hWin, 0, 0);
          }
          else if(module_prop[sel].win_state == 1)
          {
            module_prop[sel].module->startup(pMsg->hWin, 0, 0);
          }          
        }
        else
        {
          WM_InvalidateWindow (pMsg->hWinSrc);
        }
      }
      break;

    case WM_NOTIFICATION_CHILD_DELETED:
        module_prop[sel].in_use = 0;
        module_prop[sel].win_state = 0;
        break;
      
    default:
      break;
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/**
  * @brief  set the GUI profile
  * @param  None.
  * @retval None.
  */
void k_PeriodicProcesses(void)
{
  char tmp[20];
  WM_HWIN   hItem; 
  static uint32_t InitTick = 0;
  
  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_CPU);
  
  if(hItem)
  {
    if((WM_IsVisible(hItem)) && ((osKernelSysTick() - InitTick ) > 500))
    {
      InitTick = osKernelSysTick();
      
      sprintf((char *)tmp , "MCU Load : %d%%",  osGetCPUUsage());
      TEXT_SetText(hItem, tmp);
      
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_X_ACCEL);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "X-Acc: %5d", mpu_data[0]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_Y_ACCEL);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "Y-Acc: %5d", mpu_data[1]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_Z_ACCEL);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "Z-Acc: %5d", mpu_data[2]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_X_GYRO);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "X-Gyro: %5d", mpu_data[4]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_Y_GYRO);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "Y-Gyro: %5d", mpu_data[5]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_Z_GYRO);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "Z-Gyro: %5d", mpu_data[6]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

  hItem = WM_GetDialogItem(WM_GetDesktopWindowEx(0), ID_FEATURES_TEMP);
  if(hItem)
  {
    if( WM_IsVisible(hItem) )
    {
      sprintf((char *)tmp , "Temp: %5d", mpu_data[3]);
      TEXT_SetText(hItem, tmp);
      WM_InvalidateWindow(hItem);
      WM_Update(hItem);
    }
  }

}

/**
  * @brief  set the GUI profile
  * @param  None.
  * @retval None.
  */
void k_SetGuiProfile(void) 
{
  BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
  
  FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
  FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);      
  
  FRAMEWIN_SetDefaultBarColor(0, GUI_STCOLOR_LIGHTBLUE);
  FRAMEWIN_SetDefaultBarColor(1, GUI_STCOLOR_LIGHTBLUE);
  
  FRAMEWIN_SetDefaultFont(&GUI_FontAvantGarde20B);
  
  FRAMEWIN_SetDefaultClientColor(GUI_WHITE);  
  FRAMEWIN_SetDefaultTitleHeight(25);
  
  
  WINDOW_SetDefaultBkColor(GUI_WHITE);
  
  LISTVIEW_SetDefaultGridColor(GUI_WHITE);
  LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
  DROPDOWN_SetDefaultColor(DROPDOWN_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
  LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
  DROPDOWN_SetDefaultColor(DROPDOWN_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
  SCROLLBAR_SetDefaultWidth(8);
  SCROLLBAR_SetDefaultSTSkin();
  
  HEADER_SetDefaultBkColor(GUI_STCOLOR_LIGHTBLUE);
  HEADER_SetDefaultTextColor(GUI_WHITE);
  HEADER_SetDefaultFont(GUI_FONT_16_1);
  HEADER_SetDefaultSTSkin();
  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_THUMB);  
  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_SHAFT);  
  SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_ARROW);    
  ST_CHOOSEFILE_SetDelim('/');
  GUI_SetDefaultFont(GUI_FONT_20_ASCII);
  
  ST_CHOOSEFILE_SetButtonSize(40, 20);
    
  TEXT_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);
  TEXT_SetDefaultFont(&GUI_FontLubalGraph20);
}

/**
  * @brief  Initializes the menu.
  * @param  None.
  * @retval None.
  */
void k_InitMenu(void) 
{
  uint8_t i = 0;
  WM_HWIN   hItem;    //WW: IMU-HACK
  memset(mpu_data, 0, 2*7);

  WM_SetCallback(WM_GetDesktopWindowEx(0), _cbBk);
  
  // ST_ANIMATED_ICONVIEW_Handle ST_AnimatedIconView_CreateEx(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, int xSizeItems, int ySizeItems, int delay, int FrameNbr);
  hIcon = ST_AnimatedIconView_CreateEx(0,                                              // int x0
                                       0,                                              // int y0
                                       LCD_GetXSize() - 80,                            // int xSize - let a frame of 80 pxs on the rigth side
                                       LCD_GetYSize() - 0,                             // int ySize
                                       WM_GetDesktopWindowEx(0),                       // WM_HWIN hParent
                                       WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_LATE_CLIP,  // int WinFlags
                                       0,                                              // int ExFlags
                                       ID_ICONVIEW_MENU,                               // int Id
                                       100,                                            // int xSizeItems
                                       120,                                            // int ySizeItems
									   40,                                             // int delay
									   5);                                             // int FrameNbr
  
  ST_AnimatedIconView_SetDualFont(hIcon, &GUI_FontLubalGraph16, &GUI_FontLubalGraph16);
  
  ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_Y, 0);
  ST_AnimatedIconView_SetSpace(hIcon, GUI_COORD_X, 0);
  ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_Y, 0);
  ST_AnimatedIconView_SetFrame(hIcon, GUI_COORD_X, 0);
  
  ST_AnimatedIconView_SetTextColor(hIcon, ICONVIEW_CI_UNSEL, GUI_STCOLOR_LIGHTBLUE);
  ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_UNSEL, GUI_WHITE);
  ST_AnimatedIconView_SetBkColor(hIcon, ICONVIEW_CI_SEL, GUI_WHITE);
  ST_AnimatedIconView_SetSel(hIcon, -1);
  ST_AnimatedIconView_SetDualTextColor(hIcon, ICONVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE, GUI_STCOLOR_DARKBLUE    );  
  
  for (i = 0; i < k_ModuleGetNumber(); i++)
  {   
    ST_AnimatedIconView_AddIcon(hIcon, module_prop[i].module->open_icon, module_prop[i].module->close_icon, (char *)module_prop[i].module->name);
  } 
  
#if 0
  // TEXT_Handle TEXT_CreateEx(int x0, int y0, int xSize, int ySize, WM_HWIN hParent, int WinFlags, int ExFlags, int Id, const char * pText);
  hItem = TEXT_CreateEx(407, 152-30, 260, 20, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FPU, "FPU : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE); 
  
  hItem = TEXT_CreateEx(407, 167, 260, 20, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_IC, "I-Cache : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);
  
  hItem = TEXT_CreateEx(407, 182, 260, 20, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_DC, "D-Cache : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);  
  
  hItem = TEXT_CreateEx(407, 198, 260, 20, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_CPU, "MCU Load < 1%");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);  
  
  hItem = TEXT_CreateEx(407, 212, 260, 20, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FREQ, "Clk : 200 MHz");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);    
#else
  hItem = TEXT_CreateEx(407, 115, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FPU, "FPU : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);

  hItem = TEXT_CreateEx(407, 125, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_IC, "I-Cache : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE); //GUI_WHITE);

  hItem = TEXT_CreateEx(407, 135, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_DC, "D-Cache : ON");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);

  hItem = TEXT_CreateEx(407, 145, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_CPU, "MCU Load < 1%");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);

  hItem = TEXT_CreateEx(407, 155, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_FREQ, "Clk : 200 MHz");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_WHITE);
#endif
  
  hItem = TEXT_CreateEx(407, 165, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_X_ACCEL, "X-Acc: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);

  hItem = TEXT_CreateEx(407, 175, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_Y_ACCEL, "Y-Acc: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE); //GUI_WHITE);

  hItem = TEXT_CreateEx(407, 185, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_Z_ACCEL, "Z-Acc: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_LIGHTBLUE);

  hItem = TEXT_CreateEx(407, 195, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_X_GYRO, "X-Gyro: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_DARKBLUE);

  hItem = TEXT_CreateEx(407, 205, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_Y_GYRO, "Y-Gyro: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_DARKBLUE);

  hItem = TEXT_CreateEx(407, 215, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_Z_GYRO, "Z-Gyro: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_STCOLOR_DARKBLUE);

  hItem = TEXT_CreateEx(407, 225, 260, 10, WM_GetDesktopWindowEx(0), WM_CF_SHOW, TEXT_CF_LEFT, ID_FEATURES_TEMP,   "Temp: -----");
  TEXT_SetFont(hItem, GUI_FONT_8_1);
  TEXT_SetTextColor(hItem, GUI_RED);

}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
