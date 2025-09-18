#include "nc_dbstruct.h"

#define NCF01_LOCAL_TABLESIZE 500
NCF01    m_ncf01_local_table[NCF01_LOCAL_TABLESIZE];
NCF01 m_ncf01;
pPTE_MSG GetNCF01List(NCF01 m_ncf01);
BYTE Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize) ;
BYTE Generate_Usage_Error_Message (int SystemMonitor, BYTE* strTemp, BYTE* function,
								   BYTE general_status, BYTE* response_code, AUTH_TX auth_tx);
BYTE Mracon_Get_mar01_table();
void mracon_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details );
void mracon_get_error_warning_file_name_path(void );
INT mracon_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func ,int detail);
void mracon_create_Error_Filename();
BYTE mracon_Send_Request_To_Service (BYTE* que_name, BYTE nType, BYTE nSubType1,
							  BYTE nSubType2, BYTE nAppDataType,
							  pBYTE p_data,   INT nDataSize);

int Convert_Amt_To_Auth_Tx_String_Format
                              (double nAmount, char* strAmount);

void LeftFill_String_With_Character (unsigned int destination_array_size,
                                     char* strAmount,
                                     unsigned char fill_character);

INT MRACON_read_CUP_merchant_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_1_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_2_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_3_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_4_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_5_indicator( void );
INT MRACON_read_CUP_MP_Bin_range_len_1( void );
INT MRACON_read_CUP_MP_Bin_range_len_2( void );
INT MRACON_read_CUP_MP_Bin_range_len_3( void );
INT MRACON_read_CUP_MP_Bin_range_len_4( void );
INT MRACON_read_CUP_MP_Bin_range_len_5( void );


#define MRACON_HEADER_FIELD_SIZE 69
#define MRACON_FIELD1_SIZE    16
#define MRACON_FIELD2_SIZE    8
#define MRACON_FIELD3_SIZE    5
#define MRACON_FIELD4_SIZE    16
#define MRACON_FIELD5_SIZE    8
#define MRACON_FIELD6_SIZE    6
#define MRACON_FIELD7_SIZE    3
#define MRACON_FIELD8_SIZE    6
#define MRACON_FIELD9_SIZE	  20
#define MRACON_FIELD10_SIZE	  40
#define MRACON_FIELD11_SIZE	  32
#define MRACON_FIELD12_SIZE	  19
#define MRACON_FIELD13_SIZE	  1
#define MRACON_FIELD14_SIZE	  9
#define MRACON_FIELD15_SIZE	  3
#define MRACON_FIELD16_SIZE	  8
#define MRACON_FIELD17_SIZE	  8
#define MRACON_FIELD18_SIZE	  1
#define MRACON_FIELD19_SIZE	  8
#define MRACON_FIELD20_SIZE	  8
#define MRACON_FIELD21_SIZE	  10
#define MRACON_FIELD22_SIZE	  10
#define MRACON_FIELD23_SIZE	  1
#define MRACON_FIELD24_SIZE	  8
#define MRACON_FIELD25_SIZE	  3
#define MRACON_FIELD26_SIZE	  10
#define MRACON_FIELD27_SIZE	  1
#define MRACON_FIELD28_SIZE	  1
#define MRACON_FIELD29_SIZE	  8
#define MRACON_FIELD30_SIZE	  6
#define MRACON_FIELD31_SIZE	  13
#define MRACON_FIELD32_SIZE	  3
#define MRACON_FIELD33_SIZE	  13
#define MRACON_FIELD34_SIZE	  1
#define MRACON_FIELD35_SIZE	  1
#define MRACON_FIELD36_SIZE	  4
#define MRACON_FIELD37_SIZE	  9
#define MRACON_FIELD38_SIZE	  3
#define MRACON_FIELD39_SIZE	  1
#define MRACON_FIELD40_SIZE	  1
#define MRACON_FIELD41_SIZE	  1
#define MRACON_FIELD42_SIZE	  8
#define MRACON_FIELD43_SIZE	  1
#define MRACON_FIELD44_SIZE	  1
#define MRACON_FIELD45_SIZE	  1
#define MRACON_FIELD46_SIZE	  1
#define MRACON_FIELD47_SIZE	  1
#define MRACON_FIELD48_SIZE	  10
#define MRACON_FIELD49_SIZE	  10
#define MRACON_FIELD50_SIZE	  10
#define MRACON_FIELD51_SIZE	  1
#define MRACON_FIELD52_SIZE	  1
#define MRACON_FIELD53_SIZE	  4
#define MRACON_FIELD54_SIZE	  4
#define MRACON_FIELD55_SIZE	  4
#define MRACON_FIELD56_SIZE	  1
#define MRACON_FIELD57_SIZE	  1
#define MRACON_FIELD58_SIZE	  2
#define MRACON_FIELD59_SIZE	  14
#define MRACON_FIELD60_SIZE	  14
#define MRACON_FIELD61_SIZE	  6
#define MRACON_FIELD62_SIZE	  40
#define MRACON_FIELD63_SIZE	  30
#define MRACON_FIELD64_SIZE	  3
#define MRACON_FIELD65_SIZE	  1
#define MRACON_FIELD66_SIZE	  5
#define MRACON_FIELD67_SIZE	  5
#define MRACON_FIELD68_SIZE	  3
#define MRACON_FIELD69_SIZE	  20
#define MRACON_FIELD70_SIZE	  40
#define MRACON_FIELD71_SIZE	  1
#define MRACON_FIELD72_SIZE	  1
#define MRACON_FIELD73_SIZE	  8
#define MRACON_FIELD74_SIZE	  19
#define MRACON_FIELD75_SIZE	  1
#define MRACON_FIELD76_SIZE	  1
#define MRACON_FIELD77_SIZE	  1
#define MRACON_FIELD78_SIZE	  1
#define MRACON_FIELD79_SIZE	  1
#define MRACON_FIELD80_SIZE	  1
#define MRACON_FIELD81_SIZE	  1
#define MRACON_FIELD82_SIZE	  1
#define MRACON_FIELD83_SIZE	  13
#define MRACON_FIELD84_SIZE	  13
#define MRACON_FIELD85_SIZE	  13
#define MRACON_FIELD86_SIZE	  12
#define MRACON_FIELD87_SIZE	  1
#define MRACON_FIELD88_SIZE	  1
#define MRACON_FIELD89_SIZE	  1
#define MRACON_FIELD90_SIZE	  1
#define MRACON_FIELD91_SIZE	  1
#define MRACON_FIELD92_SIZE	  12
#define MRACON_FIELD93_SIZE	  3
#define MRACON_FIELD94_SIZE	  16
#define MRACON_FIELD95_SIZE	  1
#define MRACON_FIELD96_SIZE	  1
#define MRACON_FIELD97_SIZE	  2
#define MRACON_FIELD98_SIZE	  1
#define MRACON_FIELD99_SIZE	  1
#define MRACON_FIELD100_SIZE  6
#define MRACON_FIELD101_SIZE  10
#define MRACON_FIELD102_SIZE  10
#define MRACON_FIELD103_SIZE  1
#define MRACON_FIELD104_SIZE  5
#define MRACON_FIELD105_SIZE  5
#define MRACON_FIELD106_SIZE  2
#define MRACON_FIELD107_SIZE  2
#define MRACON_FIELD108_SIZE  2
#define MRACON_FIELD109_SIZE  2
#define MRACON_FIELD110_SIZE  8
#define MRACON_FIELD111_SIZE  1
#define MRACON_FIELD112_SIZE  1
#define MRACON_FIELD113_SIZE  2
#define MRACON_FIELD114_SIZE  8
#define MRACON_FIELD115_SIZE  5
#define MRACON_FIELD116_SIZE  1
#define MRACON_FIELD117_SIZE  16
#define MRACON_FIELD118_SIZE  1
#define MRACON_FIELD119_SIZE  13
#define MRACON_FIELD120_SIZE  13
#define MRACON_FIELD121_SIZE  40
#define MRACON_FIELD122_SIZE  30
#define MRACON_FIELD123_SIZE  1
#define MRACON_FIELD124_SIZE  1
#define MRACON_FIELD125_SIZE  1
#define MRACON_FIELD126_SIZE  1
#define MRACON_FIELD127_SIZE  1
#define MRACON_FIELD128_SIZE  1
#define MRACON_FIELD129_SIZE  1
#define MRACON_FIELD130_SIZE  1
#define MRACON_FIELD131_SIZE  6
#define MRACON_FIELD132_SIZE  10
#define MRACON_FIELD133_SIZE  10
#define MRACON_FIELD134_SIZE  1
#define MRACON_FIELD135_SIZE  1
#define MRACON_FIELD136_SIZE  5
#define MRACON_FIELD137_SIZE  5
#define MRACON_FIELD138_SIZE  1
#define MRACON_FIELD139_SIZE  1
#define MRACON_FIELD140_SIZE  1
#define MRACON_FIELD141_SIZE  1
#define MRACON_FIELD142_SIZE  1
#define MRACON_FIELD143_SIZE  1
#define MRACON_FIELD144_SIZE  1

#define MRACON_FIELD10_MERCHANT_ID_SIZE		16
#define MRACON_FIELD10_CARD_NUM_SIZE		20
#define MRACON_FIELD120_SIZE_ECI_MC			3
#define MRACON_FIELD120_SIZE_ECI_VISA		11
#define MRACON_FIELD120_SIZE_ECI_AMEX    	5
#define MRACON_FIELD120_SIZE_ECI_CUP     	2
#define MRACON_FIELD120_SIZE_ECI		 	3
#define MRACON_FIELD121_SIZE_MC          	3
#define MRACON_FIELD121_SIZE_VISA        	11
#define MRACON_FIELD121_SIZE_ECI_AMEX    	13
#define MRACON_FIELD121_SIZE_ECI_DINERS  	13

#define MERCHANT_ID_SIZE 16
#define   MAX_FILENAME_SIZE  80
#define   MAX_INI_INFO_SIZE  20

#define MRACON_DE37_NAME_STREET_CITY_SEPARATOR_SYSMOL							"/"
#define MRACON_DE37_POSTAL_CODE_POSTION 										83
#define MRACON_DE37_POSTAL_CODE_LEN 											10
#define MRACON_DE37_REGION_POSTION 												93
#define MRACON_DE37_REGION_LEN 													3
#define MRACON_DE37_COUNTRY_CODE_POSTION										96
#define MRACON_DE37_COUNTRY_CODE_LEN											3
#define MRACON_FIELD37_LEN_SIZE													2

#define TRANSACTION_SEND_TIME				17
#define TRANSACTION_ID						20
#define SYSTEM_DATE							8
#define SPACES								' '


	void MRACON_build_request_HeaderFile();
	void MRACON_build_request_field_1();
	void MRACON_build_request_field_2();
	void MRACON_build_request_field_3();
	void MRACON_build_request_field_4();
	void MRACON_build_request_field_5();
	void MRACON_build_request_field_6();
	void MRACON_build_request_field_7();
	void MRACON_build_request_field_8();
	void MRACON_build_request_field_9();
	void MRACON_build_request_field_10();
	void MRACON_build_request_field_11();
	void MRACON_build_request_field_12();
	void MRACON_build_request_field_13();
	void MRACON_build_request_field_14();
	void MRACON_build_request_field_15();
	void MRACON_build_request_field_16();
	void MRACON_build_request_field_17();
	void MRACON_build_request_field_18();
	void MRACON_build_request_field_19();
	void MRACON_build_request_field_20();
	void MRACON_build_request_field_21();
	void MRACON_build_request_field_22();
	void MRACON_build_request_field_23();
	void MRACON_build_request_field_24();
	void MRACON_build_request_field_25();
	void MRACON_build_request_field_26();
	void MRACON_build_request_field_27();
	void MRACON_build_request_field_28();
	void MRACON_build_request_field_29();
	void MRACON_build_request_field_30();
	void MRACON_build_request_field_31();
	void MRACON_build_request_field_32();
	void MRACON_build_request_field_33();
	void MRACON_build_request_field_34();
	void MRACON_build_request_field_35();
	void MRACON_build_request_field_36();
	void MRACON_build_request_field_37();
	void MRACON_build_request_field_38();
	void MRACON_build_request_field_39();
	void MRACON_build_request_field_40();
	void MRACON_build_request_field_41();
	void MRACON_build_request_field_42();
	void MRACON_build_request_field_43();
	void MRACON_build_request_field_44();
	void MRACON_build_request_field_45();
	void MRACON_build_request_field_46();
	void MRACON_build_request_field_47();
	void MRACON_build_request_field_48();
	void MRACON_build_request_field_49();
	void MRACON_build_request_field_50();
	void MRACON_build_request_field_51();
	void MRACON_build_request_field_52();
	void MRACON_build_request_field_53();
	void MRACON_build_request_field_54();
	void MRACON_build_request_field_55();
	void MRACON_build_request_field_56();
	void MRACON_build_request_field_57();
	void MRACON_build_request_field_58();
	void MRACON_build_request_field_59();
	void MRACON_build_request_field_60();
	void MRACON_build_request_field_61();
	void MRACON_build_request_field_62();
	void MRACON_build_request_field_63();
	void MRACON_build_request_field_64();
	void MRACON_build_request_field_65();
	void MRACON_build_request_field_66();
	void MRACON_build_request_field_67();
	void MRACON_build_request_field_68();
	void MRACON_build_request_field_69();
	void MRACON_build_request_field_70();
	void MRACON_build_request_field_71();
	void MRACON_build_request_field_72();
	void MRACON_build_request_field_73();
	void MRACON_build_request_field_74();
	void MRACON_build_request_field_75();
	void MRACON_build_request_field_76();
	void MRACON_build_request_field_77();
	void MRACON_build_request_field_78();
	void MRACON_build_request_field_79();
	void MRACON_build_request_field_80();
	void MRACON_build_request_field_81();
	void MRACON_build_request_field_82();
	void MRACON_build_request_field_83();
	void MRACON_build_request_field_84();
	void MRACON_build_request_field_85();
	void MRACON_build_request_field_86();
	void MRACON_build_request_field_87();
	void MRACON_build_request_field_88();
	void MRACON_build_request_field_89();
	void MRACON_build_request_field_90();
	void MRACON_build_request_field_91();
	void MRACON_build_request_field_92();
	void MRACON_build_request_field_93();
	void MRACON_build_request_field_94();
	void MRACON_build_request_field_95();
	void MRACON_build_request_field_96();
	void MRACON_build_request_field_97();
	void MRACON_build_request_field_98();
	void MRACON_build_request_field_99();
	void MRACON_build_request_field_100();
	void MRACON_build_request_field_101();
	void MRACON_build_request_field_102();
	void MRACON_build_request_field_103();
	void MRACON_build_request_field_104();
	void MRACON_build_request_field_105();
	void MRACON_build_request_field_106();
	void MRACON_build_request_field_107();
	void MRACON_build_request_field_108();
	void MRACON_build_request_field_109();
	void MRACON_build_request_field_110();
	void MRACON_build_request_field_111();
	void MRACON_build_request_field_112();
	void MRACON_build_request_field_113();
	void MRACON_build_request_field_114();
	void MRACON_build_request_field_115();
	void MRACON_build_request_field_116();
	void MRACON_build_request_field_117();
	void MRACON_build_request_field_118();
	void MRACON_build_request_field_119();
	void MRACON_build_request_field_120();
	void MRACON_build_request_field_121();
	void MRACON_build_request_field_122();
	void MRACON_build_request_field_123();
	void MRACON_build_request_field_124();
	void MRACON_build_request_field_125();
	void MRACON_build_request_field_126();
	void MRACON_build_request_field_127();
	void MRACON_build_request_field_128();
	void MRACON_build_request_field_129();
	void MRACON_build_request_field_130();
	void MRACON_build_request_field_131();
	void MRACON_build_request_field_132();
	void MRACON_build_request_field_133();
	void MRACON_build_request_field_134();
	void MRACON_build_request_field_135();
	void MRACON_build_request_field_136();
	void MRACON_build_request_field_137();
	void MRACON_build_request_field_138();
	void MRACON_build_request_field_139();
	void MRACON_build_request_field_140();
	void MRACON_build_request_field_141();
	void MRACON_build_request_field_142();
	void MRACON_build_request_field_143();
	void MRACON_build_request_field_144();
