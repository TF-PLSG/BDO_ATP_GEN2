//ptepci.h

/*
Krishna@TF : Header file for class CPtepci
*/

#ifndef _PTEPCI
#define _PTEPCI



class CPtepci
{

public :

	//Krishna@TF, 18.5.11, Function to Validate User ID & PWD during Idle Login

	short ValidateIdleLogin(char m_userid[], char m_userpwd[], char *errormsg);
	short BlockUser(char m_userid[], char *error);
	short isalphabet(char string[]);
	short isnumeric(char string[]);
};
#endif