#include "Amex3DS2ParameterDefinition.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CAmex3DS2PropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CAmex3DS2PropertySheet)  // *

public:
	CAmex3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CAmex3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CAmex3DS2Parameter m_Amex3DS2ParameterPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CAmex3DS2PropertySheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

