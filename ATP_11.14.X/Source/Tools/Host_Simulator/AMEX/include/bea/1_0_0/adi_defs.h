/****************************************************************************
 Copyright © 1999, Hypercom Inc.
 All Rights Reserved

 Project: Lynk RealPay

 File Name: adi_defs.h
 Purpose: Definitions and prototypes used by the Ascendent Database
          Interface (ADI).

 Author: Jeff Edelen

 Revision History:
 $Log:   N:\PVCS\PTE\Lynk\app_lib\ADI_DEFS.H  $
   
      Rev 1.1   Feb 21 2000 14:18:54   nshort
   1_0_8 link to core 1_3_2_1
*****************************************************************************/


#ifndef ADI_DEFS_H
#define ADI_DEFS_H

#ifdef ADI_SQLSERVER

#ifndef SQLSERVER
#define SQLSERVER 1
#endif

#define ADI_NULL_COLUMN_DATA 100
#define ADI_NO_DATA_FOUND    ADI_NULL_COLUMN_DATA

void ErrorHandler(void);
void CopyNonWhiteSpace( char* pOut, char* pIn, int nMaxLen );
BYTE dbcommon_SQLServer_to_pte_rcode( LONG oracle_rcode, pCHAR ErrorMsg );

#endif /* ADI_SQLSERVER */

#ifdef ADI_ORACLE

#ifndef ORACLE
#define ORACLE 1
#endif

#define ADI_NULL_COLUMN_DATA (-1405)
#define ADI_NO_DATA_FOUND    1403

void ErrorHandler(void);
BYTE dbcommon_oracle_to_pte_rcode ( LONG oracle_rcode, pCHAR ErrorMsg );

#endif /* ADI_ORACLE */

#endif /* ADI_DEFS_H */