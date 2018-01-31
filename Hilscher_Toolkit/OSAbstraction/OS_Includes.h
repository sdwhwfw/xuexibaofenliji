/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile: OS_Includes.h $
   Last Modification:
    $Author: Robert $
    $Modtime: 30.07.07 16:02 $
    $Revision: 2785 $
   
   Targets:
     None
 
   Description:
     Headerfile for specific target system includes, data types and definitions
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         08.08.2006  MT       initial version (special OS dependencies must be added)
 
**************************************************************************************/

#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H

#include "stm32f2xx.h"

  #define APIENTRY

  #ifndef NULL
    #define NULL 0
  #endif

  #define UNREFERENCED_PARAMETER(a) (a=a)

#endif /* __OS_INCLUDES__H */
