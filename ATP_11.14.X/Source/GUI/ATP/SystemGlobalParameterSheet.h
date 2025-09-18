#include "SystemGlobalParameterDefinition.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CSystemGlobalParameterSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSystemGlobalParameterSheet)  // *

public:
	CSystemGlobalParameterSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CSystemGlobalParameterSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CSystemGlobalParameter m_SystemGlobalParameterPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CSystemGlobalParameterSheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

