#include "FraudControlConfigureLinkMainPage.h"
#include "FraudControlConfigureLinkGeneralPage.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CFraudControlConfigureLinkSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFraudControlConfigureLinkSheet)  

public:
	CFraudControlConfigureLinkSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CFraudControlConfigureLinkSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CFraudControlConfigurationLinkpage m_ConfigureLinkPage;
	CFraudControlConfigurationLinkGeneralpage m_ConfigureGenralPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CFraudControlConfigureLinkSheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

