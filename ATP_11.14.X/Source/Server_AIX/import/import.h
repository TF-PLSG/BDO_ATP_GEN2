/*******************************************************************************
*  
* MODULE:      import.h
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent import Processor
*
* REVISION HISTORY:

*******************************************************************************/

#define OFFSET_YEAR  1900
#define OFFSET_MONTH 1
#define CORRECT      1
#define NOT_CORRECT  0

#define DATE_SIZE 9

/* record length used to read records from the different files. */
#define terminalRecordLength       202
#define primaryRecordLength        530
#define cardHoldRecordLength       750
#define balanceRecordLength        92
#define merchantRecordLength       3000 //  For supporting card types
#define negativeFraudRecordLength  46
#define mpparameterRecordLength    240
#define merchantgroupRecordLength  900
#define MAX_TPKV_LEN                48

#define POPULATE_DATA_SERVER     1
#define NOT_POPULATE_DATA_SERVER 0

#define DEFAULT_RECORD_LENGTH "200" /*Default record length used if value not found in tf.ini file*/
#define DEFAULT_KEY_LENGTH     "16" /*Default key length used if value not found in tf.ini file*/
#define MAX_KEY_LENGTH          48
#define MAX_RECORD_LENGTH      232
/* Secondary EXP DATE- TF PHANI*/
#define SECONDARY_EXP_DATE_LEN 4


#define HEADER_SIZE        498

#define FILLERCHAR		' '
#define SEPERATORCHAR	'|'

void WriteDSVHeader();
BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** hstptr, char *header);
long int findHST01FileSize(char file_name[]);
BOOLEAN OpenDSV_WriteHST01(HST01 *hst01);
void get_HST01_file_name_path(void );
void get_HST01_MCF01_file_name_path(void );
void import_HSTMCF01_create_Error_Filename();
void import_create_Error_Filename();
void Mask_sensitive_fields(pHST01 hst01, CHAR * masked_field_Previous, CHAR *masked_field_Current);
void Mask_Fields(char * fields, char * masked_fields);

/* Defination value for HST01 table for import utility*/
#define FILE_NAME "import"
#define CHANGE_BY "import utility"
#define FEILD_TAG  "import"
#define CURRENT_DATA  "New device record added is "

#define MCF01_FILE_NAME "MCF01_I"
#define MCT01_FILE_NAME "MCT01_I"

#define RESOURCE_BUSY 54

/* strucure of the terminal file. */
typedef struct 
{
   CHAR headerRecordType      [4];
   CHAR headerFileName        [9];
   CHAR headerFileCreateDate  [7];
   CHAR headerFiller          [184]; 
} myTerminalHeader, *pmyTerminalHeader;


typedef struct
{
   CHAR detailRecordType            [4];
   CHAR detailTerminalNumber        [9];
   CHAR detailMerchantNumber        [16];
   CHAR detailOrganizaNumber        [5];
   CHAR detailIndustryCode          [4];
   CHAR detailMCC                   [5];
   CHAR detailStatus                [2];
   CHAR detailTotalIndicator        [2];
   CHAR detailStatusIndicator       [2];
   CHAR detailAmexCharDesc          [24];
   CHAR detailDinnCharDesc          [24];
   CHAR detailValidTrans            [41];
   CHAR detailAutoInitAct           [2];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoInitTime          [5];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoInitRefNbr        [7];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadAct    [2];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadTime   [5];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadRefNbr [7];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoIcepacAct         [2];  /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailTPKValue              [49];
   CHAR detailDeviceType            [3];
   //CHAR detailEmvCapability         [7];
   CHAR detailFiller                [3]; 

} myTerminalDetail, *pmyTerminalDetail;

typedef struct
{
   CHAR detailRecordType            [3];
   CHAR detailTerminalNumber        [8];
   CHAR detailMerchantNumber        [15];
   CHAR detailOrganizaNumber        [4]; 
   CHAR detailIndustryCode          [3];
   CHAR detailMCC                   [4];
   CHAR detailStatus                [1];
   CHAR detailTotalIndicator        [1];
   CHAR detailStatusIndicator       [1];
   CHAR detailAmexCharDesc          [23];
   CHAR detailDinnCharDesc          [23];
   CHAR detailValidTrans            [40];
   CHAR detailAutoInitAct           [1]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoInitTime          [4]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoInitRefNbr        [6]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadAct    [1]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadTime   [4]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoProgramLoadRefNbr [6]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailAutoIcepacAct         [1]; /* 07/09/2004 LM: AutoInit Enhancements */
   CHAR detailTPKValue              [48];
   CHAR detailDeviceType            [1];
   //CHAR detailEmvCapability         [6];
   CHAR detailFiller                [2]; 
} RawTerminalDetail, *pRawTerminalDetail;

typedef struct
{
   CHAR trailRecordType       [4];
   CHAR trailFileName         [9];
   CHAR trailFileCreateDate   [7];
   CHAR trailNumberofRecords  [10];
   CHAR trailFiller           [175];   
} myTerminalTrailer, *pmyTerminalTrailer;

/* structure of the merchant file. */
typedef struct
{
   CHAR headRecordType       [4];
   CHAR headFileName         [9];
   CHAR headFileCreateDate   [7];
   CHAR headFiller           [1274];
} myMerchantHeader, *pmyMerchantHeader;

typedef struct
{
   /* changed the lenght of card type account from 5 to 4 1/14/1999 farad. */
   /* CHAR cardTypeAcc          [6]; */
   CHAR cardTypeAcc          [5];
   CHAR cardTypeMerchID      [17];
   CHAR cardTypeCorpID       [7];
   CHAR cardTypeAcquiID      [7];
   CHAR cardTypeFloorLimit   [13];
}  cardType, *pcardType;

typedef struct
{
   CHAR cardTypeAcc          [4];
   CHAR cardTypeMerchID      [16];
   CHAR cardTypeCorpID       [6];
   CHAR cardTypeAcquiID      [6];
   CHAR cardTypeFloorLimit   [12];
}  RawCardType, *pRawCardType;

typedef struct
{
   CHAR merchRecordType       [4];
   CHAR merchantNumber        [16];
   CHAR merchOrginNumb        [5];    /*  farad 3/17/1999 bug #252 CHAR merchOrginNumb        [4]; */
   CHAR merchStatus           [2];
   CHAR merchLegalName        [31];
   CHAR merchDBA              [31];
   CHAR merchAddres1          [31];
   CHAR merchAddres2          [31];
   CHAR merchCity             [19];
   CHAR merchCounty           [16];
   CHAR merchState            [3];
   CHAR merchZipCode          [11];
   CHAR merchCurrencyCode     [4];
   CHAR merchContact          [31];
   CHAR merchTelphone1        [18];
   CHAR merchTelphone2        [18];
   CHAR merchInvoiceBatch     [4];
   CHAR merchInvoiceSubcode   [3];
   CHAR merchProcContId       [7];
   CHAR merchTotalCardType    [4];
   CHAR merchRegionNumber     [4];
   CHAR merchDiviNumber       [4];
   CHAR merchPassWord         [8];
   CHAR merchRPS              [2];
   CHAR merchHouCardMerch     [2];
   CHAR merchProcMerch        [2];
   CHAR merchDeffMerch        [2];
   CHAR merchCashBonusElg     [2];
   CHAR merchDefMaxAcc        [11];
   CHAR merchDefStartDate     [9];
   CHAR merchDefEndDate       [9];
   CHAR merchDefPayTerm1      [4];
   CHAR merchDefFactor1       [8];  /* CHAR merchDefFactor1       [6];  modified by farad on 2/22/1999 for enhancement */
   CHAR merchDefMinAmount1    [11];
   CHAR merchDefPayTerm2      [4];
   CHAR merchDefFactor2       [8];  /* CHAR merchDefFactor2       [6];  modified by farad on 2/22/1999 for enhancement  */
   CHAR merchDefMinAmount2    [11];
   CHAR merchDefPayTerm3      [4];
   CHAR merchDefFactor3       [8];  /* CHAR merchDefFactor3       [6];  modified by farad on 2/22/1999 for enhancement  */
   CHAR merchDefMinAmount3    [11];
   CHAR merchDefPayTerm4      [4];
   CHAR merchDefFactor4       [8];  /* CHAR merchDefFactor4       [6];  modified by farad on 2/22/1999 for enhancement  */
   CHAR merchDefMinAmount4    [11];
   CHAR merchDefPayTerm5      [4];
   CHAR merchDefFactor5       [8];  /* CHAR merchDefFactor5       [6];  modified by farad on 2/22/1999 for enhancement  */
   CHAR merchDefMinAmount5    [11];
   CHAR merchDefPayTerm6      [4];
   CHAR merchDefFactor6       [8];  /* CHAR merchDefFactor6       [6];  modified by farad on 2/22/1999 for enhancement  */
   CHAR merchDefMinAmount6    [11];
   CHAR merchCredPurcMax      [13];
   CHAR merchCredCashBack     [13];
   CHAR merchCredAuthMax      [13];
   CHAR merchCredRetMax       [13];
   CHAR merchCredCashAdvMax   [13];
   CHAR merchCredManEntMax    [13];
   CHAR merchDebitPurchMax    [13];
   CHAR merchDebitCashBackMax [13];
   CHAR merchDebitAuthMax     [13];
   CHAR merchDebitRetMax      [13];
   CHAR merchDebitCashAdvMax  [13];
   CHAR merchDebitManEntMax   [13];
   CHAR merchDateAdded        [9];
   CHAR merchTotalIndicator   [2];
   CHAR merchStatsIndicator   [2];
   CHAR merchVipcode          [5];
   CHAR merchPosCode          [2];
   CHAR merchMCC              [5];  /* added by farad on 3/17/1999 */
   CHAR merchMSTCCcode        [2];
   CHAR merchMCFCCcode        [2];
   CHAR merchJCPSubCode       [3];
   CHAR merchMallCode         [4];
   CHAR merchHCCode           [4];
   CHAR merchSolicCode        [4];
   CHAR classCode             [4];  /* added by farad on 2/22/1999 for enhancments required by Rod. */
   CHAR merchGroupOutletCode  [6];
   CHAR merchGroupType        [5];
   cardType merchCardType1;
   cardType merchCardType2;
   cardType merchCardType3;
   cardType merchCardType4;
   cardType merchCardType5;
   cardType merchCardType6;
   cardType merchCardType7;
   cardType merchCardType8;
   cardType merchCardType9;
   cardType merchCardType10;
   cardType merchCardType11;
   cardType merchCardType12;
   cardType merchCardType13;
   cardType merchCardType14;
   cardType merchCardType15;
   cardType merchCardType16;
   cardType merchCardType17;
   cardType merchCardType18;
   cardType merchCardType19;
   cardType merchCardType20;
   cardType merchCardType21;
   cardType merchCardType22;
   cardType merchCardType23;
   cardType merchCardType24;
   cardType merchCardType25;
   cardType merchCardType26;
   cardType merchCardType27;
   cardType merchCardType28;
   cardType merchCardType29;
   cardType merchCardType30;
   cardType merchCardType31;
   cardType merchCardType32;
   cardType merchCardType33;
   cardType merchCardType34;
   cardType merchCardType35;
   cardType merchCardType36;
   cardType merchCardType37;
   cardType merchCardType38;
   cardType merchCardType39;
   cardType merchCardType40; /* from 21 to 40 added to support extra card type inclusion*/
   CHAR reload_merchant_flag  [2];	/*lm 07/9/2004-Merchant Reload transaction support*/
   CHAR own_merchant[11];			/* Added for Falcon req -TF PHANI*/
   CHAR mastercard_assigned_id [7];
   CHAR cup_acceptor_id          [16];
   CHAR merchGroupId [16];
   CHAR visa_mvv[11];
   CHAR mc_mco[4];
   CHAR merchFiller           [8];
}  myMerchantDetail, *pmyMerchantDetail;


typedef struct
{
   CHAR merchRecordType       [3];
   CHAR merchantNumber        [15];
   CHAR merchOrginNumb        [4];    
   CHAR merchStatus           [1];
   CHAR merchLegalName        [30];
   CHAR merchDBA              [30];
   CHAR merchAddres1          [30];
   CHAR merchAddres2          [30];
   CHAR merchCity             [18];
   CHAR merchCounty           [15];
   CHAR merchState            [2];
   CHAR merchZipCode          [10];
   CHAR merchCurrencyCode     [3];
   CHAR merchContact          [30];
   CHAR merchTelphone1        [17];
   CHAR merchTelphone2        [17];
   CHAR merchInvoiceBatch     [3];
   CHAR merchInvoiceSubcode   [2];
   CHAR merchProcContId       [6];
   CHAR merchTotalCardType    [3];
   CHAR merchRegionNumber     [3];
   CHAR merchDiviNumber       [3];
   CHAR merchPassWord         [7];
   CHAR merchRPS              [1];
   CHAR merchHouCardMerch     [1];
   CHAR merchProcMerch        [1];
   CHAR merchDeffMerch        [1];
   CHAR merchCashBonusElg     [1];
   CHAR merchDefMaxAcc        [10];
   CHAR merchDefStartDate     [8];
   CHAR merchDefEndDate       [8];
   CHAR merchDefPayTerm1      [3];
   CHAR merchDefFactor1       [7];  
   CHAR merchDefMinAmount1    [10];
   CHAR merchDefPayTerm2      [3];
   CHAR merchDefFactor2       [7];  
   CHAR merchDefMinAmount2    [10];
   CHAR merchDefPayTerm3      [3];
   CHAR merchDefFactor3       [7];  
   CHAR merchDefMinAmount3    [10];
   CHAR merchDefPayTerm4      [3];
   CHAR merchDefFactor4       [7];  
   CHAR merchDefMinAmount4    [10];
   CHAR merchDefPayTerm5      [3];
   CHAR merchDefFactor5       [7];  
   CHAR merchDefMinAmount5    [10];
   CHAR merchDefPayTerm6      [3];
   CHAR merchDefFactor6       [7];  
   CHAR merchDefMinAmount6    [10];
   CHAR merchCredPurcMax      [12];
   CHAR merchCredCashBack     [12];
   CHAR merchCredAuthMax      [12];
   CHAR merchCredRetMax       [12];
   CHAR merchCredCashAdvMax   [12];
   CHAR merchCredManEntMax    [12];
   CHAR merchDebitPurchMax    [12];
   CHAR merchDebitCashBackMax [12];
   CHAR merchDebitAuthMax     [12];
   CHAR merchDebitRetMax      [12];
   CHAR merchDebitCashAdvMax  [12];
   CHAR merchDebitManEntMax   [12];
   CHAR merchDateAdded        [8];
   CHAR merchTotalIndicator   [1];
   CHAR merchStatsIndicator   [1];
   CHAR merchVipcode          [4];
   CHAR merchPosCode          [1];
   CHAR merchMCC              [4];  
   CHAR merchMSTCCcode        [1];
   CHAR merchMCFCCcode        [1];
   CHAR merchJCPSubCode       [2];
   CHAR merchMallCode         [3];
   CHAR merchHCCode           [3];
   CHAR merchSolicCode        [3];
   CHAR classCode             [3];  
   CHAR merchGroupOutletCode  [5];
   CHAR merchGroupType        [4];
   RawCardType merchCardType1;
   RawCardType merchCardType2;
   RawCardType merchCardType3;
   RawCardType merchCardType4;
   RawCardType merchCardType5;
   RawCardType merchCardType6;
   RawCardType merchCardType7;
   RawCardType merchCardType8;
   RawCardType merchCardType9;
   RawCardType merchCardType10;
   RawCardType merchCardType11;
   RawCardType merchCardType12;
   RawCardType merchCardType13;
   RawCardType merchCardType14;
   RawCardType merchCardType15;
   RawCardType merchCardType16;
   RawCardType merchCardType17;
   RawCardType merchCardType18;
   RawCardType merchCardType19;
   RawCardType merchCardType20;
   RawCardType merchCardType21;
   RawCardType merchCardType22;
   RawCardType merchCardType23;
   RawCardType merchCardType24;
   RawCardType merchCardType25;
   RawCardType merchCardType26;
   RawCardType merchCardType27;
   RawCardType merchCardType28;
   RawCardType merchCardType29;
   RawCardType merchCardType30;
   RawCardType merchCardType31;
   RawCardType merchCardType32;
   RawCardType merchCardType33;
   RawCardType merchCardType34;
   RawCardType merchCardType35;
   RawCardType merchCardType36;
   RawCardType merchCardType37;
   RawCardType merchCardType38;
   RawCardType merchCardType39;
   RawCardType merchCardType40;
   CHAR reload_merchant_flag  [1];	/*lm 07/9/2004-Merchant Reload transaction support*/
   CHAR own_merchant[10];			/* Added for Falcon req -TF PHANI*/
   CHAR mastercard_assigned_id [6];   
   CHAR cup_acceptor_id [15];
   CHAR merchGroupId [15];
   CHAR visa_mvv[10];
   CHAR mc_mco[3];
   CHAR merchFiller           [7];
}  RawMerchantDetail, *pRawMerchantDetail;


typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [1265];
} myMerchantTrailer, *pmyMerchantTrailer;

/* structure of the negative fraud file. */
typedef struct 
{
   CHAR headRecordType     [4];
   CHAR headFileName       [9];
   CHAR headFileCreateDate [7];
} myNegFraudHeader, *pmyNegFraudHeader;

typedef struct
{
   CHAR detailRecordType     [4];
   CHAR detailMCC            [6];
   CHAR detailBinLow         [17];
   CHAR detailBinHigh        [17];
   CHAR detailPanLength      [3];
   CHAR detailCountryCode    [4];

} myNegFraudDetail, *pmyNegFraudDetail;

typedef struct
{
   CHAR detailRecordType     [3];
   CHAR detailMCC            [5];
   CHAR detailBinLow         [16];
   CHAR detailBinHigh        [16];
   CHAR detailPanLength      [2];
   CHAR detailCountryCode    [3];  

} RawNegFraudDetail, *pRawNegFraudDetail;

typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecords [7];
} myNegFraudTrailer, *pmyNegFraudTrailer;

/* structure of the primary file. */
typedef struct 
{
   CHAR headRecordType     [4];
   CHAR headFileName       [9];
   CHAR headFileCreateDate [7];
   CHAR headFiller         [234];  /* farad 1/16/1999 - CHAR headFiller         [214]; */
} myPrimaryHeader, *pmyPrimaryHeader;

typedef struct
{
   CHAR detailRecordType     [4];
   CHAR detailAccount        [20];
   CHAR detailNameDBA        [151];
   CHAR detailAddres1        [31];
   CHAR detailAddres2        [31];
   CHAR detailCity           [21];
   CHAR detailState          [4];
   CHAR detailZipCode        [11];
   CHAR detailHomePhone      [31];
   CHAR detailWorkPhone      [31];
   CHAR detailStatus         [3];
   CHAR detailBlockStatus    [5];
   CHAR detailCoorAccountNo  [11];
   CHAR detailAcountName     [31];
   CHAR detailPostDueAmount  [13];  
   CHAR detailMinPayCode     [2];
   CHAR detailCredit_limit   [11];
   CHAR detailCustr_nbr	 	 [13];
   CHAR detailAPP_Source	 [11];
   CHAR detailCOMP_Name	  	 [101];
   CHAR detailFiller         [13];

} myPrimaryDetail, *pmyPrimaryDetail;

typedef struct
{
   CHAR detailRecordType     [3];
   CHAR detailAccount        [19];
   CHAR detailNameDBA        [150];
   CHAR detailAddres1        [30];
   CHAR detailAddres2        [30];
   CHAR detailCity           [20];
   CHAR detailState          [3];
   CHAR detailZipCode        [10];
   CHAR detailHomePhone      [30];
   CHAR detailWorkPhone      [30];
   CHAR detailStatus         [2];
   CHAR detailCafProfileId	 [4];
   CHAR detailtype			 [1];
   CHAR detailCoorAccountNo  [10];
   CHAR detailBlockStatus    [4];
   CHAR detailAcountName     [30];
   CHAR detailPostDueAmount  [12];  
   CHAR detailMinPayCode     [1];
   CHAR detailCredit_limit   [10];
   CHAR detailCustr_nbr	 	 [12];
   CHAR detailAPP_Source	 [10];
   CHAR detailCOMP_Name	 	 [100];
   CHAR detailFiller         [12];

} RawPrimaryDetail, *pRawPrimaryDetail;

typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [225]; /*  farad 1/15/1999 CHAR trailFiller         [208]; */
} myPrimaryTrailer, *pmyPrimaryTrailer;


/* struct for the card holder file. */
typedef struct
{
   CHAR headRecordType     [4];
   CHAR headFileName       [9];
   CHAR headFileCreateDate [7];
   CHAR headFiller         [524];
} myCardHoldHeader, *pmyCardHoldHeader;

typedef struct
{
   CHAR detailRecordType      [4];
   CHAR detailCardType        [5];
   CHAR detailCardNumber      [20];
   CHAR detailStatus          [3];
   CHAR detailPrimaryAccount  [20];
   CHAR detailNameDBA         [151];
   CHAR detailAddres1         [31];
   CHAR detailAddres2         [31];
   CHAR detailCity            [21];
   CHAR detailState           [4];
   CHAR detailZipCode         [11];
   CHAR detailCounty          [21];
   CHAR detailHomePhone       [31];
   CHAR detailWorkPhone       [31];
   CHAR detailDateBirth       [9];
   CHAR detailProfession      [21];
   CHAR detailSpeciCode       [3];
   CHAR detailVIPAccountCode  [2];
   CHAR detailSex             [2];
   CHAR detailMothMaidenName  [31];
   CHAR detailMembershipID    [10];
   CHAR detailCardHolder      [20];
   CHAR detailBlockStatus     [5];
   CHAR detailPinOffSet       [7];
   CHAR detailPesoSaveAccount [13];
   CHAR detailPesoCurrAccount [13];
   CHAR detailDollarSaveAcc   [13];
   CHAR detailDollarCurrAcc   [13];
   CHAR detailProductCode     [21];
   CHAR detailNumberofCard    [3];
   CHAR detailDateAdded       [9];
   CHAR detailExpirDate       [5];
   CHAR detailEffectiveDate   [9];
   CHAR detailPesoAccount     [20];
   CHAR detailUSDolAccount    [20];
   CHAR detailDefferedAccont  [20];
   CHAR detailHouseCardAcc    [20];
   CHAR detailCashBounEligib  [2];
   CHAR detailVisaPurchCard   [2];
   CHAR detailTransAllowed    [17];
   CHAR detailAddOnRate       [6];
   CHAR detailAddOnTerm       [4];
   CHAR detailPesoTranLimit   [13];
   CHAR detailDollarTranLimit [13];
   CHAR detailMaint_date	  [5];
   CHAR detailPanSeqNbr		  [3];
   CHAR IBMOffset             [13];
   CHAR Mobilenumber	      [31];

} myCardHoldDetail, *pmyCardHoldDetail;

typedef struct
{
   CHAR detailRecordType      [3];
   CHAR detailCardType        [4];
   CHAR detailCardNumber      [19];
   CHAR detailStatus          [2];
   CHAR detailPrimaryAccount  [19];
   CHAR detailNameDBA         [150];
   CHAR detailAddres1         [30];
   CHAR detailAddres2         [30];
   CHAR detailCity            [20];
   CHAR detailState           [3];
   CHAR detailZipCode         [10];
   CHAR detailCounty          [20];
   CHAR detailHomePhone       [30];
   CHAR detailWorkPhone       [30];
   CHAR detailDateBirth       [8];
   CHAR detailProfession      [20];
   CHAR detailSpeciCode       [2];
   CHAR detailVIPAccountCode  [1];
   CHAR detailSex             [1];
   CHAR detailMothMaidenName  [30];
   CHAR detailMembershipID    [9];
   CHAR detailCardHolder      [19];
   CHAR detailBlockStatus     [4];
   CHAR detailPinOffSet       [6];
   CHAR detailPesoSaveAccount [12];
   CHAR detailPesoCurrAccount [12];
   CHAR detailDollarSaveAcc   [12];
   CHAR detailDollarCurrAcc   [12];
   CHAR detailProductCode     [20];
   CHAR detailNumberofCard    [2];
   CHAR detailDateAdded       [8];
   CHAR detailExpirDate       [4];
   CHAR detailEffectiveDate   [8];
   CHAR detailPesoAccount     [19];
   CHAR detailUSDolAccount    [19];
   CHAR detailDefferedAccont  [19];
   CHAR detailHouseCardAcc    [19];
   CHAR detailCashBounEligib  [1];
   CHAR detailVisaPurchCard   [1];
   CHAR detailTransAllowed    [16];
   CHAR detailAddOnRate       [5];
   CHAR detailAddOnTerm       [3];
   CHAR detailPesoTranLimit   [12];
   CHAR detailDollarTranLimit [12];
   CHAR detailMaint_date      [4];
   CHAR detailPanSeqNbr		  [2];
   CHAR IBMOffset             [12];
   CHAR Mobilenumber	      [30];
   CHAR detailFiller		  [4];
 } RawCardHoldDetail, *pRawCardHoldDetail;

typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [514];
} myCardHoldTrailer, *pmyCardHoldTrailer;

/* struct for the Balance Acount file. */
typedef struct
{
   CHAR headRecordType     [4];
   CHAR headFileName       [9];
   CHAR headFileCreateDate [7];
   CHAR headFiller         [74];
} myBalanceHeader, *pmyBalanceHeader;

typedef struct
{
   CHAR detailRecordType      [4];
   CHAR detailAccType         [3];
   CHAR detailAccountNum      [20];
   CHAR detailStatus          [2];
   CHAR detailDateAdded       [9];
   CHAR detailCreditLimit     [13];
   CHAR detailOutBalance      [13];
   CHAR detailCashLimit       [13];
   CHAR detailCashOutBal      [13];
   CHAR detailFiller          [10];
} myBalanceDetail, *pmyBalanceDetail;

typedef struct
{
   CHAR detailRecordType      [3];
   CHAR detailAccType         [2];
   CHAR detailAccountNum      [19];
   CHAR detailStatus          [1];
   CHAR detailDateAdded       [8];
   CHAR detailCreditLimit     [12];
   CHAR detailOutBalance      [12];
   CHAR detailCashLimit       [12];
   CHAR detailCashOutBal      [12];
   CHAR detailFiller          [9];
} RawBalanceDetail, *pRawBalanceDetail;

typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [65];
} myBalanceTrailer, *pmyBalanceTrailer;

/* structure of the MpParameter file. */


/* structure of the MpParameter header. */
typedef struct
{
   CHAR headRecordType       [4];
   CHAR headFileName         [9];
   CHAR headFileCreateDate   [7];
   CHAR headFiller           [1274];
} myMpParameterHeader, *pmyMpParameterHeader;


/* structure of the MpParameter detail. */
typedef struct
{
   CHAR mpparaRecordType                  [4];
   CHAR mpparaProductCode                 [5];
   CHAR mpparaMinMajorPurchaseAmount      [13];    
   CHAR mpparaMaxMajorPurchaseAmount      [13];
   CHAR mpparaMinInstallmentAmount        [13];
   CHAR mpparaMaxInstallmentAmount        [13];
   CHAR mpparaMinMajorPurchaseLimit       [11];
   CHAR mpparaMinPenaltyAmount            [16];
   CHAR mpparaMaxPenaltyAmount            [16];
   CHAR mpparaPenaltyPercentage           [8];
   CHAR mpparaPenaltyDays                 [4];
   CHAR mpparaMinAmtForAging              [19];
   CHAR mpparaMonthsToWriteOff            [3];
   CHAR mpparaBPIMinimumAmount            [13];
   CHAR mpparaBrokenPeriodInterest        [2];
   CHAR mpparaMinPurchaseAmtForFee        [13];
   CHAR mpparaErrorUnauthorizedAmount     [16];
   CHAR mpparaFixedMPFee                  [16];
   CHAR mpparaMPFeePercentage             [7];
   CHAR mpparaEarlyRepaymentFeePercentage [9];
   CHAR mpparaEffectiveDate               [9];
   CHAR mpparaMaxNoOfTransPerDay          [3];
   CHAR mpparaMaxAmountPerDay             [16];
   CHAR mpparaAuthPercentOverlimit        [8];
   CHAR mpparaRespCode1                   [3];
   CHAR mpparaMessage1                    [3];
   CHAR mpparaRespCode2                   [3];
   CHAR mpparaMessage2                    [3];
   CHAR mpparaRespCode3                   [3];
   CHAR mpparaMessage3                    [3];
   CHAR mpparaFiller                      [4];
   }  myMpParameterDetail, *pmyMpParameterDetail;



typedef struct
{
   CHAR mpparaRecordType                  [3];
   CHAR mpparaProductCode                 [4];
   CHAR mpparaMinMajorPurchaseAmount      [12];    
   CHAR mpparaMaxMajorPurchaseAmount      [12];
   CHAR mpparaMinInstallmentAmount        [12];
   CHAR mpparaMaxInstallmentAmount        [12];
   CHAR mpparaMinMajorPurchaseLimit       [10];
   CHAR mpparaMinPenaltyAmount            [15];
   CHAR mpparaMaxPenaltyAmount            [15];
   CHAR mpparaPenaltyPercentage           [7];
   CHAR mpparaPenaltyDays                 [3];
   CHAR mpparaMinAmtForAging              [18];
   CHAR mpparaMonthsToWriteOff            [2];
   CHAR mpparaBPIMinimumAmount            [12];
   CHAR mpparaBrokenPeriodInterest        [1];
   CHAR mpparaMinPurchaseAmtForFee        [12];
   CHAR mpparaErrorUnauthorizedAmount     [15];
   CHAR mpparaFixedMPFee                  [15];
   CHAR mpparaMPFeePercentage             [6];
   CHAR mpparaEarlyRepaymentFeePercentage [8];
   CHAR mpparaEffectiveDate               [8];
   CHAR mpparaMaxNoOfTransPerDay          [2];
   CHAR mpparaMaxAmountPerDay             [15];
   CHAR mpparaAuthPercentOverlimit        [7];
   CHAR mpparaRespCode1                   [2];
   CHAR mpparaMessage1                    [2];
   CHAR mpparaRespCode2                   [2];
   CHAR mpparaMessage2                    [2];
   CHAR mpparaRespCode3                   [2];
   CHAR mpparaMessage3                    [2];
   CHAR mpparaFiller                      [3];
   }  RawMpParameterDetail, *pRawMpParameterDetail;


/* structure of the MpParameter trailer. */

   typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [1265];
} myMpParameterTrailer, *pmyMpParameterTrailer;


/* structure of the MerchantGroup header. */
typedef struct
{
   CHAR headRecordType       [4];
   CHAR headFileName         [9];
   CHAR headFileCreateDate   [7];
   CHAR headFiller           [1274];
} myMerchantGroupHeader, *pmyMerchantGroupHeader;


/* structure of the MerchantGroup detail. */

typedef struct
{
     CHAR mgfRecordType                 [3];
	 CHAR mgfMerchantGroupID 			[15];		
	 CHAR mgfEffectiveDate				[8];
	 
	 CHAR mgfAmount_limit_1				[12];
	 CHAR mgfnbr_of_months_1			[2];
	 CHAR mgfINT_rate_1					[7];
	 CHAR mgfMSF_rate_1					[7];
	 CHAR mgfnbr_of_months_2			[2];
	 CHAR mgfINT_rate_2					[7];
	 CHAR mgfMSF_rate_2					[7];
	 CHAR mgfnbr_of_months_3			[2];
	 CHAR mgfINT_rate_3					[7];
	 CHAR mgfMSF_rate_3					[7];
	 CHAR mgfnbr_of_months_4			[2];
	 CHAR mgfINT_rate_4					[7];
	 CHAR mgfMSF_rate_4					[7];
	 CHAR mgfnbr_of_months_5			[2];
	 CHAR mgfINT_rate_5					[7];
	 CHAR mgfMSF_rate_5					[7];
	 CHAR mgfnbr_of_months_6			[2];
	 CHAR mgfINT_rate_6					[7];
	 CHAR mgfMSF_rate_6					[7];
	 CHAR mgfnbr_of_months_7			[2];
	 CHAR mgfINT_rate_7					[7];
	 CHAR mgfMSF_rate_7					[7];
	 CHAR mgfnbr_of_months_8			[2];
	 CHAR mgfINT_rate_8					[7];
	 CHAR mgfMSF_rate_8					[7];
	 CHAR mgfnbr_of_months_9			[2];
	 CHAR mgfINT_rate_9					[7];
	 CHAR mgfMSF_rate_9					[7];
	 CHAR mgfnbr_of_months_10 			[2];
	 CHAR mgfINT_rate_10				[7];
	 CHAR mgfMSF_rate_10				[7];
	 CHAR mgfnbr_of_months_10A 			[2];
	 CHAR mgfINT_rate_10A				[7];
	 CHAR mgfMSF_rate_10A				[7];
	 CHAR mgfnbr_of_months_10B 			[2];
	 CHAR mgfINT_rate_10B				[7];
	 CHAR mgfMSF_rate_10B				[7];
	 
	 CHAR mgfAmount_limit_2			    [12];
	 CHAR mgfnbr_of_months_11 			[2];
	 CHAR mgfINT_rate_11				[7];
	 CHAR mgfMSF_rate_11				[7];
	 CHAR mgfnbr_of_months_12 			[2];
	 CHAR mgfINT_rate_12				[7];
	 CHAR mgfMSF_rate_12				[7];
	 CHAR mgfnbr_of_months_13 			[2];
	 CHAR mgfINT_rate_13				[7];
	 CHAR mgfMSF_rate_13				[7];
	 CHAR mgfnbr_of_months_14 			[2];
	 CHAR mgfINT_rate_14				[7];
	 CHAR mgfMSF_rate_14				[7];
	 CHAR mgfnbr_of_months_15 			[2];
	 CHAR mgfINT_rate_15				[7];
	 CHAR mgfMSF_rate_15				[7];
	 CHAR mgfnbr_of_months_16 			[2];
	 CHAR mgfINT_rate_16				[7];
	 CHAR mgfMSF_rate_16				[7];
	 CHAR mgfnbr_of_months_17 			[2];
	 CHAR mgfINT_rate_17				[7];
	 CHAR mgfMSF_rate_17				[7];
	 CHAR mgfnbr_of_months_18           [2];
	 CHAR mgfINT_rate_18				[7];
	 CHAR mgfMSF_rate_18				[7];
	 CHAR mgfnbr_of_months_19 			[2];
	 CHAR mgfINT_rate_19				[7];
	 CHAR mgfMSF_rate_19				[7];
	 CHAR mgfnbr_of_months_20 			[2];
	 CHAR mgfINT_rate_20				[7];
	 CHAR mgfMSF_rate_20				[7];
	 CHAR mgfnbr_of_months_20A 			[2];
	 CHAR mgfINT_rate_20A				[7];
	 CHAR mgfMSF_rate_20A				[7];
	 CHAR mgfnbr_of_months_20B 			[2];
	 CHAR mgfINT_rate_20B				[7];
	 CHAR mgfMSF_rate_20B				[7];
	 
	 CHAR mgfAmount_limit_3				[12];
	 CHAR mgfnbr_of_months_21 			[2];
	 CHAR mgfINT_rate_21				[7];
	 CHAR mgfMSF_rate_21				[7];
	 CHAR mgfnbr_of_months_22 			[2];
	 CHAR mgfINT_rate_22				[7];
	 CHAR mgfMSF_rate_22				[7];
	 CHAR mgfnbr_of_months_23 			[2];
	 CHAR mgfINT_rate_23				[7];
	 CHAR mgfMSF_rate_23				[7];
	 CHAR mgfnbr_of_months_24 			[2];
	 CHAR mgfINT_rate_24				[7];
	 CHAR mgfMSF_rate_24				[7];
	 CHAR mgfnbr_of_months_25 			[2];
	 CHAR mgfINT_rate_25				[7];
	 CHAR mgfMSF_rate_25				[7];
	 CHAR mgfnbr_of_months_26 			[2];
	 CHAR mgfINT_rate_26				[7];
	 CHAR mgfMSF_rate_26				[7];
	 CHAR mgfnbr_of_months_27 			[2];
	 CHAR mgfINT_rate_27				[7];
	 CHAR mgfMSF_rate_27				[7];
	 CHAR mgfnbr_of_months_28 		    [2];
	 CHAR mgfINT_rate_28				[7];
	 CHAR mgfMSF_rate_28				[7];
	 CHAR mgfnbr_of_months_29 			[2];
	 CHAR mgfINT_rate_29				[7];
	 CHAR mgfMSF_rate_29				[7];	 
	 CHAR mgfnbr_of_months_30 			[2];
	 CHAR mgfINT_rate_30				[7];
	 CHAR mgfMSF_rate_30				[7];
	 CHAR mgfnbr_of_months_30A 			[2];
	 CHAR mgfINT_rate_30A				[7];
	 CHAR mgfMSF_rate_30A				[7];
	 CHAR mgfnbr_of_months_30B 			[2];
	 CHAR mgfINT_rate_30B				[7];
	 CHAR mgfMSF_rate_30B				[7];
	 
	 CHAR mgfAmount_limit_4				[12];
	 CHAR mgfnbr_of_months_31 			[2];
 	 CHAR mgfINT_rate_31				[7];
	 CHAR mgfMSF_rate_31				[7];
	 CHAR mgfnbr_of_months_32 			[2];
	 CHAR mgfINT_rate_32				[7];
	 CHAR mgfMSF_rate_32				[7];
	 CHAR mgfnbr_of_months_33 			[2];
	 CHAR mgfINT_rate_33				[7];
	 CHAR mgfMSF_rate_33				[7];
	 CHAR mgfnbr_of_months_34 			[2];
	 CHAR mgfINT_rate_34				[7];
	 CHAR mgfMSF_rate_34				[7];
	 CHAR mgfnbr_of_months_35 			[2];
	 CHAR mgfINT_rate_35				[7];
	 CHAR mgfMSF_rate_35				[7];
	 CHAR mgfnbr_of_months_36 			[2];
	 CHAR mgfINT_rate_36				[7];
	 CHAR mgfMSF_rate_36				[7];
	 CHAR mgfnbr_of_months_37 			[2];
	 CHAR mgfINT_rate_37				[7];
	 CHAR mgfMSF_rate_37				[7];
	 CHAR mgfnbr_of_months_38 			[2];
	 CHAR mgfINT_rate_38				[7];
	 CHAR mgfMSF_rate_38				[7];
	 CHAR mgfnbr_of_months_39 		    [2];
	 CHAR mgfINT_rate_39				[7];
	 CHAR mgfMSF_rate_39				[7];
	 CHAR mgfnbr_of_months_40 	        [2];
	 CHAR mgfINT_rate_40				[7];
	 CHAR mgfMSF_rate_40                [7];
	 CHAR mgfnbr_of_months_40A 	        [2];
	 CHAR mgfINT_rate_40A				[7];
	 CHAR mgfMSF_rate_40A               [7];
	 CHAR mgfnbr_of_months_40B 	        [2];
	 CHAR mgfINT_rate_40B				[7];
	 CHAR mgfMSF_rate_40B               [7];
	 
	 CHAR mgfFiller                      [10];
}RawMerchantGroupDetail, *pRawMerchantGroupDetail;




typedef struct
{
     CHAR mgfRecordType                 [4];
	 CHAR mgfMerchantGroupID 			[16];		
	 CHAR mgfEffectiveDate				[9];
	 
	 CHAR mgfAmount_limit_1				[13];
	 CHAR mgfnbr_of_months_1			[3];
	 CHAR mgfINT_rate_1					[8];
	 CHAR mgfMSF_rate_1					[8];
	 CHAR mgfnbr_of_months_2			[3];
	 CHAR mgfINT_rate_2					[8];
	 CHAR mgfMSF_rate_2					[8];
	 CHAR mgfnbr_of_months_3			[3];
	 CHAR mgfINT_rate_3					[8];
	 CHAR mgfMSF_rate_3					[8];
	 CHAR mgfnbr_of_months_4			[3];
	 CHAR mgfINT_rate_4					[8];
	 CHAR mgfMSF_rate_4					[8];
	 CHAR mgfnbr_of_months_5			[3];
	 CHAR mgfINT_rate_5					[8];
	 CHAR mgfMSF_rate_5					[8];
	 CHAR mgfnbr_of_months_6			[3];
	 CHAR mgfINT_rate_6					[8];
	 CHAR mgfMSF_rate_6					[8];
	 CHAR mgfnbr_of_months_7			[3];
	 CHAR mgfINT_rate_7					[8];
	 CHAR mgfMSF_rate_7					[8];
	 CHAR mgfnbr_of_months_8			[3];
	 CHAR mgfINT_rate_8					[8];
	 CHAR mgfMSF_rate_8					[8];
	 CHAR mgfnbr_of_months_9			[3];
	 CHAR mgfINT_rate_9					[8];
	 CHAR mgfMSF_rate_9					[8];
	 CHAR mgfnbr_of_months_10           [3];
	 CHAR mgfINT_rate_10				[8];
	 CHAR mgfMSF_rate_10				[8];
	 CHAR mgfnbr_of_months_10A          [3];
	 CHAR mgfINT_rate_10A				[8];
	 CHAR mgfMSF_rate_10A				[8];
	 CHAR mgfnbr_of_months_10B          [3];
	 CHAR mgfINT_rate_10B				[8];
	 CHAR mgfMSF_rate_10B				[8];
	 
	 CHAR mgfAmount_limit_2		        [13];
	 CHAR mgfnbr_of_months_11 	        [3];
	 CHAR mgfINT_rate_11				[8];
	 CHAR mgfMSF_rate_11				[8];
	 CHAR mgfnbr_of_months_12 	        [3];
	 CHAR mgfINT_rate_12				[8];
	 CHAR mgfMSF_rate_12				[8];
	 CHAR mgfnbr_of_months_13 	        [3];
	 CHAR mgfINT_rate_13				[8];
	 CHAR mgfMSF_rate_13				[8];
	 CHAR mgfnbr_of_months_14 	        [3];
	 CHAR mgfINT_rate_14				[8];
	 CHAR mgfMSF_rate_14				[8];
	 CHAR mgfnbr_of_months_15 	        [3];
	 CHAR mgfINT_rate_15				[8];
	 CHAR mgfMSF_rate_15				[8];
	 CHAR mgfnbr_of_months_16 	        [3];
	 CHAR mgfINT_rate_16				[8];
	 CHAR mgfMSF_rate_16				[8];
	 CHAR mgfnbr_of_months_17 	        [3];
	 CHAR mgfINT_rate_17				[8];
	 CHAR mgfMSF_rate_17				[8];
	 CHAR mgfnbr_of_months_18 	        [3];
	 CHAR mgfINT_rate_18				[8];
	 CHAR mgfMSF_rate_18				[8];
	 CHAR mgfnbr_of_months_19 	        [3];
	 CHAR mgfINT_rate_19				[8];
	 CHAR mgfMSF_rate_19				[8];
	 CHAR mgfnbr_of_months_20 	        [3];
	 CHAR mgfINT_rate_20				[8];
	 CHAR mgfMSF_rate_20				[8];
	 CHAR mgfnbr_of_months_20A 	        [3];
	 CHAR mgfINT_rate_20A				[8];
	 CHAR mgfMSF_rate_20A				[8];
	 CHAR mgfnbr_of_months_20B 	        [3];
	 CHAR mgfINT_rate_20B				[8];
	 CHAR mgfMSF_rate_20B				[8];
	 
	 CHAR mgfAmount_limit_3				[13];
	 CHAR mgfnbr_of_months_21 	        [3];
	 CHAR mgfINT_rate_21				[8];
	 CHAR mgfMSF_rate_21				[8];
	 CHAR mgfnbr_of_months_22 	        [3];
	 CHAR mgfINT_rate_22				[8];
	 CHAR mgfMSF_rate_22				[8];
	 CHAR mgfnbr_of_months_23 	        [3];
	 CHAR mgfINT_rate_23				[8];
	 CHAR mgfMSF_rate_23				[8];
	 CHAR mgfnbr_of_months_24 	        [3];
	 CHAR mgfINT_rate_24				[8];
	 CHAR mgfMSF_rate_24				[8];
	 CHAR mgfnbr_of_months_25 	        [3];
	 CHAR mgfINT_rate_25				[8];
	 CHAR mgfMSF_rate_25				[8];
	 CHAR mgfnbr_of_months_26 	        [3];
	 CHAR mgfINT_rate_26				[8];
	 CHAR mgfMSF_rate_26				[8];
	 CHAR mgfnbr_of_months_27 	        [3];
	 CHAR mgfINT_rate_27				[8];
	 CHAR mgfMSF_rate_27				[8];
	 CHAR mgfnbr_of_months_28 	        [3];
	 CHAR mgfINT_rate_28				[8];
	 CHAR mgfMSF_rate_28				[8];
	 CHAR mgfnbr_of_months_29 	        [3];
	 CHAR mgfINT_rate_29				[8];
	 CHAR mgfMSF_rate_29				[8];	 
	 CHAR mgfnbr_of_months_30 	        [3];
	 CHAR mgfINT_rate_30				[8];
	 CHAR mgfMSF_rate_30				[8];
	 CHAR mgfnbr_of_months_30A 	        [3];
	 CHAR mgfINT_rate_30A				[8];
	 CHAR mgfMSF_rate_30A				[8];
	 CHAR mgfnbr_of_months_30B 	        [3];
	 CHAR mgfINT_rate_30B				[8];
	 CHAR mgfMSF_rate_30B				[8];
	 
	 CHAR mgfAmount_limit_4				[13];
	 CHAR mgfnbr_of_months_31 	        [3];
 	 CHAR mgfINT_rate_31				[8];
	 CHAR mgfMSF_rate_31				[8];
	 CHAR mgfnbr_of_months_32 	        [3];
	 CHAR mgfINT_rate_32				[8];
	 CHAR mgfMSF_rate_32				[8];
	 CHAR mgfnbr_of_months_33 	        [3];
	 CHAR mgfINT_rate_33				[8];
	 CHAR mgfMSF_rate_33				[8];
	 CHAR mgfnbr_of_months_34 	        [3];
	 CHAR mgfINT_rate_34				[8];
	 CHAR mgfMSF_rate_34				[8];
	 CHAR mgfnbr_of_months_35 	        [3];
	 CHAR mgfINT_rate_35				[8];
	 CHAR mgfMSF_rate_35				[8];
	 CHAR mgfnbr_of_months_36 	        [3];
	 CHAR mgfINT_rate_36				[8];
	 CHAR mgfMSF_rate_36				[8];
	 CHAR mgfnbr_of_months_37 	        [3];
	 CHAR mgfINT_rate_37				[8];
	 CHAR mgfMSF_rate_37				[8];
	 CHAR mgfnbr_of_months_38 	        [3];
	 CHAR mgfINT_rate_38				[8];
	 CHAR mgfMSF_rate_38			    [8];
	 CHAR mgfnbr_of_months_39 	        [3];
	 CHAR mgfINT_rate_39				[8];
	 CHAR mgfMSF_rate_39				[8];
	 CHAR mgfnbr_of_months_40 	        [3];
	 CHAR mgfINT_rate_40				[8];
	 CHAR mgfMSF_rate_40                [8];
	 CHAR mgfnbr_of_months_40A 	        [3];
	 CHAR mgfINT_rate_40A				[8];
	 CHAR mgfMSF_rate_40A               [8];
	 CHAR mgfnbr_of_months_40B 	        [3];
	 CHAR mgfINT_rate_40B				[8];
	 CHAR mgfMSF_rate_40B               [8];
	 
	 CHAR    mgfFiller                   [10];
}myMerchantGroupDetail, *pmyMerchantGroupDetail;


/* structure of the MerchantGroup trailer. */

   typedef struct
{
   CHAR trailRecordType     [4];
   CHAR trailFileName       [9];
   CHAR trailFileCreateDate [7];
   CHAR trailNumberofRecord [10];
   CHAR trailFiller         [1265];
} myMerchantGroupTrailer, *pmyMerchantGroupTrailer;

/* strucure of the EDTRM Extract file. */
typedef struct 
{
   CHAR headerRecordType      [4];
   CHAR headerFileName        [9];
   CHAR headerFileCreateDate  [7];
   CHAR headerFiller          [184]; 
} myEDTRHeader, *pmyEDTRHeader;

typedef struct
{
   CHAR detailRecordType            [3];
   CHAR detailMerchantNumber       [10];
   CHAR detailTerminalNumber       [8];
   CHAR detailMCC                     [4];
   CHAR ChipActivatedFlag           [1];
   CHAR detailFiller                  [4]; 
} RawEDTRDetail, *pRawEDTRDetail;

typedef struct
{
   CHAR detailRecordType            [4];
   CHAR detailMerchantNumber       [16];
   CHAR detailTerminalNumber       [9];
   CHAR detailOrganizaNumber       [5];
   CHAR detailMCC                     [5];
   CHAR ChipActivatedFlag           [2];
   CHAR detailFiller                 [5]; 

} myEDTRDetail, *pmyEDTRDetail;

typedef struct
{
   CHAR trailRecordType       [4];
   CHAR trailFileName         [9];
   CHAR trailFileCreateDate   [7];
   CHAR trailNumberofRecords  [10];
   CHAR trailFiller           [175];   
} myEDTRTrailer, *pmyEDTRTrailer;


/* strucure of the Merchant Pre Auth Validation Extract file. */
typedef struct
{
   CHAR headerRecordType      [4];
   CHAR headerFileName        [9];
   CHAR headerFileCreateDate  [7];
   CHAR headerFiller          [184];
} myMPAVHeader, *pmyMPAVHeader;

typedef struct
{
   CHAR detailRecordType            [3];
   CHAR detailMerchantNumber        [15];
   CHAR PreAuthValidationFlag       [1];
   CHAR detailFiller                [4];
} RawMPAVDetail, *pRawMPAVDetail;

typedef struct
{
   CHAR detailRecordType            [4];
   CHAR detailMerchantNumber        [16];
   CHAR detailOrganizaNumber        [5];
   CHAR PreAuthValidationFlag       [2];
   CHAR detailFiller                [5];
} myMPAVDetail, *pmyMPAVDetail;

typedef struct
{
   CHAR trailRecordType       [4];
   CHAR trailFileName         [9];
   CHAR trailFileCreateDate   [7];
   CHAR trailNumberofRecords  [10];
   CHAR trailFiller           [175];
} myMPAVTrailer, *pmyMPAVTrailer;


/* structure of the EMVC capability Extract file. */
typedef struct
{
   CHAR headerRecordType      [4];
   CHAR headerFileName        [9];
   CHAR headerFileCreateDate  [7];
   CHAR headerFiller          [184];
} myEMVCHeader, *pmyEMVCHeader;

typedef struct
{
   CHAR detailRecordType           [3];
   CHAR detailTerminalNumber       [8];
   CHAR detailMerchantNumber       [15];
   CHAR detailEmvCapability        [6];
   CHAR detailFiller               [1];
} RawEMVCDetail, *pRawEMVCDetail;

typedef struct
{
   CHAR detailRecordType            [4];
   CHAR detailTerminalNumber       [9];
   CHAR detailMerchantNumber       [16];
   CHAR detailEmvCapability        [7];
   CHAR detailFiller               [2];

} myEMVCDetail, *pmyEMVCDetail;

typedef struct
{
   CHAR trailRecordType       [4];
   CHAR trailFileName         [9];
   CHAR trailFileCreateDate   [7];
   CHAR trailNumberofRecords  [10];
   CHAR trailFiller           [175];
} myEMVCTrailer, *pmyEMVCTrailer;

extern void negative_fraud_to_db( pNEGATIVE_FRAUD negfraud_ptr);

extern BYTE db_select_dcf01_with_merchant (pCHAR device,pCHAR merchant_id, pDCF01 dcf01_ptr, pCHAR ErrorMsg );
extern BYTE db_select_dcf01_EDTR (  pDCF01 dcf01_ptr,  pCHAR ErrorMsg );


extern BYTE db_update_dcf01( pDCF01 dcf01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_dcf01_EDTR( pDCF01 dcf01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_dcf01_EDTR( pDCF01 dcf01_ptr, pCHAR ErrorMsg );


extern BYTE db_insert_dcf01( pDCF01 dcf01_ptr, pCHAR ErrorMsg );

extern BYTE db_select_paf01( pCHAR  Primary_key, pPAF01 paf01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_paf01( pPAF01 paf01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_paf01( pPAF01 paf01_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_acf01( pCHAR  Primary_key1, pCHAR Primary_key2, pACF01 acf01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_acf01( pACF01 acf01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_acf01( pACF01 acf01_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_ccf01( pCHAR  Primary_key1, pCHAR Primary_key2, pCCF01 ccf01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_ccf01( pCCF01 ccf01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_ccf01( pCCF01 ccf01_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_ccf02p( pCHAR  Primary_key1, pCHAR Primary_key2, pCCF02P ccf02_ptr, pCHAR ErrorMsg );
extern BYTE db_update_ccf02p( pCCF02P ccf02_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_ccf02p( pCCF02P ccf02_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_mcf01( pCHAR  Primary_key1, pCHAR Primary_key2, pMCF01 mcf01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_mcf01( pMCF01 mcf01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_mcf01( pMCF01 mcf01_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_mct01 ( pCHAR Primary_key1, pCHAR Primary_key2, pCHAR Primary_key3, 
								pMCT01 mct01_ptr, pCHAR ErrorMsg );
extern BYTE db_clear_mct01( pMCT01 mct01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_mct01( pMCT01 mct01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_mct01( pMCT01 mct01_ptr, pCHAR ErrorMsg );

extern BYTE my_db_select_def01 ( pCHAR Primary_key1, pCHAR Primary_key2, pCHAR Primary_key3, 
								 pDEF01 def01_ptr, pCHAR ErrorMsg );
extern BYTE db_update_def01( pDEF01 def01_ptr, pCHAR ErrorMsg );
extern BYTE db_insert_def01( pDEF01 def01_ptr, pCHAR ErrorMsg );

extern BYTE db_select_neg_fraud ( pNEGATIVE_FRAUD Primary_key, pCHAR ErrorMsg );
extern BYTE db_update_neg_fraud ( pNEGATIVE_FRAUD Primary_key, pCHAR ErrorMsg );
extern BYTE db_insert_neg_fraud ( pNEGATIVE_FRAUD Primary_key, pCHAR ErrorMsg );

extern BYTE db_insert_hst01 (pHST01, pCHAR);

extern BYTE dbcommon_connect ( pCHAR username,  pCHAR password,  pCHAR alias, pCHAR database,	 pCHAR ErrorMsg );
extern BYTE dbcommon_disconnect( pCHAR ErrorMsg );
extern void dbcommon_commit();
extern BYTE db_set_rollback_segment_online(pCHAR ErrorMsg);
extern BYTE db_set_rollback_segment_offline(pCHAR ErrorMsg);
void exit_import(); 
extern BYTE db_select_dcf01_EMVC ( pDCF01 dcf01_ptr,  pCHAR ErrorMsg );
extern BYTE db_update_dcf01_EMVC ( pDCF01 dcf01_ptr, pCHAR ErrorMsg );


void   populateCcf01Table (pmyCardHoldDetail  cardHoldDetailRecord, pCCF01 ccf01);
void   populateMcf01Table (pmyMerchantDetail  merchDetailRecord   , pMCF01 mcf01);
void   populateMct01Table (pmyMerchantDetail  merchDetailRecord   , pcardType subStruct, pMCT01 mct01);
void   populateCcf02pTable(pmyCardHoldDetail  cardHoldDetailRecord, pCCF02P ccf02p);
void   populateMPPARA01Table(pmyMpParameterDetail mpparaDetailRecord, pMPPARA01 mppara01);
void   populateMGF01Table(pmyMerchantGroupDetail mgfDetailRecord, pMGF01 mgf01);


void get_time( pCHAR time_date, pCHAR time_str );
void get_date( pCHAR time_date, pCHAR date_str );

INT  processTerminalImportCommand (pCHAR filename,INT choice);
INT  processMerchantImportCommand (pCHAR filename,INT choice);
INT  processPrimaryImportCommand  (pCHAR filename,INT choice);              
INT  processCardHoldImportCommand (pCHAR filename,INT choice);
INT  processBalanceImportCommand  (pCHAR filename,INT choice);

INT  processMultipleBalanceImportCommand  (pCHAR filename,pCHAR filename1,INT choice);
INT  processMultipleMerchantImportCommand (pCHAR filename,pCHAR filename1,INT choice);
INT  processMultiplePrimaryImportCommand  (pCHAR filename,pCHAR filename1,INT choice);
INT  processMultipleTerminalImportCommand (pCHAR filename,pCHAR filename1,INT choice);
INT  processMultipleCardHoldImportCommand  (pCHAR filename,pCHAR filename1,INT choice);

/* added by farad on 2/18/1999 to fix bug #2. */
INT  dcf01HasBlankReqField        (pDCF01    dcf01);
INT  paf01HasBlankReqField        (pPAF01    paf01);
INT  ccf01HasBlankReqField        (pCCF01    ccf01);
INT  ccf02pHasBlankReqField       (pCCF02P   ccf02p);
INT  acf01HasBlankReqField        (pACF01    acf01);                            
INT  mcf01HasBlankReqField        (pMCF01    mcf01, pcardType subStruct);
INT  negfraudHasBlankReqField     (pNEGATIVE_FRAUD neg_fraud);

void mask_card_number_import(char * card_num, char *);
void errorMsgBlankField(pCHAR record, pCHAR recordid);
void errorMsgNotInsert(pCHAR record, pCHAR recordid, pCHAR errorMsg);
void errorMsgNotUpdate(pCHAR record, pCHAR recordid, pCHAR errorMsg, int ret);
void log_the_updated_fields_of_mcf01(pMCF01 DB_mcf01, pMCF01 File_mcf01);

/* ****************************************************** */
void parseTerminalHeader   (pCHAR dataRecord, pmyTerminalHeader termHeaderRecord);
void parseTerminalDetail   (pCHAR dataRecord, pDCF01 dcf01);
void parseTerminalTrailer  (pCHAR dataRecord, pmyTerminalTrailer termTrailRecord);

/* ************************************************************ */
void parsePrimaryHeader(pCHAR dataRecord,    pmyPrimaryHeader  primHeadRecord  );
void parsePrimaryDetail(pCHAR dataRecord,    pPAF01 paf01 );
void parsePrimaryTrailer(pCHAR dataRecord,   pmyPrimaryTrailer primTrailRecord );

/* ***********************************************************/
void parseCardHoldHeader(pCHAR dataRecord,   pmyCardHoldHeader  cardHoldHeadRecord);
void parseCardHoldDetail(pCHAR dataRecord,   pmyCardHoldDetail  cardHoldDetailRec);
void parsecardHoldTrailer(pCHAR dataRecord,  pmyCardHoldTrailer cardHoldTrailRec);

/* *********************************************** */
void parseBalanceHeader(pCHAR dataRecord,    pmyBalanceHeader balanceHeadRec);
void parseBalanceDetail(pCHAR dataRecord,    pACF01 acf01);
void parseBalanceTrailer(pCHAR dataRecord,   pmyBalanceTrailer balanceTrailRec);

/* ******************************************************** */
void parseMerchantHeader   (pCHAR dataRecord, pmyMerchantHeader merchHeadRec);
void parseMerchantDetail   (pCHAR dataRecord, pmyMerchantDetail merchDetailRec);
void parseMerchantTrailer  (pCHAR dataRecord, pmyMerchantTrailer merchTrailRec);

/* ******************************************************** */
void parseNegFraudHeader (pCHAR dataRecord, pmyNegFraudHeader NegFraudHeadRecord);
void parseNegFraudDetail (pCHAR dataRecord, pNEGATIVE_FRAUD neg_fraud);
void parseNegFraudTrailer(pCHAR dataRecord, pmyNegFraudTrailer NegFraudTrailRecord);

/* ******************************************************** */
void parseMpParameterHeader(pCHAR dataRecord, pmyMpParameterHeader mpparaHeadRec);
void parseMpParameterDetail(pCHAR dataRecord, pmyMpParameterDetail mpparaDetailRec);
void parseMpParameterTrailer(pCHAR dataRecord, pmyMpParameterTrailer mpparaTrailRec);
/***********************************************************/
void parseMerchantGroupHeader(pCHAR dataRecord, pmyMerchantGroupHeader mgfHeadRec);
void parseMerchantGroupDetail(pCHAR dataRecord, pmyMerchantGroupDetail mgfDetailRec);
void parseMerchantGroupTrailer(pCHAR dataRecord, pmyMerchantGroupTrailer mgfTrailRec);

/***********************************************************/
void parseEDTRHeader (pCHAR dataRecord, pmyEDTRHeader EDTRHeaderRecord);
void parseEDTRDetail(pCHAR dataRecord, pDCF01 dcf01, pCHAR ChipActiveFlag);
void parseEDTRTrailer(pCHAR dataRecord, pmyEDTRTrailer EDTRTrailRecord);
/*************************************************************/

/***********************************************************/
void parseMPAVHeader(pCHAR dataRecord, pmyMPAVHeader MPAVHeaderRecord);
void parseMPAVDetail(pCHAR dataRecord, pmyMPAVDetail merchDetailRec);
void parseMPAVTrailer(pCHAR dataRecord, pmyMPAVTrailer MPAVTrailRecord);
/*************************************************************/

INT clearMCT01Table(pmyMerchantDetail merchDetailRecord ,pcardType subStruct);
INT processMCT01Table(pmyMerchantDetail merchDetailRecord ,pcardType subStruct);
INT processDEF01Table(pmyMerchantDetail merchDetailRecord ,pCHAR term,
                                                           pCHAR factor,
                                                           pCHAR minAmount);
/***********************************************************/
void parseEMVCHeader (pCHAR dataRecord, pmyEMVCHeader EMVCHeaderRecord);
void parseEMVCDetail(pCHAR dataRecord, pDCF01 dcf01, pCHAR EMVCpabilityindicator);
void parseEMVCTrailer(pCHAR dataRecord, pmyEMVCTrailer EMVCTrailRecord);
/*************************************************************/

void hst01update ( BYTE * fileprimarykey);

void WriteDSVHeader();
BOOLEAN WriteString(char *str, int size, char fillerChar, char seperator, FILE** fptr, char *header);
long int findHST01FileSize(char file_name[]);
BOOLEAN OpenDSV_WriteHST01(HST01 *hst01);


void returnNumbers(pCHAR input);
void InitMsgLogging(char *fileName);
void ShutDownMsgLogging();
void LogMsg(pCHAR msg, INT type);
void get_term_ini_info();

INT log_db_timing_statistics_report( pTIMINGS_LIST  timing_list );

INT get_dataserver_ini_db_error_statics_value(pINT  ds_timer_flag,
                                   double *  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf );
void Create_Db_Statics_And_Db_Oracle_Error_Filename();
void Log_Db_Statics_And_Oracle_Db_Error_To_File(pCHAR Error_Buf);


struct appdata_struct 
{
	CHAR appdata_str[20];
	BYTE appdata_type;
}; 

static struct appdata_struct appdata_table[] = 
{
	{ "HST01",    HST01_DATA },
	{ "hst01",    HST01_DATA },
	{ "TLF01",    TLF01_DATA },
	{ "tlf01",    TLF01_DATA },
	{ "BCH01",    BCH01_DATA },
	{ "bch01",    BCH01_DATA },
	{ "CCF10",    CCF10_DATA },
	{ "ccf10",    CCF10_DATA },
	{ "NFI01",    NFI01_DATA },
	{ "nfi01",    NFI01_DATA }
};

#define N_APPDATATYPES ( sizeof( appdata_table ) / sizeof( struct appdata_struct ) )

typedef struct
{
   LONG readCount1;  /* Total records read from the file1, if only one file is inputted */
   LONG skipCount1;  /* Total records skip after read from the file if only one file is inputted*/

   LONG readCount2;  /* Total records read from the file2, if two files are given */
   
   LONG insertCount;/* Total records selected for insertion */
   LONG updateCount;/* Total records seleted for update */

   LONG matchCount; /* Total records compare and match */

   LONG errorCount1; /* Total records which has wrong syntax or unable to complete database operation 
					   successfully */

      
} ImportCount, *pImportCount;
