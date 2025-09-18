#include "FraudControlConfigureFraudRule.h"
#include "FraudControlConfigureFraudRuleGeneral.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CFraudControlConfigureRuleSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFraudControlConfigureRuleSheet)  // *

public:
	CFraudControlConfigureRuleSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CFraudControlConfigureRuleSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CFraudControlConfigurationLink m_ConfigureRulemainPage;
	CFraudControlConfigurationFraudRule m_ConfigureRuleGenralPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CFraudControlConfigureRuleSheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

