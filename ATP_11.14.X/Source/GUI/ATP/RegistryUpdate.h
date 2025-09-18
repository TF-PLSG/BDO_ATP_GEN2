

/*************************************************************************

Registry.Cpp :

  This file contains the functions to Open, Get & Set Registry Values from
  the Regitry file used for updating Idle Login Screen State

*************************************************************************/


#define REGISTRY_FILE_PATH  "Software\\ATP"
#define RGISTRY_CHECK_INTERVAL 5
class CRegistryUpdate
{

public : 

		LONG result;
		HKEY hKey;
		unsigned long dwDisp;
		int ret_val;

		unsigned char buffer[_MAX_PATH];
		unsigned long datatype;
		unsigned long bufferlength;
		unsigned char State[2];


		short OpenRegistryFile();
		short SetRegistryParam(char val);
		short GetRegistryParam();
		short CloseRegistryFile();
	
};