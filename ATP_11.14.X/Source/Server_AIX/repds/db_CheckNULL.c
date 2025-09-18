

#include "ntutils.h"
#include "app_info.h"
#include "nc_dbstruct.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "equitdb.h"

void checknull(pCHAR data , INT Len, pCHAR table, pCHAR fieldname, pCHAR funcnname, pCHAR mode);


void CheckNULLTerminatedinUCF01(pUCF01 ucf01,pCHAR mode);
void CheckNULLTerminatedinUCF02(pUCF02   ucf02,pCHAR mode);
void CheckNULLTerminatedinUCF03(pUCF03 ucf03,pCHAR mode);
void CheckNULLTerminatedinBIN01(pBIN01   bin01,pCHAR mode);
void CheckNULLTerminatedinHST01(pHST01   hst01,pCHAR mode);
void CheckNULLTerminatedinATP01(pATP01   atp01,pCHAR mode);
void CheckNULLTerminatedinATP02(pATP02   atp02,pCHAR mode);
void CheckNULLTerminatedinCCF02P(pCCF02P  ccf02p,pCHAR mode);
void CheckNULLTerminatedinCCF04(pCCF04   ccf04,pCHAR mode);
void CheckNULLTerminatedinCCF10(pCCF10   ccf10,pCHAR mode);
void CheckNULLTerminatedinBON01(pBON01   bon01,pCHAR mode);
void CheckNULLTerminatedinBON02(pBON02   bon02,pCHAR mode);
void CheckNULLTerminatedinCCF01(pCCF01   ccf01,pCHAR mode);
void CheckNULLTerminatedinCAF01(pCAF01   caf01,pCHAR mode);
void CheckNULLTerminatedinCTF01(pCTF01   ctf01,pCHAR mode);
void CheckNULLTerminatedinMCT01(pMCT01   mct01,pCHAR mode);
void CheckNULLTerminatedinCCF03(pCCF03   ccf03p,pCHAR mode);
void CheckNULLTerminatedinPAF01(pPAF01   paf01,pCHAR mode);
void CheckNULLTerminatedinDEF01(pDEF01   def01,pCHAR mode);
void CheckNULLTerminatedinCSF01(pCSF01   csf01,pCHAR mode);
void CheckNULLTerminatedinCRF01(pCRF01   crf01,pCHAR mode);
void CheckNULLTerminatedinACF01(pACF01   acf01,pCHAR mode);
void CheckNULLTerminatedinFAF01(pFAF01   faf01,pCHAR mode);
void CheckNULLTerminatedinCLF01(pCLF01   clf01,pCHAR mode);
void CheckNULLTerminatedinBPF01(pBPF01   bpf01,pCHAR mode);
void CheckNULLTerminatedinJBF01(pJBF01   jbf01,pCHAR mode);
void CheckNULLTerminatedinAPF01(pAPF01   apf01,pCHAR mode);
void CheckNULLTerminatedinFCF02(pFCF02   fcf02,pCHAR mode);
void CheckNULLTerminatedinCLF01(pCLF01   clf01,pCHAR mode);
void CheckNULLTerminatedinMCF01(pMCF01   mcf01,pCHAR mode);
void CheckNULLTerminatedinGPCF01(pGPCF01   gpcf01,pCHAR mode);
void CheckNULLTerminatedinMPPARA01(pMPPARA01   mppara01,pCHAR mode);
void CheckNULLTerminatedinMraDbPasswords(pMraDbPasswords   mradbpasswords,pCHAR mode);
void CheckNULLTerminatedinArchDbPasswords(pArchDbPasswords   archbpasswords,pCHAR mode);
void CheckNULLTerminatedinCOUNTRY_AMT_LIMITS(pCOUNTRY_AMT_LIMITS   country_amt_limits,pCHAR mode);
void CheckNULLTerminatedinMCC_LIMITS(pMCC_LIMITS   mcc_limits,pCHAR mode);
void CheckNULLTerminatedinNEGATIVE_FRAUD(pNEGATIVE_FRAUD   negative_fraud,pCHAR mode);
void CheckNULLTerminatedinOffus_velocity(pOffus_velocity   offus_velocity,pCHAR mode);
void CheckNULLTerminatedinCAF01_EMV(pCAF01_EMV   caf01_emv,pCHAR mode);
void CheckNULLTerminatedinMCC01(pMCC01   mcc01,pCHAR mode);
void CheckNULLTerminatedinOMC01(pOMC01   omc01,pCHAR mode);
void CheckNULLTerminatedinBIN02(pBIN02   bin02,pCHAR mode);
void CheckNULLTerminatedinUAG01(pUAG01   uag01,pCHAR mode);
void CheckNULLTerminatedinTLF01(pTLF01   tlf01,pCHAR mode);
void CheckNULLTerminatedinDCF01(pDCF01   dcf01,pCHAR mode);
void CheckNULLTerminatedinBCH01(pBCH01   bch01,pCHAR mode);
void CheckNULLTerminatedinBCH10(pBCH10   bch10,pCHAR mode);
void CheckNULLTerminatedinOCF01(pOCF01   ocf01,pCHAR mode);
void CheckNULLTerminatedinOCF20(pOCF20   ocf20,pCHAR mode);
void CheckNULLTerminatedinMCF20(pMCF20   mcf20,pCHAR mode);
void CheckNULLTerminatedinNMF01(pNMF01   nmf01,pCHAR mode);
void CheckNULLTerminatedinNBF01(pNBF01   nbf01,pCHAR mode);
void CheckNULLTerminatedinNGF01(pNGF01   ngf01,pCHAR mode);
void CheckNULLTerminatedinNFI01(pNFI01   nfi01,pCHAR mode);
void CheckNULLTerminatedinNTF01(pNTF01   ntf01,pCHAR mode);
void CheckNULLTerminatedinBCH11(pBCH11   bch11,pCHAR mode);
void CheckNULLTerminatedinBCH20(pBCH20   bch20,pCHAR mode);
void CheckNULLTerminatedinBCH01_MP(pBCH01_MP bch01_mp,pCHAR mode);
void CheckNULLTerminatedinNCF01(pNCF01 ncf01,pCHAR mode);
void CheckNULLTerminatedinNCF02(pNCF02 ncf02,pCHAR mode);
void CheckNULLTerminatedinMGF01(pMGF01   mgf01,pCHAR mode);
void CheckNULLTerminatedinNCF20(pNCF20   ncf20,pCHAR mode);
void CheckNULLTerminatedinNCF21(pNCF21   ncf21,pCHAR mode);
void CheckNULLTerminatedinNCF30(pNCF30   ncf30,pCHAR mode);
void CheckNULLTerminatedinATM01(pATM01   atm01,pCHAR mode);
void CheckNULLTerminatedinSAF01(pSAF01   saf01,pCHAR mode);
void CheckNULLTerminatedinFG01(pFG01    fg01,pCHAR mode);
void CheckNULLTerminatedinFGUARD(pFGUARD   fguard,pCHAR mode);
void CheckNULLTerminatedinEMV_RAW(pEMV_RAW  emvraw,pCHAR mode);
void CheckNULLTerminatedinFG_RAW(pFG_RAW   fgraw,pCHAR mode);
void CheckNULLTerminatedinTPOS01(pTPOS01 tps01,pCHAR mode );


void CheckNULLTerminatedinUCF01(pUCF01 ucf01,pCHAR mode)
{
	checknull(ucf01->primary_key.name,sizeof(ucf01->primary_key.name),"UCF01","primary_key.name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password			  			   ,sizeof(ucf01->password),"UCF01","password","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->date_added                           ,sizeof(ucf01->date_added),"UCF01","date_added","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->group_id                         ,sizeof(ucf01->group_id),"UCF01","group_id","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[0].subsystem_name          ,sizeof(ucf01->control[0].subsystem_name),"UCF01","control[0].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[0].action          ,sizeof(ucf01->control[0].action),"UCF01","control[0].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[1].subsystem_name          ,sizeof(ucf01->control[1].subsystem_name),"UCF01","control[1].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[1].action          ,sizeof(ucf01->control[1].action),"UCF01","control[1].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[2].subsystem_name          ,sizeof(ucf01->control[2].subsystem_name),"UCF01","control[2].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[2].action          ,sizeof(ucf01->control[2].action),"UCF01","control[2].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[3].subsystem_name          ,sizeof(ucf01->control[3].subsystem_name),"UCF01","control[3].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[3].action          ,sizeof(ucf01->control[3].action),"UCF01","control[3].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[4].subsystem_name          ,sizeof(ucf01->control[4].subsystem_name),"UCF01","control[4].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[4].action          ,sizeof(ucf01->control[4].action),"UCF01","control[4].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[5].subsystem_name          ,sizeof(ucf01->control[5].subsystem_name),"UCF01","control[5].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[5].action          ,sizeof(ucf01->control[5].action),"UCF01","control[5].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[6].subsystem_name          ,sizeof(ucf01->control[6].subsystem_name),"UCF01","control[6].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[6].action          ,sizeof(ucf01->control[6].action),"UCF01","control[6].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[7].subsystem_name          ,sizeof(ucf01->control[7].subsystem_name),"UCF01","control[7].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[7].action          ,sizeof(ucf01->control[7].action),"UCF01","control[7].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[8].subsystem_name          ,sizeof(ucf01->control[8].subsystem_name),"UCF01","control[8].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[8].action          ,sizeof(ucf01->control[8].action),"UCF01","control[8].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[9].subsystem_name          ,sizeof(ucf01->control[9].subsystem_name),"UCF01","control[9].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[9].action          ,sizeof(ucf01->control[9].action),"UCF01","control[9].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[10].subsystem_name          ,sizeof(ucf01->control[10].subsystem_name),"UCF01","control[10].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[10].action          ,sizeof(ucf01->control[10].action),"UCF01","control[10].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[11].subsystem_name          ,sizeof(ucf01->control[11].subsystem_name),"UCF01","control[11].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[11].action          ,sizeof(ucf01->control[11].action),"UCF01","control[11].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[12].subsystem_name          ,sizeof(ucf01->control[12].subsystem_name),"UCF01","control[12].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[12].action          ,sizeof(ucf01->control[12].action),"UCF01","control[12].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[13].subsystem_name          ,sizeof(ucf01->control[13].subsystem_name),"UCF01","control[13].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[13].action          ,sizeof(ucf01->control[13].action),"UCF01","control[13].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[14].subsystem_name          ,sizeof(ucf01->control[14].subsystem_name),"UCF01","control[14].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[14].action          ,sizeof(ucf01->control[14].action),"UCF01","control[14].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[15].subsystem_name          ,sizeof(ucf01->control[15].subsystem_name),"UCF01","control[15].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[15].action          ,sizeof(ucf01->control[15].action),"UCF01","control[15].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[16].subsystem_name          ,sizeof(ucf01->control[16].subsystem_name),"UCF01","control[16].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[16].action          ,sizeof(ucf01->control[16].action),"UCF01","control[16].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[17].subsystem_name          ,sizeof(ucf01->control[17].subsystem_name),"UCF01","control[17].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[17].action          ,sizeof(ucf01->control[17].action),"UCF01","control[17].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[18].subsystem_name          ,sizeof(ucf01->control[18].subsystem_name),"UCF01","control[18].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[18].action          ,sizeof(ucf01->control[18].action),"UCF01","control[18].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[19].subsystem_name          ,sizeof(ucf01->control[19].subsystem_name),"UCF01","control[19].subsystem_name","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->control[19].action          ,sizeof(ucf01->control[19].action),"UCF01","control[19].action","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[0].terminals          ,sizeof(ucf01->login[0].terminals),"UCF01","login[0].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[1].terminals          ,sizeof(ucf01->login[1].terminals),"UCF01","login[1].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[2].terminals          ,sizeof(ucf01->login[2].terminals),"UCF01","login[2].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[3].terminals          ,sizeof(ucf01->login[3].terminals),"UCF01","login[3].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[4].terminals          ,sizeof(ucf01->login[4].terminals),"UCF01","login[4].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[5].terminals          ,sizeof(ucf01->login[5].terminals),"UCF01","login[5].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[6].terminals          ,sizeof(ucf01->login[6].terminals),"UCF01","login[6].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[7].terminals          ,sizeof(ucf01->login[7].terminals),"UCF01","login[7].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[8].terminals          ,sizeof(ucf01->login[8].terminals),"UCF01","login[8].terminals","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->login[9].terminals          ,sizeof(ucf01->login[9].terminals),"UCF01","login[9].terminals","CheckNULLTerminatedinUCF01",mode);

	checknull(ucf01->password_1          ,sizeof(ucf01->password_1),"UCF01","password_1","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_2          ,sizeof(ucf01->password_2),"UCF01","password_2","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_3          ,sizeof(ucf01->password_3),"UCF01","password_3","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_4          ,sizeof(ucf01->password_4),"UCF01","password_4","CheckNULLTerminatedinUCF01",mode);

	checknull(ucf01->accountstatus          ,sizeof(ucf01->accountstatus),"UCF01","accountstatus","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_modified_date          ,sizeof(ucf01->password_modified_date),"UCF01","password_modified_date","CheckNULLTerminatedinUCF01",mode);

	checknull(ucf01->user_info_1          ,sizeof(ucf01->user_info_1),"UCF01","user_info_1","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_2          ,sizeof(ucf01->user_info_2),"UCF01","user_info_2","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_3          ,sizeof(ucf01->user_info_3),"UCF01","user_info_3","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_4          ,sizeof(ucf01->user_info_4),"UCF01","user_info_4","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_5          ,sizeof(ucf01->user_info_5),"UCF01","user_info_5","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_6          ,sizeof(ucf01->user_info_6),"UCF01","user_info_6","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->user_info_7          ,sizeof(ucf01->user_info_7),"UCF01","user_info_7","CheckNULLTerminatedinUCF01",mode);


	checknull(ucf01->ekey          ,sizeof(ucf01->ekey),"UCF01","ekey","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->dekey          ,sizeof(ucf01->dekey),"UCF01","dekey","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->buffer          ,sizeof(ucf01->buffer),"UCF01","buffer","CheckNULLTerminatedinUCF01",mode);

	checknull(ucf01->Login_WorkStationId          ,sizeof(ucf01->Login_WorkStationId),"UCF01","Login_WorkStationId","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->Login_Status          ,sizeof(ucf01->Login_Status),"UCF01","Login_Status","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->User_Status          ,sizeof(ucf01->User_Status),"UCF01","User_Status","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->Days_Inactive          ,sizeof(ucf01->Days_Inactive),"UCF01","Days_Inactive","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->Login_Date          ,sizeof(ucf01->Login_Date),"UCF01","Login_Date","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->User_Expiry_Date          ,sizeof(ucf01->User_Expiry_Date),"UCF01","User_Expiry_Date","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->Date_Of_Deactivation          ,sizeof(ucf01->Date_Of_Deactivation),"UCF01","Date_Of_Deactivation","CheckNULLTerminatedinUCF01",mode);

	checknull(ucf01->password_5          ,sizeof(ucf01->password_5),"UCF01","password_5","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_6          ,sizeof(ucf01->password_6),"UCF01","password_6","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_7          ,sizeof(ucf01->password_7),"UCF01","password_7","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_8          ,sizeof(ucf01->password_8),"UCF01","password_8","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_9          ,sizeof(ucf01->password_9),"UCF01","password_9","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->password_10          ,sizeof(ucf01->password_10),"UCF01","password_10","CheckNULLTerminatedinUCF01",mode);
	checknull(ucf01->Logout_Date          ,sizeof(ucf01->Logout_Date),"UCF01","Logout_Date","CheckNULLTerminatedinUCF01",mode);

}

void CheckNULLTerminatedinGPCF01(pGPCF01   gpcf01, pCHAR mode)
{
	checknull(gpcf01->days_inactive,sizeof(gpcf01->days_inactive),"GPCF01","days_inactive","CheckNULLTerminatedinGPCF01",mode);
	checknull(gpcf01->max_prev_passwords_not_allowed,sizeof(gpcf01->max_prev_passwords_not_allowed),"GPCF01","max_prev_passwords_not_allowed","CheckNULLTerminatedinGPCF01",mode);
	checknull(gpcf01->max_password_retries,sizeof(gpcf01->max_password_retries),"GPCF01","max_password_retries","CheckNULLTerminatedinGPCF01",mode);
	checknull(gpcf01->min_password_length,sizeof(gpcf01->min_password_length),"GPCF01","min_password_length","CheckNULLTerminatedinGPCF01",mode);

}

void CheckNULLTerminatedinUCF03(pUCF03 ucf03,pCHAR mode)
{
	checknull(ucf03->foreign_key.name, sizeof(ucf03->foreign_key.name),"UCF03","foreign_key.name","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->password_1, sizeof(ucf03->password_1),"UCF03","password_1","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->password_2, sizeof(ucf03->password_2),"UCF03","password_2","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->password_3, sizeof(ucf03->password_3),"UCF03","password_3","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->password_4, sizeof(ucf03->password_4),"UCF03","password_4","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->accountstatus, sizeof(ucf03->accountstatus),"UCF03","accountstatus","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->password_modified_date, sizeof(ucf03->password_modified_date),"UCF03","password_modified_date","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->user_info_1, sizeof(ucf03->user_info_1),"UCF03","user_info_1","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->user_info_2, sizeof(ucf03->user_info_2),"UCF03","user_info_2","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->user_info_3, sizeof(ucf03->user_info_3),"UCF03","user_info_3","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->user_info_4, sizeof(ucf03->user_info_4),"UCF03","user_info_4","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->ekey, sizeof(ucf03->ekey),"UCF03","ekey","CheckNULLTerminatedinUCF03",mode);
	checknull(ucf03->dekey, sizeof(ucf03->dekey),"UCF03","dekey","CheckNULLTerminatedinUCF03",mode);
}

void CheckNULLTerminatedinUCF02(pUCF02   ucf02,pCHAR mode)
{
		checknull(ucf02->primary_key.group_id, sizeof(ucf02->primary_key.group_id),"UCF02","primary_key.group_id","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->date_added, sizeof(ucf02->date_added),"UCF02","date_added","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[0].subsystem_name          ,sizeof(ucf02->control[0].subsystem_name),"UCF02","control[0].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[0].action          ,sizeof(ucf02->control[0].action),"UCF02","control[0].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[1].subsystem_name          ,sizeof(ucf02->control[1].subsystem_name),"UCF02","control[1].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[1].action          ,sizeof(ucf02->control[1].action),"UCF02","control[1].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[2].subsystem_name          ,sizeof(ucf02->control[2].subsystem_name),"UCF02","control[2].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[2].action          ,sizeof(ucf02->control[2].action),"UCF02","control[2].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[3].subsystem_name          ,sizeof(ucf02->control[3].subsystem_name),"UCF02","control[3].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[3].action          ,sizeof(ucf02->control[3].action),"UCF02","control[3].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[4].subsystem_name          ,sizeof(ucf02->control[4].subsystem_name),"UCF02","control[4].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[4].action          ,sizeof(ucf02->control[4].action),"UCF02","control[4].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[5].subsystem_name          ,sizeof(ucf02->control[5].subsystem_name),"UCF02","control[5].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[5].action          ,sizeof(ucf02->control[5].action),"UCF02","control[5].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[6].subsystem_name          ,sizeof(ucf02->control[6].subsystem_name),"UCF02","control[6].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[6].action          ,sizeof(ucf02->control[6].action),"UCF02","control[6].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[7].subsystem_name          ,sizeof(ucf02->control[7].subsystem_name),"UCF02","control[7].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[7].action          ,sizeof(ucf02->control[7].action),"UCF02","control[7].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[8].subsystem_name          ,sizeof(ucf02->control[8].subsystem_name),"UCF02","control[8].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[8].action          ,sizeof(ucf02->control[8].action),"UCF02","control[8].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[9].subsystem_name          ,sizeof(ucf02->control[9].subsystem_name),"UCF02","control[9].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[9].action          ,sizeof(ucf02->control[9].action),"UCF02","control[9].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[10].subsystem_name          ,sizeof(ucf02->control[10].subsystem_name),"UCF02","control[10].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[10].action          ,sizeof(ucf02->control[10].action),"UCF02","control[10].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[11].subsystem_name          ,sizeof(ucf02->control[11].subsystem_name),"UCF02","control[11].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[11].action          ,sizeof(ucf02->control[11].action),"UCF02","control[11].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[12].subsystem_name          ,sizeof(ucf02->control[12].subsystem_name),"UCF02","control[12].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[12].action          ,sizeof(ucf02->control[12].action),"UCF02","control[12].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[13].subsystem_name          ,sizeof(ucf02->control[13].subsystem_name),"UCF02","control[13].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[13].action          ,sizeof(ucf02->control[13].action),"UCF02","control[13].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[14].subsystem_name          ,sizeof(ucf02->control[14].subsystem_name),"UCF02","control[14].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[14].action          ,sizeof(ucf02->control[14].action),"UCF02","control[14].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[15].subsystem_name          ,sizeof(ucf02->control[15].subsystem_name),"UCF02","control[15].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[15].action          ,sizeof(ucf02->control[15].action),"UCF02","control[15].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[16].subsystem_name          ,sizeof(ucf02->control[16].subsystem_name),"UCF02","control[16].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[16].action          ,sizeof(ucf02->control[16].action),"UCF02","control[16].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[17].subsystem_name          ,sizeof(ucf02->control[17].subsystem_name),"UCF02","control[17].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[17].action          ,sizeof(ucf02->control[17].action),"UCF02","control[17].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[18].subsystem_name          ,sizeof(ucf02->control[18].subsystem_name),"UCF02","ucf02->control[18].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[18].action          ,sizeof(ucf02->control[18].action),"UCF02","control[18].action","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[19].subsystem_name          ,sizeof(ucf02->control[19].subsystem_name),"UCF02","control[19].subsystem_name","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->control[19].action          ,sizeof(ucf02->control[19].action),"UCF02","control[19].action","CheckNULLTerminatedinUCF02",mode);

		checknull(ucf02->login[0].terminals          ,sizeof(ucf02->login[0].terminals),"UCF02","login[0].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[1].terminals          ,sizeof(ucf02->login[1].terminals),"UCF02","login[1].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[2].terminals          ,sizeof(ucf02->login[2].terminals),"UCF02","login[2].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[3].terminals          ,sizeof(ucf02->login[3].terminals),"UCF02","login[3].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[4].terminals          ,sizeof(ucf02->login[4].terminals),"UCF02","login[4].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[5].terminals          ,sizeof(ucf02->login[5].terminals),"UCF02","login[5].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[6].terminals          ,sizeof(ucf02->login[6].terminals),"UCF02","login[6].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[7].terminals          ,sizeof(ucf02->login[7].terminals),"UCF02","login[7].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[8].terminals          ,sizeof(ucf02->login[8].terminals),"UCF02","login[8].terminals","CheckNULLTerminatedinUCF02",mode);
		checknull(ucf02->login[9].terminals          ,sizeof(ucf02->login[9].terminals),"UCF02","login[9].terminals","CheckNULLTerminatedinUCF02",mode);

		checknull(ucf02->buffer          ,sizeof(ucf02->buffer),"UCF02","buffer","CheckNULLTerminatedinUCF02",mode);
}


void CheckNULLTerminatedinBIN01(pBIN01   bin01,pCHAR mode)
{
	checknull(bin01->primary_key.length     ,sizeof(bin01->primary_key.length),"BIN01","primary_key.length","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->primary_key.low     ,sizeof(bin01->primary_key.low),"BIN01","primary_key.low","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->primary_key.type     ,sizeof(bin01->primary_key.type),"bin01","primary_key.type","CheckNULLTerminatedinBIN01",mode);

	checknull(bin01->high     ,sizeof(bin01->high),"BIN01","high","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->pan_length     ,sizeof(bin01->pan_length),"BIN01","pan_length","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->auth_ind     ,sizeof(bin01->auth_ind),"BIN01","auth_ind","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->caf_profile_id     ,sizeof(bin01->caf_profile_id),"BIN01","caf_profile_id","CheckNULLTerminatedinBIN01",mode);

	checknull(bin01->destination_name     ,sizeof(bin01->destination_name),"BIN01","destination_name","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->alt_destination_name     ,sizeof(bin01->alt_destination_name),"BIN01","alt_destination_name","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->credit_subtype     ,sizeof(bin01->credit_subtype),"BIN01","credit_subtype","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->description     ,sizeof(bin01->description),"BIN01","description","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->card_type     ,sizeof(bin01->card_type),"BIN01","card_type","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->settle_file_prefix     ,sizeof(bin01->settle_file_prefix),"BIN01","settle_file_prefix","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->card_family     ,sizeof(bin01->card_family),"BIN01","card_family","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[0].acquirer_id     ,sizeof(bin01->acquirer_blocking[0].acquirer_id),"BIN01","acquirer_blocking[0].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[1].acquirer_id     ,sizeof(bin01->acquirer_blocking[1].acquirer_id),"BIN01","acquirer_blocking[1].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[2].acquirer_id     ,sizeof(bin01->acquirer_blocking[2].acquirer_id),"BIN01","acquirer_blocking[2].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[3].acquirer_id     ,sizeof(bin01->acquirer_blocking[3].acquirer_id),"BIN01","acquirer_blocking[3].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[4].acquirer_id     ,sizeof(bin01->acquirer_blocking[4].acquirer_id),"BIN01","acquirer_blocking[4].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[5].acquirer_id     ,sizeof(bin01->acquirer_blocking[5].acquirer_id),"BIN01","acquirer_blocking[5].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[6].acquirer_id     ,sizeof(bin01->acquirer_blocking[6].acquirer_id),"BIN01","acquirer_blocking[6].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[7].acquirer_id     ,sizeof(bin01->acquirer_blocking[7].acquirer_id),"BIN01","acquirer_blocking[7].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[8].acquirer_id     ,sizeof(bin01->acquirer_blocking[8].acquirer_id),"BIN01","acquirer_blocking[8].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[9].acquirer_id     ,sizeof(bin01->acquirer_blocking[9].acquirer_id),"BIN01","acquirer_blocking[9].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[10].acquirer_id     ,sizeof(bin01->acquirer_blocking[10].acquirer_id),"BIN01","acquirer_blocking[10].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[11].acquirer_id     ,sizeof(bin01->acquirer_blocking[11].acquirer_id),"BIN01","acquirer_blocking[11].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[12].acquirer_id     ,sizeof(bin01->acquirer_blocking[12].acquirer_id),"BIN01","acquirer_blocking[12].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[13].acquirer_id     ,sizeof(bin01->acquirer_blocking[13].acquirer_id),"BIN01","acquirer_blocking[13].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[14].acquirer_id     ,sizeof(bin01->acquirer_blocking[14].acquirer_id),"BIN01","acquirer_blocking[14].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[15].acquirer_id     ,sizeof(bin01->acquirer_blocking[15].acquirer_id),"BIN01","acquirer_blocking[15].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[16].acquirer_id     ,sizeof(bin01->acquirer_blocking[16].acquirer_id),"BIN01","acquirer_blocking[16].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[17].acquirer_id     ,sizeof(bin01->acquirer_blocking[17].acquirer_id),"BIN01","acquirer_blocking[17].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[18].acquirer_id     ,sizeof(bin01->acquirer_blocking[18].acquirer_id),"BIN01","acquirer_blocking[18].acquirer_id","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->acquirer_blocking[19].acquirer_id     ,sizeof(bin01->acquirer_blocking[19].acquirer_id),"BIN01","acquirer_blocking[19].acquirer_id","CheckNULLTerminatedinBIN01",mode);

	checknull(bin01->mcc_low[0].low_range     ,sizeof(bin01->mcc_low[0].low_range),"BIN01","mcc_low[0].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[1].low_range     ,sizeof(bin01->mcc_low[1].low_range),"BIN01","mcc_low[1].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[2].low_range     ,sizeof(bin01->mcc_low[2].low_range),"BIN01","mcc_low[2].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[3].low_range     ,sizeof(bin01->mcc_low[3].low_range),"BIN01","mcc_low[3].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[4].low_range     ,sizeof(bin01->mcc_low[4].low_range),"BIN01","mcc_low[4].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[5].low_range     ,sizeof(bin01->mcc_low[5].low_range),"BIN01","mcc_low[5].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[6].low_range     ,sizeof(bin01->mcc_low[6].low_range),"BIN01","mcc_low[6].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[7].low_range     ,sizeof(bin01->mcc_low[7].low_range),"BIN01","mcc_low[7].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[8].low_range     ,sizeof(bin01->mcc_low[8].low_range),"BIN01","mcc_low[8].low_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_low[9].low_range     ,sizeof(bin01->mcc_low[9].low_range),"BIN01","mcc_low[9].low_range","CheckNULLTerminatedinBIN01",mode);

	checknull(bin01->mcc_high[0].high_range     ,sizeof(bin01->mcc_high[0].high_range),"BIN01","mcc_high[0].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[1].high_range     ,sizeof(bin01->mcc_high[1].high_range),"BIN01","mcc_high[1].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[2].high_range     ,sizeof(bin01->mcc_high[2].high_range),"BIN01","mcc_high[2].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[3].high_range     ,sizeof(bin01->mcc_high[3].high_range),"BIN01","mcc_high[3].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[4].high_range     ,sizeof(bin01->mcc_high[4].high_range),"BIN01","mcc_high[4].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[5].high_range     ,sizeof(bin01->mcc_high[5].high_range),"BIN01","mcc_high[5].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[6].high_range     ,sizeof(bin01->mcc_high[6].high_range),"BIN01","mcc_high[6].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[7].high_range     ,sizeof(bin01->mcc_high[7].high_range),"BIN01","mcc_high[7].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[8].high_range     ,sizeof(bin01->mcc_high[8].high_range),"BIN01","mcc_high[8].high_range","CheckNULLTerminatedinBIN01",mode);
	checknull(bin01->mcc_high[9].high_range     ,sizeof(bin01->mcc_high[9].high_range),"BIN01","mcc_high[9].high_range","CheckNULLTerminatedinBIN01",mode);

	checknull(bin01->mod_check     ,sizeof(bin01->mod_check),"BIN01","mod_check","CheckNULLTerminatedinBIN01",mode);

}

void CheckNULLTerminatedinHST01(pHST01   hst01,pCHAR mode)
{
	checknull(hst01->primary_key.unique_id , sizeof(hst01->primary_key.unique_id),"HST01","primary_key.unique_id","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->file_name , sizeof(hst01->file_name),"HST01","file_name","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->file_primary_key , sizeof(hst01->file_primary_key),"HST01","file_primary_key","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->changed_by , sizeof(hst01->changed_by),"HST01","changed_by","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->date_modified , sizeof(hst01->date_modified),"HST01","date_modified","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->time_modified , sizeof(hst01->time_modified),"HST01","time_modified","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->field_tag , sizeof(hst01->field_tag),"HST01","field_tag","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->previous_data , sizeof(hst01->previous_data),"HST01","previous_data","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->current_data , sizeof(hst01->current_data),"HST01","current_data","CheckNULLTerminatedinHST01",mode);
	checknull(hst01->reserved , sizeof(hst01->reserved),"HST01","reserved","CheckNULLTerminatedinHST01",mode);
}

void CheckNULLTerminatedinATP01(pATP01   atp01,pCHAR mode)
{
	checknull(atp01->site_name , sizeof(atp01->site_name),"ATP01","site_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->tlf_log_date , sizeof(atp01->tlf_log_date),"ATP01","tlf_log_date","CheckNULLTerminatedinATP01",mode);

	checknull(atp01->system_components[0].subsystem_name, sizeof(atp01->system_components[0].subsystem_name),"ATP01","system_components[0].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[1].subsystem_name, sizeof(atp01->system_components[1].subsystem_name),"ATP01","system_components[1].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[2].subsystem_name, sizeof(atp01->system_components[2].subsystem_name),"ATP01","system_components[2].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[3].subsystem_name, sizeof(atp01->system_components[3].subsystem_name),"ATP01","system_components[3].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[4].subsystem_name, sizeof(atp01->system_components[4].subsystem_name),"ATP01","system_components[4].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[5].subsystem_name, sizeof(atp01->system_components[5].subsystem_name),"ATP01","system_components[5].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[6].subsystem_name, sizeof(atp01->system_components[6].subsystem_name),"ATP01","system_components[6].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[7].subsystem_name, sizeof(atp01->system_components[7].subsystem_name),"ATP01","system_components[7].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[8].subsystem_name, sizeof(atp01->system_components[8].subsystem_name),"ATP01","system_components[8].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[9].subsystem_name, sizeof(atp01->system_components[9].subsystem_name),"ATP01","system_components[9].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[10].subsystem_name, sizeof(atp01->system_components[10].subsystem_name),"ATP01","system_components[10].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[11].subsystem_name, sizeof(atp01->system_components[11].subsystem_name),"ATP01","system_components[11].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[12].subsystem_name, sizeof(atp01->system_components[12].subsystem_name),"ATP01","system_components[12].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[13].subsystem_name, sizeof(atp01->system_components[13].subsystem_name),"ATP01","system_components[13].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[14].subsystem_name, sizeof(atp01->system_components[14].subsystem_name),"ATP01","system_components[14].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[15].subsystem_name, sizeof(atp01->system_components[15].subsystem_name),"ATP01","system_components[15].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[16].subsystem_name, sizeof(atp01->system_components[16].subsystem_name),"ATP01","system_components[16].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[17].subsystem_name, sizeof(atp01->system_components[17].subsystem_name),"ATP01","system_components[17].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[18].subsystem_name, sizeof(atp01->system_components[18].subsystem_name),"ATP01","system_components[18].subsystem_name","CheckNULLTerminatedinATP01",mode);
	checknull(atp01->system_components[19].subsystem_name, sizeof(atp01->system_components[19].subsystem_name),"ATP01","system_components[19].subsystem_name","CheckNULLTerminatedinATP01",mode);
}

void CheckNULLTerminatedinATP02(pATP02   atp02,pCHAR mode)
{
	checknull(atp02->primary_key.workstation_id , sizeof(atp02->primary_key.workstation_id),"ATP02","primary_key.workstation_id","CheckNULLTerminatedinATP02",mode);
	checknull(atp02->workstation_status , sizeof(atp02->workstation_status),"ATP02","workstation_status","CheckNULLTerminatedinATP02",mode);
}

void CheckNULLTerminatedinCCF02P(pCCF02P  ccf02p,pCHAR mode)
{
	checknull(ccf02p->primary_key.card_type , sizeof(ccf02p->primary_key.card_type),"CCF02P","primary_key.card_type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->primary_key.card_nbr , sizeof(ccf02p->primary_key.card_nbr),"CCF02P","primary_key.card_nbr","CheckNULLTerminatedinCCF02P",mode);

	checknull(ccf02p->primary_acct_nbr , sizeof(ccf02p->primary_acct_nbr),"CCF02P","primary_acct_nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->caf_profile_id , sizeof(ccf02p->caf_profile_id),"CCF02P","caf_profile_id","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->status , sizeof(ccf02p->status),"CCF02P","status","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->pin_offset , sizeof(ccf02p->pin_offset),"CCF02P","pin_offset","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->date_added , sizeof(ccf02p->date_added),"CCF02P","date_added","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->maint_date , sizeof(ccf02p->maint_date),"CCF02P","maint_date","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->changed_by , sizeof(ccf02p->changed_by),"CCF02P","changed_by","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->card_class , sizeof(ccf02p->card_class),"CCF02P","card_class","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->expire_date , sizeof(ccf02p->expire_date),"CCF02P","expire_date","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->effect_date , sizeof(ccf02p->effect_date),"CCF02P","effect_date","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->percent_deposit , sizeof(ccf02p->percent_deposit),"CCF02P","percent_deposit","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->percent_avail_allowed , sizeof(ccf02p->percent_avail_allowed),"CCF02P","percent_avail_allowed","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->credit_line , sizeof(ccf02p->credit_line),"CCF02P","outstanding_auths","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->outstanding_auths , sizeof(ccf02p->outstanding_auths),"CCF02P","","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->allowed , sizeof(ccf02p->allowed),"CCF02P","allowed","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->currency_code , sizeof(ccf02p->currency_code),"CCF02P","currency_code","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->country_code , sizeof(ccf02p->country_code),"CCF02P","country_code","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->peso_saving_acct , sizeof(ccf02p->peso_saving_acct),"CCF02P","peso_saving_acct","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->peso_current_acct , sizeof(ccf02p->peso_current_acct),"CCF02P","peso_current_acct","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->dollar_saving_acct , sizeof(ccf02p->dollar_saving_acct),"CCF02P","dollar_saving_acct","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->dollar_current_acct , sizeof(ccf02p->dollar_current_acct),"CCF02P","dollar_current_acct","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->product_codes , sizeof(ccf02p->product_codes),"CCF02P","product_codes","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->cash_bonus_eligible , sizeof(ccf02p->cash_bonus_eligible),"CCF02P","cash_bonus_eligible","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->visa_purchase_card , sizeof(ccf02p->visa_purchase_card),"CCF02P","visa_purchase_card","CheckNULLTerminatedinCCF02P",mode);

	checknull(ccf02p->related_acct[0].nbr , sizeof(ccf02p->related_acct[0].nbr),"CCF02P","related_acct[0].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[0].type , sizeof(ccf02p->related_acct[0].type),"CCF02P","related_acct[0].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[1].nbr , sizeof(ccf02p->related_acct[1].nbr),"CCF02P","related_acct[1].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[1].type , sizeof(ccf02p->related_acct[1].type),"CCF02P","related_acct[1].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[2].nbr , sizeof(ccf02p->related_acct[2].nbr),"CCF02P","related_acct[2].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[2].type , sizeof(ccf02p->related_acct[2].type),"CCF02P","related_acct[2].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[3].nbr , sizeof(ccf02p->related_acct[3].nbr),"CCF02P","related_acct[3].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[3].type , sizeof(ccf02p->related_acct[3].type),"CCF02P","related_acct[3].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[4].nbr , sizeof(ccf02p->related_acct[4].nbr),"CCF02P","related_acct[4].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[4].type , sizeof(ccf02p->related_acct[4].type),"CCF02P","related_acct[4].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[5].nbr , sizeof(ccf02p->related_acct[5].nbr),"CCF02P","related_acct[5].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[5].type , sizeof(ccf02p->related_acct[5].type),"CCF02P","related_acct[5].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[6].nbr , sizeof(ccf02p->related_acct[6].nbr),"CCF02P","related_acct[6].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[6].type , sizeof(ccf02p->related_acct[6].type),"CCF02P","related_acct[6].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[7].nbr , sizeof(ccf02p->related_acct[7].nbr),"CCF02P","related_acct[7].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[7].type , sizeof(ccf02p->related_acct[7].type),"CCF02P","related_acct[7].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[8].nbr , sizeof(ccf02p->related_acct[8].nbr),"CCF02P","related_acct[8].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[8].type , sizeof(ccf02p->related_acct[8].type),"CCF02P","related_acct[8].type","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[9].nbr , sizeof(ccf02p->related_acct[9].nbr),"CCF02P","related_acct[9].nbr","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->related_acct[9].type , sizeof(ccf02p->related_acct[9].type),"CCF02P","related_acct[9].type","CheckNULLTerminatedinCCF02P",mode);

	checknull(ccf02p->pin_verification_override , sizeof(ccf02p->pin_verification_override),"CCF02P","pin_verification_override","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->pin , sizeof(ccf02p->pin),"CCF02P","pin","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->pvki , sizeof(ccf02p->pvki),"CCF02P","pvki","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->add_on_rate , sizeof(ccf02p->add_on_rate),"CCF02P","add_on_rate","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->add_on_term , sizeof(ccf02p->add_on_term),"CCF02P","add_on_term","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->peso_tran_limit , sizeof(ccf02p->peso_tran_limit),"CCF02P","peso_tran_limit","CheckNULLTerminatedinCCF02P",mode);
	checknull(ccf02p->dollar_tran_limit , sizeof(ccf02p->dollar_tran_limit),"CCF02P","dollar_tran_limit","CheckNULLTerminatedinCCF02P",mode);


}

void CheckNULLTerminatedinCCF04(pCCF04   ccf04,pCHAR mode)
{
	checknull(ccf04->primary_key.card_type , sizeof(ccf04->primary_key.card_type),"CCF04","primary_key.card_type","CheckNULLTerminatedinCCF04",mode);
	checknull(ccf04->primary_key.card_nbr , sizeof(ccf04->primary_key.card_nbr),"CCF04","primary_key.card_nbr","CheckNULLTerminatedinCCF04",mode);

	checknull(ccf04->mcc_lo_range , sizeof(ccf04->mcc_lo_range),"CCF04","mcc_lo_range","CheckNULLTerminatedinCCF04",mode);
	checknull(ccf04->mcc_hi_range , sizeof(ccf04->mcc_hi_range),"CCF04","mcc_hi_range","CheckNULLTerminatedinCCF04",mode);
}

void CheckNULLTerminatedinCCF10(pCCF10   ccf10,pCHAR mode)
{
	checknull(ccf10->primary_key.retrieval_ref_num , sizeof(ccf10->primary_key.retrieval_ref_num),"CCF10","primary_key.retrieval_ref_num","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->primary_key.card_nbr , sizeof(ccf10->primary_key.card_nbr),"CCF10","primary_key.card_nbr","CheckNULLTerminatedinCCF10",mode);

	checknull(ccf10->processing_code , sizeof(ccf10->processing_code),"CCF10","processing_code","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->primary_acct_nbr , sizeof(ccf10->primary_acct_nbr),"CCF10","primary_acct_nbr","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->tran_amount , sizeof(ccf10->tran_amount),"CCF10","tran_amount","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->response_code , sizeof(ccf10->response_code),"CCF10","response_code","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->auth_number , sizeof(ccf10->auth_number),"CCF10","auth_number","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->tran_date , sizeof(ccf10->tran_date),"CCF10","tran_date","CheckNULLTerminatedinCCF10",mode);
	checknull(ccf10->tran_time , sizeof(ccf10->tran_time),"CCF10","tran_time","CheckNULLTerminatedinCCF10",mode);
}

void CheckNULLTerminatedinBON01(pBON01   bon01,pCHAR mode)
{
	checknull(bon01->minimum_amount , sizeof(bon01->minimum_amount),"BON01","minimum_amount","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->program_start_date , sizeof(bon01->program_start_date),"BON01","program_start_date","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->program_end_date , sizeof(bon01->program_end_date),"BON01","program_end_date","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->transaction_threshold , sizeof(bon01->transaction_threshold),"BON01","transaction_threshold","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->transaction_counter , sizeof(bon01->transaction_counter),"BON01","transaction_counter","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->maximum_reward , sizeof(bon01->maximum_reward),"BON01","maximum_reward","CheckNULLTerminatedinBON01",mode);
	checknull(bon01->response_message , sizeof(bon01->response_message),"BON01","response_message","CheckNULLTerminatedinBON01",mode);
}

void CheckNULLTerminatedinBON02(pBON02   bon02,pCHAR mode)
{
	checknull(bon02->cash_id , sizeof(bon02->cash_id),"BON02","cash_id","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->tag_id , sizeof(bon02->tag_id),"BON02","tag_id","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->cash_desc , sizeof(bon02->cash_desc),"BON02","cash_desc","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->program_start_date , sizeof(bon02->program_start_date),"BON02","program_start_date","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->program_end_date , sizeof(bon02->program_end_date),"BON02","program_end_date","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->tran_threshold , sizeof(bon02->tran_threshold),"BON02","tran_threshold","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->tran_ctr , sizeof(bon02->tran_ctr),"BON02","tran_ctr","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->time_period , sizeof(bon02->time_period),"BON02","time_period","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->max_period_winners , sizeof(bon02->max_period_winners),"BON02","max_period_winners","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->period_win_ctr , sizeof(bon02->period_win_ctr),"BON02","period_win_ctr","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->program_win_ctr , sizeof(bon02->program_win_ctr),"BON02","program_win_ctr","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->amount_range[0].win_type , sizeof(bon02->amount_range[0].win_type),"BON02","amount_range[0].win_type","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[0].reward_amount , sizeof(bon02->amount_range[0].reward_amount),"BON02","amount_range[0].reward_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[0].resp_text , sizeof(bon02->amount_range[0].resp_text),"BON02","amount_range[0].resp_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[0].high_amount , sizeof(bon02->amount_range[0].high_amount),"BON02","amount_range[0].high_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[0].low_amount , sizeof(bon02->amount_range[0].low_amount),"BON02","amount_range[0].low_amount","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->amount_range[1].win_type , sizeof(bon02->amount_range[1].win_type),"BON02","amount_range[1].win_type","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[1].reward_amount , sizeof(bon02->amount_range[1].reward_amount),"BON02","amount_range[1].reward_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[1].resp_text , sizeof(bon02->amount_range[1].resp_text),"BON02","amount_range[1].resp_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[1].high_amount , sizeof(bon02->amount_range[1].high_amount),"BON02","amount_range[1].high_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[1].low_amount , sizeof(bon02->amount_range[1].low_amount),"BON02","amount_range[1].low_amount","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->amount_range[2].win_type , sizeof(bon02->amount_range[2].win_type),"BON02","amount_range[2].win_type","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[2].reward_amount , sizeof(bon02->amount_range[2].reward_amount),"BON02","amount_range[2].reward_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[2].resp_text , sizeof(bon02->amount_range[2].resp_text),"BON02","amount_range[2].resp_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[2].high_amount , sizeof(bon02->amount_range[2].high_amount),"BON02","amount_range[0].high_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[2].low_amount , sizeof(bon02->amount_range[2].low_amount),"BON02","amount_range[2].low_amount","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->amount_range[3].win_type , sizeof(bon02->amount_range[3].win_type),"BON02","amount_range[3].win_type","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[3].reward_amount , sizeof(bon02->amount_range[3].reward_amount),"BON02","amount_range[3].reward_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[3].resp_text , sizeof(bon02->amount_range[3].resp_text),"BON02","amount_range[3].resp_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[3].high_amount , sizeof(bon02->amount_range[3].high_amount),"BON02","amount_range[3].high_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[3].low_amount , sizeof(bon02->amount_range[3].low_amount),"BON02","amount_range[3].low_amount","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->amount_range[4].win_type , sizeof(bon02->amount_range[4].win_type),"BON02","amount_range[4].win_type","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[4].reward_amount , sizeof(bon02->amount_range[4].reward_amount),"BON02","amount_range[4].reward_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[4].resp_text , sizeof(bon02->amount_range[4].resp_text),"BON02","amount_range[4].resp_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[4].high_amount , sizeof(bon02->amount_range[4].high_amount),"BON02","amount_range[4].high_amount","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->amount_range[4].low_amount , sizeof(bon02->amount_range[4].low_amount),"BON02","amount_range[4].low_amount","CheckNULLTerminatedinBON02",mode);

	checknull(bon02->multi_winner_text , sizeof(bon02->multi_winner_text),"BON02","multi_winner_text","CheckNULLTerminatedinBON02",mode);
	checknull(bon02->multi_winners_allowed , sizeof(bon02->multi_winners_allowed),"BON02","multi_winners_allowed","CheckNULLTerminatedinBON02",mode);
}

void CheckNULLTerminatedinCCF01(pCCF01   ccf01,pCHAR mode)
{
	checknull(ccf01->primary_key.card_type , sizeof(ccf01->primary_key.card_type),"CCF01","primary_key.card_type","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->primary_key.card_nbr , sizeof(ccf01->primary_key.card_nbr),"CCF01","primary_key.card_nbr","CheckNULLTerminatedinCCF01",mode);

	checknull(ccf01->primary_acct_nbr , sizeof(ccf01->primary_acct_nbr),"CCF01","primary_acct_nbr","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->name_dba , sizeof(ccf01->name_dba),"CCF01","name_dba","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->address1 , sizeof(ccf01->address1),"CCF01","address1","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->address2 , sizeof(ccf01->address2),"CCF01","address2","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->city , sizeof(ccf01->city),"CCF01","city","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->state , sizeof(ccf01->state),"CCF01","state","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->zip_code , sizeof(ccf01->zip_code),"CCF01","zip_code","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->home_phone , sizeof(ccf01->home_phone),"CCF01","home_phone","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->work_phone , sizeof(ccf01->work_phone),"CCF01","work_phone","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->dob , sizeof(ccf01->dob),"CCF01","dob","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->profession , sizeof(ccf01->profession),"CCF01","profession","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->country , sizeof(ccf01->country),"CCF01","country","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->nbr_cards , sizeof(ccf01->nbr_cards),"CCF01","nbr_cards","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->status , sizeof(ccf01->status),"CCF01","status","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->sex , sizeof(ccf01->sex),"CCF01","sex","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->mother_maiden_name , sizeof(ccf01->mother_maiden_name),"CCF01","mother_maiden_name","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->membership_id , sizeof(ccf01->membership_id),"CCF01","membership_id","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->cardholder_id , sizeof(ccf01->cardholder_id),"CCF01","cardholder_id","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->block_status , sizeof(ccf01->block_status),"CCF01","block_status","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->specialty_code , sizeof(ccf01->specialty_code),"CCF01","specialty_code","CheckNULLTerminatedinCCF01",mode);
	checknull(ccf01->vip_acct_code , sizeof(ccf01->vip_acct_code),"CCF01","vip_acct_code","CheckNULLTerminatedinCCF01",mode);

}

void CheckNULLTerminatedinCAF01(pCAF01   caf01,pCHAR mode)
{
	checknull(caf01->primary_key.type , sizeof(caf01->primary_key.type),"CAF01","primary_key.type","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->primary_key.caf_profile_id , sizeof(caf01->primary_key.caf_profile_id),"CAF01","primary_key.caf_profile_id","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->authorization_location , sizeof(caf01->authorization_location),"CAF01","authorization_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->authorization_method , sizeof(caf01->authorization_method),"CAF01","authorization_method","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->destination_name , sizeof(caf01->destination_name),"CAF01","destination_name","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->iso_nbr , sizeof(caf01->iso_nbr),"CAF01","iso_nbr","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->iso_length , sizeof(caf01->iso_length),"CAF01","iso_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->aba_nbr , sizeof(caf01->aba_nbr),"CAF01","aba_nbr","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->name , sizeof(caf01->name),"CAF01","name","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->contact , sizeof(caf01->contact),"CAF01","contact","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->phone , sizeof(caf01->phone),"CAF01","phone","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->date_added , sizeof(caf01->date_added),"CAF01","date_added","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->last_maint , sizeof(caf01->last_maint),"CAF01","last_maint","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->changed_by , sizeof(caf01->changed_by),"CAF01","changed_by","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->status , sizeof(caf01->status),"CAF01","status","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->max_pin_retries , sizeof(caf01->max_pin_retries),"CAF01","max_pin_retries","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin_capture_ind , sizeof(caf01->pin_capture_ind),"CAF01","pin_capture_ind","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->track_offset , sizeof(caf01->track_offset),"CAF01","track_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin_offset_location , sizeof(caf01->pin_offset_location),"CAF01","pin_offset_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->nsp_process , sizeof(caf01->nsp_process),"CAF01","nsp_process","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->expiration_date_check , sizeof(caf01->expiration_date_check),"CAF01","expiration_date_check","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->expiration_date_location , sizeof(caf01->expiration_date_location),"CAF01","expiration_date_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->merchant_same , sizeof(caf01->merchant_same),"CAF01","merchant_same","CheckNULLTerminatedinCAF01",mode);

	checknull((char *)caf01->velocity[0].nbr_usage , sizeof(caf01->velocity[0].nbr_usage),"CAF01","velocity[0].nbr_usage","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->velocity[0].day_period , sizeof(caf01->velocity[0].day_period),"CAF01","velocity[0].day_period","CheckNULLTerminatedinCAF01",mode);
	checknull((char *)caf01->velocity[0].amt_usage , sizeof(caf01->velocity[0].amt_usage),"CAF01","velocity[0].amt_usage","CheckNULLTerminatedinCAF01",mode);

	checknull((char *)caf01->velocity[1].nbr_usage , sizeof(caf01->velocity[1].nbr_usage),"CAF01","velocity[1].nbr_usage","CheckNULLTerminatedinCAF01",mode);
	checknull((char *)caf01->velocity[1].day_period , sizeof(caf01->velocity[1].day_period),"CAF01","velocity[1].day_period","CheckNULLTerminatedinCAF01",mode);
	checknull((char *)caf01->velocity[1].amt_usage , sizeof(caf01->velocity[1].amt_usage),"CAF01","velocity[1].amt_usage","CheckNULLTerminatedinCAF01",mode);

	checknull((char *)caf01->velocity[2].nbr_usage , sizeof(caf01->velocity[2].nbr_usage),"CAF01","velocity[2].nbr_usage","CheckNULLTerminatedinCAF01",mode);
	checknull((char *)caf01->velocity[2].day_period , sizeof(caf01->velocity[2].day_period),"CAF01","velocity[2].day_period","CheckNULLTerminatedinCAF01",mode);
	checknull((char *)caf01->velocity[2].amt_usage , sizeof(caf01->velocity[2].amt_usage),"CAF01","velocity[2].amt_usage","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin_verification_method , sizeof(caf01->pin_verification_method),"CAF01","pin_verification_method","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->cvv_processing , sizeof(caf01->cvv_processing),"CAF01","cvv_processing","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->service_code_offset_tr1 , sizeof(caf01->service_code_offset_tr1),"CAF01","service_code_offset_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->service_code_offset_tr2 , sizeof(caf01->service_code_offset_tr2),"CAF01","service_code_offset_tr2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->cvv_offset_tr1 , sizeof(caf01->cvv_offset_tr1),"CAF01","cvv_offset_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->cvv_offset_tr2 , sizeof(caf01->cvv_offset_tr2),"CAF01","cvv_offset_tr2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->cvv_length , sizeof(caf01->cvv_length),"CAF01","cvv_length","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->csck , sizeof(caf01->csck),"CAF01","csck","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->floor_limit , sizeof(caf01->floor_limit),"CAF01","floor_limit","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->country_limits_flag , sizeof(caf01->country_limits_flag),"CAF01","country_limits_flag","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Identikey.bankid , sizeof(caf01->pin.Identikey.bankid),"CAF01","pin.Identikey.bankid","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.pvn_location , sizeof(caf01->pin.Identikey.pvn_location),"CAF01","pin.Identikey.pvn_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.pvn_offset , sizeof(caf01->pin.Identikey.pvn_offset),"CAF01","pin.Identikey.pvn_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.pvn_length , sizeof(caf01->pin.Identikey.pvn_length),"CAF01","pin.Identikey.pvn_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.comparison_ind , sizeof(caf01->pin.Identikey.comparison_ind),"CAF01","pin.Identikey.comparison_ind","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.partial_pan_offset , sizeof(caf01->pin.Identikey.partial_pan_offset),"CAF01","pin.Identikey.partial_pan_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Identikey.partial_pan_length , sizeof(caf01->pin.Identikey.partial_pan_length),"CAF01","pin.Identikey.partial_pan_length","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.EIdentikey.bankid , sizeof(caf01->pin.EIdentikey.bankid),"CAF01","pin.EIdentikey.bankid","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.EIdentikey.pvn_location , sizeof(caf01->pin.EIdentikey.pvn_location),"CAF01","pin.EIdentikey.pvn_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.EIdentikey.pvn_offset , sizeof(caf01->pin.EIdentikey.pvn_offset),"CAF01","pin.EIdentikey.pvn_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.EIdentikey.pvn_length , sizeof(caf01->pin.EIdentikey.pvn_length),"CAF01","pin.EIdentikey.pvn_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.EIdentikey.partial_pan_offset , sizeof(caf01->pin.EIdentikey.partial_pan_offset),"CAF01","pin.EIdentikey.partial_pan_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.EIdentikey.partial_pan_length , sizeof(caf01->pin.EIdentikey.partial_pan_length),"CAF01","pin.EIdentikey.partial_pan_length","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Ibm3624.conversion_tbl , sizeof(caf01->pin.Ibm3624.conversion_tbl),"CAF01","pin.Ibm3624.conversion_tbl","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ibm3624.offset_location , sizeof(caf01->pin.Ibm3624.offset_location),"CAF01","pin.Ibm3624.offset_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ibm3624.offset_offset , sizeof(caf01->pin.Ibm3624.offset_offset),"CAF01","pin.Ibm3624.offset_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ibm3624.offset_length , sizeof(caf01->pin.Ibm3624.offset_length),"CAF01","pin.Ibm3624.offset_length","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.IbmOffset.pin_key , sizeof(caf01->pin.IbmOffset.pin_key),"CAF01","pin.IbmOffset.pin_key","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.IbmOffset.decimalization_tbl , sizeof(caf01->pin.IbmOffset.decimalization_tbl),"CAF01","pin.IbmOffset.decimalization_tbl","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.IbmOffset.pin_offset_location , sizeof(caf01->pin.IbmOffset.pin_offset_location),"CAF01","pin.IbmOffset.pin_offset_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.IbmOffset.offset_length , sizeof(caf01->pin.IbmOffset.offset_length),"CAF01","pin.IbmOffset.offset_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.IbmOffset.track_offset , sizeof(caf01->pin.IbmOffset.track_offset),"CAF01","pin.IbmOffset.track_offset","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Visa.key_left, sizeof(caf01->pin.Visa.key_left),"CAF01","pin.Visa.key_left","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.key_right, sizeof(caf01->pin.Visa.key_right),"CAF01","pin.Visa.key_right","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvv_location_tr1, sizeof(caf01->pin.Visa.pvv_location_tr1),"CAF01","pin.Visa.pvv_location_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvv_location_tr2, sizeof(caf01->pin.Visa.pvv_location_tr2),"CAF01","pin.Visa.pvv_location_tr2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvv_offset_tr1, sizeof(caf01->pin.Visa.pvv_offset_tr1),"CAF01","pin.Visa.pvv_offset_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvv_offset_tr2, sizeof(caf01->pin.Visa.pvv_offset_tr2),"CAF01","pin.Visa.pvv_offset_tr2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvkey_ind, sizeof(caf01->pin.Visa.pvkey_ind),"CAF01","pin.Visa.pvkey_ind","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvki_location_tr1, sizeof(caf01->pin.Visa.pvki_location_tr1),"CAF01","pin.Visa.pvki_location_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvki_location_tr2, sizeof(caf01->pin.Visa.pvki_location_tr2),"CAF01","pin.Visa.pvki_location_tr2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvki_length_tr1, sizeof(caf01->pin.Visa.pvki_length_tr1),"CAF01","pin.Visa.pvki_length_tr1","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Visa.pvki_length_tr2, sizeof(caf01->pin.Visa.pvki_length_tr2),"CAF01","pin.Visa.pvki_length_tr2","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.DESBiLevel.bankid, sizeof(caf01->pin.DESBiLevel.bankid),"CAF01","pin.DESBiLevel.bankid","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.partial_pan_offset, sizeof(caf01->pin.DESBiLevel.partial_pan_offset),"CAF01","pin.DESBiLevel.partial_pan_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.partial_pan_length, sizeof(caf01->pin.DESBiLevel.partial_pan_length),"CAF01","pin.DESBiLevel.partial_pan_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.pin_verification_key, sizeof(caf01->pin.DESBiLevel.pin_verification_key),"CAF01","pin.DESBiLevel.pin_verification_key","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.pvn_location, sizeof(caf01->pin.DESBiLevel.pvn_location),"CAF01","pin.DESBiLevel.pvn_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.pvn_offset, sizeof(caf01->pin.DESBiLevel.pvn_offset),"CAF01","pin.DESBiLevel.pvn_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.pvn_length, sizeof(caf01->pin.DESBiLevel.pvn_length),"CAF01","pin.DESBiLevel.pvn_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.DESBiLevel.pvn2_type, sizeof(caf01->pin.DESBiLevel.pvn2_type),"CAF01","pin.DESBiLevel.pvn2_type","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Deibold.offset_location, sizeof(caf01->pin.Deibold.offset_location),"CAF01","pin.Deibold.offset_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Deibold.offset_offset, sizeof(caf01->pin.Deibold.offset_offset),"CAF01","pin.Deibold.offset_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Deibold.algorithm, sizeof(caf01->pin.Deibold.algorithm),"CAF01","pin.Deibold.algorithm","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Deibold.key_table_location, sizeof(caf01->pin.Deibold.key_table_location),"CAF01","pin.Deibold.key_table_location","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Ncr.conversion_table, sizeof(caf01->pin.Ncr.conversion_table),"CAF01","pin.Ncr.conversion_table","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.offset_location, sizeof(caf01->pin.Ncr.offset_location),"CAF01","pin.Ncr.offset_location","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.offset_offset, sizeof(caf01->pin.Ncr.offset_offset),"CAF01","pin.Ncr.offset_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.offset_length, sizeof(caf01->pin.Ncr.offset_length),"CAF01","pin.Ncr.offset_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.partial_pan_offset, sizeof(caf01->pin.Ncr.partial_pan_offset),"CAF01","pin.Ncr.partial_pan_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.partial_pan_length, sizeof(caf01->pin.Ncr.partial_pan_length),"CAF01","pin.Ncr.partial_pan_length","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.pad_character, sizeof(caf01->pin.Ncr.pad_character),"CAF01","pin.Ncr.pad_character","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.plen, sizeof(caf01->pin.Ncr.plen),"CAF01","pin.Ncr.plen","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.pin_verification_key, sizeof(caf01->pin.Ncr.pin_verification_key),"CAF01","pin.Ncr.pin_verification_key","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.padding_flag, sizeof(caf01->pin.Ncr.padding_flag),"CAF01","pin.Ncr.padding_flag","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.counting_flag, sizeof(caf01->pin.Ncr.counting_flag),"CAF01","pin.Ncr.counting_flag","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.start_count_position, sizeof(caf01->pin.Ncr.start_count_position),"CAF01","pin.Ncr.start_count_position","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Ncr.select_plen_position, sizeof(caf01->pin.Ncr.select_plen_position),"CAF01","pin.Ncr.select_plen_position","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.Cvv.keya, sizeof(caf01->pin.Cvv.keya),"CAF01","pin.Cvv.keya","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Cvv.keyb, sizeof(caf01->pin.Cvv.keyb),"CAF01","pin.Cvv.keyb","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Cvv.service_code_offset, sizeof(caf01->pin.Cvv.service_code_offset),"CAF01","pin.Cvv.service_code_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Cvv.cvv_offset, sizeof(caf01->pin.Cvv.cvv_offset),"CAF01","pin.Cvv.cvv_offset","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.Cvv.cvv_length, sizeof(caf01->pin.Cvv.cvv_length),"CAF01","pin.Cvv.cvv_length","CheckNULLTerminatedinCAF01",mode);

	checknull(caf01->pin.PinBlock.encryption_key2, sizeof(caf01->pin.PinBlock.encryption_key2),"CAF01","pin.PinBlock.encryption_key2","CheckNULLTerminatedinCAF01",mode);
	checknull(caf01->pin.None.No_Key, sizeof(caf01->pin.None.No_Key),"CAF01","pin.None.No_Key","CheckNULLTerminatedinCAF01",mode);
}

void CheckNULLTerminatedinCTF01(pCTF01   ctf01,pCHAR mode)
{
	checknull(ctf01->primary_key.card_type, sizeof(ctf01->primary_key.card_type),"CTF01","primary_key.card_type","CheckNULLTerminatedinCAF01",mode);
	checknull(ctf01->description, sizeof(ctf01->description),"CTF01","description","CheckNULLTerminatedinCAF01",mode);
}

void CheckNULLTerminatedinMCT01(pMCT01   mct01,pCHAR mode)
{
	checknull(mct01->primary_key.card_type, sizeof(mct01->primary_key.card_type),"MCT01","primary_key.card_type","CheckNULLTerminatedinCAF01",mode);
	checknull(mct01->primary_key.merchant_id, sizeof(mct01->primary_key.merchant_id),"MCT01","primary_key.merchant_id","CheckNULLTerminatedinCAF01",mode);
	checknull(mct01->primary_key.organization_id, sizeof(mct01->primary_key.organization_id),"MCT01","primary_key.organization_id","CheckNULLTerminatedinCAF01",mode);

	checknull(mct01->acquirer_id, sizeof(mct01->acquirer_id),"MCT01","acquirer_id","CheckNULLTerminatedinCAF01",mode);
	checknull(mct01->corporate_id, sizeof(mct01->corporate_id),"MCT01","corporate_id","CheckNULLTerminatedinCAF01",mode);
	checknull(mct01->settlement_id, sizeof(mct01->settlement_id),"MCT01","settlement_id","CheckNULLTerminatedinCAF01",mode);
	checknull(mct01->floor_limit, sizeof(mct01->floor_limit),"MCT01","floor_limit","CheckNULLTerminatedinCAF01",mode);
}

void CheckNULLTerminatedinCCF03(pCCF03   ccf03p,pCHAR mode)
{
	checknull(ccf03p->primary_key.card_type, sizeof(ccf03p->primary_key.card_type),"CCF03P","primary_key.card_type","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->primary_key.card_nbr, sizeof(ccf03p->primary_key.card_nbr),"CCF03P","primary_key.card_nbr","CheckNULLTerminatedinCCF03",mode);

	checknull(ccf03p->primary_acct_nbr, sizeof(ccf03p->primary_acct_nbr),"CCF03P","primary_acct_nbr","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->daily_amt_usage, sizeof(ccf03p->daily_amt_usage),"CCF03P","daily_amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->julian_date, sizeof(ccf03p->julian_date),"CCF03P","julian_date","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_julian_date, sizeof(ccf03p->peso_julian_date),"CCF03P","peso_julian_date","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_julian_date, sizeof(ccf03p->dollar_julian_date),"CCF03P","dollar_julian_date","CheckNULLTerminatedinCCF03",mode);

	checknull(ccf03p->peso_usage[0].nbr_usage, sizeof(ccf03p->peso_usage[0].nbr_usage),"CCF03P","peso_usage[0].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[0].amt_usage, sizeof(ccf03p->peso_usage[0].amt_usage),"CCF03P","peso_usage[0].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[1].nbr_usage, sizeof(ccf03p->peso_usage[1].nbr_usage),"CCF03P","peso_usage[1].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[1].amt_usage, sizeof(ccf03p->peso_usage[1].amt_usage),"CCF03P","peso_usage[1].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[2].nbr_usage, sizeof(ccf03p->peso_usage[2].nbr_usage),"CCF03P","peso_usage[2].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[2].amt_usage, sizeof(ccf03p->peso_usage[2].amt_usage),"CCF03P","peso_usage[2].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[3].nbr_usage, sizeof(ccf03p->peso_usage[3].nbr_usage),"CCF03P","peso_usage[3].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[3].amt_usage, sizeof(ccf03p->peso_usage[3].amt_usage),"CCF03P","peso_usage[3].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[4].nbr_usage, sizeof(ccf03p->peso_usage[4].nbr_usage),"CCF03P","peso_usage[4].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[4].amt_usage, sizeof(ccf03p->peso_usage[4].amt_usage),"CCF03P","peso_usage[4].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[5].nbr_usage, sizeof(ccf03p->peso_usage[5].nbr_usage),"CCF03P","peso_usage[5].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[5].amt_usage, sizeof(ccf03p->peso_usage[5].amt_usage),"CCF03P","peso_usage[5].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[6].nbr_usage, sizeof(ccf03p->peso_usage[6].nbr_usage),"CCF03P","peso_usage[6].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[6].amt_usage, sizeof(ccf03p->peso_usage[6].amt_usage),"CCF03P","peso_usage[6].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[7].nbr_usage, sizeof(ccf03p->peso_usage[7].nbr_usage),"CCF03P","peso_usage[7].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[7].amt_usage, sizeof(ccf03p->peso_usage[7].amt_usage),"CCF03P","peso_usage[7].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[8].nbr_usage, sizeof(ccf03p->peso_usage[8].nbr_usage),"CCF03P","peso_usage[8].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[8].amt_usage, sizeof(ccf03p->peso_usage[8].amt_usage),"CCF03P","peso_usage[8].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[9].nbr_usage, sizeof(ccf03p->peso_usage[9].nbr_usage),"CCF03P","peso_usage[9].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[9].amt_usage, sizeof(ccf03p->peso_usage[9].amt_usage),"CCF03P","peso_usage[9].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[10].nbr_usage, sizeof(ccf03p->peso_usage[10].nbr_usage),"CCF03P","peso_usage[10].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[10].amt_usage, sizeof(ccf03p->peso_usage[10].amt_usage),"CCF03P","peso_usage[10].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[11].nbr_usage, sizeof(ccf03p->peso_usage[11].nbr_usage),"CCF03P","peso_usage[11].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[11].amt_usage, sizeof(ccf03p->peso_usage[11].amt_usage),"CCF03P","peso_usage[11].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[12].nbr_usage, sizeof(ccf03p->peso_usage[12].nbr_usage),"CCF03P","peso_usage[12].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[12].amt_usage, sizeof(ccf03p->peso_usage[12].amt_usage),"CCF03P","peso_usage[12].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[13].nbr_usage, sizeof(ccf03p->peso_usage[13].nbr_usage),"CCF03P","peso_usage[13].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->peso_usage[13].amt_usage, sizeof(ccf03p->peso_usage[13].amt_usage),"CCF03P","peso_usage[13].amt_usage","CheckNULLTerminatedinCCF03",mode);

	checknull(ccf03p->dollar_usage[0].nbr_usage, sizeof(ccf03p->dollar_usage[0].nbr_usage),"CCF03P","dollar_usage[0].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[0].amt_usage, sizeof(ccf03p->dollar_usage[0].amt_usage),"CCF03P","dollar_usage[0].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[1].nbr_usage, sizeof(ccf03p->dollar_usage[1].nbr_usage),"CCF03P","dollar_usage[1].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[1].amt_usage, sizeof(ccf03p->dollar_usage[1].amt_usage),"CCF03P","dollar_usage[1].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[2].nbr_usage, sizeof(ccf03p->dollar_usage[2].nbr_usage),"CCF03P","dollar_usage[2].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[2].amt_usage, sizeof(ccf03p->dollar_usage[2].amt_usage),"CCF03P","dollar_usage[2].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[3].nbr_usage, sizeof(ccf03p->dollar_usage[3].nbr_usage),"CCF03P","dollar_usage[3].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[3].amt_usage, sizeof(ccf03p->dollar_usage[3].amt_usage),"CCF03P","dollar_usage[3].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[4].nbr_usage, sizeof(ccf03p->dollar_usage[4].nbr_usage),"CCF03P","dollar_usage[4].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[4].amt_usage, sizeof(ccf03p->dollar_usage[4].amt_usage),"CCF03P","dollar_usage[4].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[5].nbr_usage, sizeof(ccf03p->dollar_usage[5].nbr_usage),"CCF03P","dollar_usage[5].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[5].amt_usage, sizeof(ccf03p->dollar_usage[5].amt_usage),"CCF03P","dollar_usage[5].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[6].nbr_usage, sizeof(ccf03p->dollar_usage[6].nbr_usage),"CCF03P","dollar_usage[6].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[6].amt_usage, sizeof(ccf03p->dollar_usage[6].amt_usage),"CCF03P","dollar_usage[6].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[7].nbr_usage, sizeof(ccf03p->dollar_usage[7].nbr_usage),"CCF03P","dollar_usage[7].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[7].amt_usage, sizeof(ccf03p->dollar_usage[7].amt_usage),"CCF03P","dollar_usage[7].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[8].nbr_usage, sizeof(ccf03p->dollar_usage[8].nbr_usage),"CCF03P","dollar_usage[8].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[8].amt_usage, sizeof(ccf03p->dollar_usage[8].amt_usage),"CCF03P","dollar_usage[8].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[9].nbr_usage, sizeof(ccf03p->dollar_usage[9].nbr_usage),"CCF03P","dollar_usage[9].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[9].amt_usage, sizeof(ccf03p->dollar_usage[9].amt_usage),"CCF03P","dollar_usage[9].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[10].nbr_usage, sizeof(ccf03p->dollar_usage[10].nbr_usage),"CCF03P","dollar_usage[10].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[10].amt_usage, sizeof(ccf03p->dollar_usage[10].amt_usage),"CCF03P","dollar_usage[10].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[11].nbr_usage, sizeof(ccf03p->dollar_usage[11].nbr_usage),"CCF03P","dollar_usage[11].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[11].amt_usage, sizeof(ccf03p->dollar_usage[11].amt_usage),"CCF03P","dollar_usage[11].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[12].nbr_usage, sizeof(ccf03p->dollar_usage[12].nbr_usage),"CCF03P","dollar_usage[12].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[12].amt_usage, sizeof(ccf03p->dollar_usage[12].amt_usage),"CCF03P","dollar_usage[12].amt_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[13].nbr_usage, sizeof(ccf03p->dollar_usage[13].nbr_usage),"CCF03P","dollar_usage[13].nbr_usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->dollar_usage[13].amt_usage, sizeof(ccf03p->dollar_usage[13].amt_usage),"CCF03P","dollar_usage[13].amt_usage","CheckNULLTerminatedinCCF03",mode);



	checknull(ccf03p->failed_pin_attempts, sizeof(ccf03p->failed_pin_attempts),"CCF03P","failed_pin_attempts","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->date_added, sizeof(ccf03p->date_added),"CCF03P","date_added","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->last_maint, sizeof(ccf03p->last_maint),"CCF03P","last_maint","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->status, sizeof(ccf03p->status),"CCF03P","status","CheckNULLTerminatedinCCF03",mode);

	checknull(ccf03p->mcc[0].category_code, sizeof(ccf03p->mcc[0].category_code),"CCF03P","mcc[0].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[0].usage, sizeof(ccf03p->mcc[0].usage),"CCF03P","mcc[0].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[1].category_code, sizeof(ccf03p->mcc[1].category_code),"CCF03P","mcc[1].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[1].usage, sizeof(ccf03p->mcc[1].usage),"CCF03P","mcc[1].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[2].category_code, sizeof(ccf03p->mcc[2].category_code),"CCF03P","mcc[2].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[2].usage, sizeof(ccf03p->mcc[2].usage),"CCF03P","mcc[2].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[3].category_code, sizeof(ccf03p->mcc[3].category_code),"CCF03P","mcc[3].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[3].usage, sizeof(ccf03p->mcc[3].usage),"CCF03P","mcc[3].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[4].category_code, sizeof(ccf03p->mcc[4].category_code),"CCF03P","mcc[4].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[4].usage, sizeof(ccf03p->mcc[4].usage),"CCF03P","mcc[4].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[5].category_code, sizeof(ccf03p->mcc[5].category_code),"CCF03P","mcc[5].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[5].usage, sizeof(ccf03p->mcc[5].usage),"ccf03p","mcc[5].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[6].category_code, sizeof(ccf03p->mcc[6].category_code),"ccf03p","mcc[6].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[6].usage, sizeof(ccf03p->mcc[6].usage),"CCF03P","mcc[6].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[7].category_code, sizeof(ccf03p->mcc[7].category_code),"CCF03P","mcc[7].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[7].usage, sizeof(ccf03p->mcc[7].usage),"CCF03P","mcc[7].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[8].category_code, sizeof(ccf03p->mcc[8].category_code),"CCF03P","mcc[8].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[8].usage, sizeof(ccf03p->mcc[8].usage),"CCF03P","mcc[8].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[9].category_code, sizeof(ccf03p->mcc[9].category_code),"CCF03P","mcc[9].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[9].usage, sizeof(ccf03p->mcc[9].usage),"CCF03P","mcc[9].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[10].category_code, sizeof(ccf03p->mcc[10].category_code),"CCF03P","mcc[10].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[10].usage, sizeof(ccf03p->mcc[10].usage),"CCF03P","mcc[10].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[11].category_code, sizeof(ccf03p->mcc[11].category_code),"CCF03P","mcc[11].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[11].usage, sizeof(ccf03p->mcc[11].usage),"CCF03P","mcc[11].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[12].category_code, sizeof(ccf03p->mcc[12].category_code),"CCF03P","mcc[12].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[12].usage, sizeof(ccf03p->mcc[12].usage),"CCF03P","mcc[12].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[13].category_code, sizeof(ccf03p->mcc[13].category_code),"CCF03P","mcc[13].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[13].usage, sizeof(ccf03p->mcc[13].usage),"CCF03P","mcc[13].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[14].category_code, sizeof(ccf03p->mcc[14].category_code),"CCF03P","mcc[14].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[14].usage, sizeof(ccf03p->mcc[14].usage),"CCF03P","mcc[14].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[15].category_code, sizeof(ccf03p->mcc[15].category_code),"CCF03P","mcc[15].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[15].usage, sizeof(ccf03p->mcc[15].usage),"CCF03P","mcc[15].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[16].category_code, sizeof(ccf03p->mcc[16].category_code),"CCF03P","mcc[16].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[16].usage, sizeof(ccf03p->mcc[16].usage),"CCF03P","mcc[16].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[17].category_code, sizeof(ccf03p->mcc[17].category_code),"CCF03P","mcc[17].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[17].usage, sizeof(ccf03p->mcc[17].usage),"CCF03P","mcc[17].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[18].category_code, sizeof(ccf03p->mcc[18].category_code),"CCF03P","mcc[18].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[18].usage, sizeof(ccf03p->mcc[18].usage),"CCF03P","mcc[18].usage","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[19].category_code, sizeof(ccf03p->mcc[19].category_code),"CCF03P","mcc[19].category_code","CheckNULLTerminatedinCCF03",mode);
	checknull(ccf03p->mcc[19].usage, sizeof(ccf03p->mcc[19].usage),"CCF03P","mcc[19].usage","CheckNULLTerminatedinCCF03",mode);

}

void CheckNULLTerminatedinPAF01(pPAF01   paf01,pCHAR mode)
{
	checknull(paf01->primary_key.primary_acct_nbr, sizeof(paf01->primary_key.primary_acct_nbr),"PAF01","primary_key.primary_acct_nbr","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->name_dba, sizeof(paf01->name_dba),"PAF01","name_dba","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->address1, sizeof(paf01->address1),"PAF01","address1","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->address2, sizeof(paf01->address2),"PAF01","address2","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->city, sizeof(paf01->city),"PAF01","city","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->state, sizeof(paf01->state),"PAF01","state","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->zip_code, sizeof(paf01->zip_code),"PAF01","zip_code","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->home_phone, sizeof(paf01->home_phone),"PAF01","home_phone","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->work_phone, sizeof(paf01->work_phone),"PAF01","work_phone","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->status, sizeof(paf01->status),"PAF01","status","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->caf_profile_id, sizeof(paf01->caf_profile_id),"PAF01","caf_profile_id","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->type, sizeof(paf01->type),"PAF01","type","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->corp_acct_nbr, sizeof(paf01->corp_acct_nbr),"PAF01","corp_acct_nbr","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->block_status, sizeof(paf01->block_status),"PAF01","block_status","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->account_name, sizeof(paf01->account_name),"PAF01","account_name","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->past_due_amount, sizeof(paf01->past_due_amount),"PAF01","past_due_amount","CheckNULLTerminatedinPAF01",mode);
	checknull(paf01->min_pay_code, sizeof(paf01->min_pay_code),"PAF01","min_pay_code","CheckNULLTerminatedinPAF01",mode);
}

void CheckNULLTerminatedinDEF01(pDEF01   def01,pCHAR mode)
{
	checknull(def01->primary_key.organization_id, sizeof(def01->primary_key.organization_id),"DEF01","primary_key.organization_id","CheckNULLTerminatedinDEF01",mode);
	checknull(def01->primary_key.merchant_id, sizeof(def01->primary_key.merchant_id),"DEF01","primary_key.merchant_id","CheckNULLTerminatedinDEF01",mode);
	checknull(def01->primary_key.term_length, sizeof(def01->primary_key.term_length),"DEF01","primary_key.term_length","CheckNULLTerminatedinDEF01",mode);
	checknull(def01->factor, sizeof(def01->factor),"DEF01","factor","CheckNULLTerminatedinDEF01",mode);
	checknull(def01->deferred_min_amount, sizeof(def01->deferred_min_amount),"DEF01","deferred_min_amount","CheckNULLTerminatedinDEF01",mode);
	checknull(def01->deferred_max_amount, sizeof(def01->deferred_max_amount),"DEF01","deferred_max_amount","CheckNULLTerminatedinDEF01",mode);
}

void CheckNULLTerminatedinCSF01(pCSF01   csf01,pCHAR mode)
{
	checknull(csf01->primary_key.status, sizeof(csf01->primary_key.status),"CSF01","primary_key.status","CheckNULLTerminatedinCSF01",mode);
	checknull(csf01->description, sizeof(csf01->description),"CSF01","description","CheckNULLTerminatedinCSF01",mode);
	checknull(csf01->iso_resp_code, sizeof(csf01->iso_resp_code),"CSF01","iso_resp_code","CheckNULLTerminatedinCSF01",mode);
	checknull(csf01->resp_text_msg, sizeof(csf01->resp_text_msg),"CSF01","resp_text_msg","CheckNULLTerminatedinCSF01",mode);
}

void CheckNULLTerminatedinCRF01(pCRF01   crf01,pCHAR mode)
{
	checknull(crf01->primary_key.currency_code, sizeof(crf01->primary_key.currency_code),"CRF01","primary_key.currency_code","CheckNULLTerminatedinCRF01",mode);
	checknull(crf01->conversion_rate, sizeof(crf01->conversion_rate),"CRF01","conversion_rate","CheckNULLTerminatedinCRF01",mode);
	checknull(crf01->currency_name, sizeof(crf01->currency_name),"CRF01","currency_name","CheckNULLTerminatedinCRF01",mode);
}

void CheckNULLTerminatedinACF01(pACF01   acf01,pCHAR mode)
{
	checknull(acf01->primary_key.account_type, sizeof(acf01->primary_key.account_type),"ACF01","primary_key.account_type","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->primary_key.account_nbr, sizeof(acf01->primary_key.account_nbr),"ACF01","primary_key.account_nbr","CheckNULLTerminatedinACF01",mode);

	checknull(acf01->status, sizeof(acf01->status),"ACF01","status","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->date_added, sizeof(acf01->date_added),"ACF01","date_added","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->maint_date, sizeof(acf01->maint_date),"ACF01","maint_date","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->changed_by, sizeof(acf01->changed_by),"ACF01","changed_by","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->credit_limit, sizeof(acf01->credit_limit),"ACF01","credit_limit","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->available, sizeof(acf01->available),"ACF01","available","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->outstanding, sizeof(acf01->outstanding),"ACF01","outstanding","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->ledger_balance, sizeof(acf01->ledger_balance),"ACF01","ledger_balance","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->cash_limit, sizeof(acf01->cash_limit),"ACF01","cash_limit","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->cash_available, sizeof(acf01->cash_available),"ACF01","cash_available","CheckNULLTerminatedinACF01",mode);
	checknull(acf01->cash_outstanding, sizeof(acf01->cash_outstanding),"ACF01","cash_outstanding","CheckNULLTerminatedinACF01",mode);
}

void CheckNULLTerminatedinFAF01(pFAF01   faf01,pCHAR mode)
{
	checknull(faf01->primary_key.caf_profile_id, sizeof(faf01->primary_key.caf_profile_id),"FAF01","primary_key.caf_profile_id","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[0].amount, sizeof(faf01->velocity[0].amount),"FAF01","velocity[0].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[0].from_time, sizeof(faf01->velocity[0].from_time),"FAF01","velocity[0].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[0].to_time, sizeof(faf01->velocity[0].to_time),"FAF01","velocity[0].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[1].amount, sizeof(faf01->velocity[1].amount),"FAF01","velocity[1].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[1].from_time, sizeof(faf01->velocity[1].from_time),"FAF01","velocity[1].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[1].to_time, sizeof(faf01->velocity[1].to_time),"FAF01","velocity[1].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[2].amount, sizeof(faf01->velocity[2].amount),"FAF01","velocity[2].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[2].from_time, sizeof(faf01->velocity[2].from_time),"FAF01","velocity[2].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[2].to_time, sizeof(faf01->velocity[2].to_time),"FAF01","velocity[2].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[3].amount, sizeof(faf01->velocity[3].amount),"FAF01","velocity[3].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[3].from_time, sizeof(faf01->velocity[3].from_time),"FAF01","velocity[3].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[3].to_time, sizeof(faf01->velocity[3].to_time),"FAF01","velocity[3].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[4].amount, sizeof(faf01->velocity[4].amount),"FAF01","velocity[4].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[4].from_time, sizeof(faf01->velocity[4].from_time),"FAF01","velocity[4].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[4].to_time, sizeof(faf01->velocity[4].to_time),"FAF01","velocity[4].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[5].amount, sizeof(faf01->velocity[5].amount),"FAF01","velocity[5].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[5].from_time, sizeof(faf01->velocity[5].from_time),"FAF01","velocity[5].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[5].to_time, sizeof(faf01->velocity[5].to_time),"FAF01","velocity[5].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[6].amount, sizeof(faf01->velocity[6].amount),"FAF01","velocity[6].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[6].from_time, sizeof(faf01->velocity[6].from_time),"FAF01","velocity[6].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[6].to_time, sizeof(faf01->velocity[6].to_time),"FAF01","velocity[6].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[7].amount, sizeof(faf01->velocity[7].amount),"FAF01","velocity[7].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[7].from_time, sizeof(faf01->velocity[7].from_time),"FAF01","velocity[7].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[7].to_time, sizeof(faf01->velocity[7].to_time),"FAF01","velocity[7].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[8].amount, sizeof(faf01->velocity[8].amount),"FAF01","velocity[8].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[8].from_time, sizeof(faf01->velocity[8].from_time),"FAF01","velocity[8].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[8].to_time, sizeof(faf01->velocity[8].to_time),"FAF01","velocity[8].to_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[9].amount, sizeof(faf01->velocity[9].amount),"FAF01","velocity[9].amount","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[9].from_time, sizeof(faf01->velocity[9].from_time),"FAF01","velocity[9].from_time","CheckNULLTerminatedinFAF01",mode);
	checknull(faf01->velocity[9].to_time, sizeof(faf01->velocity[9].to_time),"FAF01","velocity[9].to_time","CheckNULLTerminatedinFAF01",mode);
}

void CheckNULLTerminatedinCLF01(pCLF01   clf01,pCHAR mode)
{
	checknull(clf01->primary_key.year, sizeof(clf01->primary_key.year),"CLF01","primary_key.year","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->primary_key.month, sizeof(clf01->primary_key.month),"CLF01","primary_key.month","CheckNULLTerminatedinCLF01",mode);

	checknull(clf01->day[0].working_day, sizeof(clf01->day[0].working_day),"CLF01","day[0].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[0].profile_id, sizeof(clf01->day[0].profile_id),"CLF01","day[0].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[1].working_day, sizeof(clf01->day[1].working_day),"CLF01","day[1].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[1].profile_id, sizeof(clf01->day[1].profile_id),"CLF01","day[1].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[2].working_day, sizeof(clf01->day[2].working_day),"CLF01","day[2].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[2].profile_id, sizeof(clf01->day[2].profile_id),"CLF01","day[2].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[3].working_day, sizeof(clf01->day[3].working_day),"CLF01","day[3].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[3].profile_id, sizeof(clf01->day[3].profile_id),"CLF01","day[3].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[4].working_day, sizeof(clf01->day[4].working_day),"CLF01","day[4].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[4].profile_id, sizeof(clf01->day[4].profile_id),"CLF01","day[4].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[5].working_day, sizeof(clf01->day[5].working_day),"CLF01","day[5].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[5].profile_id, sizeof(clf01->day[5].profile_id),"CLF01","day[5].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[6].working_day, sizeof(clf01->day[6].working_day),"CLF01","day[6].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[6].profile_id, sizeof(clf01->day[6].profile_id),"CLF01","day[6].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[7].working_day, sizeof(clf01->day[7].working_day),"CLF01","day[7].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[7].profile_id, sizeof(clf01->day[7].profile_id),"CLF01","day[7].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[8].working_day, sizeof(clf01->day[8].working_day),"CLF01","day[8].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[8].profile_id, sizeof(clf01->day[8].profile_id),"CLF01","day[8].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[9].working_day, sizeof(clf01->day[9].working_day),"CLF01","day[9].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[9].profile_id, sizeof(clf01->day[9].profile_id),"CLF01","day[9].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[10].working_day, sizeof(clf01->day[10].working_day),"CLF01","day[10].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[10].profile_id, sizeof(clf01->day[10].profile_id),"CLF01","day[10].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[11].working_day, sizeof(clf01->day[11].working_day),"CLF01","day[11].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[11].profile_id, sizeof(clf01->day[11].profile_id),"CLF01","day[11].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[12].working_day, sizeof(clf01->day[12].working_day),"CLF01","day[12].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[12].profile_id, sizeof(clf01->day[12].profile_id),"CLF01","day[12].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[13].working_day, sizeof(clf01->day[13].working_day),"CLF01","day[13].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[13].profile_id, sizeof(clf01->day[13].profile_id),"CLF01","day[13].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[14].working_day, sizeof(clf01->day[14].working_day),"CLF01","day[14].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[14].profile_id, sizeof(clf01->day[14].profile_id),"CLF01","day[14].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[15].working_day, sizeof(clf01->day[15].working_day),"CLF01","day[15].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[15].profile_id, sizeof(clf01->day[15].profile_id),"CLF01","day[15].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[16].working_day, sizeof(clf01->day[16].working_day),"CLF01","day[16].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[16].profile_id, sizeof(clf01->day[16].profile_id),"CLF01","day[16].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[17].working_day, sizeof(clf01->day[17].working_day),"CLF01","day[17].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[17].profile_id, sizeof(clf01->day[17].profile_id),"CLF01","day[17].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[18].working_day, sizeof(clf01->day[18].working_day),"CLF01","day[18].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[18].profile_id, sizeof(clf01->day[18].profile_id),"CLF01","day[18].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[19].working_day, sizeof(clf01->day[19].working_day),"CLF01","day[19].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[19].profile_id, sizeof(clf01->day[19].profile_id),"CLF01","day[19].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[20].working_day, sizeof(clf01->day[20].working_day),"CLF01","day[20].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[20].profile_id, sizeof(clf01->day[20].profile_id),"CLF01","day[20].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[21].working_day, sizeof(clf01->day[21].working_day),"CLF01","day[21].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[21].profile_id, sizeof(clf01->day[21].profile_id),"CLF01","day[21].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[22].working_day, sizeof(clf01->day[22].working_day),"CLF01","day[22].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[22].profile_id, sizeof(clf01->day[22].profile_id),"CLF01","day[22].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[23].working_day, sizeof(clf01->day[23].working_day),"CLF01","day[23].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[23].profile_id, sizeof(clf01->day[23].profile_id),"CLF01","day[23].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[24].working_day, sizeof(clf01->day[24].working_day),"CLF01","day[24].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[24].profile_id, sizeof(clf01->day[24].profile_id),"CLF01","day[24].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[25].working_day, sizeof(clf01->day[25].working_day),"CLF01","day[25].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[25].profile_id, sizeof(clf01->day[25].profile_id),"CLF01","day[25].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[26].working_day, sizeof(clf01->day[26].working_day),"CLF01","day[26].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[26].profile_id, sizeof(clf01->day[26].profile_id),"CLF01","day[26].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[27].working_day, sizeof(clf01->day[27].working_day),"CLF01","day[27].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[27].profile_id, sizeof(clf01->day[27].profile_id),"CLF01","day[27].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[28].working_day, sizeof(clf01->day[28].working_day),"CLF01","day[28].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[28].profile_id, sizeof(clf01->day[28].profile_id),"CLF01","day[28].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[29].working_day, sizeof(clf01->day[29].working_day),"CLF01","day[29].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[29].profile_id, sizeof(clf01->day[29].profile_id),"CLF01","day[29].profile_id","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[30].working_day, sizeof(clf01->day[30].working_day),"CLF01","day[30].working_day","CheckNULLTerminatedinCLF01",mode);
	checknull(clf01->day[30].profile_id, sizeof(clf01->day[30].profile_id),"CLF01","day[30].profile_id","CheckNULLTerminatedinCLF01",mode);
}

void CheckNULLTerminatedinBPF01(pBPF01   bpf01,pCHAR mode)
{
	checknull(bpf01->primary_key.profile_id, sizeof(bpf01->primary_key.profile_id),"BPF01","primary_key.profile_id","CheckNULLTerminatedinBPF01",mode);
	checknull(bpf01->primary_key.job_id, sizeof(bpf01->primary_key.job_id),"BPF01","primary_key.job_id","CheckNULLTerminatedinBPF01",mode);
}

void CheckNULLTerminatedinJBF01(pJBF01   jbf01,pCHAR mode)
{
	checknull(jbf01->primary_key.job_id, sizeof(jbf01->primary_key.job_id),"JBF01","primary_key.job_id","CheckNULLTerminatedinJBF01",mode);
	checknull(jbf01->time_to_run, sizeof(jbf01->time_to_run),"JBF01","time_to_run","CheckNULLTerminatedinJBF01",mode);
	checknull(jbf01->status, sizeof(jbf01->status),"JBF01","status","CheckNULLTerminatedinJBF01",mode);
	checknull(jbf01->last_run, sizeof(jbf01->last_run),"JBF01","last_run","CheckNULLTerminatedinJBF01",mode);
	checknull(jbf01->app_name, sizeof(jbf01->app_name),"JBF01","app_name","CheckNULLTerminatedinJBF01",mode);
}

void CheckNULLTerminatedinAPF01(pAPF01   apf01,pCHAR mode)
{
	checknull(apf01->primary_key.app_name, sizeof(apf01->primary_key.app_name),"APF01","primary_key.app_name","CheckNULLTerminatedinAPF01",mode);
	checknull(apf01->exe_name, sizeof(apf01->exe_name),"APF01","exe_name","CheckNULLTerminatedinAPF01",mode);
	checknull(apf01->path, sizeof(apf01->path),"APF01","path","CheckNULLTerminatedinAPF01",mode);
	checknull(apf01->parameters, sizeof(apf01->parameters),"APF01","parameters","CheckNULLTerminatedinAPF01",mode);
}

void CheckNULLTerminatedinFCF02(pFCF02   fcf02,pCHAR mode)
{
	checknull(fcf02->primary_key.card_nbr, sizeof(fcf02->primary_key.card_nbr),"FCF02","primary_key.card_nbr","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->primary_key.card_type, sizeof(fcf02->primary_key.card_type),"FCF02","primary_key.card_type","CheckNULLTerminatedinFCF02",mode);

	checknull(fcf02->action_code, sizeof(fcf02->action_code),"FCF02","action_code","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->crind_autocard, sizeof(fcf02->crind_autocard),"FCF02","crind_autocard","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->odometer_required, sizeof(fcf02->odometer_required),"FCF02","odometer_required","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->order_number_required, sizeof(fcf02->order_number_required),"FCF02","order_number_required","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->signature_required, sizeof(fcf02->signature_required),"FCF02","signature_required","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->driver_flag, sizeof(fcf02->driver_flag),"FCF02","driver_flag","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->vehicle_id_flag, sizeof(fcf02->vehicle_id_flag),"FCF02","vehicle_id_flag","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->fleet_id_flag, sizeof(fcf02->fleet_id_flag),"FCF02","fleet_id_flag","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->driver_id_flag, sizeof(fcf02->driver_id_flag),"FCF02","driver_id_flag","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->registration_number, sizeof(fcf02->registration_number),"FCF02","registration_number","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->super, sizeof(fcf02->super),"FCF02","super","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->super_low_lead, sizeof(fcf02->super_low_lead),"FCF02","super_low_lead","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->gold, sizeof(fcf02->gold),"FCF02","gold","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->autogas, sizeof(fcf02->autogas),"FCF02","autogas","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->unleaded, sizeof(fcf02->unleaded),"FCF02","unleaded","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->diesel, sizeof(fcf02->diesel),"FCF02","diesel","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->oil, sizeof(fcf02->oil),"FCF02","oil","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->service, sizeof(fcf02->service),"FCF02","service","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->accessories, sizeof(fcf02->accessories),"FCF02","accessories","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->tires, sizeof(fcf02->tires),"FCF02","tires","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->batteries, sizeof(fcf02->batteries),"FCF02","batteries","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->repairs_maint, sizeof(fcf02->repairs_maint),"FCF02","repairs_maint","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->shop, sizeof(fcf02->shop),"FCF02","shop","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->clean_and_go, sizeof(fcf02->clean_and_go),"FCF02","clean_and_go","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->accident_and_damage, sizeof(fcf02->accident_and_damage),"FCF02","accident_and_damage","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->meals, sizeof(fcf02->meals),"FCF02","meals","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->parking, sizeof(fcf02->parking),"FCF02","parking","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->last_odometer_reading, sizeof(fcf02->last_odometer_reading),"FCF02","last_odometer_reading","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->value_limits, sizeof(fcf02->value_limits),"FCF02","value_limits","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->litre_limits, sizeof(fcf02->litre_limits),"FCF02","litre_limits","CheckNULLTerminatedinFCF02",mode);
	checknull(fcf02->time_limits, sizeof(fcf02->time_limits),"FCF02","time_limits","CheckNULLTerminatedinFCF02",mode);
}


void CheckNULLTerminatedinMCF01(pMCF01   mcf01,pCHAR mode)
{
	checknull(mcf01->primary_key.organization_id, sizeof(mcf01->primary_key.organization_id),"MCF01","primary_key.organization_id","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->primary_key.merchant_id, sizeof(mcf01->primary_key.merchant_id),"MCF01","primary_key.merchant_id","CheckNULLTerminatedinMCF01",mode);

	checknull(mcf01->status, sizeof(mcf01->status),"MCF01","status","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->name01, sizeof(mcf01->name01),"MCF01","name01","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->name02, sizeof(mcf01->name02),"MCF01","name02","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->address01, sizeof(mcf01->address01),"MCF01","address01","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->address02, sizeof(mcf01->address02),"MCF01","address02","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->city, sizeof(mcf01->city),"MCF01","city","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->country, sizeof(mcf01->country),"MCF01","country","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->state, sizeof(mcf01->state),"MCF01","state","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->zip_code, sizeof(mcf01->zip_code),"MCF01","zip_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->intl_code, sizeof(mcf01->intl_code),"MCF01","intl_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->currency_code, sizeof(mcf01->currency_code),"MCF01","currency_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->contact, sizeof(mcf01->contact),"MCF01","contact","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->phone01, sizeof(mcf01->phone01),"MCF01","phone01","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->phone02, sizeof(mcf01->phone02),"MCF01","phone02","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->invoice_batch, sizeof(mcf01->invoice_batch),"MCF01","invoice_batch","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->invoice_subcode, sizeof(mcf01->invoice_subcode),"MCF01","invoice_subcode","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->process_control_id, sizeof(mcf01->process_control_id),"MCF01","process_control_id","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->date_added, sizeof(mcf01->date_added),"MCF01","date_added","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->region_nbr, sizeof(mcf01->region_nbr),"MCF01","region_nbr","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->division_nbr, sizeof(mcf01->division_nbr),"MCF01","division_nbr","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->password, sizeof(mcf01->password),"MCF01","password","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->rps_merchant, sizeof(mcf01->rps_merchant),"MCF01","rps_merchant","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->house_card_merchant, sizeof(mcf01->house_card_merchant),"MCF01","house_card_merchant","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->procurement_merchant, sizeof(mcf01->procurement_merchant),"MCF01","procurement_merchant","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->deferred_merchant, sizeof(mcf01->deferred_merchant),"MCF01","deferred_merchant","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->cash_bonus_eligible, sizeof(mcf01->cash_bonus_eligible),"MCF01","cash_bonus_eligible","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->vip_code, sizeof(mcf01->vip_code),"MCF01","vip_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->pos_code, sizeof(mcf01->pos_code),"MCF01","pos_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->vs_tcc_code, sizeof(mcf01->vs_tcc_code),"MCF01","vs_tcc_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->mc_fcc_code, sizeof(mcf01->mc_fcc_code),"MCF01","mc_fcc_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->jcb_sub_code, sizeof(mcf01->jcb_sub_code),"MCF01","jcb_sub_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->mall_code, sizeof(mcf01->mall_code),"MCF01","mall_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->hc_code, sizeof(mcf01->hc_code),"MCF01","hc_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->solicit_code, sizeof(mcf01->solicit_code),"MCF01","solicit_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->block_start_date, sizeof(mcf01->block_start_date),"MCF01","block_start_date","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->block_end_date, sizeof(mcf01->block_end_date),"MCF01","block_end_date","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_purch_max, sizeof(mcf01->credit_purch_max),"MCF01","credit_purch_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_auth_max, sizeof(mcf01->credit_auth_max),"MCF01","credit_auth_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_return_max, sizeof(mcf01->credit_return_max),"MCF01","credit_return_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_cash_adv_max, sizeof(mcf01->credit_cash_adv_max),"MCF01","credit_cash_adv_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_man_entry_max, sizeof(mcf01->credit_man_entry_max),"MCF01","credit_man_entry_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->credit_cash_back_max, sizeof(mcf01->credit_cash_back_max),"MCF01","credit_cash_back_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_purch_max, sizeof(mcf01->debit_purch_max),"MCF01","debit_purch_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_auth_max, sizeof(mcf01->debit_auth_max),"MCF01","debit_auth_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_return_max, sizeof(mcf01->debit_return_max),"MCF01","debit_return_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_cash_adv_max, sizeof(mcf01->debit_cash_adv_max),"MCF01","debit_cash_adv_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_man_entry_max, sizeof(mcf01->debit_man_entry_max),"MCF01","debit_man_entry_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->debit_cash_back_max, sizeof(mcf01->debit_cash_back_max),"MCF01","debit_cash_back_max","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->aver_purch_amount, sizeof(mcf01->aver_purch_amount),"MCF01","aver_purch_amount","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->max_num_returns, sizeof(mcf01->max_num_returns),"MCF01","max_num_returns","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->amount_returns, sizeof(mcf01->amount_returns),"MCF01","amount_returns","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->number_man_entries, sizeof(mcf01->number_man_entries),"MCF01","number_man_entries","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->amount_deposits, sizeof(mcf01->amount_deposits),"MCF01","amount_deposits","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->pan_same, sizeof(mcf01->pan_same),"MCF01","pan_same","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->totals_ind, sizeof(mcf01->totals_ind),"MCF01","totals_ind","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->statistics_ind, sizeof(mcf01->statistics_ind),"MCF01","statistics_ind","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[0].priority_routing, sizeof(mcf01->network[0].priority_routing),"MCF01","network[0].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[1].priority_routing, sizeof(mcf01->network[1].priority_routing),"MCF01","network[1].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[2].priority_routing, sizeof(mcf01->network[2].priority_routing),"MCF01","network[2].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[3].priority_routing, sizeof(mcf01->network[3].priority_routing),"MCF01","network[3].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[4].priority_routing, sizeof(mcf01->network[4].priority_routing),"MCF01","network[4].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[5].priority_routing, sizeof(mcf01->network[5].priority_routing),"MCF01","network[5].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[6].priority_routing, sizeof(mcf01->network[6].priority_routing),"MCF01","network[6].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[7].priority_routing, sizeof(mcf01->network[7].priority_routing),"MCF01","network[7].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[8].priority_routing, sizeof(mcf01->network[8].priority_routing),"MCF01","network[8].priority_routing","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->network[9].priority_routing, sizeof(mcf01->network[9].priority_routing),"MCF01","network[9].priority_routing","CheckNULLTerminatedinMCF01",mode);

	checknull(mcf01->negative_group_id, sizeof(mcf01->negative_group_id),"MCF01","negative_group_id","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->voice_category_code, sizeof(mcf01->voice_category_code),"MCF01","voice_category_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->class_code, sizeof(mcf01->class_code),"MCF01","class_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->deferred_start_date, sizeof(mcf01->deferred_start_date),"MCF01","deferred_start_date","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->deferred_end_date, sizeof(mcf01->deferred_end_date),"MCF01","deferred_end_date","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->group_outlet_code, sizeof(mcf01->group_outlet_code),"MCF01","group_outlet_code","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->group_type, sizeof(mcf01->group_type),"MCF01","group_type","CheckNULLTerminatedinMCF01",mode);
	checknull(mcf01->reload_merchant, sizeof(mcf01->reload_merchant),"MCF01","reload_merchant","CheckNULLTerminatedinMCF01",mode);
}

void CheckNULLTerminatedinMPPARA01(pMPPARA01   mppara01,pCHAR mode)
{
	checknull(mppara01->primary_key.product_code,sizeof(mppara01->primary_key.product_code),"MPPARA01","primary_key.product_code","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->minmajpurchamt,sizeof(mppara01->minmajpurchamt),"MPPARA01","minmajpurchamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->maxmajpurchamt,sizeof(mppara01->maxmajpurchamt),"MPPARA01","maxmajpurchamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->mininstamt,sizeof(mppara01->mininstamt),"MPPARA01","mininstamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->maxinstamt,sizeof(mppara01->maxinstamt),"MPPARA01","maxinstamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->minmajpurchlmt,sizeof(mppara01->minmajpurchlmt),"MPPARA01","minmajpurchlmt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->minpenaltyamt,sizeof(mppara01->minpenaltyamt),"MPPARA01","minpenaltyamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->maxpenaltyamt,sizeof(mppara01->maxpenaltyamt),"MPPARA01","maxpenaltyamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->penaltyper,sizeof(mppara01->penaltyper),"MPPARA01","penaltyper","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->penaltyday,sizeof(mppara01->penaltyday),"MPPARA01","penaltyday","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->minamtofaging,sizeof(mppara01->minamtofaging),"MPPARA01","minamtofaging","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->mntowrtof,sizeof(mppara01->mntowrtof),"MPPARA01","mntowrtof","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->BPIminamt,sizeof(mppara01->BPIminamt),"MPPARA01","BPIminamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->brokenperdinst,sizeof(mppara01->brokenperdinst),"MPPARA01","brokenperdinst","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->minfreepurchamt,sizeof(mppara01->minfreepurchamt),"MPPARA01","minfreepurchamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->errorunauthamt,sizeof(mppara01->errorunauthamt),"MPPARA01","errorunauthamt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->fixedmpfee,sizeof(mppara01->fixedmpfee),"MPPARA01","fixedmpfee","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->mpfeepercnt,sizeof(mppara01->mpfeepercnt),"MPPARA01","mpfeepercnt","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->earlyrepayfeeper,sizeof(mppara01->earlyrepayfeeper),"MPPARA01","earlyrepayfeeper","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->effective_date,sizeof(mppara01->effective_date),"MPPARA01","effective_date","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->maxtrnperday,sizeof(mppara01->maxtrnperday),"MPPARA01","maxtrnperday","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->maxamtperday,sizeof(mppara01->maxamtperday),"MPPARA01","maxamtperday","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authoverlimit,sizeof(mppara01->authoverlimit),"MPPARA01","authoverlimit","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[0].message,sizeof(mppara01->authpara[0].message),"MPPARA01","authpara[0].message","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[1].message,sizeof(mppara01->authpara[1].message),"MPPARA01","authpara[1].message","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[2].message,sizeof(mppara01->authpara[2].message),"MPPARA01","authpara[2].message","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[0].response_code,sizeof(mppara01->authpara[0].response_code),"MPPARA01","authpara[0].response_code","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[1].response_code,sizeof(mppara01->authpara[1].response_code),"MPPARA01","authpara[1].response_code","CheckNULLTerminatedinMPPARA01",mode);
	checknull(mppara01->authpara[2].response_code,sizeof(mppara01->authpara[2].response_code),"MPPARA01","authpara[2].response_code","CheckNULLTerminatedinMPPARA01",mode);

}

void CheckNULLTerminatedinMraDbPasswords(pMraDbPasswords   mradbpasswords,pCHAR mode)
{
	checknull(mradbpasswords->mra_password,sizeof(mradbpasswords->mra_password),"MRADBPASSWORDS","mra_password","CheckNULLTerminatedinMraDbPasswords",mode);
	checknull(mradbpasswords->mra_user_id,sizeof(mradbpasswords->mra_user_id),"MRADBPASSWORDS","mra_user_id","CheckNULLTerminatedinMraDbPasswords",mode);
	checknull(mradbpasswords->mra_old_password,sizeof(mradbpasswords->mra_old_password),"MRADBPASSWORDS","mra_old_password","CheckNULLTerminatedinMraDbPasswords",mode);
}

void CheckNULLTerminatedinArchDbPasswords(pArchDbPasswords   archbpasswords,pCHAR mode)
{
	checknull(archbpasswords->arch_password,sizeof(archbpasswords->arch_password),"ARCHBPASSWORDS","arch_password","CheckNULLTerminatedinArchDbPasswords",mode);
	checknull(archbpasswords->arch_user_id,sizeof(archbpasswords->arch_user_id),"ARCHBPASSWORDS","arch_user_id","CheckNULLTerminatedinArchDbPasswords",mode);
	checknull(archbpasswords->arch_old_password,sizeof(archbpasswords->arch_old_password),"ARCHBPASSWORDS","arch_old_password","CheckNULLTerminatedinArchDbPasswords",mode);
}

void CheckNULLTerminatedinCOUNTRY_AMT_LIMITS(pCOUNTRY_AMT_LIMITS   country_amt_limits,pCHAR mode)
{
	checknull(country_amt_limits->primary_key.caf_profile_id,sizeof(country_amt_limits->primary_key.caf_profile_id),"COUNTRY_AMT_LIMITS","primary_key.caf_profile_id","CheckNULLTerminatedinCOUNTRY_AMT_LIMITS",mode);
	checknull(country_amt_limits->primary_key.type,sizeof(country_amt_limits->primary_key.type),"COUNTRY_AMT_LIMITS","primary_key.type","CheckNULLTerminatedinCOUNTRY_AMT_LIMITS",mode);
	checknull(country_amt_limits->country_code,sizeof(country_amt_limits->country_code),"COUNTRY_AMT_LIMITS","country_code","CheckNULLTerminatedinCOUNTRY_AMT_LIMITS",mode);
	checknull(country_amt_limits->amount_limit,sizeof(country_amt_limits->amount_limit),"COUNTRY_AMT_LIMITS","amount_limit","CheckNULLTerminatedinCOUNTRY_AMT_LIMITS",mode);
}

void CheckNULLTerminatedinMCC_LIMITS(pMCC_LIMITS   mcc_limits,pCHAR mode)
{
	checknull(mcc_limits->primary_key.caf_profile_id,sizeof(mcc_limits->primary_key.caf_profile_id),"MCC_LIMITS","primary_key.caf_profile_id","CheckNULLTerminatedinMCC_LIMITS",mode);
	checknull(mcc_limits->primary_key.type,sizeof(mcc_limits->primary_key.type),"MCC_LIMITS","primary_key.type","CheckNULLTerminatedinMCC_LIMITS",mode);
	checknull(mcc_limits->primary_key.mcc,sizeof(mcc_limits->primary_key.mcc),"MCC_LIMITS","primary_key.mcc","CheckNULLTerminatedinMCC_LIMITS",mode);
	checknull(mcc_limits->usage,sizeof(mcc_limits->usage),"MCC_LIMITS","usage","CheckNULLTerminatedinMCC_LIMITS",mode);
	checknull(mcc_limits->peso_amt_limit,sizeof(mcc_limits->peso_amt_limit),"MCC_LIMITS","peso_amt_limit","CheckNULLTerminatedinMCC_LIMITS",mode);
	checknull(mcc_limits->usd_amt_limit,sizeof(mcc_limits->usd_amt_limit),"MCC_LIMITS","usd_amt_limit","CheckNULLTerminatedinMCC_LIMITS",mode);
}

void CheckNULLTerminatedinNEGATIVE_FRAUD(pNEGATIVE_FRAUD   negative_fraud,pCHAR mode)
{
	checknull(negative_fraud->primary_key.mcc,sizeof(negative_fraud->primary_key.mcc),"NEGATIVE_FRAUD","primary_key.mcc","CheckNULLTerminatedinNEGATIVE_FRAUD",mode);
	checknull(negative_fraud->primary_key.bin_low,sizeof(negative_fraud->primary_key.bin_low),"NEGATIVE_FRAUD","primary_key.bin_low","CheckNULLTerminatedinNEGATIVE_FRAUD",mode);
	checknull(negative_fraud->primary_key.bin_high,sizeof(negative_fraud->primary_key.bin_high),"NEGATIVE_FRAUD","primary_key.bin_high","CheckNULLTerminatedinNEGATIVE_FRAUD",mode);

	checknull(negative_fraud->country_code,sizeof(negative_fraud->country_code),"NEGATIVE_FRAUD","country_code","CheckNULLTerminatedinNEGATIVE_FRAUD",mode);
}

void CheckNULLTerminatedinOffus_velocity(pOffus_velocity   offus_velocity,pCHAR mode)
{
	checknull(offus_velocity->primary_key.card_type,sizeof(offus_velocity->primary_key.card_type),"OFFUS_VELOCITY","primary_key.card_type","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->primary_key.card_nbr,sizeof(offus_velocity->primary_key.card_nbr),"OFFUS_VELOCITY","primary_key.card_nbr","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->primary_acct_nbr,sizeof(offus_velocity->primary_acct_nbr),"OFFUS_VELOCITY","primary_acct_nbr","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->daily_amt_usage,sizeof(offus_velocity->daily_amt_usage),"OFFUS_VELOCITY","daily_amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->julian_date,sizeof(offus_velocity->julian_date),"OFFUS_VELOCITY","julian_date","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_julian_date,sizeof(offus_velocity->peso_julian_date),"OFFUS_VELOCITY","peso_julian_date","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_julian_date,sizeof(offus_velocity->dollar_julian_date),"OFFUS_VELOCITY","dollar_julian_date","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->peso_usage[0].nbr_usage,sizeof(offus_velocity->peso_usage[0].nbr_usage),"OFFUS_VELOCITY","peso_usage[0].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[1].nbr_usage,sizeof(offus_velocity->peso_usage[1].nbr_usage),"OFFUS_VELOCITY","peso_usage[1].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[2].nbr_usage,sizeof(offus_velocity->peso_usage[2].nbr_usage),"OFFUS_VELOCITY","peso_usage[2].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[3].nbr_usage,sizeof(offus_velocity->peso_usage[3].nbr_usage),"OFFUS_VELOCITY","peso_usage[3].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[4].nbr_usage,sizeof(offus_velocity->peso_usage[4].nbr_usage),"OFFUS_VELOCITY","peso_usage[4].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[5].nbr_usage,sizeof(offus_velocity->peso_usage[5].nbr_usage),"OFFUS_VELOCITY","peso_usage[5].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[6].nbr_usage,sizeof(offus_velocity->peso_usage[6].nbr_usage),"OFFUS_VELOCITY","peso_usage[6].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[7].nbr_usage,sizeof(offus_velocity->peso_usage[7].nbr_usage),"OFFUS_VELOCITY","peso_usage[7].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[8].nbr_usage,sizeof(offus_velocity->peso_usage[8].nbr_usage),"OFFUS_VELOCITY","peso_usage[8].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[9].nbr_usage,sizeof(offus_velocity->peso_usage[9].nbr_usage),"OFFUS_VELOCITY","peso_usage[9].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[10].nbr_usage,sizeof(offus_velocity->peso_usage[10].nbr_usage),"OFFUS_VELOCITY","peso_usage[10].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[11].nbr_usage,sizeof(offus_velocity->peso_usage[11].nbr_usage),"OFFUS_VELOCITY","peso_usage[11].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[12].nbr_usage,sizeof(offus_velocity->peso_usage[12].nbr_usage),"OFFUS_VELOCITY","peso_usage[12].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[13].nbr_usage,sizeof(offus_velocity->peso_usage[13].nbr_usage),"OFFUS_VELOCITY","peso_usage[13].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->peso_usage[0].amt_usage,sizeof(offus_velocity->peso_usage[0].amt_usage),"OFFUS_VELOCITY","peso_usage[0].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[1].amt_usage,sizeof(offus_velocity->peso_usage[1].amt_usage),"OFFUS_VELOCITY","peso_usage[1].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[2].amt_usage,sizeof(offus_velocity->peso_usage[2].amt_usage),"OFFUS_VELOCITY","peso_usage[2].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[3].amt_usage,sizeof(offus_velocity->peso_usage[3].amt_usage),"OFFUS_VELOCITY","peso_usage[3].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[4].amt_usage,sizeof(offus_velocity->peso_usage[4].amt_usage),"OFFUS_VELOCITY","peso_usage[4].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[5].amt_usage,sizeof(offus_velocity->peso_usage[5].amt_usage),"OFFUS_VELOCITY","peso_usage[5].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[6].amt_usage,sizeof(offus_velocity->peso_usage[6].amt_usage),"OFFUS_VELOCITY","peso_usage[6].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[7].amt_usage,sizeof(offus_velocity->peso_usage[7].amt_usage),"OFFUS_VELOCITY","peso_usage[7].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[8].amt_usage,sizeof(offus_velocity->peso_usage[8].amt_usage),"OFFUS_VELOCITY","peso_usage[8].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[9].amt_usage,sizeof(offus_velocity->peso_usage[9].amt_usage),"OFFUS_VELOCITY","peso_usage[9].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[10].amt_usage,sizeof(offus_velocity->peso_usage[10].amt_usage),"OFFUS_VELOCITY","peso_usage[10].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[11].amt_usage,sizeof(offus_velocity->peso_usage[11].amt_usage),"OFFUS_VELOCITY","peso_usage[11].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[12].amt_usage,sizeof(offus_velocity->peso_usage[12].amt_usage),"OFFUS_VELOCITY","peso_usage[12].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->peso_usage[13].amt_usage,sizeof(offus_velocity->peso_usage[13].amt_usage),"OFFUS_VELOCITY","peso_usage[13].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->dollar_usage[0].nbr_usage,sizeof(offus_velocity->dollar_usage[0].nbr_usage),"OFFUS_VELOCITY","dollar_usage[0].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[1].nbr_usage,sizeof(offus_velocity->dollar_usage[1].nbr_usage),"OFFUS_VELOCITY","dollar_usage[1].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[2].nbr_usage,sizeof(offus_velocity->dollar_usage[2].nbr_usage),"OFFUS_VELOCITY","dollar_usage[2].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[3].nbr_usage,sizeof(offus_velocity->dollar_usage[3].nbr_usage),"OFFUS_VELOCITY","dollar_usage[3].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[4].nbr_usage,sizeof(offus_velocity->dollar_usage[4].nbr_usage),"OFFUS_VELOCITY","dollar_usage[4].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[5].nbr_usage,sizeof(offus_velocity->dollar_usage[5].nbr_usage),"OFFUS_VELOCITY","dollar_usage[5].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[6].nbr_usage,sizeof(offus_velocity->dollar_usage[6].nbr_usage),"OFFUS_VELOCITY","dollar_usage[6].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[7].nbr_usage,sizeof(offus_velocity->dollar_usage[7].nbr_usage),"OFFUS_VELOCITY","dollar_usage[7].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[8].nbr_usage,sizeof(offus_velocity->dollar_usage[8].nbr_usage),"OFFUS_VELOCITY","dollar_usage[8].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[9].nbr_usage,sizeof(offus_velocity->dollar_usage[9].nbr_usage),"OFFUS_VELOCITY","dollar_usage[9].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[10].nbr_usage,sizeof(offus_velocity->dollar_usage[10].nbr_usage),"OFFUS_VELOCITY","dollar_usage[10].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[11].nbr_usage,sizeof(offus_velocity->dollar_usage[11].nbr_usage),"OFFUS_VELOCITY","dollar_usage[11].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[12].nbr_usage,sizeof(offus_velocity->dollar_usage[12].nbr_usage),"OFFUS_VELOCITY","dollar_usage[12].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[13].nbr_usage,sizeof(offus_velocity->dollar_usage[13].nbr_usage),"OFFUS_VELOCITY","dollar_usage[13].nbr_usage","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->dollar_usage[0].amt_usage,sizeof(offus_velocity->dollar_usage[0].amt_usage),"OFFUS_VELOCITY","dollar_usage[0].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[1].amt_usage,sizeof(offus_velocity->dollar_usage[1].amt_usage),"OFFUS_VELOCITY","dollar_usage[1].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[2].amt_usage,sizeof(offus_velocity->dollar_usage[2].amt_usage),"OFFUS_VELOCITY","dollar_usage[2].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[3].amt_usage,sizeof(offus_velocity->dollar_usage[3].amt_usage),"OFFUS_VELOCITY","dollar_usage[3].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[4].amt_usage,sizeof(offus_velocity->dollar_usage[4].amt_usage),"OFFUS_VELOCITY","dollar_usage[4].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[5].amt_usage,sizeof(offus_velocity->dollar_usage[5].amt_usage),"OFFUS_VELOCITY","dollar_usage[5].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[6].amt_usage,sizeof(offus_velocity->dollar_usage[6].amt_usage),"OFFUS_VELOCITY","dollar_usage[6].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[7].amt_usage,sizeof(offus_velocity->dollar_usage[7].amt_usage),"OFFUS_VELOCITY","dollar_usage[7].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[8].amt_usage,sizeof(offus_velocity->dollar_usage[8].amt_usage),"OFFUS_VELOCITY","dollar_usage[8].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[9].amt_usage,sizeof(offus_velocity->dollar_usage[9].amt_usage),"OFFUS_VELOCITY","dollar_usage[9].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[10].amt_usage,sizeof(offus_velocity->dollar_usage[10].amt_usage),"OFFUS_VELOCITY","dollar_usage[10].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[11].amt_usage,sizeof(offus_velocity->dollar_usage[11].amt_usage),"OFFUS_VELOCITY","dollar_usage[11].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[12].amt_usage,sizeof(offus_velocity->dollar_usage[12].amt_usage),"OFFUS_VELOCITY","dollar_usage[12].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->dollar_usage[13].amt_usage,sizeof(offus_velocity->dollar_usage[13].amt_usage),"OFFUS_VELOCITY","dollar_usage[13].amt_usage","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->failed_pin_attempts,sizeof(offus_velocity->failed_pin_attempts),"OFFUS_VELOCITY","failed_pin_attempts","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->date_added,sizeof(offus_velocity->date_added),"OFFUS_VELOCITY","date_added","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->last_maint,sizeof(offus_velocity->last_maint),"OFFUS_VELOCITY","last_maint","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->status,sizeof(offus_velocity->status),"OFFUS_VELOCITY","status","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->mcc[0].category_code,sizeof(offus_velocity->mcc[0].category_code),"OFFUS_VELOCITY","mcc[0].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[1].category_code,sizeof(offus_velocity->mcc[1].category_code),"OFFUS_VELOCITY","mcc[1].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[2].category_code,sizeof(offus_velocity->mcc[2].category_code),"OFFUS_VELOCITY","mcc[2].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[3].category_code,sizeof(offus_velocity->mcc[3].category_code),"OFFUS_VELOCITY","mcc[3].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[4].category_code,sizeof(offus_velocity->mcc[4].category_code),"OFFUS_VELOCITY","mcc[4].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[5].category_code,sizeof(offus_velocity->mcc[5].category_code),"OFFUS_VELOCITY","mcc[5].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[6].category_code,sizeof(offus_velocity->mcc[6].category_code),"OFFUS_VELOCITY","mcc[6].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[7].category_code,sizeof(offus_velocity->mcc[7].category_code),"OFFUS_VELOCITY","mcc[7].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[8].category_code,sizeof(offus_velocity->mcc[8].category_code),"OFFUS_VELOCITY","mcc[8].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[9].category_code,sizeof(offus_velocity->mcc[9].category_code),"OFFUS_VELOCITY","mcc[9].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[10].category_code,sizeof(offus_velocity->mcc[10].category_code),"OFFUS_VELOCITY","mcc[10].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[11].category_code,sizeof(offus_velocity->mcc[11].category_code),"OFFUS_VELOCITY","mcc[11].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[12].category_code,sizeof(offus_velocity->mcc[12].category_code),"OFFUS_VELOCITY","mcc[12].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[13].category_code,sizeof(offus_velocity->mcc[13].category_code),"OFFUS_VELOCITY","mcc[13].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[14].category_code,sizeof(offus_velocity->mcc[14].category_code),"OFFUS_VELOCITY","mcc[14].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[15].category_code,sizeof(offus_velocity->mcc[15].category_code),"OFFUS_VELOCITY","mcc[15].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[16].category_code,sizeof(offus_velocity->mcc[16].category_code),"OFFUS_VELOCITY","mcc[16].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[17].category_code,sizeof(offus_velocity->mcc[17].category_code),"OFFUS_VELOCITY","mcc[17].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[18].category_code,sizeof(offus_velocity->mcc[18].category_code),"OFFUS_VELOCITY","mcc[18].category_code","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[19].category_code,sizeof(offus_velocity->mcc[19].category_code),"OFFUS_VELOCITY","mcc[19].category_code","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->mcc[0].usage,sizeof(offus_velocity->mcc[0].usage),"OFFUS_VELOCITY","mcc[0].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[1].usage,sizeof(offus_velocity->mcc[1].usage),"OFFUS_VELOCITY","mcc[1].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[2].usage,sizeof(offus_velocity->mcc[2].usage),"OFFUS_VELOCITY","mcc[2].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[3].usage,sizeof(offus_velocity->mcc[3].usage),"OFFUS_VELOCITY","mcc[3].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[4].usage,sizeof(offus_velocity->mcc[4].usage),"OFFUS_VELOCITY","mcc[4].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[5].usage,sizeof(offus_velocity->mcc[5].usage),"OFFUS_VELOCITY","mcc[5].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[6].usage,sizeof(offus_velocity->mcc[6].usage),"OFFUS_VELOCITY","mcc[6].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[7].usage,sizeof(offus_velocity->mcc[7].usage),"OFFUS_VELOCITY","mcc[7].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[8].usage,sizeof(offus_velocity->mcc[8].usage),"OFFUS_VELOCITY","mcc[8].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[9].usage,sizeof(offus_velocity->mcc[9].usage),"OFFUS_VELOCITY","mcc[9].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[10].usage,sizeof(offus_velocity->mcc[10].usage),"OFFUS_VELOCITY","mcc[10].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[11].usage,sizeof(offus_velocity->mcc[11].usage),"OFFUS_VELOCITY","mcc[11].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[12].usage,sizeof(offus_velocity->mcc[12].usage),"OFFUS_VELOCITY","mcc[12].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[13].usage,sizeof(offus_velocity->mcc[13].usage),"OFFUS_VELOCITY","mcc[13].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[14].usage,sizeof(offus_velocity->mcc[14].usage),"OFFUS_VELOCITY","mcc[14].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[15].usage,sizeof(offus_velocity->mcc[15].usage),"OFFUS_VELOCITY","mcc[15].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[16].usage,sizeof(offus_velocity->mcc[16].usage),"OFFUS_VELOCITY","mcc[16].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[17].usage,sizeof(offus_velocity->mcc[17].usage),"OFFUS_VELOCITY","mcc[17].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[18].usage,sizeof(offus_velocity->mcc[18].usage),"offus_velocity","mcc[18].usage","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->mcc[19].usage,sizeof(offus_velocity->mcc[19].usage),"OFFUS_VELOCITY","mcc[19].usage","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->repeat[0].merchant_id,sizeof(offus_velocity->repeat[0].merchant_id),"OFFUS_VELOCITY","repeat[0].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[1].merchant_id,sizeof(offus_velocity->repeat[1].merchant_id),"OFFUS_VELOCITY","repeat[1].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[2].merchant_id,sizeof(offus_velocity->repeat[2].merchant_id),"OFFUS_VELOCITY","repeat[2].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[3].merchant_id,sizeof(offus_velocity->repeat[3].merchant_id),"OFFUS_VELOCITY","repeat[3].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[4].merchant_id,sizeof(offus_velocity->repeat[4].merchant_id),"OFFUS_VELOCITY","repeat[4].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[5].merchant_id,sizeof(offus_velocity->repeat[5].merchant_id),"OFFUS_VELOCITY","repeat[5].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[6].merchant_id,sizeof(offus_velocity->repeat[6].merchant_id),"OFFUS_VELOCITY","repeat[6].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[7].merchant_id,sizeof(offus_velocity->repeat[7].merchant_id),"OFFUS_VELOCITY","repeat[7].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[8].merchant_id,sizeof(offus_velocity->repeat[8].merchant_id),"OFFUS_VELOCITY","repeat[8].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[9].merchant_id,sizeof(offus_velocity->repeat[9].merchant_id),"OFFUS_VELOCITY","repeat[9].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[10].merchant_id,sizeof(offus_velocity->repeat[10].merchant_id),"OFFUS_VELOCITY","repeat[10].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[11].merchant_id,sizeof(offus_velocity->repeat[11].merchant_id),"OFFUS_VELOCITY","repeat[11].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[12].merchant_id,sizeof(offus_velocity->repeat[12].merchant_id),"OFFUS_VELOCITY","repeat[12].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[13].merchant_id,sizeof(offus_velocity->repeat[13].merchant_id),"OFFUS_VELOCITY","repeat[13].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[14].merchant_id,sizeof(offus_velocity->repeat[14].merchant_id),"OFFUS_VELOCITY","repeat[14].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[15].merchant_id,sizeof(offus_velocity->repeat[15].merchant_id),"OFFUS_VELOCITY","repeat[15].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[16].merchant_id,sizeof(offus_velocity->repeat[16].merchant_id),"OFFUS_VELOCITY","repeat[16].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[17].merchant_id,sizeof(offus_velocity->repeat[17].merchant_id),"OFFUS_VELOCITY","repeat[17].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[18].merchant_id,sizeof(offus_velocity->repeat[18].merchant_id),"OFFUS_VELOCITY","repeat[18].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[19].merchant_id,sizeof(offus_velocity->repeat[19].merchant_id),"OFFUS_VELOCITY","repeat[19].merchant_id","CheckNULLTerminatedinOffus_velocity",mode);

	checknull(offus_velocity->repeat[0].nbr_uses,sizeof(offus_velocity->repeat[0].nbr_uses),"OFFUS_VELOCITY","repeat[0].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[1].nbr_uses,sizeof(offus_velocity->repeat[1].nbr_uses),"OFFUS_VELOCITY","repeat[1].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[2].nbr_uses,sizeof(offus_velocity->repeat[2].nbr_uses),"OFFUS_VELOCITY","repeat[2].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[3].nbr_uses,sizeof(offus_velocity->repeat[3].nbr_uses),"OFFUS_VELOCITY","repeat[3].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[4].nbr_uses,sizeof(offus_velocity->repeat[4].nbr_uses),"OFFUS_VELOCITY","repeat[4].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[5].nbr_uses,sizeof(offus_velocity->repeat[5].nbr_uses),"OFFUS_VELOCITY","repeat[5].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[6].nbr_uses,sizeof(offus_velocity->repeat[6].nbr_uses),"OFFUS_VELOCITY","repeat[6].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[7].nbr_uses,sizeof(offus_velocity->repeat[7].nbr_uses),"OFFUS_VELOCITY","repeat[7].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[8].nbr_uses,sizeof(offus_velocity->repeat[8].nbr_uses),"OFFUS_VELOCITY","repeat[8].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[9].nbr_uses,sizeof(offus_velocity->repeat[9].nbr_uses),"OFFUS_VELOCITY","repeat[9].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[10].nbr_uses,sizeof(offus_velocity->repeat[10].nbr_uses),"OFFUS_VELOCITY","repeat[10].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[11].nbr_uses,sizeof(offus_velocity->repeat[11].nbr_uses),"OFFUS_VELOCITY","repeat[11].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[12].nbr_uses,sizeof(offus_velocity->repeat[12].nbr_uses),"OFFUS_VELOCITY","repeat[12].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[13].nbr_uses,sizeof(offus_velocity->repeat[13].nbr_uses),"OFFUS_VELOCITY","repeat[13].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[14].nbr_uses,sizeof(offus_velocity->repeat[14].nbr_uses),"OFFUS_VELOCITY","repeat[14].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[15].nbr_uses,sizeof(offus_velocity->repeat[15].nbr_uses),"OFFUS_VELOCITY","repeat[15].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[16].nbr_uses,sizeof(offus_velocity->repeat[16].nbr_uses),"OFFUS_VELOCITY","repeat[16].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[17].nbr_uses,sizeof(offus_velocity->repeat[17].nbr_uses),"OFFUS_VELOCITY","repeat[17].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[18].nbr_uses,sizeof(offus_velocity->repeat[18].nbr_uses),"OFFUS_VELOCITY","repeat[18].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
	checknull(offus_velocity->repeat[19].nbr_uses,sizeof(offus_velocity->repeat[19].nbr_uses),"OFFUS_VELOCITY","repeat[19].nbr_uses","CheckNULLTerminatedinOffus_velocity",mode);
}

void CheckNULLTerminatedinCAF01_EMV(pCAF01_EMV   caf01_emv,pCHAR mode)
{
	checknull(caf01_emv->primary_key.caf_profile_id,sizeof(caf01_emv->primary_key.caf_profile_id),"CAF01_EMV","primary_key.caf_profile_id","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->primary_key.type,sizeof(caf01_emv->primary_key.type),"CAF01_EMV","primary_key.type","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->cc_tk[0].service_code_offset,sizeof(caf01_emv->cc_tk[0].service_code_offset),"CAF01_EMV","cc_tk[0].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->cc_tk[1].service_code_offset,sizeof(caf01_emv->cc_tk[1].service_code_offset),"CAF01_EMV","cc_tk[1].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->cc_tk[0].icvc_offset,sizeof(caf01_emv->cc_tk[0].icvc_offset),"CAF01_EMV","cc_tk[0].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->cc_tk[1].icvc_offset,sizeof(caf01_emv->cc_tk[1].icvc_offset),"CAF01_EMV","cc_tk[1].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->cc_tk[0].pan_seq_nbr_offset,sizeof(caf01_emv->cc_tk[0].pan_seq_nbr_offset),"CAF01_EMV","cc_tk[0].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->cc_tk[1].pan_seq_nbr_offset,sizeof(caf01_emv->cc_tk[1].pan_seq_nbr_offset),"CAF01_EMV","cc_tk[1].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->ccless_tk[0].service_code_offset,sizeof(caf01_emv->ccless_tk[0].service_code_offset),"CAF01_EMV","ccless_tk[0].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->ccless_tk[1].service_code_offset,sizeof(caf01_emv->ccless_tk[1].service_code_offset),"CAF01_EMV","ccless_tk[1].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->ccless_tk[0].icvc_offset,sizeof(caf01_emv->ccless_tk[0].icvc_offset),"CAF01_EMV","ccless_tk[0].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->ccless_tk[1].icvc_offset,sizeof(caf01_emv->ccless_tk[1].icvc_offset),"CAF01_EMV","ccless_tk[1].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->ccless_tk[0].pan_seq_nbr_offset,sizeof(caf01_emv->ccless_tk[0].pan_seq_nbr_offset),"CAF01_EMV","ccless_tk[0].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->ccless_tk[1].pan_seq_nbr_offset,sizeof(caf01_emv->ccless_tk[1].pan_seq_nbr_offset),"CAF01_EMV","ccless_tk[1].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->mcless_tk[0].service_code_offset,sizeof(caf01_emv->mcless_tk[0].service_code_offset),"CAF01_EMV","mcless_tk[0].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->mcless_tk[1].service_code_offset,sizeof(caf01_emv->mcless_tk[1].service_code_offset),"CAF01_EMV","mcless_tk[1].service_code_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->mcless_tk[0].icvc_offset,sizeof(caf01_emv->mcless_tk[0].icvc_offset),"CAF01_EMV","mcless_tk[0].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->mcless_tk[1].icvc_offset,sizeof(caf01_emv->mcless_tk[1].icvc_offset),"CAF01_EMV","mcless_tk[1].icvc_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->mcless_tk[0].pan_seq_nbr_offset,sizeof(caf01_emv->mcless_tk[0].pan_seq_nbr_offset),"CAF01_EMV","mcless_tk[0].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->mcless_tk[1].pan_seq_nbr_offset,sizeof(caf01_emv->mcless_tk[1].pan_seq_nbr_offset),"CAF01_EMV","mcless_tk[1].pan_seq_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->mcless_tk[0].atc_offset,sizeof(caf01_emv->mcless_tk[0].atc_offset),"CAF01_EMV","mcless_tk[0].atc_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->mcless_tk[1].atc_offset,sizeof(caf01_emv->mcless_tk[1].atc_offset),"CAF01_EMV","mcless_tk[1].atc_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->mcless_tk[0].unpre_nbr_offset,sizeof(caf01_emv->mcless_tk[0].unpre_nbr_offset),"CAF01_EMV","mcless_tk[0].unpre_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->mcless_tk[1].unpre_nbr_offset,sizeof(caf01_emv->mcless_tk[1].unpre_nbr_offset),"CAF01_EMV","mcless_tk[1].unpre_nbr_offset","CheckNULLTerminatedinCAF01_EMV",mode);

	checknull(caf01_emv->MC_CVC3_code,sizeof(caf01_emv->MC_CVC3_code),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->MK_AC_code,sizeof(caf01_emv->MK_AC_code),"CAF01_EMV","MK_AC_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->future_key1,sizeof(caf01_emv->future_key1),"CAF01_EMV","future_key1","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->future_key2,sizeof(caf01_emv->future_key2),"CAF01_EMV","future_key2","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->future_key3,sizeof(caf01_emv->future_key3),"CAF01_EMV","future_key3","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->future_key4,sizeof(caf01_emv->future_key4),"CAF01_EMV","future_key4","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->future_key5,sizeof(caf01_emv->future_key5),"CAF01_EMV","future_key5","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->temp1,sizeof(caf01_emv->temp1),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->temp2,sizeof(caf01_emv->temp2),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->temp3,sizeof(caf01_emv->temp3),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->temp4,sizeof(caf01_emv->temp4),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
	checknull(caf01_emv->temp5,sizeof(caf01_emv->temp5),"CAF01_EMV","MC_CVC3_code","CheckNULLTerminatedinCAF01_EMV",mode);
}

void CheckNULLTerminatedinMCC01(pMCC01   mcc01,pCHAR mode)
{
	checknull(mcc01->primary_key.low,sizeof(mcc01->primary_key.low),"MCC01","primary_key.low","CheckNULLTerminatedinMCC01",mode);
	checknull(mcc01->primary_key.high,sizeof(mcc01->primary_key.high),"MCC01","primary_key.high","CheckNULLTerminatedinMCC01",mode);
	checknull(mcc01->allow,sizeof(mcc01->allow),"MCC01","allow","CheckNULLTerminatedinMCC01",mode);
}

void CheckNULLTerminatedinOMC01(pOMC01   omc01,pCHAR mode)
{
	checknull(omc01->primary_key.merchant_id,sizeof(omc01->primary_key.merchant_id),"OMC01","primary_key.merchant_id","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.card_nbr,sizeof(omc01->primary_key.card_nbr),"OMC01","primary_key.card_nbr","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.max_repeat,sizeof(omc01->primary_key.max_repeat),"OMC01","primary_key.max_repeat","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.repeated,sizeof(omc01->primary_key.repeated),"OMC01","primary_key.repeated","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.expiry_date,sizeof(omc01->primary_key.expiry_date),"OMC01","","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.peso_amt_limit,sizeof(omc01->primary_key.peso_amt_limit),"OMC01","primary_key.peso_amt_limit","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->primary_key.usd_amt_limit,sizeof(omc01->primary_key.usd_amt_limit),"OMC01","primary_key.usd_amt_limit","CheckNULLTerminatedinOMC01",mode);

	checknull(omc01->bypass,sizeof(omc01->bypass),"OMC01","bypass","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->future_use1,sizeof(omc01->future_use1),"OMC01","future_use1","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->future_use2,sizeof(omc01->future_use2),"OMC01","future_use2","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->future_use3,sizeof(omc01->future_use3),"OMC01","future_use3","CheckNULLTerminatedinOMC01",mode);
	checknull(omc01->future_use4,sizeof(omc01->future_use4),"OMC01","future_use4","CheckNULLTerminatedinOMC01",mode);
}

void CheckNULLTerminatedinBIN02(pBIN02   bin02,pCHAR mode)
{
	checknull(bin02->primary_key.length,sizeof(bin02->primary_key.length),"BIN02","primary_key.length","CheckNULLTerminatedinBIN02",mode);
	checknull(bin02->primary_key.lo,sizeof(bin02->primary_key.lo),"BIN02","primary_key.lo","CheckNULLTerminatedinBIN02",mode);

	checknull(bin02->high,sizeof(bin02->high),"BIN02","high","CheckNULLTerminatedinBIN02",mode);
	checknull(bin02->pan_length,sizeof(bin02->pan_length),"BIN02","pan_length","CheckNULLTerminatedinBIN02",mode);
	checknull(bin02->destination_name,sizeof(bin02->destination_name),"BIN02","destination_name","CheckNULLTerminatedinBIN02",mode);
	checknull(bin02->description,sizeof(bin02->description),"BIN02","description","CheckNULLTerminatedinBIN02",mode);
}

void CheckNULLTerminatedinUAG01(pUAG01   uag01,pCHAR mode)
{
	checknull(uag01->auth_id,sizeof(uag01->auth_id),"UAG01","auth_id","CheckNULLTerminatedinUAG01",mode);
}

void CheckNULLTerminatedinTLF01(pTLF01   tlf01,pCHAR mode)
{
	checknull(tlf01->primary_key.transaction_id,sizeof(tlf01->primary_key.transaction_id),"TLF01","primary_key.transaction_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->entry_type,sizeof(tlf01->entry_type),"TLF01","entry_type","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tpdu_id,sizeof(tlf01->tpdu_id),"TLF01","tpdu_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->issuer_id,sizeof(tlf01->issuer_id),"TLF01","issuer_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->acquirer_id,sizeof(tlf01->acquirer_id),"TLF01","acquirer_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tx_key,sizeof(tlf01->tx_key),"TLF01","tx_key","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->terminal_id,sizeof(tlf01->terminal_id),"TLF01","terminal_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->terminal_type,sizeof(tlf01->terminal_type),"TLF01","terminal_type","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->response_text,sizeof(tlf01->response_text),"TLF01","response_text","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->card_num_len,sizeof(tlf01->card_num_len),"TLF01","card_num_len","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->card_num,sizeof(tlf01->card_num),"TLF01","card_num","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->exp_date,sizeof(tlf01->exp_date),"TLF01","exp_date","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->total_amount,sizeof(tlf01->total_amount),"TLF01","total_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->add_amounts,sizeof(tlf01->add_amounts),"TLF01","add_amounts","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->invoice_number,sizeof(tlf01->invoice_number),"TLF01","invoice_number","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->orig_amount,sizeof(tlf01->orig_amount),"TLF01","orig_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->auth_number,sizeof(tlf01->auth_number),"TLF01","auth_number","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->processing_code,sizeof(tlf01->processing_code),"TLF01","processing_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->sys_trace_audit_num,sizeof(tlf01->sys_trace_audit_num),"TLF01","sys_trace_audit_num","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->orig_sys_trace_num,sizeof(tlf01->orig_sys_trace_num),"TLF01","orig_sys_trace_num","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->pos_entry_mode,sizeof(tlf01->pos_entry_mode),"TLF01","pos_entry_mode","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->nii,sizeof(tlf01->nii),"TLF01","nii","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->pos_condition_code,sizeof(tlf01->pos_condition_code),"TLF01","pos_condition_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->track1,sizeof(tlf01->track1),"TLF01","track1","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->track2,sizeof(tlf01->track2),"TLF01","track2","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->retrieval_ref_num,sizeof(tlf01->retrieval_ref_num),"TLF01","retrieval_ref_num","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->orig_retrieval_ref_num,sizeof(tlf01->orig_retrieval_ref_num),"TLF01","orig_retrieval_ref_num","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->merchant_id,sizeof(tlf01->merchant_id),"TLF01","merchant_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->orig_message,sizeof(tlf01->orig_message),"TLF01","orig_message","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->settlement_total,sizeof(tlf01->settlement_total),"TLF01","settlement_total","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->message_type,sizeof(tlf01->message_type),"TLF01","message_type","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->card_holder_name,sizeof(tlf01->card_holder_name),"TLF01","card_holder_name","CheckNULLTerminatedinTLF01",mode);

	checknull((char *)tlf01->general_status,sizeof(tlf01->general_status),"TLF01","general_status","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->originator_queue,sizeof(tlf01->originator_queue),"TLF01","originator_queue","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->originator_info,sizeof(tlf01->originator_info),"TLF01","originator_info","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->batch_number,sizeof(tlf01->batch_number),"TLF01","batch_number","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->response_code,sizeof(tlf01->response_code),"TLF01","response_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->date_yyyymmdd,sizeof(tlf01->date_yyyymmdd),"TLF01","date_yyyymmdd","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->time_hhmmss,sizeof(tlf01->time_hhmmss),"TLF01","time_hhmmss","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->num_sales,sizeof(tlf01->num_sales),"TLF01","num_sales","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->sales_amount,sizeof(tlf01->sales_amount),"TLF01","sales_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->num_refunds,sizeof(tlf01->num_refunds),"TLF01","sales_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->refund_amount,sizeof(tlf01->refund_amount),"TLF01","refund_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->num_dbsales,sizeof(tlf01->num_dbsales),"TLF01","num_dbsales","CheckNULLTerminatedinTLF01",mode);

	checknull((char *)tlf01->dbsales_amount,sizeof(tlf01->dbsales_amount),"TLF01","dbsales_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->num_dbrefunds,sizeof(tlf01->num_dbrefunds),"TLF01","num_dbrefunds","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->dbrefund_amount,sizeof(tlf01->dbrefund_amount),"TLF01","dbrefund_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->pin_block,sizeof(tlf01->pin_block),"TLF01","pin_block","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tax_amount,sizeof(tlf01->tax_amount),"TLF01","tax_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tip_amount,sizeof(tlf01->tip_amount),"TLF01","tip_amount","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tran_start_time,sizeof(tlf01->tran_start_time),"TLF01","tran_start_time","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->host_start_time,sizeof(tlf01->host_start_time),"TLF01","host_start_time","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->host_finish_time,sizeof(tlf01->host_finish_time),"TLF01","host_finish_time","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->tran_finish_time,sizeof(tlf01->tran_finish_time),"TLF01","tran_finish_time","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->settle_file_prefix,sizeof(tlf01->settle_file_prefix),"TLF01","settle_file_prefix","CheckNULLTerminatedinTLF01",mode);

	checknull((char *)tlf01->resp_source_len,sizeof(tlf01->resp_source_len),"TLF01","resp_source_len","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->visa_char_indicator,sizeof(tlf01->visa_char_indicator),"TLF01","visa_char_indicator","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->visa_tran_id,sizeof(tlf01->visa_tran_id),"TLF01","visa_tran_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->visa_validation_code,sizeof(tlf01->visa_validation_code),"TLF01","visa_validation_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->mcard_banknet_len,sizeof(tlf01->mcard_banknet_len),"TLF01","mcard_banknet_len","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->mcard_banknet,sizeof(tlf01->mcard_banknet),"TLF01","mcard_banknet","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->agent_id_len,sizeof(tlf01->agent_id_len),"TLF01","agent_id_len","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->agent_id,sizeof(tlf01->agent_id),"TLF01","agent_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->cvc,sizeof(tlf01->cvc),"TLF01","cvc","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->settlement_date,sizeof(tlf01->settlement_date),"TLF01","settlement_date","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->category_code,sizeof(tlf01->category_code),"TLF01","category_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->conversion_rate,sizeof(tlf01->conversion_rate),"TLF01","conversion_rate","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->product_code,sizeof(tlf01->product_code),"TLF01","product_code","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->odometer,sizeof(tlf01->odometer),"TLF01","odometer","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->organization_id,sizeof(tlf01->organization_id),"TLF01","organization_id","CheckNULLTerminatedinTLF01",mode);

	checknull((char *)tlf01->dynamic_keychange,sizeof(tlf01->dynamic_keychange),"TLF01","dynamic_keychange","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->source_key,sizeof(tlf01->source_key),"TLF01","source_key","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->txn_cnt,sizeof(tlf01->txn_cnt),"TLF01","txn_cnt","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->dcf01_retired_cwk,sizeof(tlf01->dcf01_retired_cwk),"TLF01","dcf01_retired_cwk","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->dest_key,sizeof(tlf01->dest_key),"TLF01","dest_key","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->ncf01_retired_cwk,sizeof(tlf01->ncf01_retired_cwk),"TLF01","ncf01_retired_cwk","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->handler_queue,sizeof(tlf01->handler_queue),"TLF01","handler_queue","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->authorizing_host_queue,sizeof(tlf01->authorizing_host_queue),"TLF01","authorizing_host_queue","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->max_net_consec_tmouts,sizeof(tlf01->max_net_consec_tmouts),"TLF01","max_net_consec_tmouts","CheckNULLTerminatedinTLF01",mode);

	checknull((char *)tlf01->max_active_txns,sizeof(tlf01->max_active_txns),"TLF01","max_active_txns","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->forwarding_institution_id,sizeof(tlf01->forwarding_institution_id),"TLF01","forwarding_institution_id","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->remote_nsp_type,sizeof(tlf01->remote_nsp_type),"TLF01","remote_nsp_type","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->kek,sizeof(tlf01->kek),"TLF01","kek","CheckNULLTerminatedinTLF01",mode);
	checknull((char *)tlf01->acquiring_id,sizeof(tlf01->acquiring_id),"TLF01","acquiring_id","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->outstanding_balance,sizeof(tlf01->outstanding_balance),"TLF01","outstanding_balance","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->credit_limit,sizeof(tlf01->credit_limit),"TLF01","credit_limit","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->nfi_name,sizeof(tlf01->nfi_name),"TLF01","nfi_name","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->nfi_seq_nbr,sizeof(tlf01->nfi_seq_nbr),"TLF01","nfi_seq_nbr","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->nfi_start_date,sizeof(tlf01->nfi_start_date),"TLF01","nfi_start_date","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->nfi_end_date,sizeof(tlf01->nfi_end_date),"TLF01","nfi_end_date","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->resp_source,sizeof(tlf01->resp_source),"TLF01","resp_source","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->dispensation.origin,sizeof(tlf01->dispensation.origin),"TLF01","dispensation.origin","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->dispensation.auth_1,sizeof(tlf01->dispensation.auth_1),"TLF01","dispensation.auth_1","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->dispensation.auth_2,sizeof(tlf01->dispensation.auth_2),"TLF01","dispensation.auth_2","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->update_mask,sizeof(tlf01->update_mask),"TLF01","update_mask","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->type_of_data,sizeof(tlf01->type_of_data),"TLF01","type_of_data","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->down_payment,sizeof(tlf01->down_payment),"TLF01","down_payment","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->period,sizeof(tlf01->period),"TLF01","period","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->interest_rate,sizeof(tlf01->interest_rate),"TLF01","interest_rate","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->total_interest,sizeof(tlf01->total_interest),"TLF01","total_interest","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->redemption_amount,sizeof(tlf01->redemption_amount),"TLF01","redemption_amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->vehicle_number,sizeof(tlf01->vehicle_number),"TLF01","vehicle_number","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->profile,sizeof(tlf01->profile),"TLF01","profile","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->nbr_of_prod_codes,sizeof(tlf01->nbr_of_prod_codes),"TLF01","nbr_of_prod_codes","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->product_codes[0].code,sizeof(tlf01->product_codes[0].code),"TLF01","product_codes[0].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[1].code,sizeof(tlf01->product_codes[1].code),"TLF01","product_codes[1].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[2].code,sizeof(tlf01->product_codes[2].code),"TLF01","product_codes[2].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[3].code,sizeof(tlf01->product_codes[3].code),"TLF01","product_codes[3].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[4].code,sizeof(tlf01->product_codes[4].code),"TLF01","product_codes[4].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[5].code,sizeof(tlf01->product_codes[5].code),"TLF01","product_codes[5].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[6].code,sizeof(tlf01->product_codes[6].code),"TLF01","product_codes[6].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[7].code,sizeof(tlf01->product_codes[7].code),"TLF01","product_codes[7].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[8].code,sizeof(tlf01->product_codes[8].code),"TLF01","product_codes[8].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[9].code,sizeof(tlf01->product_codes[9].code),"TLF01","product_codes[9].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[10].code,sizeof(tlf01->product_codes[10].code),"TLF01","product_codes[10].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[11].code,sizeof(tlf01->product_codes[11].code),"TLF01","product_codes[11].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[12].code,sizeof(tlf01->product_codes[12].code),"TLF01","product_codes[12].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[13].code,sizeof(tlf01->product_codes[13].code),"TLF01","product_codes[13].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[14].code,sizeof(tlf01->product_codes[14].code),"TLF01","product_codes[14].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[15].code,sizeof(tlf01->product_codes[15].code),"TLF01","product_codes[15].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[16].code,sizeof(tlf01->product_codes[16].code),"TLF01","product_codes[16].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[17].code,sizeof(tlf01->product_codes[17].code),"TLF01","product_codes[17].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[18].code,sizeof(tlf01->product_codes[18].code),"TLF01","product_codes[18].code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[19].code,sizeof(tlf01->product_codes[19].code),"TLF01","product_codes[19].code","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->product_codes[0].quantity,sizeof(tlf01->product_codes[0].quantity),"TLF01","product_codes[0].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[1].quantity,sizeof(tlf01->product_codes[1].quantity),"TLF01","product_codes[1].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[2].quantity,sizeof(tlf01->product_codes[2].quantity),"TLF01","product_codes[2].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[3].quantity,sizeof(tlf01->product_codes[3].quantity),"TLF01","product_codes[3].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[4].quantity,sizeof(tlf01->product_codes[4].quantity),"TLF01","product_codes[4].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[5].quantity,sizeof(tlf01->product_codes[5].quantity),"TLF01","product_codes[5].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[6].quantity,sizeof(tlf01->product_codes[6].quantity),"TLF01","product_codes[6].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[7].quantity,sizeof(tlf01->product_codes[7].quantity),"TLF01","product_codes[7].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[8].quantity,sizeof(tlf01->product_codes[8].quantity),"TLF01","product_codes[8].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[9].quantity,sizeof(tlf01->product_codes[9].quantity),"TLF01","product_codes[9].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[10].quantity,sizeof(tlf01->product_codes[10].quantity),"TLF01","product_codes[10].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[11].quantity,sizeof(tlf01->product_codes[11].quantity),"TLF01","product_codes[11].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[12].quantity,sizeof(tlf01->product_codes[12].quantity),"TLF01","product_codes[12].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[13].quantity,sizeof(tlf01->product_codes[13].quantity),"TLF01","product_codes[13].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[14].quantity,sizeof(tlf01->product_codes[14].quantity),"TLF01","product_codes[14].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[15].quantity,sizeof(tlf01->product_codes[15].quantity),"TLF01","product_codes[15].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[16].quantity,sizeof(tlf01->product_codes[16].quantity),"TLF01","product_codes[16].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[17].quantity,sizeof(tlf01->product_codes[17].quantity),"TLF01","product_codes[17].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[18].quantity,sizeof(tlf01->product_codes[18].quantity),"TLF01","product_codes[18].quantity","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[19].quantity,sizeof(tlf01->product_codes[19].quantity),"TLF01","product_codes[19].quantity","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->product_codes[0].amount,sizeof(tlf01->product_codes[0].amount),"TLF01","product_codes[0].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[1].amount,sizeof(tlf01->product_codes[1].amount),"TLF01","product_codes[1].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[2].amount,sizeof(tlf01->product_codes[2].amount),"TLF01","product_codes[2].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[3].amount,sizeof(tlf01->product_codes[3].amount),"TLF01","product_codes[3].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[4].amount,sizeof(tlf01->product_codes[4].amount),"TLF01","product_codes[4].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[5].amount,sizeof(tlf01->product_codes[5].amount),"TLF01","product_codes[5].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[6].amount,sizeof(tlf01->product_codes[6].amount),"TLF01","product_codes[6].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[7].amount,sizeof(tlf01->product_codes[7].amount),"TLF01","product_codes[7].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[8].amount,sizeof(tlf01->product_codes[8].amount),"TLF01","product_codes[8].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[9].amount,sizeof(tlf01->product_codes[9].amount),"TLF01","product_codes[9].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[10].amount,sizeof(tlf01->product_codes[10].amount),"TLF01","product_codes[10].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[11].amount,sizeof(tlf01->product_codes[11].amount),"TLF01","product_codes[11].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[12].amount,sizeof(tlf01->product_codes[12].amount),"TLF01","product_codes[12].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[13].amount,sizeof(tlf01->product_codes[13].amount),"TLF01","product_codes[13].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[14].amount,sizeof(tlf01->product_codes[14].amount),"TLF01","product_codes[14].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[15].amount,sizeof(tlf01->product_codes[15].amount),"TLF01","product_codes[15].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[16].amount,sizeof(tlf01->product_codes[16].amount),"TLF01","product_codes[16].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[17].amount,sizeof(tlf01->product_codes[17].amount),"TLF01","product_codes[17].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[18].amount,sizeof(tlf01->product_codes[18].amount),"TLF01","product_codes[18].amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->product_codes[19].amount,sizeof(tlf01->product_codes[19].amount),"TLF01","product_codes[19].amount","CheckNULLTerminatedinTLF01",mode);

	checknull(tlf01->visa_merchant_id,sizeof(tlf01->visa_merchant_id),"TLF01","visa_merchant_id","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->cash_bonus,sizeof(tlf01->cash_bonus),"TLF01","cash_bonus","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->deferred_factor,sizeof(tlf01->deferred_factor),"TLF01","deferred_factor","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->deferred_term_length,sizeof(tlf01->deferred_term_length),"TLF01","deferred_term_length","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->currency_code,sizeof(tlf01->currency_code),"TLF01","currency_code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->transmission_timestamp,sizeof(tlf01->transmission_timestamp),"TLF01","transmission_timestamp","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->tran_fee_amount,sizeof(tlf01->tran_fee_amount),"TLF01","tran_fee_amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->verify_mag_stripe,sizeof(tlf01->verify_mag_stripe),"TLF01","verify_mag_stripe","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->saf,sizeof(tlf01->saf),"CheckNULLTerminatedinTLF01","TLF01","saf",mode);
	checknull(tlf01->security_response_code,sizeof(tlf01->security_response_code),"TLF01","security_response_code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->service_code,sizeof(tlf01->service_code),"TLF01","service_code","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->source_pin_format,sizeof(tlf01->source_pin_format),"TLF01","source_pin_format","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->dest_pin_format,sizeof(tlf01->dest_pin_format),"TLF01","dest_pin_format","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->voice_auth,sizeof(tlf01->voice_auth),"TLF01","voice_auth","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->actual_amount,sizeof(tlf01->actual_amount),"TLF01","actual_amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->reversal_amount,sizeof(tlf01->reversal_amount),"TLF01","reversal_amount","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->voice_auth_text,sizeof(tlf01->voice_auth_text),"TLF01","voice_auth_text","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->operator_id,sizeof(tlf01->operator_id),"TLF01","operator_id","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->workstation,sizeof(tlf01->workstation),"TLF01","workstation","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->orig_auth_number,sizeof(tlf01->orig_auth_number),"TLF01","orig_auth_number","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->ticket_nbr,sizeof(tlf01->ticket_nbr),"TLF01","ticket_nbr","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->system_date,sizeof(tlf01->system_date),"TLF01","system_date","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->def_gross_amt,sizeof(tlf01->def_gross_amt),"TLF01","def_gross_amt","CheckNULLTerminatedinTLF01",mode);
	checknull(tlf01->monthly_amort_amt,sizeof(tlf01->monthly_amort_amt),"TLF01","monthly_amort_amt","CheckNULLTerminatedinTLF01",mode);
}


void CheckNULLTerminatedinDCF01(pDCF01   dcf01,pCHAR mode)
{
	checknull(dcf01->primary_key.device_id,sizeof(dcf01->primary_key.device_id),"DCF01","primary_key.device_id","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->short_name,sizeof(dcf01->short_name),"DCF01","short_name","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->phone,sizeof(dcf01->phone),"DCF01","phone","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->industry_code,sizeof(dcf01->industry_code),"DCF01","industry_code","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->category_code,sizeof(dcf01->category_code),"DCF01","category_code","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->contact,sizeof(dcf01->contact),"DCF01","contact","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->device_type,sizeof(dcf01->device_type),"DCF01","device_type","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->date_added,sizeof(dcf01->date_added),"DCF01","date_added","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->current_bus_date,sizeof(dcf01->current_bus_date),"DCF01","current_bus_date","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->dept_nbr,sizeof(dcf01->dept_nbr),"DCF01","dept_nbr","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->organization_id,sizeof(dcf01->organization_id),"DCF01","merchant_id","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->merchant_id,sizeof(dcf01->merchant_id),"DCF01","merchant_id","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->aba_nbr,sizeof(dcf01->aba_nbr),"DCF01","aba_nbr","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->status,sizeof(dcf01->status),"DCF01","status","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->totals_ind,sizeof(dcf01->totals_ind),"DCF01","totals_ind","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->statistics_ind,sizeof(dcf01->statistics_ind),"DCF01","statistics_ind","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->valid_transactions,sizeof(dcf01->valid_transactions),"DCF01","valid_transactions","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->draft_capture_type,sizeof(dcf01->draft_capture_type),"DCF01","draft_capture_type","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->amex_charge_desc,sizeof(dcf01->amex_charge_desc),"DCF01","amex_charge_desc","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->diners_charge_desc,sizeof(dcf01->diners_charge_desc),"DCF01","diners_charge_desc","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->additional_pos_information,sizeof(dcf01->additional_pos_information),"DCF01","additional_pos_information","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->batch_nbr,sizeof(dcf01->batch_nbr),"DCF01","batch_nbr","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->rental_program,sizeof(dcf01->rental_program),"DCF01","rental_program","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->nsp_process,sizeof(dcf01->nsp_process),"DCF01","nsp_process","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->pin_type,sizeof(dcf01->pin_type),"DCF01","pin_type","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->kc,sizeof(dcf01->kc),"DCF01","kc","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->encr_type,sizeof(dcf01->encr_type),"DCF01","encr_type","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->pad_char,sizeof(dcf01->pad_char),"DCF01","pad_char","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->keyxchange_cnt,sizeof(dcf01->keyxchange_cnt),"DCF01","keyxchange_cnt","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->txn_cnt,sizeof(dcf01->txn_cnt),"DCF01","txn_cnt","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->cwk,sizeof(dcf01->cwk),"DCF01","cwk","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->kek,sizeof(dcf01->kek),"DCF01","kek","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->retired_cwk,sizeof(dcf01->retired_cwk),"DCF01","retired_cwk","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->icv,sizeof(dcf01->icv),"DCF01","icv","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->pin_block_format,sizeof(dcf01->pin_block_format),"DCF01","pin_block_format","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->encryption_method,sizeof(dcf01->encryption_method),"DCF01","encryption_method","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->pin_key,sizeof(dcf01->pin_key),"DCF01","pin_key","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->communication_key,sizeof(dcf01->communication_key),"DCF01","communication_key","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_init_activation,sizeof(dcf01->auto_init_activation),"DCF01","auto_init_activation","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_init_time,sizeof(dcf01->auto_init_time),"DCF01","auto_init_time","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_init_ref_number,sizeof(dcf01->auto_init_ref_number),"DCF01","auto_init_ref_number","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_program_load_activation,sizeof(dcf01->auto_program_load_activation),"DCF01","auto_program_load_activation","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_program_load_time,sizeof(dcf01->auto_program_load_time),"DCF01","auto_program_load_time","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_program_load_ref_number,sizeof(dcf01->auto_program_load_ref_number),"DCF01","auto_program_load_ref_number","CheckNULLTerminatedinDCF01",mode);
	checknull(dcf01->auto_icepac_activation,sizeof(dcf01->auto_icepac_activation),"DCF01","auto_icepac_activation","CheckNULLTerminatedinDCF01",mode);
}

void CheckNULLTerminatedinBCH01(pBCH01   bch01,pCHAR mode)
{
	checknull(bch01->primary_key.device_id,sizeof(bch01->primary_key.device_id),"BCH01","primary_key.device_id","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->primary_key.batch_nbr,sizeof(bch01->primary_key.batch_nbr),"BCH01","primary_key.batch_nbr","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->merchant_id,sizeof(bch01->merchant_id),"BCH01","merchant_id","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_credit_cnt,sizeof(bch01->total_credit_cnt),"BCH01","total_credit_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_credit_amt,sizeof(bch01->total_credit_amt),"BCH01","total_credit_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_credit_refund_cnt,sizeof(bch01->total_credit_refund_cnt),"BCH01","total_credit_refund_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_credit_refund_amt,sizeof(bch01->total_credit_refund_amt),"BCH01","total_credit_refund_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_debit_cnt,sizeof(bch01->total_debit_cnt),"BCH01","total_debit_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_debit_amt,sizeof(bch01->total_debit_amt),"BCH01","total_debit_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_debit_refund_cnt,sizeof(bch01->total_debit_refund_cnt),"BCH01","total_debit_refund_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_debit_refund_amt,sizeof(bch01->total_debit_refund_amt),"BCH01","total_debit_refund_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_void_credit_cnt,sizeof(bch01->total_void_credit_cnt),"BCH01","total_void_credit_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_void_credit_amt,sizeof(bch01->total_void_credit_amt),"BCH01","total_void_credit_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_void_credit_refund_cnt,sizeof(bch01->total_void_credit_refund_cnt),"BCH01","total_void_credit_refund_cnt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->total_void_credit_refund_amt,sizeof(bch01->total_void_credit_refund_amt),"BCH01","total_void_credit_refund_amt","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->open_date,sizeof(bch01->open_date),"BCH01","open_date","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->open_time,sizeof(bch01->open_time),"BCH01","open_time","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->release_date,sizeof(bch01->release_date),"BCH01","release_date","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->release_time,sizeof(bch01->release_time),"BCH01","release_time","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->batch_status,sizeof(bch01->batch_status),"BCH01","batch_status","CheckNULLTerminatedinBCH01",mode);
	checknull(bch01->visa_merchant_id,sizeof(bch01->visa_merchant_id),"BCH01","visa_merchant_id","CheckNULLTerminatedinBCH01",mode);
}

void CheckNULLTerminatedinBCH10(pBCH10   bch10,pCHAR mode)
{
	checknull(bch10->primary_key.device_id,sizeof(bch10->primary_key.device_id),"BCH10","primary_key.device_id","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->primary_key.batch_nbr,sizeof(bch10->primary_key.batch_nbr),"BCH10","primary_key.batch_nbr","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->primary_key.retrieval_ref_num,sizeof(bch10->primary_key.retrieval_ref_num),"BCH10","primary_key.retrieval_ref_num","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->merchant_id,sizeof(bch10->merchant_id),"BCH10","merchant_id","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->processing_code,sizeof(bch10->processing_code),"BCH10","processing_code","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->tran_amount,sizeof(bch10->tran_amount),"BCH10","tran_amount","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->response_code,sizeof(bch10->response_code),"BCH10","response_code","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->auth_number,sizeof(bch10->auth_number),"BCH10","auth_number","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->tran_date,sizeof(bch10->tran_date),"BCH10","tran_date","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->tran_time,sizeof(bch10->tran_time),"BCH10","tran_time","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->transaction_id,sizeof(bch10->transaction_id),"BCH10","transaction_id","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->message_type,sizeof(bch10->message_type),"BCH10","message_type","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->card_nbr,sizeof(bch10->card_nbr),"BCH10","card_nbr","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->invoice_nbr,sizeof(bch10->invoice_nbr),"BCH10","invoice_nbr","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->settle_file_prefix,sizeof(bch10->settle_file_prefix),"BCH10","settle_file_prefix","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->product_code,sizeof(bch10->product_code),"BCH10","product_code","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->odometer,sizeof(bch10->odometer),"BCH10","odometer","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->sys_trace_audit_num,sizeof(bch10->sys_trace_audit_num),"BCH10","sys_trace_audit_num","CheckNULLTerminatedinBCH10",mode);
	checknull((char *)bch10->tx_key,sizeof(bch10->tx_key),"BCH10","tx_key","CheckNULLTerminatedinBCH10",mode);
	checknull((char *)bch10->bin_type,sizeof(bch10->bin_type),"BCH10","bin_type","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->type_of_data,sizeof(bch10->type_of_data),"BCH10","type_of_data","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->down_payment,sizeof(bch10->down_payment),"BCH10","down_payment","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->period,sizeof(bch10->period),"BCH10","period","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->interest_rate,sizeof(bch10->interest_rate),"BCH10","interest_rate","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->total_interest,sizeof(bch10->total_interest),"BCH10","total_interest","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->redemption_amount,sizeof(bch10->redemption_amount),"BCH10","redemption_amount","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->vehicle_number,sizeof(bch10->vehicle_number),"BCH10","vehicle_number","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->pos_entry_mode,sizeof(bch10->pos_entry_mode),"BCH10","pos_entry_mode","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->profile,sizeof(bch10->profile),"BCH10","profile","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->nbr_of_prod_codes,sizeof(bch10->nbr_of_prod_codes),"BCH10","nbr_of_prod_codes","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->aportionment_flag,sizeof(bch10->aportionment_flag),"BCH10","aportionment_flag","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->exp_date,sizeof(bch10->exp_date),"BCH10","exp_date","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->open_date,sizeof(bch10->open_date),"BCH10","open_date","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->open_time,sizeof(bch10->open_time),"BCH10","open_time","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->cash_bonus,sizeof(bch10->cash_bonus),"BCH10","cash_bonus","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->transmission_timestamp,sizeof(bch10->transmission_timestamp),"BCH10","transmission_timestamp","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->mcard_banknet,sizeof(bch10->mcard_banknet),"BCH10","mcard_banknet","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->ticket_nbr,sizeof(bch10->ticket_nbr),"BCH10","ticket_nbr","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->def_gross_amt,sizeof(bch10->def_gross_amt),"BCH10","def_gross_amt","CheckNULLTerminatedinBCH10",mode);
	checknull(bch10->term,sizeof(bch10->term),"BCH10","term","CheckNULLTerminatedinBCH10",mode);
}

void CheckNULLTerminatedinOCF01(pOCF01   ocf01,pCHAR mode)
{
	checknull(ocf01->primary_key.organization_id,sizeof(ocf01->primary_key.organization_id),"OCF01","primary_key.organization_id","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->name01,sizeof(ocf01->name01),"OCF01","name01","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->addr01,sizeof(ocf01->addr01),"OCF01","addr01","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->addr02,sizeof(ocf01->addr02),"OCF01","addr02","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->city,sizeof(ocf01->city),"OCF01","city","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->state,sizeof(ocf01->state),"OCF01","state","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->zip_code,sizeof(ocf01->zip_code),"OCF01","zip_code","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->intl_code,sizeof(ocf01->intl_code),"OCF01","intl_code","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->contact,sizeof(ocf01->contact),"OCF01","contact","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->phone01,sizeof(ocf01->phone01),"OCF01","phone01","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->phone02,sizeof(ocf01->phone02),"OCF01","phone02","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->date_added,sizeof(ocf01->date_added),"OCF01","date_added","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->status,sizeof(ocf01->status),"OCF01","status","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->totals_ind,sizeof(ocf01->totals_ind),"OCF01","totals_ind","CheckNULLTerminatedinOCF01",mode);
	checknull(ocf01->statistics_ind,sizeof(ocf01->statistics_ind),"OCF01","statistics_ind","CheckNULLTerminatedinOCF01",mode);
}

void CheckNULLTerminatedinOCF20(pOCF20   ocf20,pCHAR mode)
{
	checknull(ocf20->primary_key.yyyymmdd,sizeof(ocf20->primary_key.yyyymmdd),"OCF20","primary_key.yyyymmdd","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->primary_key.organization_id,sizeof(ocf20->primary_key.organization_id),"OCF20","primary_key.organization_id","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->ytd_total_tran_cnt,sizeof(ocf20->ytd_total_tran_cnt),"OCF20","ytd_total_tran_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->ytd_total_tran_amt,sizeof(ocf20->ytd_total_tran_amt),"OCF20","ytd_total_tran_amt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->mtd_total_tran_cnt,sizeof(ocf20->mtd_total_tran_cnt),"OCF20","mtd_total_tran_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->mtd_total_tran_amt,sizeof(ocf20->mtd_total_tran_amt),"OCF20","mtd_total_tran_amt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->tran_cnt,sizeof(ocf20->tran_cnt),"OCF20","tran_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->tran_amt,sizeof(ocf20->tran_amt),"OCF20","tran_amt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->tran_approved_cnt,sizeof(ocf20->tran_approved_cnt),"OCF20","tran_approved_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->tran_declined_cnt,sizeof(ocf20->tran_declined_cnt),"OCF20","tran_declined_cnt","CheckNULLTerminatedinOCF20",mode);

	checknull(ocf20->decline[0].category_code,sizeof(ocf20->decline[0].category_code),"OCF20","decline[0].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[0].category_cnt,sizeof(ocf20->decline[0].category_cnt),"OCF20","decline[0].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[1].category_code,sizeof(ocf20->decline[1].category_code),"OCF20","decline[1].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[1].category_cnt,sizeof(ocf20->decline[1].category_cnt),"OCF20","decline[1].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[2].category_code,sizeof(ocf20->decline[2].category_code),"OCF20","decline[2].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[2].category_cnt,sizeof(ocf20->decline[2].category_cnt),"OCF20","decline[2].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[3].category_code,sizeof(ocf20->decline[3].category_code),"OCF20","decline[3].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[3].category_cnt,sizeof(ocf20->decline[3].category_cnt),"OCF20","decline[3].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[4].category_code,sizeof(ocf20->decline[4].category_code),"OCF20","decline[4].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[4].category_cnt,sizeof(ocf20->decline[4].category_cnt),"OCF20","decline[4].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[5].category_code,sizeof(ocf20->decline[5].category_code),"OCF20","decline[5].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[5].category_cnt,sizeof(ocf20->decline[5].category_cnt),"OCF20","decline[5].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[6].category_code,sizeof(ocf20->decline[6].category_code),"OCF20","decline[6].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[6].category_cnt,sizeof(ocf20->decline[6].category_cnt),"OCF20","decline[6].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[7].category_code,sizeof(ocf20->decline[7].category_code),"OCF20","decline[7].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[7].category_cnt,sizeof(ocf20->decline[7].category_cnt),"OCF20","decline[7].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[8].category_code,sizeof(ocf20->decline[8].category_code),"OCF20","decline[8].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[8].category_cnt,sizeof(ocf20->decline[8].category_cnt),"OCF20","decline[8].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[9].category_code,sizeof(ocf20->decline[9].category_code),"OCF20","decline[9].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[9].category_cnt,sizeof(ocf20->decline[9].category_cnt),"OCF20","decline[9].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[10].category_code,sizeof(ocf20->decline[10].category_code),"OCF20","decline[10].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[10].category_cnt,sizeof(ocf20->decline[10].category_cnt),"OCF20","decline[10].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[11].category_code,sizeof(ocf20->decline[11].category_code),"OCF20","decline[11].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[11].category_cnt,sizeof(ocf20->decline[11].category_cnt),"OCF20","decline[11].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[12].category_code,sizeof(ocf20->decline[12].category_code),"OCF20","decline[12].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[12].category_cnt,sizeof(ocf20->decline[12].category_cnt),"OCF20","decline[12].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[13].category_code,sizeof(ocf20->decline[13].category_code),"OCF20","decline[13].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[13].category_cnt,sizeof(ocf20->decline[13].category_cnt),"OCF20","decline[13].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[14].category_code,sizeof(ocf20->decline[14].category_code),"OCF20","decline[14].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[14].category_cnt,sizeof(ocf20->decline[14].category_cnt),"OCF20","decline[14].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[15].category_code,sizeof(ocf20->decline[15].category_code),"OCF20","decline[15].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[15].category_cnt,sizeof(ocf20->decline[15].category_cnt),"OCF20","decline[15].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[16].category_code,sizeof(ocf20->decline[16].category_code),"OCF20","decline[16].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[16].category_cnt,sizeof(ocf20->decline[16].category_cnt),"OCF20","decline[16].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[17].category_code,sizeof(ocf20->decline[17].category_code),"OCF20","decline[17].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[17].category_cnt,sizeof(ocf20->decline[17].category_cnt),"OCF20","decline[17].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[18].category_code,sizeof(ocf20->decline[18].category_code),"OCF20","decline[18].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[18].category_cnt,sizeof(ocf20->decline[18].category_cnt),"OCF20","decline[18].category_cnt","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[19].category_code,sizeof(ocf20->decline[19].category_code),"OCF20","decline[19].category_code","CheckNULLTerminatedinOCF20",mode);
	checknull(ocf20->decline[19].category_cnt,sizeof(ocf20->decline[19].category_cnt),"OCF20","decline[19].category_cnt","CheckNULLTerminatedinOCF20",mode);
}

void CheckNULLTerminatedinMCF20(pMCF20   mcf20,pCHAR mode)
{
	checknull(mcf20->primary_key.organization_id,sizeof(mcf20->primary_key.organization_id),"MCF20","primary_key.organization_id","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->primary_key.merchant_id,sizeof(mcf20->primary_key.merchant_id),"MCF20","primary_key.merchant_id","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->primary_key.yyyymmdd,sizeof(mcf20->primary_key.yyyymmdd),"MCF20","primary_key.yyyymmdd","CheckNULLTerminatedinMCF20",mode);

	checknull(mcf20->ytd_total_adds,sizeof(mcf20->ytd_total_adds),"MCF20","ytd_total_adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->ytd_total_deletes,sizeof(mcf20->ytd_total_deletes),"MCF20","ytd_total_deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->mtd_total_adds,sizeof(mcf20->mtd_total_adds),"MCF20","mtd_total_adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->mtd_total_deletes,sizeof(mcf20->mtd_total_deletes),"MCF20","mtd_total_deletes","CheckNULLTerminatedinMCF20",mode);

	checknull(mcf20->daily[0].adds,sizeof(mcf20->daily[0].adds),"MCF20","daily[0].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[0].deletes,sizeof(mcf20->daily[0].deletes),"MCF20","daily[0].deletes","CheckNULLTerminatedinMCF20",mode);

	checknull(mcf20->daily[1].adds,sizeof(mcf20->daily[1].adds),"MCF20","daily[1].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[1].deletes,sizeof(mcf20->daily[1].deletes),"MCF20","daily[1].adds","CheckNULLTerminatedinMCF20",mode);

	checknull(mcf20->daily[2].adds,sizeof(mcf20->daily[2].adds),"MCF20","daily[2].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[2].deletes,sizeof(mcf20->daily[2].deletes),"MCF20","daily[2].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[3].adds,sizeof(mcf20->daily[3].adds),"MCF20","daily[3].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[3].deletes,sizeof(mcf20->daily[3].deletes),"MCF20","daily[3].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[4].adds,sizeof(mcf20->daily[4].adds),"MCF20","daily[4].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[4].deletes,sizeof(mcf20->daily[4].deletes),"MCF20","daily[4].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[5].adds,sizeof(mcf20->daily[5].adds),"MCF20","daily[5].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[5].deletes,sizeof(mcf20->daily[5].deletes),"MCF20","daily[5].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[6].adds,sizeof(mcf20->daily[6].adds),"MCF20","daily[6].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[6].deletes,sizeof(mcf20->daily[6].deletes),"MCF20","daily[6].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[7].adds,sizeof(mcf20->daily[7].adds),"MCF20","daily[7].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[7].deletes,sizeof(mcf20->daily[7].deletes),"MCF20","daily[7].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[8].adds,sizeof(mcf20->daily[8].adds),"MCF20","daily[8].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[8].deletes,sizeof(mcf20->daily[8].deletes),"MCF20","daily[8].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[9].adds,sizeof(mcf20->daily[9].adds),"MCF20","daily[9].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[9].deletes,sizeof(mcf20->daily[9].deletes),"MCF20","daily[9].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[10].adds,sizeof(mcf20->daily[10].adds),"MCF20","daily[10].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[10].deletes,sizeof(mcf20->daily[10].deletes),"MCF20","daily[10].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[11].adds,sizeof(mcf20->daily[11].adds),"MCF20","daily[11].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[11].deletes,sizeof(mcf20->daily[11].deletes),"MCF20","daily[11].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[12].adds,sizeof(mcf20->daily[12].adds),"MCF20","daily[12].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[12].deletes,sizeof(mcf20->daily[12].deletes),"MCF20","daily[12].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[13].adds,sizeof(mcf20->daily[13].adds),"MCF20","daily[13].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[13].deletes,sizeof(mcf20->daily[13].deletes),"MCF20","daily[13].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[14].adds,sizeof(mcf20->daily[14].adds),"MCF20","daily[14].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[14].deletes,sizeof(mcf20->daily[14].deletes),"MCF20","daily[14].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[15].adds,sizeof(mcf20->daily[15].adds),"MCF20","daily[15].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[15].deletes,sizeof(mcf20->daily[15].deletes),"MCF20","daily[15].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[16].adds,sizeof(mcf20->daily[16].adds),"MCF20","daily[16].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[16].deletes,sizeof(mcf20->daily[16].deletes),"MCF20","daily[16].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[17].adds,sizeof(mcf20->daily[17].adds),"MCF20","daily[17].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[17].deletes,sizeof(mcf20->daily[17].deletes),"MCF20","daily[17].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[18].adds,sizeof(mcf20->daily[18].adds),"MCF20","daily[18].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[18].deletes,sizeof(mcf20->daily[18].deletes),"MCF20","daily[18].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[19].adds,sizeof(mcf20->daily[19].adds),"MCF20","daily[19].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[19].deletes,sizeof(mcf20->daily[19].deletes),"MCF20","daily[19].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[20].adds,sizeof(mcf20->daily[20].adds),"MCF20","daily[20].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[20].deletes,sizeof(mcf20->daily[20].deletes),"MCF20","daily[20].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[21].adds,sizeof(mcf20->daily[21].adds),"MCF20","daily[21].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[21].deletes,sizeof(mcf20->daily[21].deletes),"MCF20","daily[21].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[22].adds,sizeof(mcf20->daily[22].adds),"MCF20","daily[22].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[22].deletes,sizeof(mcf20->daily[22].deletes),"MCF20","daily[22].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[23].adds,sizeof(mcf20->daily[23].adds),"MCF20","daily[23].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[23].deletes,sizeof(mcf20->daily[23].deletes),"MCF20","daily[23].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[24].adds,sizeof(mcf20->daily[24].adds),"MCF20","daily[24].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[24].deletes,sizeof(mcf20->daily[24].deletes),"MCF20","daily[24].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[25].adds,sizeof(mcf20->daily[25].adds),"MCF20","daily[25].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[25].deletes,sizeof(mcf20->daily[25].deletes),"MCF20","daily[25].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[26].adds,sizeof(mcf20->daily[26].adds),"MCF20","daily[26].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[26].deletes,sizeof(mcf20->daily[26].deletes),"MCF20","daily[26].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[27].adds,sizeof(mcf20->daily[27].adds),"MCF20","daily[27].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[27].deletes,sizeof(mcf20->daily[27].deletes),"MCF20","daily[27].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[28].adds,sizeof(mcf20->daily[28].adds),"MCF20","daily[28].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[28].deletes,sizeof(mcf20->daily[28].deletes),"MCF20","daily[28].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[29].adds,sizeof(mcf20->daily[29].adds),"MCF20","daily[29].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[29].deletes,sizeof(mcf20->daily[29].deletes),"MCF20","daily[29].deletes","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[30].adds,sizeof(mcf20->daily[30].adds),"MCF20","daily[30].adds","CheckNULLTerminatedinMCF20",mode);
	checknull(mcf20->daily[30].deletes,sizeof(mcf20->daily[30].deletes),"MCF20","daily[30].deletes","CheckNULLTerminatedinMCF20",mode);
}

void CheckNULLTerminatedinNBF01(pNBF01   nbf01,pCHAR mode)
{
	checknull(nbf01->primary_key.low,sizeof(nbf01->primary_key.low),"NBF01","primary_key.low","CheckNULLTerminatedinNBF01",mode);
	checknull(nbf01->primary_key.high,sizeof(nbf01->primary_key.high),"NBF01","primary_key.high","CheckNULLTerminatedinNBF01",mode);
}

void CheckNULLTerminatedinNMF01(pNMF01   nmf01,pCHAR mode)
{
	checknull(nmf01->primary_key.seq_nbr,sizeof(nmf01->primary_key.seq_nbr),"NMF01","primary_key.seq_nbr","CheckNULLTerminatedinNMF01",mode);
	checknull(nmf01->action_code,sizeof(nmf01->action_code),"NMF01","action_code","CheckNULLTerminatedinNMF01",mode);
	checknull(nmf01->format_code,sizeof(nmf01->format_code),"NMF01","format_code","CheckNULLTerminatedinNMF01",mode);
	checknull(nmf01->account_info,sizeof(nmf01->account_info),"NMF01","account_info","CheckNULLTerminatedinNMF01",mode);
	checknull(nmf01->file_name,sizeof(nmf01->file_name),"NMF01","file_name","CheckNULLTerminatedinNMF01",mode);
}

void CheckNULLTerminatedinNGF01(pNGF01   ngf01,pCHAR mode)
{
	checknull(ngf01->primary_key.negative_group_id,sizeof(ngf01->primary_key.negative_group_id),"NGF01","primary_key.negative_group_id","CheckNULLTerminatedinNGF01",mode);

	checknull(ngf01->bin_range[0].low,sizeof(ngf01->bin_range[0].low),"NGF01","bin_range[0].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[0].high,sizeof(ngf01->bin_range[0].high),"NGF01","bin_range[0].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[1].low,sizeof(ngf01->bin_range[1].low),"NGF01","bin_range[1].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[1].high,sizeof(ngf01->bin_range[1].high),"NGF01","bin_range[1].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[2].low,sizeof(ngf01->bin_range[2].low),"NGF01","bin_range[2].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[2].high,sizeof(ngf01->bin_range[2].high),"NGF01","bin_range[2].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[3].low,sizeof(ngf01->bin_range[3].low),"NGF01","bin_range[3].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[3].high,sizeof(ngf01->bin_range[3].high),"NGF01","bin_range[3].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[4].low,sizeof(ngf01->bin_range[4].low),"NGF01","bin_range[4].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[4].high,sizeof(ngf01->bin_range[4].high),"NGF01","bin_range[4].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[5].low,sizeof(ngf01->bin_range[5].low),"NGF01","bin_range[5].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[5].high,sizeof(ngf01->bin_range[5].high),"NGF01","bin_range[5].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[6].low,sizeof(ngf01->bin_range[6].low),"NGF01","bin_range[6].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[6].high,sizeof(ngf01->bin_range[6].high),"NGF01","bin_range[6].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[7].low,sizeof(ngf01->bin_range[7].low),"NGF01","bin_range[7].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[7].high,sizeof(ngf01->bin_range[7].high),"NGF01","bin_range[7].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[8].low,sizeof(ngf01->bin_range[8].low),"NGF01","bin_range[8].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[8].high,sizeof(ngf01->bin_range[8].high),"NGF01","bin_range[8].high","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[9].low,sizeof(ngf01->bin_range[9].low),"NGF01","bin_range[9].low","CheckNULLTerminatedinNGF01",mode);
	checknull(ngf01->bin_range[9].high,sizeof(ngf01->bin_range[9].high),"NGF01","bin_range[9].high","CheckNULLTerminatedinNGF01",mode);
}

void CheckNULLTerminatedinNFI01(pNFI01   nfi01,pCHAR mode)
{
	checknull(nfi01->primary_key.file_name,sizeof(nfi01->primary_key.file_name),"NFI01","primary_key.file_name","CheckNULLTerminatedinNFI01",mode);
	checknull(nfi01->start_date,sizeof(nfi01->start_date),"NFI01","start_date","CheckNULLTerminatedinNFI01",mode);
	checknull(nfi01->end_date,sizeof(nfi01->end_date),"NFI01","end_date","CheckNULLTerminatedinNFI01",mode);
}

void CheckNULLTerminatedinNFI02(pNFI02   nfi02,pCHAR mode)
{
	checknull(nfi02->primary_key.file_name,sizeof(nfi02->primary_key.file_name),"NFI02","primary_key.file_name","CheckNULLTerminatedinNFI02",mode);
	checknull(nfi02->start_date,sizeof(nfi02->start_date),"NFI02","start_date","CheckNULLTerminatedinNFI02",mode);
	checknull(nfi02->end_date,sizeof(nfi02->end_date),"NFI02","end_date","CheckNULLTerminatedinNFI02",mode);
}

void CheckNULLTerminatedinNTF01(pNTF01   ntf01,pCHAR mode)
{
	checknull(ntf01->primary_key.account_info,sizeof(ntf01->primary_key.account_info),"NTF01","primary_key.account_info","CheckNULLTerminatedinNTF01",mode);
	checknull(ntf01->format_code,sizeof(ntf01->format_code),"NTF01","format_code","CheckNULLTerminatedinNTF01",mode);
	checknull(ntf01->file_name,sizeof(ntf01->file_name),"NTF01","file_name","CheckNULLTerminatedinNTF01",mode);
}

void CheckNULLTerminatedinBCH11(pBCH11   bch11,pCHAR mode)
{
	checknull(bch11->primary_key.retrieval_ref_num,sizeof(bch11->primary_key.retrieval_ref_num),"BCH11","primary_key.retrieval_ref_num","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->primary_key.batch_nbr,sizeof(bch11->primary_key.batch_nbr),"BCH11","primary_key.batch_nbr","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->primary_key.device_id,sizeof(bch11->primary_key.device_id),"BCH11","primary_key.device_id","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->merchant_id,sizeof(bch11->merchant_id),"BCH11","merchant_id","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->processing_code,sizeof(bch11->processing_code),"BCH11","processing_code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->tran_amount,sizeof(bch11->tran_amount),"BCH11","tran_amount","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->response_code,sizeof(bch11->response_code),"BCH11","response_code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->auth_number,sizeof(bch11->auth_number),"BCH11","auth_number","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->tran_date,sizeof(bch11->tran_date),"BCH11","tran_date","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->tran_time,sizeof(bch11->tran_time),"BCH11","tran_time","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->transaction_id,sizeof(bch11->transaction_id),"BCH11","transaction_id","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->message_type,sizeof(bch11->message_type),"BCH11","message_type","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->card_nbr,sizeof(bch11->card_nbr),"BCH11","card_nbr","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->invoice_nbr,sizeof(bch11->invoice_nbr),"BCH11","invoice_nbr","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->settle_file_prefix,sizeof(bch11->settle_file_prefix),"BCH11","settle_file_prefix","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_code,sizeof(bch11->product_code),"BCH11","product_code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->odometer,sizeof(bch11->odometer),"BCH11","odometer","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->sys_trace_audit_num,sizeof(bch11->sys_trace_audit_num),"BCH11","sys_trace_audit_num","CheckNULLTerminatedinBCH11",mode);
	checknull((char *)bch11->tx_key,sizeof(bch11->tx_key),"BCH11","tx_key","CheckNULLTerminatedinBCH11",mode);
	checknull((char *)bch11->bin_type,sizeof(bch11->bin_type),"BCH11","bin_type","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->type_of_data,sizeof(bch11->type_of_data),"BCH11","type_of_data","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->down_payment,sizeof(bch11->down_payment),"BCH11","down_payment","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->period,sizeof(bch11->period),"BCH11","period","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->interest_rate,sizeof(bch11->interest_rate),"BCH11","interest_rate","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->total_interest,sizeof(bch11->total_interest),"BCH11","total_interest","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->redemption_amount,sizeof(bch11->redemption_amount),"BCH11","redemption_amount","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->vehicle_number,sizeof(bch11->vehicle_number),"BCH11","vehicle_number","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->pos_entry_mode,sizeof(bch11->pos_entry_mode),"BCH11","pos_entry_mode","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->profile,sizeof(bch11->profile),"BCH11","profile","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->nbr_of_prod_codes,sizeof(bch11->nbr_of_prod_codes),"BCH11","nbr_of_prod_codes","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[0].code,sizeof(bch11->product_codes[0].code),"BCH11","product_codes[0].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[0].quantity,sizeof(bch11->product_codes[0].quantity),"BCH11","product_codes[0].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[0].amount,sizeof(bch11->product_codes[0].amount),"BCH11","product_codes[0].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[1].code,sizeof(bch11->product_codes[1].code),"BCH11","product_codes[1].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[1].quantity,sizeof(bch11->product_codes[1].quantity),"BCH11","product_codes[1].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[1].amount,sizeof(bch11->product_codes[1].amount),"BCH11","product_codes[1].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[2].code,sizeof(bch11->product_codes[2].code),"BCH11","product_codes[2].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[2].quantity,sizeof(bch11->product_codes[2].quantity),"BCH11","product_codes[2].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[2].amount,sizeof(bch11->product_codes[2].amount),"BCH11","product_codes[2].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[3].code,sizeof(bch11->product_codes[3].code),"BCH11","product_codes[3].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[3].quantity,sizeof(bch11->product_codes[3].quantity),"BCH11","product_codes[3].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[3].amount,sizeof(bch11->product_codes[3].amount),"BCH11","product_codes[3].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[4].code,sizeof(bch11->product_codes[4].code),"BCH11","product_codes[4].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[4].quantity,sizeof(bch11->product_codes[4].quantity),"BCH11","product_codes[4].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[4].amount,sizeof(bch11->product_codes[4].amount),"BCH11","product_codes[4].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[5].code,sizeof(bch11->product_codes[5].code),"BCH11","product_codes[5].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[5].quantity,sizeof(bch11->product_codes[5].quantity),"BCH11","product_codes[5].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[5].amount,sizeof(bch11->product_codes[5].amount),"BCH11","product_codes[5].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[6].code,sizeof(bch11->product_codes[6].code),"BCH11","product_codes[6].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[6].quantity,sizeof(bch11->product_codes[6].quantity),"BCH11","product_codes[6].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[6].amount,sizeof(bch11->product_codes[6].amount),"BCH11","product_codes[6].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[7].code,sizeof(bch11->product_codes[7].code),"BCH11","product_codes[7].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[7].quantity,sizeof(bch11->product_codes[7].quantity),"BCH11","product_codes[7].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[7].amount,sizeof(bch11->product_codes[7].amount),"BCH11","product_codes[7].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[8].code,sizeof(bch11->product_codes[8].code),"BCH11","product_codes[8].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[8].quantity,sizeof(bch11->product_codes[8].quantity),"BCH11","product_codes[8].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[8].amount,sizeof(bch11->product_codes[8].amount),"BCH11","product_codes[8].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[9].code,sizeof(bch11->product_codes[9].code),"BCH11","product_codes[9].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[9].quantity,sizeof(bch11->product_codes[9].quantity),"BCH11","product_codes[9].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[9].amount,sizeof(bch11->product_codes[9].amount),"BCH11","product_codes[9].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[10].code,sizeof(bch11->product_codes[10].code),"BCH11","product_codes[10].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[10].quantity,sizeof(bch11->product_codes[10].quantity),"BCH11","product_codes[10].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[10].amount,sizeof(bch11->product_codes[10].amount),"BCH11","product_codes[10].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[11].code,sizeof(bch11->product_codes[11].code),"BCH11","product_codes[11].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[11].quantity,sizeof(bch11->product_codes[11].quantity),"BCH11","product_codes[11].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[11].amount,sizeof(bch11->product_codes[11].amount),"BCH11","product_codes[11].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[12].code,sizeof(bch11->product_codes[12].code),"BCH11","product_codes[12].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[12].quantity,sizeof(bch11->product_codes[12].quantity),"BCH11","product_codes[12].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[12].amount,sizeof(bch11->product_codes[12].amount),"BCH11","product_codes[12].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[13].code,sizeof(bch11->product_codes[13].code),"BCH11","product_codes[13].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[13].quantity,sizeof(bch11->product_codes[13].quantity),"BCH11","product_codes[13].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[13].amount,sizeof(bch11->product_codes[13].amount),"BCH11","product_codes[13].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[14].code,sizeof(bch11->product_codes[14].code),"BCH11","product_codes[14].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[14].quantity,sizeof(bch11->product_codes[14].quantity),"BCH11","product_codes[14].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[14].amount,sizeof(bch11->product_codes[14].amount),"BCH11","product_codes[14].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[15].code,sizeof(bch11->product_codes[15].code),"BCH11","product_codes[15].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[15].quantity,sizeof(bch11->product_codes[15].quantity),"BCH11","product_codes[15].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[15].amount,sizeof(bch11->product_codes[15].amount),"BCH11","product_codes[15].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[16].code,sizeof(bch11->product_codes[16].code),"BCH11","product_codes[16].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[16].quantity,sizeof(bch11->product_codes[16].quantity),"BCH11","product_codes[16].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[16].amount,sizeof(bch11->product_codes[16].amount),"BCH11","product_codes[16].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[17].code,sizeof(bch11->product_codes[17].code),"BCH11","product_codes[17].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[17].quantity,sizeof(bch11->product_codes[17].quantity),"BCH11","product_codes[17].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[17].amount,sizeof(bch11->product_codes[17].amount),"BCH11","product_codes[17].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[18].code,sizeof(bch11->product_codes[18].code),"BCH11","product_codes[18].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[18].quantity,sizeof(bch11->product_codes[18].quantity),"BCH11","product_codes[18].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[18].amount,sizeof(bch11->product_codes[18].amount),"BCH11","product_codes[18].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->product_codes[19].code,sizeof(bch11->product_codes[19].code),"BCH11","product_codes[19].code","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[19].quantity,sizeof(bch11->product_codes[19].quantity),"BCH11","product_codes[19].quantity","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->product_codes[19].amount,sizeof(bch11->product_codes[19].amount),"BCH11","product_codes[19].amount","CheckNULLTerminatedinBCH11",mode);

	checknull(bch11->aportionment_flag,sizeof(bch11->aportionment_flag),"BCH11","aportionment_flag","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->exp_date,sizeof(bch11->exp_date),"BCH11","exp_date","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->open_date,sizeof(bch11->open_date),"BCH11","open_date","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->open_time,sizeof(bch11->open_time),"BCH11","open_time","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->cash_bonus,sizeof(bch11->cash_bonus),"BCH11","cash_bonus","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->transmission_timestamp,sizeof(bch11->transmission_timestamp),"BCH11","transmission_timestamp","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->mcard_banknet,sizeof(bch11->mcard_banknet),"BCH11","mcard_banknet","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->ticket_nbr,sizeof(bch11->ticket_nbr),"BCH11","ticket_nbr","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->def_gross_amt,sizeof(bch11->def_gross_amt),"BCH11","def_gross_amt","CheckNULLTerminatedinBCH11",mode);
	checknull(bch11->term,sizeof(bch11->term),"BCH11","term","CheckNULLTerminatedinBCH11",mode);
}

void CheckNULLTerminatedinBCH20(pBCH20   bch20,pCHAR mode)
{
	checknull(bch20->primary_key.retrieval_ref_num,sizeof(bch20->primary_key.retrieval_ref_num),"BCH20","primary_key.retrieval_ref_num","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->primary_key.batch_nbr,sizeof(bch20->primary_key.batch_nbr),"BCH20","primary_key.batch_nbr","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->primary_key.device_id,sizeof(bch20->primary_key.device_id),"BCH20","primary_key.device_id","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->merchant_id,sizeof(bch20->merchant_id),"BCH20","merchant_id","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->processing_code,sizeof(bch20->processing_code),"BCH20","processing_code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->tran_amount,sizeof(bch20->tran_amount),"BCH20","tran_amount","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->response_code,sizeof(bch20->response_code),"BCH20","response_code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->auth_number,sizeof(bch20->auth_number),"BCH20","auth_number","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->tran_date,sizeof(bch20->tran_date),"BCH20","tran_date","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->tran_time,sizeof(bch20->tran_time),"BCH20","tran_time","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->transaction_id,sizeof(bch20->transaction_id),"BCH20","transaction_id","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->message_type,sizeof(bch20->message_type),"BCH20","message_type","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->card_nbr,sizeof(bch20->card_nbr),"BCH20","card_nbr","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->invoice_nbr,sizeof(bch20->invoice_nbr),"BCH20","invoice_nbr","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->settle_file_prefix,sizeof(bch20->settle_file_prefix),"BCH20","settle_file_prefix","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_code,sizeof(bch20->product_code),"BCH20","product_code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->odometer,sizeof(bch20->odometer),"BCH20","odometer","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->sys_trace_audit_num,sizeof(bch20->sys_trace_audit_num),"BCH20","sys_trace_audit_num","CheckNULLTerminatedinBCH20",mode);
	checknull((char *)bch20->tx_key,sizeof(bch20->tx_key),"BCH20","tx_key","CheckNULLTerminatedinBCH20",mode);
	checknull((char *)bch20->bin_type,sizeof(bch20->bin_type),"BCH20","bin_type","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->type_of_data,sizeof(bch20->type_of_data),"BCH20","type_of_data","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->down_payment,sizeof(bch20->down_payment),"BCH20","down_payment","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->period,sizeof(bch20->period),"BCH20","period","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->interest_rate,sizeof(bch20->interest_rate),"BCH20","interest_rate","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->total_interest,sizeof(bch20->total_interest),"BCH20","total_interest","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->redemption_amount,sizeof(bch20->redemption_amount),"BCH20","redemption_amount","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->vehicle_number,sizeof(bch20->vehicle_number),"BCH20","vehicle_number","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->pos_entry_mode,sizeof(bch20->pos_entry_mode),"BCH20","pos_entry_mode","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->profile,sizeof(bch20->profile),"BCH20","profile","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->nbr_of_prod_codes,sizeof(bch20->nbr_of_prod_codes),"BCH20","nbr_of_prod_codes","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[0].code,sizeof(bch20->product_codes[0].code),"BCH20","product_codes[0].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[0].quantity,sizeof(bch20->product_codes[0].quantity),"BCH20","product_codes[0].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[0].amount,sizeof(bch20->product_codes[0].amount),"BCH20","product_codes[0].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[1].code,sizeof(bch20->product_codes[1].code),"BCH20","product_codes[1].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[1].quantity,sizeof(bch20->product_codes[1].quantity),"BCH20","product_codes[1].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[1].amount,sizeof(bch20->product_codes[1].amount),"BCH20","product_codes[1].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[2].code,sizeof(bch20->product_codes[2].code),"BCH20","product_codes[2].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[2].quantity,sizeof(bch20->product_codes[2].quantity),"BCH20","product_codes[2].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[2].amount,sizeof(bch20->product_codes[2].amount),"BCH20","product_codes[2].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[3].code,sizeof(bch20->product_codes[3].code),"BCH20","product_codes[3].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[3].quantity,sizeof(bch20->product_codes[3].quantity),"BCH20","product_codes[3].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[3].amount,sizeof(bch20->product_codes[3].amount),"BCH20","product_codes[3].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[4].code,sizeof(bch20->product_codes[4].code),"BCH20","product_codes[4].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[4].quantity,sizeof(bch20->product_codes[4].quantity),"BCH20","product_codes[4].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[4].amount,sizeof(bch20->product_codes[4].amount),"BCH20","product_codes[4].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[5].code,sizeof(bch20->product_codes[5].code),"BCH20","product_codes[5].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[5].quantity,sizeof(bch20->product_codes[5].quantity),"BCH20","product_codes[5].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[5].amount,sizeof(bch20->product_codes[5].amount),"BCH20","product_codes[5].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[6].code,sizeof(bch20->product_codes[6].code),"BCH20","product_codes[6].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[6].quantity,sizeof(bch20->product_codes[6].quantity),"BCH20","product_codes[6].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[6].amount,sizeof(bch20->product_codes[6].amount),"BCH20","product_codes[6].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[7].code,sizeof(bch20->product_codes[7].code),"BCH20","product_codes[7].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[7].quantity,sizeof(bch20->product_codes[7].quantity),"BCH20","product_codes[7].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[7].amount,sizeof(bch20->product_codes[7].amount),"BCH20","product_codes[7].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[8].code,sizeof(bch20->product_codes[8].code),"BCH20","product_codes[8].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[8].quantity,sizeof(bch20->product_codes[8].quantity),"BCH20","product_codes[8].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[8].amount,sizeof(bch20->product_codes[8].amount),"BCH20","product_codes[8].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[9].code,sizeof(bch20->product_codes[9].code),"BCH20","product_codes[9].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[9].quantity,sizeof(bch20->product_codes[9].quantity),"BCH20","product_codes[9].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[9].amount,sizeof(bch20->product_codes[9].amount),"BCH20","product_codes[9].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[10].code,sizeof(bch20->product_codes[10].code),"BCH20","product_codes[10].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[10].quantity,sizeof(bch20->product_codes[10].quantity),"BCH20","product_codes[10].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[10].amount,sizeof(bch20->product_codes[10].amount),"BCH20","product_codes[10].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[11].code,sizeof(bch20->product_codes[11].code),"BCH20","product_codes[11].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[11].quantity,sizeof(bch20->product_codes[11].quantity),"BCH20","product_codes[11].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[11].amount,sizeof(bch20->product_codes[11].amount),"BCH20","product_codes[11].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[12].code,sizeof(bch20->product_codes[12].code),"BCH20","product_codes[12].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[12].quantity,sizeof(bch20->product_codes[12].quantity),"BCH20","product_codes[12].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[12].amount,sizeof(bch20->product_codes[12].amount),"BCH20","product_codes[12].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[13].code,sizeof(bch20->product_codes[13].code),"BCH20","product_codes[13].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[13].quantity,sizeof(bch20->product_codes[13].quantity),"BCH20","product_codes[13].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[13].amount,sizeof(bch20->product_codes[13].amount),"BCH20","product_codes[13].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[14].code,sizeof(bch20->product_codes[14].code),"BCH20","product_codes[14].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[14].quantity,sizeof(bch20->product_codes[14].quantity),"BCH20","product_codes[14].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[14].amount,sizeof(bch20->product_codes[14].amount),"BCH20","product_codes[14].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[15].code,sizeof(bch20->product_codes[15].code),"BCH20","product_codes[15].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[15].quantity,sizeof(bch20->product_codes[15].quantity),"BCH20","product_codes[15].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[15].amount,sizeof(bch20->product_codes[15].amount),"BCH20","product_codes[15].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[16].code,sizeof(bch20->product_codes[16].code),"BCH20","product_codes[16].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[16].quantity,sizeof(bch20->product_codes[16].quantity),"BCH20","product_codes[16].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[16].amount,sizeof(bch20->product_codes[16].amount),"BCH20","product_codes[16].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[17].code,sizeof(bch20->product_codes[17].code),"BCH20","product_codes[17].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[17].quantity,sizeof(bch20->product_codes[17].quantity),"BCH20","product_codes[17].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[17].amount,sizeof(bch20->product_codes[17].amount),"BCH20","product_codes[17].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[18].code,sizeof(bch20->product_codes[18].code),"BCH20","product_codes[18].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[18].quantity,sizeof(bch20->product_codes[18].quantity),"BCH20","product_codes[18].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[18].amount,sizeof(bch20->product_codes[18].amount),"BCH20","product_codes[18].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->product_codes[19].code,sizeof(bch20->product_codes[19].code),"BCH20","product_codes[19].code","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[19].quantity,sizeof(bch20->product_codes[19].quantity),"BCH20","product_codes[19].quantity","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->product_codes[19].amount,sizeof(bch20->product_codes[19].amount),"BCH20","product_codes[19].amount","CheckNULLTerminatedinBCH20",mode);

	checknull(bch20->aportionment_flag,sizeof(bch20->aportionment_flag),"BCH20","aportionment_flag","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->exp_date,sizeof(bch20->exp_date),"BCH20","exp_date","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->open_date,sizeof(bch20->open_date),"BCH20","open_date","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->open_time,sizeof(bch20->open_time),"BCH20","open_time","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->cash_bonus,sizeof(bch20->cash_bonus),"BCH20","cash_bonus","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->transmission_timestamp,sizeof(bch20->transmission_timestamp),"BCH20","transmission_timestamp","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->mcard_banknet,sizeof(bch20->mcard_banknet),"BCH20","mcard_banknet","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->ticket_nbr,sizeof(bch20->ticket_nbr),"BCH20","ticket_nbr","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->def_gross_amt,sizeof(bch20->def_gross_amt),"BCH20","def_gross_amt","CheckNULLTerminatedinBCH20",mode);
	checknull(bch20->term,sizeof(bch20->term),"BCH20","term","CheckNULLTerminatedinBCH20",mode);
}

void CheckNULLTerminatedinBCH01_MP(pBCH01_MP bch01_mp,pCHAR mode)
{
	checknull(bch01_mp->primary_key.device_id,sizeof(bch01_mp->primary_key.device_id),"BCH01_MP","primary_key.device_id","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->primary_key.batch_nbr,sizeof(bch01_mp->primary_key.batch_nbr),"BCH01_MP","primary_key.batch_nbr","CheckNULLTerminatedinBCH01_MP",mode);

	checknull(bch01_mp->merchant_id,sizeof(bch01_mp->merchant_id),"BCH01_MP","merchant_id","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_credit_cnt,sizeof(bch01_mp->total_credit_cnt),"BCH01_MP","total_credit_cnt","CheckNULLTerminatedinBCH01_MP",mode);

	checknull(bch01_mp->total_credit_amt,sizeof(bch01_mp->total_credit_amt),"BCH01_MP","total_credit_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_credit_refund_cnt,sizeof(bch01_mp->total_credit_refund_cnt),"BCH01_MP","total_credit_refund_cnt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_credit_refund_amt,sizeof(bch01_mp->total_credit_refund_amt),"BCH01_MP","total_credit_refund_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_debit_cnt,sizeof(bch01_mp->total_debit_cnt),"BCH01_MP","total_debit_cnt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_debit_amt,sizeof(bch01_mp->total_debit_amt),"BCH01_MP","total_debit_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_debit_refund_cnt,sizeof(bch01_mp->total_debit_refund_cnt),"BCH01_MP","total_debit_refund_cnt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_debit_refund_amt,sizeof(bch01_mp->total_debit_refund_amt),"BCH01_MP","total_debit_refund_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_void_credit_cnt,sizeof(bch01_mp->total_void_credit_cnt),"BCH01_MP","total_void_credit_cnt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_void_credit_amt,sizeof(bch01_mp->total_void_credit_amt),"BCH01_MP","total_void_credit_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_void_credit_refund_cnt,sizeof(bch01_mp->total_void_credit_refund_cnt),"BCH01_MP","total_void_credit_refund_cnt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->total_void_credit_refund_amt,sizeof(bch01_mp->total_void_credit_refund_amt),"BCH01_MP","total_void_credit_refund_amt","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->open_date,sizeof(bch01_mp->open_date),"BCH01_MP","open_date","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->open_time,sizeof(bch01_mp->open_time),"BCH01_MP","open_time","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->release_date,sizeof(bch01_mp->release_date),"BCH01_MP","release_date","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->release_time,sizeof(bch01_mp->release_time),"BCH01_MP","release_time","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->batch_status,sizeof(bch01_mp->batch_status),"BCH01_MP","batch_status","CheckNULLTerminatedinBCH01_MP",mode);
	checknull(bch01_mp->visa_merchant_id,sizeof(bch01_mp->visa_merchant_id),"BCH01_MP","visa_merchant_id","CheckNULLTerminatedinBCH01_MP",mode);
}

void CheckNULLTerminatedinMGF01(pMGF01   mgf01,pCHAR mode)
{
	checknull(mgf01->primary_key.merchant_group_id,sizeof(mgf01->primary_key.merchant_group_id),"MGF01","primary_key.merchant_group_id","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->effective_date,sizeof(mgf01->effective_date),"MGF01","effective_date","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].Amount_limit,sizeof(mgf01->merchgrp[0].Amount_limit),"MGF01","merchgrp[0].Amount_limit","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].Amount_limit,sizeof(mgf01->merchgrp[1].Amount_limit),"MGF01","merchgrp[1].Amount_limit","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].Amount_limit,sizeof(mgf01->merchgrp[2].Amount_limit),"MGF01","merchgrp[2].Amount_limit","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].Amount_limit,sizeof(mgf01->merchgrp[3].Amount_limit),"MGF01","merchgrp[3].Amount_limit","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[0].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[0].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[0].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[0].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[0].INT_rate),"MGF01","merchgrp[0].merchgrepint[0].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[0].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[0].MSF_rate),"MGF01","merchgrp[0].merchgrepint[0].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[1].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[1].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[1].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[1].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[1].INT_rate),"MGF01","merchgrp[0].merchgrepint[0].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[1].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[1].MSF_rate),"MGF01","merchgrp[0].merchgrepint[1].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[2].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[2].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[2].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[2].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[2].INT_rate),"MGF01","merchgrp[0].merchgrepint[2].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[2].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[2].MSF_rate),"MGF01","merchgrp[0].merchgrepint[2].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[3].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[3].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[3].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[3].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[3].INT_rate),"MGF01","merchgrp[0].merchgrepint[3].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[3].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[3].MSF_rate),"MGF01","merchgrp[0].merchgrepint[3].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[4].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[4].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[4].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[4].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[4].INT_rate),"MGF01","merchgrp[0].merchgrepint[4].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[4].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[4].MSF_rate),"MGF01","merchgrp[0].merchgrepint[4].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[5].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[5].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[5].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[5].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[5].INT_rate),"MGF01","merchgrp[0].merchgrepint[5].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[5].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[5].MSF_rate),"MGF01","merchgrp[0].merchgrepint[5].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[6].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[6].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[6].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[6].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[6].INT_rate),"MGF01","merchgrp[0].merchgrepint[6].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[6].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[6].MSF_rate),"MGF01","merchgrp[0].merchgrepint[6].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[7].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[7].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[7].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[7].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[7].INT_rate),"MGF01","merchgrp[0].merchgrepint[7].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[7].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[7].MSF_rate),"MGF01","merchgrp[0].merchgrepint[7].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[8].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[8].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[8].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[8].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[8].INT_rate),"MGF01","merchgrp[0].merchgrepint[8].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[8].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[8].MSF_rate),"MGF01","merchgrp[0].merchgrepint[8].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[9].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[9].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[9].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[9].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[9].INT_rate),"MGF01","merchgrp[0].merchgrepint[9].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[9].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[9].MSF_rate),"MGF01","merchgrp[0].merchgrepint[9].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[0].merchgrepint[10].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[10].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[10].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[10].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[10].INT_rate),"MGF01","merchgrp[0].merchgrepint[10].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[10].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[10].MSF_rate),"MGF01","merchgrp[0].merchgrepint[10].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[11].nbr_of_months,sizeof(mgf01->merchgrp[0].merchgrepint[11].nbr_of_months),"MGF01","merchgrp[0].merchgrepint[11].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[11].INT_rate,sizeof(mgf01->merchgrp[0].merchgrepint[11].INT_rate),"MGF01","merchgrp[0].merchgrepint[11].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[0].merchgrepint[11].MSF_rate,sizeof(mgf01->merchgrp[0].merchgrepint[11].MSF_rate),"MGF01","merchgrp[0].merchgrepint[11].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[0].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[0].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[0].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[0].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[0].INT_rate),"MGF01","merchgrp[1].merchgrepint[0].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[0].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[0].MSF_rate),"MGF01","merchgrp[1].merchgrepint[0].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[1].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[1].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[1].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[1].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[1].INT_rate),"MGF01","merchgrp[1].merchgrepint[1].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[1].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[1].MSF_rate),"MGF01","merchgrp[1].merchgrepint[1].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[2].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[2].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[2].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[2].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[2].INT_rate),"MGF01","merchgrp[1].merchgrepint[2].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[2].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[2].MSF_rate),"MGF01","merchgrp[1].merchgrepint[2].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[3].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[3].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[3].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[3].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[3].INT_rate),"MGF01","merchgrp[1].merchgrepint[3].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[3].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[3].MSF_rate),"MGF01","merchgrp[1].merchgrepint[3].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[4].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[4].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[4].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[4].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[4].INT_rate),"MGF01","merchgrp[1].merchgrepint[4].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[4].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[4].MSF_rate),"MGF01","merchgrp[1].merchgrepint[4].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[5].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[5].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[5].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[5].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[5].INT_rate),"MGF01","merchgrp[1].merchgrepint[5].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[5].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[5].MSF_rate),"MGF01","merchgrp[1].merchgrepint[5].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[6].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[6].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[6].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[6].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[6].INT_rate),"MGF01","merchgrp[1].merchgrepint[6].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[6].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[6].MSF_rate),"MGF01","merchgrp[1].merchgrepint[6].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[7].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[7].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[7].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[7].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[7].INT_rate),"MGF01","merchgrp[1].merchgrepint[7].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[7].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[7].MSF_rate),"MGF01","merchgrp[1].merchgrepint[7].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[8].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[8].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[8].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[8].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[8].INT_rate),"MGF01","merchgrp[1].merchgrepint[8].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[8].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[8].MSF_rate),"MGF01","merchgrp[1].merchgrepint[8].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[9].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[9].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[9].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[9].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[9].INT_rate),"MGF01","merchgrp[1].merchgrepint[9].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[9].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[9].MSF_rate),"MGF01","merchgrp[1].merchgrepint[9].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[1].merchgrepint[10].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[10].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[10].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[10].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[10].INT_rate),"MGF01","merchgrp[1].merchgrepint[10].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[10].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[10].MSF_rate),"MGF01","merchgrp[1].merchgrepint[10].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[11].nbr_of_months,sizeof(mgf01->merchgrp[1].merchgrepint[11].nbr_of_months),"MGF01","merchgrp[1].merchgrepint[11].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[11].INT_rate,sizeof(mgf01->merchgrp[1].merchgrepint[11].INT_rate),"MGF01","merchgrp[1].merchgrepint[11].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[1].merchgrepint[11].MSF_rate,sizeof(mgf01->merchgrp[1].merchgrepint[11].MSF_rate),"MGF01","merchgrp[1].merchgrepint[11].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[0].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[0].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[0].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[0].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[0].INT_rate),"MGF01","merchgrp[2].merchgrepint[0].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[0].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[0].MSF_rate),"MGF01","merchgrp[2].merchgrepint[0].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[1].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[1].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[1].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[1].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[1].INT_rate),"MGF01","merchgrp[2].merchgrepint[1].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[1].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[1].MSF_rate),"MGF01","merchgrp[2].merchgrepint[1].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[2].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[2].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[2].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[2].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[2].INT_rate),"MGF01","merchgrp[2].merchgrepint[2].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[2].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[2].MSF_rate),"MGF01","merchgrp[2].merchgrepint[2].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[3].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[3].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[3].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[3].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[3].INT_rate),"MGF01","merchgrp[2].merchgrepint[3].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[3].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[3].MSF_rate),"MGF01","merchgrp[2].merchgrepint[3].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[4].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[4].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[4].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[4].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[4].INT_rate),"MGF01","merchgrp[2].merchgrepint[4].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[4].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[4].MSF_rate),"MGF01","merchgrp[2].merchgrepint[4].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[5].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[5].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[5].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[5].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[5].INT_rate),"MGF01","merchgrp[2].merchgrepint[5].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[5].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[5].MSF_rate),"MGF01","merchgrp[2].merchgrepint[5].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[6].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[6].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[6].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[6].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[6].INT_rate),"MGF01","merchgrp[2].merchgrepint[6].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[6].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[6].MSF_rate),"MGF01","merchgrp[2].merchgrepint[6].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[7].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[7].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[7].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[7].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[7].INT_rate),"MGF01","merchgrp[2].merchgrepint[7].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[7].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[7].MSF_rate),"MGF01","merchgrp[2].merchgrepint[7].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[8].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[8].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[8].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[8].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[8].INT_rate),"MGF01","merchgrp[2].merchgrepint[8].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[8].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[8].MSF_rate),"MGF01","merchgrp[2].merchgrepint[8].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[9].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[9].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[9].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[9].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[9].INT_rate),"MGF01","merchgrp[2].merchgrepint[9].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[9].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[9].MSF_rate),"MGF01","merchgrp[2].merchgrepint[9].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[2].merchgrepint[10].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[10].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[10].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[10].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[10].INT_rate),"MGF01","merchgrp[2].merchgrepint[10].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[10].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[10].MSF_rate),"MGF01","merchgrp[2].merchgrepint[10].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[11].nbr_of_months,sizeof(mgf01->merchgrp[2].merchgrepint[11].nbr_of_months),"MGF01","merchgrp[2].merchgrepint[11].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[11].INT_rate,sizeof(mgf01->merchgrp[2].merchgrepint[11].INT_rate),"MGF01","merchgrp[2].merchgrepint[11].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[2].merchgrepint[11].MSF_rate,sizeof(mgf01->merchgrp[2].merchgrepint[11].MSF_rate),"MGF01","merchgrp[2].merchgrepint[11].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[0].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[0].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[0].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[0].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[0].INT_rate),"MGF01","merchgrp[3].merchgrepint[0].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[0].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[0].MSF_rate),"MGF01","merchgrp[3].merchgrepint[0].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[1].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[1].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[1].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[1].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[1].INT_rate),"MGF01","merchgrp[3].merchgrepint[1].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[1].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[1].MSF_rate),"MGF01","merchgrp[3].merchgrepint[1].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[2].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[2].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[2].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[2].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[2].INT_rate),"MGF01","merchgrp[3].merchgrepint[2].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[2].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[2].MSF_rate),"MGF01","merchgrp[3].merchgrepint[2].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[3].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[3].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[3].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[3].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[3].INT_rate),"MGF01","merchgrp[3].merchgrepint[3].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[3].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[3].MSF_rate),"MGF01","merchgrp[3].merchgrepint[3].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[4].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[4].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[4].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[4].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[4].INT_rate),"MGF01","merchgrp[3].merchgrepint[4].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[4].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[4].MSF_rate),"MGF01","merchgrp[3].merchgrepint[4].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[5].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[5].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[5].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[5].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[5].INT_rate),"MGF01","merchgrp[3].merchgrepint[5].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[5].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[5].MSF_rate),"MGF01","merchgrp[3].merchgrepint[5].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[6].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[6].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[6].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[6].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[6].INT_rate),"MGF01","merchgrp[3].merchgrepint[6].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[6].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[6].MSF_rate),"MGF01","merchgrp[3].merchgrepint[6].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[7].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[7].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[7].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[7].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[7].INT_rate),"MGF01","merchgrp[3].merchgrepint[7].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[7].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[7].MSF_rate),"MGF01","merchgrp[3].merchgrepint[7].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[8].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[8].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[8].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[8].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[8].INT_rate),"MGF01","merchgrp[3].merchgrepint[8].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[8].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[8].MSF_rate),"MGF01","merchgrp[3].merchgrepint[8].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[9].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[9].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[9].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[9].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[9].INT_rate),"MGF01","merchgrp[3].merchgrepint[9].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[9].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[9].MSF_rate),"MGF01","merchgrp[3].merchgrepint[9].MSF_rate","CheckNULLTerminatedinMGF01",mode);

	checknull(mgf01->merchgrp[3].merchgrepint[10].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[10].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[10].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[10].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[10].INT_rate),"MGF01","merchgrp[3].merchgrepint[10].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[10].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[10].MSF_rate),"MGF01","merchgrp[3].merchgrepint[10].MSF_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[11].nbr_of_months,sizeof(mgf01->merchgrp[3].merchgrepint[11].nbr_of_months),"MGF01","merchgrp[3].merchgrepint[11].nbr_of_months","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[11].INT_rate,sizeof(mgf01->merchgrp[3].merchgrepint[11].INT_rate),"MGF01","merchgrp[3].merchgrepint[11].INT_rate","CheckNULLTerminatedinMGF01",mode);
	checknull(mgf01->merchgrp[3].merchgrepint[11].MSF_rate,sizeof(mgf01->merchgrp[3].merchgrepint[11].MSF_rate),"MGF01","merchgrp[3].merchgrepint[11].MSF_rate","CheckNULLTerminatedinMGF01",mode);
}

void CheckNULLTerminatedinNCF30(pNCF30 ncf30,pCHAR mode)
{
	checknull(ncf30->primary_key.retrieval_ref_num,sizeof(ncf30->primary_key.retrieval_ref_num),"NCF30","primary_key.retrieval_ref_num","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->primary_key.network_type,sizeof(ncf30->primary_key.network_type),"NCF30","primary_key.network_type","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->primary_key.network_id,sizeof(ncf30->primary_key.network_id),"NCF30","primary_key.network_id","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->primary_key.batch_number,sizeof(ncf30->primary_key.batch_number),"NCF30","primary_key.batch_number","CheckNULLTerminatedinNCF30",mode);

	checknull(ncf30->open_date,sizeof(ncf30->open_date),"NCF30","open_date","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->open_time,sizeof(ncf30->open_time),"NCF30","open_time","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->merchant_id,sizeof(ncf30->merchant_id),"NCF30","merchant_id","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->transaction_id,sizeof(ncf30->transaction_id),"NCF30","transaction_id","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->device_id,sizeof(ncf30->device_id),"NCF30","device_id","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->card_nbr,sizeof(ncf30->card_nbr),"NCF30","card_nbr","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->message_type,sizeof(ncf30->message_type),"NCF30","message_type","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->processing_code,sizeof(ncf30->processing_code),"NCF30","processing_code","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->tran_amount,sizeof(ncf30->tran_amount),"NCF30","tran_amount","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->auth_number,sizeof(ncf30->auth_number),"NCF30","auth_number","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->response_code,sizeof(ncf30->response_code),"NCF30","response_code","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->tran_date,sizeof(ncf30->tran_date),"NCF30","tran_date","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->tran_time,sizeof(ncf30->tran_time),"NCF30","tran_time","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->sys_trace_audit_nbr,sizeof(ncf30->sys_trace_audit_nbr),"NCF30","sys_trace_audit_nbr","CheckNULLTerminatedinNCF30",mode);
	checknull((char *)ncf30->tx_key,sizeof(ncf30->tx_key),"NCF30","tx_key","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->invoice_nbr,sizeof(ncf30->invoice_nbr),"NCF30","invoice_nbr","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->network_data,sizeof(ncf30->network_data),"NCF30","network_data","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->currency_code,sizeof(ncf30->currency_code),"NCF30","currency_code","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->acquiring_inst_id_code,sizeof(ncf30->acquiring_inst_id_code),"NCF30","acquiring_inst_id_code","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->settlement_date,sizeof(ncf30->settlement_date),"NCF30","settlement_date","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->transmission_timestamp,sizeof(ncf30->transmission_timestamp),"NCF30","transmission_timestamp","CheckNULLTerminatedinNCF30",mode);
	checknull(ncf30->tran_fee_amount,sizeof(ncf30->tran_fee_amount),"NCF30","tran_fee_amount","CheckNULLTerminatedinNCF30",mode);
}

void CheckNULLTerminatedinNCF01(pNCF01 ncf01,pCHAR mode)
{
	checknull(ncf01->primary_key.network_type,sizeof(ncf01->primary_key.network_type),"NCF01","primary_key.network_type","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->primary_key.network_id,sizeof(ncf01->primary_key.network_id),"NCF01","primary_key.network_id","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->name,sizeof(ncf01->name),"NCF01","name","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->status,sizeof(ncf01->status),"NCF01","status","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->totals_ind,sizeof(ncf01->totals_ind),"NCF01","totals_ind","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->statistics_ind,sizeof(ncf01->statistics_ind),"NCF01","statistics_ind","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->acquirer_inst_id_code,sizeof(ncf01->acquirer_inst_id_code),"NCF01","acquirer_inst_id_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->forwarding_inst_id_code,sizeof(ncf01->forwarding_inst_id_code),"NCF01","forwarding_inst_id_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->receiving_inst_id_code,sizeof(ncf01->receiving_inst_id_code),"NCF01","receiving_inst_id_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->remote_nsp_type,sizeof(ncf01->remote_nsp_type),"NCF01","remote_nsp_type","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->pin_type,sizeof(ncf01->pin_type),"NCF01","pin_type","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->encr_type,sizeof(ncf01->encr_type),"NCF01","encr_type","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->pad_char,sizeof(ncf01->pad_char),"NCF01","pad_char","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->cwk,sizeof(ncf01->cwk),"NCF01","cwk","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->kek,sizeof(ncf01->kek),"NCF01","kek","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->retired_cwk,sizeof(ncf01->retired_cwk),"NCF01","retired_cwk","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->auto_logon,sizeof(ncf01->auto_logon),"NCF01","auto_logon","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->echo_interval,sizeof(ncf01->echo_interval),"NCF01","echo_interval","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->max_consecutive_timeouts,sizeof(ncf01->max_consecutive_timeouts),"NCF01","max_consecutive_timeouts","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->max_active_txns,sizeof(ncf01->max_active_txns),"NCF01","max_active_txns","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->que_name,sizeof(ncf01->que_name),"NCF01","que_name","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->local_nsp_type,sizeof(ncf01->local_nsp_type),"NCF01","local_nsp_type","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->request_timer,sizeof(ncf01->request_timer),"NCF01","request_timer","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->idle_line_timer,sizeof(ncf01->idle_line_timer),"NCF01","idle_line_timer","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->advice_repeat_counter,sizeof(ncf01->advice_repeat_counter),"NCF01","advice_repeat_counter","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->admin_message_timer,sizeof(ncf01->admin_message_timer),"NCF01","admin_message_timer","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->store_forward_processing,sizeof(ncf01->store_forward_processing),"NCF01","store_forward_processing","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->currency_code_purchase,sizeof(ncf01->currency_code_purchase),"NCF01","currency_code_purchase","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->currency_code_cash,sizeof(ncf01->currency_code_cash),"NCF01","currency_code_cash","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->country_code,sizeof(ncf01->country_code),"NCF01","country_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->station_id1,sizeof(ncf01->station_id1),"NCF01","station_id1","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->station_id2,sizeof(ncf01->station_id2),"NCF01","station_id2","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->prev_src_station,sizeof(ncf01->prev_src_station),"NCF01","prev_src_station","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->tpdu_id,sizeof(ncf01->tpdu_id),"NCF01","tpdu_id","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[0].identifier,sizeof(ncf01->logon_bin[0].identifier),"NCF01","logon_bin[0].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[1].identifier,sizeof(ncf01->logon_bin[1].identifier),"NCF01","logon_bin[1].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[2].identifier,sizeof(ncf01->logon_bin[2].identifier),"NCF01","logon_bin[2].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[3].identifier,sizeof(ncf01->logon_bin[3].identifier),"NCF01","logon_bin[3].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[4].identifier,sizeof(ncf01->logon_bin[4].identifier),"NCF01","logon_bin[4].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[5].identifier,sizeof(ncf01->logon_bin[5].identifier),"NCF01","logon_bin[5].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[6].identifier,sizeof(ncf01->logon_bin[6].identifier),"NCF01","logon_bin[6].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[7].identifier,sizeof(ncf01->logon_bin[7].identifier),"NCF01","logon_bin[7].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[8].identifier,sizeof(ncf01->logon_bin[8].identifier),"NCF01","logon_bin[8].identifier","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->logon_bin[9].identifier,sizeof(ncf01->logon_bin[9].identifier),"NCF01","logon_bin[9].identifier","CheckNULLTerminatedinNCF01",mode);

	checknull(ncf01->allowed[0].processing_code,sizeof(ncf01->allowed[0].processing_code),"NCF01","allowed[0].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[1].processing_code,sizeof(ncf01->allowed[1].processing_code),"NCF01","allowed[1].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[2].processing_code,sizeof(ncf01->allowed[2].processing_code),"NCF01","allowed[2].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[3].processing_code,sizeof(ncf01->allowed[3].processing_code),"NCF01","allowed[3].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[4].processing_code,sizeof(ncf01->allowed[4].processing_code),"NCF01","allowed[4].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[5].processing_code,sizeof(ncf01->allowed[5].processing_code),"NCF01","allowed[5].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[6].processing_code,sizeof(ncf01->allowed[6].processing_code),"NCF01","allowed[6].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[7].processing_code,sizeof(ncf01->allowed[7].processing_code),"NCF01","allowed[7].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[8].processing_code,sizeof(ncf01->allowed[8].processing_code),"NCF01","allowed[8].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[9].processing_code,sizeof(ncf01->allowed[9].processing_code),"NCF01","allowed[9].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[10].processing_code,sizeof(ncf01->allowed[10].processing_code),"NCF01","allowed[10].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[11].processing_code,sizeof(ncf01->allowed[11].processing_code),"NCF01","allowed[11].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[12].processing_code,sizeof(ncf01->allowed[12].processing_code),"NCF01","allowed[12].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[13].processing_code,sizeof(ncf01->allowed[13].processing_code),"NCF01","allowed[13].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[14].processing_code,sizeof(ncf01->allowed[14].processing_code),"NCF01","allowed[14].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[15].processing_code,sizeof(ncf01->allowed[15].processing_code),"NCF01","allowed[15].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[16].processing_code,sizeof(ncf01->allowed[16].processing_code),"NCF01","allowed[16].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[17].processing_code,sizeof(ncf01->allowed[17].processing_code),"NCF01","allowed[17].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->allowed[18].processing_code,sizeof(ncf01->allowed[18].processing_code),"NCF01","allowed[18].processing_code","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->zmk,sizeof(ncf01->zmk),"NCF01","zmk","CheckNULLTerminatedinNCF01",mode);
	checknull(ncf01->neg_access_code,sizeof(ncf01->neg_access_code),"NCF01","neg_access_code","CheckNULLTerminatedinNCF01",mode);

}

void CheckNULLTerminatedinNCF02(pNCF02 ncf02,pCHAR mode)
{
	checknull(ncf02->primary_key.network_type,sizeof(ncf02->primary_key.network_type),"NCF02","primary_key.network_type","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->primary_key.network_id,sizeof(ncf02->primary_key.network_id),"NCF02","primary_key.network_id","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_cnt,sizeof(ncf02->credit_cnt),"NCF02","credit_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_reversal_cnt,sizeof(ncf02->credit_reversal_cnt),"NCF02","credit_reversal_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_cnt,sizeof(ncf02->debit_cnt),"NCF02","debit_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_reversal_cnt,sizeof(ncf02->debit_reversal_cnt),"NCF02","debit_reversal_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->transfer_cnt,sizeof(ncf02->transfer_cnt),"NCF02","transfer_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->transfer_reversal_cnt,sizeof(ncf02->transfer_reversal_cnt),"NCF02","transfer_reversal_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->inquiries_cnt,sizeof(ncf02->inquiries_cnt),"NCF02","inquiries_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->authorizations_cnt,sizeof(ncf02->authorizations_cnt),"NCF02","authorizations_cnt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_proc_fee_amt,sizeof(ncf02->credit_proc_fee_amt),"NCF02","credit_proc_fee_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_tran_fee_amt,sizeof(ncf02->credit_tran_fee_amt),"NCF02","credit_tran_fee_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_proc_fee_amt,sizeof(ncf02->debit_proc_fee_amt),"NCF02","debit_proc_fee_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_tran_fee_amt,sizeof(ncf02->debit_tran_fee_amt),"NCF02","debit_tran_fee_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_amt,sizeof(ncf02->credit_amt),"NCF02","credit_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->credit_reversal_amt,sizeof(ncf02->credit_reversal_amt),"NCF02","credit_reversal_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_amt,sizeof(ncf02->debit_amt),"NCF02","debit_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->debit_reversal_amt,sizeof(ncf02->debit_reversal_amt),"NCF02","debit_reversal_amt","CheckNULLTerminatedinNCF02",mode);
	checknull(ncf02->net_settlement_amt,sizeof(ncf02->net_settlement_amt),"NCF02","net_settlement_amt","CheckNULLTerminatedinNCF02",mode);
}

void CheckNULLTerminatedinNCF20(pNCF20   ncf20,pCHAR mode)
{
	checknull(ncf20->primary_key.network_id,sizeof(ncf20->primary_key.network_id),"NCF20","primary_key.network_id","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->primary_key.yyyymmdd,sizeof(ncf20->primary_key.yyyymmdd),"NCF20","primary_key.yyyymmdd","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->ytd_total_tran_cnt,sizeof(ncf20->ytd_total_tran_cnt),"NCF20","ytd_total_tran_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->ytd_total_tran_amt,sizeof(ncf20->ytd_total_tran_amt),"NCF20","ytd_total_tran_amt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->mtd_total_tran_cnt,sizeof(ncf20->mtd_total_tran_cnt),"NCF20","mtd_total_tran_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->mtd_total_tran_amt,sizeof(ncf20->mtd_total_tran_amt),"NCF20","mtd_total_tran_amt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->tran_cnt,sizeof(ncf20->tran_cnt),"NCF20","tran_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->tran_amt,sizeof(ncf20->tran_amt),"NCF20","tran_amt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->tran_approved_cnt,sizeof(ncf20->tran_approved_cnt),"NCF20","tran_approved_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->tran_declined_cnt,sizeof(ncf20->tran_declined_cnt),"NCF20","tran_declined_cnt","CheckNULLTerminatedinNCF20",mode);

	checknull(ncf20->decline[0].category_code,sizeof(ncf20->decline[0].category_code),"NCF20","decline[0].category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[0].category_cnt,sizeof(ncf20->decline[0].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[1].category_code,sizeof(ncf20->decline[1].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[1].category_cnt,sizeof(ncf20->decline[1].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[2].category_code,sizeof(ncf20->decline[2].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[2].category_cnt,sizeof(ncf20->decline[2].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[3].category_code,sizeof(ncf20->decline[3].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[3].category_cnt,sizeof(ncf20->decline[3].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[4].category_code,sizeof(ncf20->decline[4].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[4].category_cnt,sizeof(ncf20->decline[4].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[5].category_code,sizeof(ncf20->decline[5].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[5].category_cnt,sizeof(ncf20->decline[5].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[6].category_code,sizeof(ncf20->decline[6].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[6].category_cnt,sizeof(ncf20->decline[6].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[7].category_code,sizeof(ncf20->decline[7].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[7].category_cnt,sizeof(ncf20->decline[7].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[8].category_code,sizeof(ncf20->decline[8].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[8].category_cnt,sizeof(ncf20->decline[8].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[9].category_code,sizeof(ncf20->decline[9].category_code),"NCF20","category_code","CheckNULLTerminatedinNCF20",mode);
	checknull(ncf20->decline[9].category_cnt,sizeof(ncf20->decline[9].category_cnt),"NCF20","category_cnt","CheckNULLTerminatedinNCF20",mode);
}

void CheckNULLTerminatedinNCF21(pNCF21   ncf21,pCHAR mode)
{
	checknull(ncf21->primary_key.batch_number,sizeof(ncf21->primary_key.batch_number),"NCF21","primary_key.batch_number","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->primary_key.network_type,sizeof(ncf21->primary_key.network_type),"NCF21","primary_key.network_type","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->primary_key.network_id,sizeof(ncf21->primary_key.network_id),"NCF21","primary_key.network_id","CheckNULLTerminatedinNCF21",mode);

	checknull(ncf21->open_date,sizeof(ncf21->open_date),"NCF21","open_date","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->open_time,sizeof(ncf21->open_time),"NCF21","open_time","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->release_date,sizeof(ncf21->release_date),"NCF21","release_date","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->release_time,sizeof(ncf21->release_time),"NCF21","release_time","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->credit_cnt,sizeof(ncf21->credit_cnt),"NCF21","credit_cnt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->credit_amt,sizeof(ncf21->credit_amt),"NCF21","credit_amt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->credit_rev_cnt,sizeof(ncf21->credit_rev_cnt),"NCF21","credit_rev_cnt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->credit_rev_amt,sizeof(ncf21->credit_rev_amt),"NCF21","credit_rev_amt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->debit_cnt,sizeof(ncf21->debit_cnt),"NCF21","debit_cnt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->debit_amt,sizeof(ncf21->debit_amt),"NCF21","debit_amt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->debit_rev_cnt,sizeof(ncf21->debit_rev_cnt),"NCF21","debit_rev_cnt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->debit_rev_amt,sizeof(ncf21->debit_rev_amt),"NCF21","debit_rev_amt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->total_amt,sizeof(ncf21->total_amt),"NCF21","total_amt","CheckNULLTerminatedinNCF21",mode);
	checknull(ncf21->total_amt_type,sizeof(ncf21->total_amt_type),"NCF21","total_amt_type","CheckNULLTerminatedinNCF21",mode);
}

void CheckNULLTerminatedinATM01(pATM01   atm01,pCHAR mode)
{
	checknull(atm01->primary_key.bkcb_name,sizeof(atm01->primary_key.bkcb_name),"ATM01","primary_key.bkcb_name","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->primary_key.termnumr,sizeof(atm01->primary_key.termnumr),"ATM01","primary_key.termnumr","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->bourder,sizeof(atm01->bourder),"ATM01","bourder","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->mnemonic,sizeof(atm01->mnemonic),"ATM01","mnemonic","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->brchname,sizeof(atm01->brchname),"ATM01","brchname","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termsite,sizeof(atm01->termsite),"ATM01","termsite","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termaddr,sizeof(atm01->termaddr),"ATM01","termaddr","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termdist,sizeof(atm01->termdist),"ATM01","termdist","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termcimu,sizeof(atm01->termcimu),"ATM01","termcimu","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termprov,sizeof(atm01->termprov),"ATM01","termprov","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termregn,sizeof(atm01->termregn),"ATM01","termregn","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->machtype,sizeof(atm01->machtype),"ATM01","machtype","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termodel,sizeof(atm01->termodel),"ATM01","termodel","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->dateinst,sizeof(atm01->dateinst),"ATM01","dateinst","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termloc,sizeof(atm01->termloc),"ATM01","termloc","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->termtype,sizeof(atm01->termtype),"ATM01","termtype","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->refnum,sizeof(atm01->refnum),"ATM01","refnum","CheckNULLTerminatedinATM01",mode);
	checknull(atm01->linktype,sizeof(atm01->linktype),"ATM01","linktype","CheckNULLTerminatedinATM01",mode);
}

void CheckNULLTerminatedinSAF01(pSAF01   saf01,pCHAR mode)
{
	checknull(saf01->primary_key.transaction_id,sizeof(saf01->primary_key.transaction_id),"SAF01","primary_key.transaction_id","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->card_num,sizeof(saf01->card_num),"SAF01","card_num","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->tran_amount,sizeof(saf01->tran_amount),"SAF01","tran_amount","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->device_id,sizeof(saf01->device_id),"SAF01","device_id","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->message_type,sizeof(saf01->message_type),"SAF01","message_type","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->processing_code,sizeof(saf01->processing_code),"SAF01","processing_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->sys_trace_audit_nbr,sizeof(saf01->sys_trace_audit_nbr),"SAF01","sys_trace_audit_nbr","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->tran_date,sizeof(saf01->tran_date),"SAF01","tran_date","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->tran_time,sizeof(saf01->tran_time),"SAF01","tran_time","CheckNULLTerminatedinSAF01",mode);
	checknull((char *)saf01->tx_key,sizeof(saf01->tx_key),"SAF01","tx_key","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->retrieval_ref_num,sizeof(saf01->retrieval_ref_num),"SAF01","retrieval_ref_num","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->merchant_id,sizeof(saf01->merchant_id),"SAF01","merchant_id","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->auth_number,sizeof(saf01->auth_number),"SAF01","auth_number","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->response_code,sizeof(saf01->response_code),"SAF01","response_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->invoice_nbr,sizeof(saf01->invoice_nbr),"SAF01","invoice_nbr","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->network_data,sizeof(saf01->network_data),"SAF01","network_data","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->currency_code,sizeof(saf01->currency_code),"SAF01","currency_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->transmission_timestamp,sizeof(saf01->transmission_timestamp),"SAF01","transmission_timestamp","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->exp_date,sizeof(saf01->exp_date),"SAF01","exp_date","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->settlement_amount,sizeof(saf01->settlement_amount),"SAF01","settlement_amount","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->settlement_date,sizeof(saf01->settlement_date),"SAF01","settlement_date","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->settlement_conv_rate,sizeof(saf01->settlement_conv_rate),"SAF01","settlement_conv_rate","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->settlement_curr_code,sizeof(saf01->settlement_curr_code),"SAF01","settlement_curr_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->billing_amount,sizeof(saf01->billing_amount),"SAF01","billing_amount","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->billing_conv_rate,sizeof(saf01->billing_conv_rate),"SAF01","billing_conv_rate","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->billing_curr_code,sizeof(saf01->billing_curr_code),"SAF01","billing_curr_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->tran_fee_amount,sizeof(saf01->tran_fee_amount),"SAF01","tran_fee_amount","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->handler_queue,sizeof(saf01->handler_queue),"SAF01","handler_queue","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->acquiring_inst_id_code,sizeof(saf01->acquiring_inst_id_code),"SAF01","acquiring_inst_id_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->forwarding_inst_id_code,sizeof(saf01->forwarding_inst_id_code),"SAF01","forwarding_inst_id_code","CheckNULLTerminatedinSAF01",mode);
	checknull(saf01->reversal_amount,sizeof(saf01->reversal_amount),"SAF01","reversal_amount","CheckNULLTerminatedinSAF01",mode);
}

void CheckNULLTerminatedinFG01(pFG01   fg01,pCHAR mode)
{
	checknull(fg01->name,sizeof(fg01->name),"FG01","name","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->type,sizeof(fg01->type),"FG01","type","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->caller_id,sizeof(fg01->caller_id),"FG01","caller_id","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->source,sizeof(fg01->source),"FG01","source","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->username,sizeof(fg01->username),"FG01","username","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->application,sizeof(fg01->application),"FG01","application","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->rule_domain,sizeof(fg01->rule_domain),"FG01","rule_domain","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->product,sizeof(fg01->product),"FG01","product","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->priority,sizeof(fg01->priority),"FG01","priority","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->tpdu_id,sizeof(fg01->tpdu_id),"FG01","tpdu_id","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->queue_name,sizeof(fg01->queue_name),"FG01","queue_name","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->request_timer,sizeof(fg01->request_timer),"FG01","request_timer","CheckNULLTerminatedinFG01",mode);
	checknull(fg01->echo_timer,sizeof(fg01->echo_timer),"FG01","echo_timer","CheckNULLTerminatedinFG01",mode);
}

void CheckNULLTerminatedinFGUARD(pFGUARD   fguard,pCHAR mode)
{
	checknull(fguard->transaction_id,sizeof(fguard->transaction_id),"FGUARD","transaction_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->timestamp,sizeof(fguard->timestamp),"FGUARD","timestamp","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->message_type,sizeof(fguard->message_type),"FGUARD","message_type","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->card_num,sizeof(fguard->card_num),"FGUARD","card_num","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->processing_code,sizeof(fguard->processing_code),"FGUARD","processing_code","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->amount_peso,sizeof(fguard->amount_peso),"FGUARD","amount_peso","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->amount_usd,sizeof(fguard->amount_usd),"FGUARD","amount_usd","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->gmt,sizeof(fguard->gmt),"FGUARD","gmt","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->sys_trace_audit_num,sizeof(fguard->sys_trace_audit_num),"FGUARD","","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->mcc,sizeof(fguard->mcc),"FGUARD","mcc","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->country_code,sizeof(fguard->country_code),"FGUARD","country_code","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->pos_entry_mode,sizeof(fguard->pos_entry_mode),"FGUARD","pos_entry_mode","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->pos_condition_code,sizeof(fguard->pos_condition_code),"FGUARD","pos_condition_code","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->acquiring_id,sizeof(fguard->acquiring_id),"FGUARD","acquiring_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->auth_number,sizeof(fguard->auth_number),"FGUARD","auth_number","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->response_code,sizeof(fguard->response_code),"FGUARD","response_code","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->terminal_id,sizeof(fguard->terminal_id),"FGUARD","terminal_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->merchant_id,sizeof(fguard->merchant_id),"FGUARD","merchant_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->card_acceptor_name,sizeof(fguard->card_acceptor_name),"FGUARD","card_acceptor_name","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->card_acceptor_city,sizeof(fguard->card_acceptor_city),"FGUARD","card_acceptor_city","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->card_acceptor_country,sizeof(fguard->card_acceptor_country),"FGUARD","card_acceptor_country","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->currency_code_txn,sizeof(fguard->currency_code_txn),"FGUARD","currency_code_txn","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->currency_code_billing,sizeof(fguard->currency_code_billing),"FGUARD","currency_code_billing","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->pos_info,sizeof(fguard->pos_info),"FGUARD","pos_info","CheckNULLTerminatedinFGUARD",mode);
	checknull((char *)fguard->status,sizeof(fguard->status),"FGUARD","status","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->system_date,sizeof(fguard->system_date),"FGUARD","system_date","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->result_code,sizeof(fguard->result_code),"FGUARD","result_code","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->application,sizeof(fguard->application),"FGUARD","application","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->rule_domain,sizeof(fguard->rule_domain),"FGUARD","rule_domain","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->caller_id,sizeof(fguard->caller_id),"FGUARD","caller_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->caller_msg_id,sizeof(fguard->caller_msg_id),"FGUARD","caller_msg_id","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->result_text,sizeof(fguard->result_text),"FGUARD","result_text","CheckNULLTerminatedinFGUARD",mode);
	checknull(fguard->product,sizeof(fguard->product),"FGUARD","product","CheckNULLTerminatedinFGUARD",mode);
}

void CheckNULLTerminatedinEMV_RAW(pEMV_RAW  emvraw,pCHAR mode)
{
	checknull(emvraw->transaction_id,sizeof(emvraw->transaction_id),"EMVRAW","transaction_id","CheckNULLTerminatedinEMV_RAW",mode);
	checknull(emvraw->system_date,sizeof(emvraw->system_date),"EMVRAW","system_date","CheckNULLTerminatedinEMV_RAW",mode);
	checknull(emvraw->DATA_LENGTH,sizeof(emvraw->DATA_LENGTH),"EMVRAW","DATA_LENGTH","CheckNULLTerminatedinEMV_RAW",mode);
	checknull(emvraw->RAW_DATA,sizeof(emvraw->RAW_DATA),"EMVRAW","RAW_DATA","CheckNULLTerminatedinEMV_RAW",mode);
}

void CheckNULLTerminatedinFG_RAW(pFG_RAW   fgraw,pCHAR mode)
{
	checknull(fgraw->transaction_id,sizeof(fgraw->transaction_id),"FGRAW","transaction_id","CheckNULLTerminatedinFG_RAW",mode);
	checknull(fgraw->system_date,sizeof(fgraw->system_date),"FGRAW","system_date","CheckNULLTerminatedinFG_RAW",mode);
	checknull(fgraw->length,sizeof(fgraw->length),"FGRAW","length","CheckNULLTerminatedinFG_RAW",mode);
	checknull(fgraw->raw_data,sizeof(fgraw->raw_data),"FGRAW","raw_data","CheckNULLTerminatedinFG_RAW",mode);
}

void CheckNULLTerminatedinTPOS01(pTPOS01 tps01,pCHAR mode )
{
	checknull(tps01->primary_key.transaction_id ,sizeof(tps01->primary_key.transaction_id),"TPOS01","primary_key.transaction_id","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->primary_key.system_date ,sizeof(tps01->primary_key.system_date),"TPOS01","primary_key.system_date","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->tx_key ,sizeof(tps01->tx_key),"TPOS01","tx_key","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->message_type,sizeof(tps01->message_type),"TPOS01","message_type","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->card_num ,sizeof(tps01->card_num),"TPOS01","card_num","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->processing_code ,sizeof(tps01->processing_code),"TPOS01","processing_code","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->sys_trace_audit_num ,sizeof(tps01->sys_trace_audit_num),"TPOS01","sys_trace_audit_num","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->time_HHMMSS ,sizeof(tps01->time_HHMMSS),"TPOS01","time_HHMMSS","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->date_YYYYMMDD ,sizeof(tps01->date_YYYYMMDD),"TPOS01","date_YYYYMMDD","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->pos_entry_mode ,sizeof(tps01->pos_entry_mode),"TPOS01","pos_entry_mode","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->card_sequence_number ,sizeof(tps01->card_sequence_number),"TPOS01","card_sequence_number","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->nii ,sizeof(tps01->nii),"TPOS01","nii","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->pos_condition_code ,sizeof(tps01->pos_condition_code),"TPOS01","pos_condition_code","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->retrieval_ref_num ,sizeof(tps01->retrieval_ref_num),"TPOS01","retrieval_ref_num","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->response_code ,sizeof(tps01->response_code),"TPOS01","response_code","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->terminal_id ,sizeof(tps01->terminal_id),"TPOS01","terminal_id","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->merchant_id ,sizeof(tps01->merchant_id),"TPOS01","merchant_id","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->bin_low ,sizeof(tps01->bin_low),"TPOS01","bin_low","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->bin_high ,sizeof(tps01->bin_high),"TPOS01","bin_high","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->bin_type ,sizeof(tps01->bin_type),"TPOS01","bin_type","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->card_family ,sizeof(tps01->card_family),"TPOS01","card_family","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->bin_description ,sizeof(tps01->bin_description),"TPOS01","bin_description","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->response_text ,sizeof(tps01->response_text),"TPOS01","response_text","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->future_use_1 ,sizeof(tps01->future_use_1),"TPOS01","future_use_1","CheckNULLTerminatedinTPOS01",mode);
	checknull(tps01->future_use_2 ,sizeof(tps01->future_use_2),"TPOS01","future_use_2","CheckNULLTerminatedinTPOS01",mode);
}
void checknull(pCHAR data , INT Len, pCHAR table, pCHAR fieldname, pCHAR funcnname, pCHAR mode)
{
	int len = 0;
	char DATA[1024]={0};
	char strTemp[1024] = {0};
	strncpy(DATA,data,Len);
	while ( *data++ !='\0')
	{
		len ++;
	}
	if(len > Len -1)
	{
		sprintf(strTemp,"DB_NULL_CHECK : Error in table %s,fieldname %s, data is %s under %s mode",table,fieldname,DATA,mode);
		Log_Db_Statics_And_Oracle_Db_Error_To_File( strTemp);
	}

}
