#include "OnlinePINConfigureBDKKey.h"
#include "OnlinePINConfigureBDKKeyGeneral.h"
#include "ChangeHistory.h"
// this class will give the page window .


class COnlinePINConfigureBDKKeySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COnlinePINConfigureBDKKeySheet)  

public:
	COnlinePINConfigureBDKKeySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	COnlinePINConfigureBDKKeySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 


public:

	CBDKKeyConfigurationLink m_ConfigureBDKmainPage;
	CBDKKeyConfigurationGeneral m_ConfigureBDKGenralPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	BOOL COnlinePINConfigureBDKKeySheet::OnInitDialog();

public:
		virtual ~COnlinePINConfigureBDKKeySheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

