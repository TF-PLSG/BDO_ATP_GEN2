#include "Diners3DS2ParameterDefinition.h"
#include "ChangeHistory.h"
// this class will give the page window .


class CDiners3DS2PropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDiners3DS2PropertySheet)  // *

public:
	CDiners3DS2PropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0); // as per rqrmnt
	CDiners3DS2PropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);//// as per rqrmnt
 // *


public:

	CDiners3DS2Parameter m_Diners3DS2ParameterPage;
	CChangeHistory m_ChangeHistoryPage;
	int m_nActiveTab;
	void SetData(); // if u want multiple page like 
					// here we have ATP_Monitor_Service with this if u want change history u will use 
					//  SetData fun
	public:
	virtual BOOL OnInitDialog();

public:
		virtual ~CDiners3DS2PropertySheet();

protected:
	DECLARE_MESSAGE_MAP() // reqrd
};

