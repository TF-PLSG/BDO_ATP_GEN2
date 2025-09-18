#include "Cup3DS2ParameterDefinition.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CCup3DS2PropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCup3DS2PropertySheet)  // *

public:
	CCup3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CCup3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CCup3DS2Parameter m_Cup3DS2ParameterPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CCup3DS2PropertySheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

