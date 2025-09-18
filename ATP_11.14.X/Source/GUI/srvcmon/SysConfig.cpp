// SysConfig.cpp : implementation file
//

#include "stdafx.h"
#include "srvcmon.h"
#include "SysConfig.h"

extern "C"
{
#include "srvccomm.h"
#include "ptemsg.h"
#include "ntutils.h"

BOOLEAN ThisServerIsPrimary;
BOOLEAN EnableMakeThisPrimaryButton;
BOOLEAN EnableMakeOtherPrimaryButton;
BOOLEAN EnableSwitchPrimaryButton;
BOOLEAN DuplexMode;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysConfig dialog


CSysConfig::CSysConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CSysConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSysConfig)
	m_HighAvailabilityIndicator = _T("");
	m_OtherMachineId = _T("");
	m_OtherXipcInstance = _T("");
	m_ThisMachineId = _T("");
	m_ThisXipcInstance = _T("");
	m_PrimaryServer = _T("");
	m_SecondaryServer = _T("");
	//}}AFX_DATA_INIT
}


void CSysConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysConfig)
	DDX_Control(pDX, IDC_SWITCH_PRIMARY_SECONDARY, m_SwitchPrimarySecondary);
	DDX_Control(pDX, IDC_MAKE_THIS_SERVER_PRIMARY, m_MakeThisServerPrimary);
	DDX_Control(pDX, IDC_MAKE_OTHER_SERVER_PRIMARY, m_MakeOtherServerPrimary);
	DDX_Text(pDX, IDC_HIGH_AVAILABILITY_INDICATOR, m_HighAvailabilityIndicator);
	DDV_MaxChars(pDX, m_HighAvailabilityIndicator, 256);
	DDX_Text(pDX, IDC_OTHER_MACHINE_ID, m_OtherMachineId);
	DDV_MaxChars(pDX, m_OtherMachineId, 256);
	DDX_Text(pDX, IDC_OTHER_XIPC_INSTANCE, m_OtherXipcInstance);
	DDV_MaxChars(pDX, m_OtherXipcInstance, 256);
	DDX_Text(pDX, IDC_THIS_MACHINE_ID, m_ThisMachineId);
	DDV_MaxChars(pDX, m_ThisMachineId, 256);
	DDX_Text(pDX, IDC_THIS_XIPC_INSTANCE, m_ThisXipcInstance);
	DDV_MaxChars(pDX, m_ThisXipcInstance, 256);
	DDX_Text(pDX, IDC_PRIMARY_SERVER, m_PrimaryServer);
	DDV_MaxChars(pDX, m_PrimaryServer, 256);
	DDX_Text(pDX, IDC_SECONDARY_SERVER, m_SecondaryServer);
	DDV_MaxChars(pDX, m_SecondaryServer, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysConfig, CDialog)
	//{{AFX_MSG_MAP(CSysConfig)
	ON_BN_CLICKED(IDC_MAKE_OTHER_SERVER_PRIMARY, OnMakeOtherServerPrimary)
	ON_BN_CLICKED(IDC_MAKE_THIS_SERVER_PRIMARY, OnMakeThisServerPrimary)
	ON_BN_CLICKED(IDC_SWITCH_PRIMARY_SECONDARY, OnSwitchPrimarySecondary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysConfig message handlers

BOOLEAN CSysConfig::PopulateSysConfigDialog( pINT   HighAvailability,
                                             pCHAR  ThisMachineId,    
                                             pCHAR  OtherMachineId,   
                                             pCHAR  ThisXipcInstance, 
                                             pCHAR  OtherXipcInstance,
                                             pCHAR  ThisLoggerMSQueName)
{
   INT      status;
   CHAR     tmpstr[256];

   /* IMPORTANT:
      DuplexMode : Logger/Reply Duplex Mode
      High Availability :  Two Servers, either two simplexes or 
                           two Servers in Duplex 
   */
   if (srvccomm_get_primary_info(ThisLoggerMSQueName, tmpstr, &status ) == PTEMSG_OK)
   {
      switch(status)
      {
      /* Server not in duplex and it is Primary, this could mean:
         1) The system is configured to be simplex, therefore, each of
            the servers will be independent of the other and it would
            be the primary as far as logger/reply by default
         2) It was in a duplex configuration as the primary system, and
            the connection to the secondary got lost, so it is now running
            in simplex mode as the primary machine.

         In both cases, it is already primary, so there is nothing to do
      */
      case ST1_SYS_NOT_IN_DUPLEX_MODE_AND_PRIMARY:
         if (*HighAvailability)
         {
	         m_HighAvailabilityIndicator = "Simplex Dual Server Configuration";
            sprintf(tmpstr, "This Server Name  : %s", ThisMachineId);
            m_PrimaryServer             = tmpstr;
            sprintf(tmpstr, "Other Server Name : %s", OtherMachineId);
            m_SecondaryServer           = tmpstr;
            ThisServerIsPrimary = TRUE;
            EnableMakeThisPrimaryButton = FALSE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = FALSE;
            DuplexMode = FALSE;
         }
         else
         {
	         m_HighAvailabilityIndicator = "Simplex Single Server Configuration";
            sprintf(tmpstr, "This Server Name  : %s", ThisMachineId);
            m_PrimaryServer             = tmpstr;
            m_SecondaryServer = "Other Server Name : None";
            ThisServerIsPrimary = TRUE;
            EnableMakeThisPrimaryButton = FALSE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = FALSE;
            DuplexMode = FALSE;
         }
         break;

      /* System not in Duplex Mode, but it is not primary, this could mean:
         1) The system was in duplex configuration, and it was the secondary
            server, then the connection the the primary server got lost, 
            so system went back to simplex configuration
         2) This is the first time ascendent is being started, there exists no
            checkpoint files to figure out whose primary and whose secondary,
            also keep in mind that we always check on the MS queue for status
            since the AP queue is not listening when the status has not been
            determined yet.

         We allow to make this primary, in case the primary server went down,
         but only after making sure the the other server is down, WE CANNOT
         ALLOW FOR TWO PRIMARY SYSTEMS RUNNING AT THE SAME TIME
      */
      case ST1_SYS_NOT_IN_DUPLEX_MODE_AND_NOT_PRIMARY:
         if (*HighAvailability)
         {
	         m_HighAvailabilityIndicator = "Simplex Dual Server Configuration";
            sprintf(tmpstr, "This Server Name  : %s", ThisMachineId);
            m_PrimaryServer             = tmpstr;
            sprintf(tmpstr, "Other Server Name : %s", OtherMachineId);
            m_SecondaryServer           = tmpstr;
            ThisServerIsPrimary = FALSE;
            EnableMakeThisPrimaryButton = TRUE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = FALSE;
            DuplexMode = FALSE;
         }
         else
         {
	         m_HighAvailabilityIndicator = "Simplex Single Server Configuration";
            sprintf(tmpstr, "This Server Name  : %s", ThisMachineId);
            m_PrimaryServer             = tmpstr;
            m_SecondaryServer = "Other Server Name : None";
            ThisServerIsPrimary = FALSE;
            EnableMakeThisPrimaryButton = TRUE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = FALSE;
            DuplexMode = FALSE;
         }
         break;

      /* System in Duplex Mode, it is primary, this means:
         1) Two servers are communicating
         2) This is primary

         Only option that should be available is to switch the two
         servers
      */
      case ST1_SYS_I_AM_PRIMARY:
         if (*HighAvailability)
         {
	         m_HighAvailabilityIndicator = "Duplex Dual Server Configuration";
            sprintf(tmpstr, "Primary Server Name : %s", ThisMachineId);
            m_PrimaryServer             = tmpstr;
            sprintf(tmpstr, "Secondary Server Name : %s", OtherMachineId);
            m_SecondaryServer           = tmpstr;
            ThisServerIsPrimary = TRUE;
            EnableMakeThisPrimaryButton = FALSE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = TRUE;
            DuplexMode = TRUE;
         }
         else
         {
            AfxMessageBox( "Error in tf.ini, HIGH_AVAILABILITY_INDICATOR should be set to 1", MB_ICONSTOP | MB_OK );
         }
         break;

      /* System in Duplex Mode, it is Secondary, this means:
         1) Two servers are communicating
         2) This is Secondary

         Only option that should be available is to switch the two
         servers
      */
      case ST1_SYS_I_AM_SECONDARY:
         if (*HighAvailability)
         {
	         m_HighAvailabilityIndicator = "Duplex Dual Server Configuration";
            sprintf(tmpstr, "Primary Server Name : %s", OtherMachineId);
            m_PrimaryServer             = tmpstr;
            sprintf(tmpstr, "Secondary Server Name : %s", ThisMachineId);
            m_SecondaryServer           = tmpstr;
            ThisServerIsPrimary = FALSE;
            EnableMakeThisPrimaryButton = FALSE;
            EnableMakeOtherPrimaryButton = FALSE;
            EnableSwitchPrimaryButton = TRUE;
            DuplexMode = TRUE;
         }
         else
         {
            AfxMessageBox( "Error in tf.ini, HIGH_AVAILABILITY_INDICATOR should be set to 1", MB_ICONSTOP | MB_OK );
         }
         break;

      default:
         m_PrimaryServer = "Primary Server Name : UNDEFINED";
         m_SecondaryServer = "Secondary Server Name : UNDEFINED";
         EnableMakeThisPrimaryButton = FALSE;
         EnableMakeOtherPrimaryButton = FALSE;
         EnableSwitchPrimaryButton = FALSE;
         DuplexMode = FALSE;

         strcpy(tmpstr, "Could Not Determine Status of Servers");
         AfxMessageBox( tmpstr, MB_ICONSTOP | MB_OK );
         return false;
      }
   }
   else
   {
      AfxMessageBox( tmpstr, MB_ICONSTOP | MB_OK );
      return false;
   }

   sprintf(tmpstr, "This Server Name : %s", ThisMachineId);
   m_ThisMachineId      = tmpstr;
   sprintf(tmpstr, "Other Server Name : %s", OtherMachineId);
   m_OtherMachineId     = tmpstr;
   sprintf(tmpstr, "This XIPC Instance Name : %s", ThisXipcInstance);
   m_ThisXipcInstance   = tmpstr;
   sprintf(tmpstr, "Other XIPC Instance Name : %s", OtherXipcInstance);
   m_OtherXipcInstance  = tmpstr;

   return true;
}

void CSysConfig::OnMakeThisServerPrimary() 
{
   INT   response;
   BYTE  rcode;
   CHAR  ThisLoggerMSQueName  [MAX_QUE_NAME_SIZE];
   CHAR  error_msg[256];

   if (ThisServerIsPrimary)
   {
      AfxMessageBox( "This Server is Already Primary", MB_ICONSTOP | MB_OK );
   }
   else
   {
      response = AfxMessageBox( "Force This Server to be Primary?", 
                                 MB_ICONQUESTION | MB_YESNO);
      if (response == IDYES)
      {
         response = AfxMessageBox( "Are You Sure You Want to Force This Server to be Primary?\nLoss of Data Will Occur if This is Done Incorrectly!", 
                                    MB_ICONQUESTION | MB_YESNO);
         if (response == IDYES)
         {
            GetLoggerMSQueName(ThisLoggerMSQueName);

            rcode = srvccomm_send_be_primary_command(ThisLoggerMSQueName, error_msg);
            if (rcode == PTEMSG_OK)
            {
               AfxMessageBox( "Check The Event Viewer to Ensure Proper Primary/Secondary Designation", 
                              MB_ICONINFORMATION | MB_OK );

               ThisServerIsPrimary = TRUE;

               EndDialog(response);
            }
            else
               AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
         }
      }
      else
      {
         response = AfxMessageBox( "Is Ascendent on the Other Server Still Running?", 
                                    MB_ICONQUESTION | MB_YESNO);
         if (response == IDNO)
         {
            response = AfxMessageBox( "Are You Sure Ascendent on the Other Server is NOT Running?", 
                                     MB_ICONQUESTION | MB_YESNO);
            if (response == IDYES)
            {
               GetLoggerMSQueName(ThisLoggerMSQueName);

               rcode = srvccomm_send_be_primary_command(ThisLoggerMSQueName, error_msg);
               if (rcode == PTEMSG_OK)
               {
                  AfxMessageBox( "Check The Event Viewer to Ensure Proper Primary/Secondary Designation", 
                          MB_ICONINFORMATION | MB_OK );

                  ThisServerIsPrimary = TRUE;

                  EndDialog(response);
               }
               else
                  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
            }
            else
               AfxMessageBox( "If the Two Servers are in Duplex Mode, Use the Switch Command\n Otherwise, Shutdown Ascendent on the Other Server First", 
                           MB_ICONSTOP | MB_OK );
         }
         else
            AfxMessageBox( "If the Two Servers are in Duplex Mode, Use the Switch Command\n Otherwise, Shutdown Ascendent on the Other Server First", 
                           MB_ICONSTOP | MB_OK );
      }
   }
}

void CSysConfig::OnMakeOtherServerPrimary() 
{
   INT   response;
   BYTE  rcode;
   CHAR  OtherLoggerMSQueName  [MAX_QUE_NAME_SIZE];
   CHAR  ThisMachineId         [MAX_SYSTEM_NAME_SIZE];
   CHAR  OtherMachineId        [MAX_SYSTEM_NAME_SIZE];
   CHAR  error_msg[256];

   if (DuplexMode)
   {
      if (!ThisServerIsPrimary)
      {
         AfxMessageBox( "Other Server is Already Primary", MB_ICONSTOP | MB_OK );
         return;
      }

      response = AfxMessageBox( "Is Ascendent on the this Server Still Running?", 
                                 MB_ICONQUESTION | MB_YESNO);
      if (response == IDNO)
      {
         response = AfxMessageBox( "Are You Sure Ascendent on the this Server is NOT Running?", 
                                 MB_ICONQUESTION | MB_YESNO);
         if (response == IDYES)
         {
            response = GetThisMachineId(ThisMachineId);
            response = GetOtherMachineId(OtherMachineId);
            if( !srvccomm_change_connection( OtherMachineId ) )
            {
               srvccomm_change_connection( ThisMachineId );
               sprintf( error_msg, "Unable to Reconnect to Server %s", OtherMachineId );
               AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );

            }
            else
            {
               GetOtherLoggerMSQueName(OtherLoggerMSQueName);

               rcode = srvccomm_send_be_primary_command(OtherLoggerMSQueName, error_msg);
               if (rcode == PTEMSG_OK)
               {
                  AfxMessageBox( "Check the Event Viewer to Ensure Proper Primary/Secondary Designation", 
                          MB_ICONINFORMATION | MB_OK );

                  ThisServerIsPrimary = FALSE;
                  srvccomm_change_connection( ThisMachineId );

                  EndDialog(response);
               }
               else
               {
                  srvccomm_change_connection( ThisMachineId );
                  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
               }
            }
         }
         else
            AfxMessageBox( "If the Two Servers are in Duplex Mode, Use the Switch Command\n Otherwise, Shutdown Ascendent on the this Server First", 
                        MB_ICONSTOP | MB_OK );
      }
      else
         AfxMessageBox( "If the Two Servers are in Duplex Mode, Use the Switch Command\n Otherwise, Shutdown Ascendent on the this Server First", 
                        MB_ICONSTOP | MB_OK );
   }
   else
      AfxMessageBox( "Logger/Reply Not in Duplex Mode", MB_ICONSTOP | MB_OK );
}

void CSysConfig::OnSwitchPrimarySecondary() 
{
   INT   response;
   BYTE  rcode;
   CHAR  LoggerMSQueName  [MAX_QUE_NAME_SIZE];
   CHAR  ThisMachineId    [MAX_SYSTEM_NAME_SIZE];
   CHAR  OtherMachineId   [MAX_SYSTEM_NAME_SIZE];
   CHAR  error_msg[256];

   if (DuplexMode)
   {
      response = AfxMessageBox( "Is Ascendent on Both Servers Still Running?", 
                                 MB_ICONQUESTION | MB_YESNO);
      if (response == IDYES)
      {
         response = AfxMessageBox( "Are You Sure Ascendent on Both Servers is Running\nAnd Servers are in Duplex Mode?", 
                                 MB_ICONQUESTION | MB_YESNO);
         if (response == IDYES)
         {
            if (ThisServerIsPrimary)
            {
               GetLoggerMSQueName(LoggerMSQueName);

               rcode = srvccomm_send_switch_primary_command(LoggerMSQueName, error_msg);
               if (rcode == PTEMSG_OK)
               {
                  AfxMessageBox( "Check the Event Viewer to Ensure Proper Primary/Secondary Designation", 
                       MB_ICONINFORMATION | MB_OK );

                  EndDialog(response);
               }
               else
                  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
            }
            else
            {
               GetOtherLoggerMSQueName(LoggerMSQueName);
               response = GetThisMachineId(ThisMachineId);
               response = GetOtherMachineId(OtherMachineId);
               if( !srvccomm_change_connection( OtherMachineId ) )
               {
                  srvccomm_change_connection( ThisMachineId );
                  sprintf( error_msg, "Unable to Reconnect to Server %s", OtherMachineId );
                  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );

               }

               rcode = srvccomm_send_switch_primary_command(LoggerMSQueName, error_msg);
               if (rcode == PTEMSG_OK)
               {
                  AfxMessageBox( "Check the Event Viewer to Ensure Proper Primary/Secondary Designation", 
                       MB_ICONINFORMATION | MB_OK );

                  srvccomm_change_connection( ThisMachineId );
                  EndDialog(response);
               }
               else
               {
                  srvccomm_change_connection( ThisMachineId );
                  AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
               }
            }
         }
         else
            AfxMessageBox( "Cannot Use the Switch Command", MB_ICONSTOP | MB_OK );
      }
      else
         AfxMessageBox( "Cannot Use the Switch Command", MB_ICONSTOP | MB_OK );
   }
   else
      AfxMessageBox( "Logger/Reply Not in Duplex Mode", MB_ICONSTOP | MB_OK );
}

BOOL CSysConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_MakeThisServerPrimary.EnableWindow(EnableMakeThisPrimaryButton);
   m_MakeOtherServerPrimary.EnableWindow(EnableMakeOtherPrimaryButton);
   m_SwitchPrimarySecondary.EnableWindow(EnableSwitchPrimaryButton);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
