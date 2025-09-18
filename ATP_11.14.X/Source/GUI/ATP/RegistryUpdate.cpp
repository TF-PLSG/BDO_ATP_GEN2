
/*************************************************************************

Registry.Cpp :

  This file contains the functions to Open, Get & Set Registry Values from
  the Regitry file used for updating Idle Login Screen State

*************************************************************************/

#include "stdafx.h"
#include "atlbase.h"
#include "RegistryUpdate.h"


short CRegistryUpdate::OpenRegistryFile()
{
	ret_val = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(REGISTRY_FILE_PATH),
             0, KEY_ALL_ACCESS, &hKey);

	if(ret_val != 0)
	ret_val = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT(REGISTRY_FILE_PATH),
					   0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					   NULL, &hKey, &dwDisp);

	//DO NO CLOSE THE REGISTRY FILE HERE
	//RegCloseKey(hKey);
	return ret_val;
}

short CRegistryUpdate::GetRegistryParam()
{
	//ret_val = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(REGISTRY_FILE_PATH),
    //         0, KEY_ALL_ACCESS, &hKey);

	if(0 == OpenRegistryFile())
	{
		bufferlength = sizeof(buffer);
		memset(buffer, 0x00, sizeof(buffer));
		
		ret_val = RegQueryValueEx(hKey, TEXT("IdleState"), NULL, &datatype, buffer, &bufferlength);
		
		if(0 == ret_val)
		{
			CloseRegistryFile();		

			return atoi((const char *) buffer);
		}
		
	}
	return -1;
}

short CRegistryUpdate::SetRegistryParam(char val)
{
	//ret_val = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT(REGISTRY_FILE_PATH),
    //         0, KEY_ALL_ACCESS, &hKey);

	if(0 == OpenRegistryFile())
	{		
		State[0] = val;
		State[1] = '\0';
		ret_val = RegSetValueEx(hKey, "IdleState", NULL, REG_SZ, State, 1);
		//RegCloseKey(hKey);
		CloseRegistryFile();
		return ret_val;
	}
	return -1;

}

short CRegistryUpdate::CloseRegistryFile()
{
	RegCloseKey(hKey);
	return ret_val;
}