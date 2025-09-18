/******************************************************************************
  
        Module: des.c
  
         Title: DES Encryption APIs
  
   Description: This module provides a set of APIs to encrypt/decrypt data
                using DES.

   Application: general   

   $Log:   N:\PVCS\PTE\des.c  $
   
      Rev 1.0   Oct 23 1997 12:18:20   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.0   Jun 09 1997 11:10:20   JGRGURIC
   Added as part of the initial Pinnacle
   security implementation for password
   encryption/decryption.
   
******************************************************************************/

/*
	 This is a 'C' implementation of the National Bureau of Statistics
	 Data Encryption Standard.

	 All attempts have been made to use standard C facilities to maintain
	 transportability.

	 The DES procedure accepts 64-bits of data, 64-bits of key, an
	 indication of whether the required function is an ENCRYPT or DECRYPT,
	 and a 52 word work area.

	 The output from the DES procedure is the data (which is now
	 ENCRYPTed or DECRYPTed according to the input parameters).

	 The work area is used for efficiency reasons, and may be used to avoid
	 recalculating the subkeys if a single key is used repeatedly (as is the
	 case in MAC processing).


	 C Language Function Definition :

					DES( data, key, encode, work )

					unsigned short  data[4],
												 key[4],
												 encrypt,
												 work[52];

	 where the following rules apply :

	 in:     data        64 bits of data to be encrypted or decrypted. The most
											 significant byte is data[0]; the least significant byte
											 is data[3].
					 key         64 bits of encryption key. This routine does not check
											 the parity of the key before using it; It is the
											 responsibility of the calling routine to ensure that the
											 key is in good working order.
					 encrypt     FALSE (zero)     - selects a DECRYPT operation
											 TRUE  (non-zero) - selects an ENCRYPT operation
					 work        This area is used to store the sub-keys for the supplied
											 key. If the same key is being used repeatedly, savings
											 can be made by eliminating the recalculation of the
											 sub-keys for each operation by passing the same work
											 area back to this function, without modifying any of the
											 work area between calls.

	 out:    data        The output of the encryption or decryption process.
											 The original data is destroyed.
					 key         Unchanged.
					 encrypt     Unchanged.
					 work        Contains the key and the subkeys from the operation
											 just performed.
											 NOTE: The work area should be zeroed if there is no need
											 to use the same key in the next operation (for security
											 reasons)
*/
/**/
/*
		Calling this routine from a C environment :

				 A sample program is listed below :


#include <stdio.h>

#define TRUE  -1
#define FALSE 0

unsigned short key[] = { 0x0123, 0x4567, 0x89ab, 0xcdef },
			     data[]= { 0x4e6f, 0x7720, 0x6973, 0x2074 },
			   work[52];

void DES( unsigned short *, unsigned short *, int, unsigned short  * );

void main(void);
void main()
{
	printf( "   Plain Text          Cipher Text        Decipher text \n" );
	printf( "----------------    ----------------    ----------------\n" );

	printf( "%04X%04X%04X%04X    ", data[0], data[1], data[2], data[3] );

	DES( data, key, TRUE, work );

	printf( "%04X%04X%04X%04X    ", data[0], data[1], data[2], data[3] );

	DES( data, key, FALSE, work );

	printf( "%04X%04X%04X%04X\n", data[0], data[1], data[2], data[3] );

}



		 The output should look like


				Plain Text          Cipher Text        Decipher text
		 ----------------    ----------------    ----------------
		 4E6F772069732074    3FA40E8A984D4815    4E6F772069732074




	 Calling this function from a non-C environment :

				 In C, when an array is passed as an argument, the address of the
				 start of the array is placed on the stack. When an int value is
				 passed as a parameter, the value of that variable is placed on the
				 stack.

				 For this function, there will be three addresses ( for data, key,
				 and work) and one value ( for encrypt ).

				 It is necessary to establish exactly how the C compiler used to
				 compile this function expects to see the parameters on the stack
				 before being able to call this function from any non-C environment.


*/
/**/

#include <memory.h>

/*
	 Offsets to tranformation rules
 */
#define IP       0   /* Initial Permutation         64 * 64   */
#define IP_1    65   /* Inverse Initial Permutation 64 * 64   */
#define E      130   /* E Function                  32 * 48   */
#define P      179   /* P Function                  32 * 32   */
#define PC1A   212   /* Permutation Choice 1 - C0   64 * 28   */
#define PC1B   245   /* Permutation Choice 1 - D0   64 * 28   */
#define PC2    278   /* Permutation Choice 2        56 * 48   */

/*
	 General defines
 */
#define TRUE  -1
#define FALSE 0

typedef unsigned char byte;     /* If not defined as standard */

/*
	 The Microsoft C compiler will only support 8 parameters, so the
	 tform macro has been split into two separate macros. The net effect
	 is the same
 */
#define tform1(a,b,c,d,e,f,g,h)  a-1, b-1, c-1, d-1, e-1, f-1, g-1, h-1,
#define tform2(a,b,c,d,e,f,g,h)  a-1, b-1, c-1, d-1, e-1, f-1, g-1, h-1 | 0x80,

/**/
static byte rules[] =  {
/* 0    Initial Permutation             64 * 64            */
tform1( 58, 50, 42, 34, 26, 18, 10,  2)tform2( 60, 52, 44, 36, 28, 20, 12,  4)
tform1( 62, 54, 46, 38, 30, 22, 14,  6)tform2( 64, 56, 48, 40, 32, 24, 16,  8)
tform1( 57, 49, 41, 33, 25, 17,  9,  1)tform2( 59, 51, 43, 35, 27, 19, 11,  3)
tform1( 61, 53, 45, 37, 29, 21, 13,  5)tform2( 63, 55, 47, 39, 31, 23, 15,  7)
0xFF,
/* 65   Inverse Initial Permutation     64 * 64            */
tform1( 40,  8, 48, 16, 56, 24, 64, 32)tform2( 39,  7, 47, 15, 55, 23, 63, 31)
tform1( 38,  6, 46, 14, 54, 22, 62, 30)tform2( 37,  5, 45, 13, 53, 21, 61, 29)
tform1( 36,  4, 44, 12, 52, 20, 60, 28)tform2( 35,  3, 43, 11, 51, 19, 59, 27)
tform1( 34,  2, 42, 10, 50, 18, 58, 26)tform2( 33,  1, 41,  9, 49, 17, 57, 25)
0xFF,
/* 130  E Function                      32 * 48            */
tform1( 32,  1,  2,  3,  4,  5,  4,  5)tform2(  6,  7,  8,  9,  8,  9, 10, 11)
tform1( 12, 13, 12, 13, 14, 15, 16, 17)tform2( 16, 17, 18, 19, 20, 21, 20, 21)
tform1( 22, 23, 24, 25, 24, 25, 26, 27)tform2( 28, 29, 28, 29, 30, 31, 32,  1)
0xFF,
/* 179  P Function                      32 * 32            */
tform1( 16,  7, 20, 21, 29, 12, 28, 17)tform2(  1, 15, 23, 26,  5, 18, 31, 10)
tform1(  2,  8, 24, 14, 32, 27,  3,  9)tform2( 19, 13, 30,  6, 22, 11,  4, 25)
0xFF,
/* 212  Permutation Choice 1 - C0       64 * 28            */
tform1( 57, 49, 41, 33, 25, 17,  9,  1)tform2( 58, 50, 42, 34, 26, 18, 10,  2)
tform1( 59, 51, 43, 35, 27, 19, 11,  3)tform2( 60, 52, 44, 36,  1,  1,  1,  1)
0xFF,
/* 245  Permutation Choice 1- D0        64 * 28            */
tform1( 63, 55, 47, 39, 31, 23, 15,  7)tform2( 62, 54, 46, 38, 30, 22, 14,  6)
tform1( 61, 53, 45, 37, 29, 21, 13,  5)tform2( 28, 20, 12,  4,  1,  1,  1,  1)
0xFF,
/* 278  Permutation Choice 2            56 * 48            */
tform1( 14, 17, 11, 24,  1,  5,  3, 28)tform2( 15,  6, 21, 10, 23, 19, 12,  4)
tform1( 26,  8, 16,  7, 27, 20, 13,  2)tform2( 41, 52, 31, 37, 47, 55, 30, 40)
tform1( 51, 45, 33, 48, 44, 49, 39, 56)tform2( 34, 53, 46, 42, 50, 36, 29, 32)
0xFF
};
/**/
/* Selection Functions */
static byte s[] = {
/* S1 */
			 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
				0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
				4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
			 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
/* S2 */
			 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
				3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
				0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
			 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
/* S3 */
			 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
			 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
			 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
				1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
/* S4 */
				7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
			 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
			 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
				3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
/* S5 */
				2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
			 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
				4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
			 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
/* S6 */
			 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
			 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
				9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
				4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
/* S7 */
				4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
			 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
				1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
				6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
/* S8 */
			 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
				1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
				7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
				2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

/**/
/*
	 Shift Values
 */
static byte shft[] =  { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

void TRANSFORM( unsigned short  *, unsigned short  *, unsigned short  );
void TRANSFORM( in, out, base_rule )

/* transform up to 64 bits according to a rule */

unsigned short  in[], out[], base_rule;

{

		static unsigned short  bits[] =  { 0x8000,
																		0x4000,
																		0x2000,
																		0x1000,
																		0x0800,
																		0x0400,
																		0x0200,
																		0x0100,
																		0x0080,
																		0x0040,
																		0x0020,
																		0x0010,
																		0x0008,
																		0x0004,
																		0x0002,
																		0x0001 };

		unsigned short  outword,x,i;

		i = 0;
		outword = 0;
		while ( (x = rules[base_rule]) != 0xFF ) {
				outword <<= 1;               /* shift the result left one bit   */
																		 /* (x & 060) >> 4 is index into in */
																		 /* x & 017        is bit #  in     */

				outword |= ( (in[ (x & 060 ) >> 4 ] & bits[ x & 017 ]) ? 1 : 0 );

				if (x & 0200) {
						out[i] = outword;
						i++;
				};
				base_rule++;
		}
}

/**/
unsigned short  bf( unsigned short , unsigned short , unsigned short  );
unsigned short  bf( x, p1, p2 ) /* get bit field from bit p1 to bit p2 */
														 /* bits are numbered msb=0, lsb=15 */
unsigned short  x, p1, p2;
{

		return ( (x >> 15-p2) & ~(~0 << p2-p1+1) );

}

#define s_function(s_value,in)  s[ s_value + map[in] ]

void SELECT( unsigned short  *, unsigned short  * );
void SELECT( in, out )
unsigned short  in[], out[];
{
		static byte map[] = {
				0, 16,  1, 17,  2, 18,  3, 19,  4, 20,  5, 21,  6, 22,  7, 23,
				8, 24,  9, 25, 10, 26, 11, 27, 12, 28, 13, 29, 14, 30, 15, 31,
			 32, 48, 33, 49, 34, 50, 35, 51, 36, 52, 37, 53, 38, 54, 39, 55,
			 40, 56, 41, 57, 42, 58, 43, 59, 44, 60, 45, 61, 46, 62, 47, 63
		};

		unsigned short  hi, lo;

		hi     = s_function(   0, bf(in[0],0,5)  )                          << 4  |
						 s_function(  64, bf(in[0],6,11) )                          ;
		lo     = s_function( 128, ((bf(in[0],12,15) << 2) | bf(in[1],0,1))) <<  4 |
						 s_function( 192, bf(in[1],2,7)  );
		lo     = lo & 0xFF;
		out[0] = hi * 0x100 + lo;

		hi     = s_function( 256, bf(in[1],8,13) )                          << 4  |
						 s_function( 320, ((bf(in[1],14,15) << 4) | bf(in[2],0,3))) ;
		lo     = s_function( 384, bf(in[2],4,9)  )                          <<  4 |
						 s_function( 448, bf(in[2],10,15));
		lo     = lo & 0xFF;
		out[1] = hi * 0x100 + lo;

}

/**/
void SETKS( unsigned short  *, unsigned short  * );
void SETKS( key, subkeys )

unsigned short  key[],
						 subkeys[];
{
		unsigned short  i,
								 y,
								 z,
								 C[2],       /* key permutation register 28 bits */
								 D[2],       /* key permutation register 28 bits */
								 CD[4];      /* combined key permutation register 56 bits */

		TRANSFORM( key, C, PC1A );
		TRANSFORM( key, D, PC1B );

		for ( i = 0 ; i <= 15 ; i++ )
		{
				y = shft[i];
				switch(y)
				{
				case 1 : z = bf(C[0],0,0);
		 C[0] = bf(C[0],1,15) << 01;              /* 1.0.2 */
								 C[0] |=  bf(C[1],0,0);
		 C[1] = bf(C[1],1,11) << 05;              /* 1.0.2 */
								 C[1] |=  z << 4;

								 z = bf(D[0],0,0);
		 D[0] = bf(D[0],1,15) << 01;              /* 1.0.2 */
								 D[0] |=  bf(D[1],0,0);
		 D[1] = bf(D[1],1,11) << 05;              /* 1.0.2 */
								 D[1] |=  z << 4;

								 break;

				case 2 : z = bf(C[0],0,1);
		 C[0] = bf(C[0],2,15) << 02;              /* 1.0.2 */
								 C[0] |=  bf(C[1],0,1);
		 C[1] = bf(C[1],2,11) << 06;              /* 1.0.2 */
								 C[1] |=  z << 4;

								 z = bf(D[0],0,1);
		 D[0] = bf(D[0],2,15) << 02;              /* 1.0.2 */
								 D[0] |=  bf(D[1],0,1);
		 D[1] = bf(D[1],2,11) << 06;              /* 1.0.2 */
								 D[1] |=  z << 4;

								 break;
				}
				CD[0] = C[0];
				CD[1] = bf(C[1],0,11) << 4 | bf(D[0],0,3);
				CD[2] = bf(D[0],4,15) << 4 | bf(D[1],0,3);
				CD[3] = bf(D[1],4,15) << 4;
				TRANSFORM( CD, &subkeys[3*i], PC2 );
		}

}

/**/
void DES( unsigned short  *, unsigned short  *, unsigned short , unsigned short  * );
void DES( text, key, encrypt, work )
unsigned short  text[4],
						 key[4],
						 encrypt,
						 work[52];  /* four words of key, and 48 words of subkeys */
{
		unsigned short  start = 15, /* defaults are set to decode values */
				end_  = 0xffff,
				incr  = 0xffff;

		unsigned short  L[4],       /* leftmost (and rightmost) 32 bits */
								 TEMP1[4],
								 TEMP2[4];
		unsigned short           i,xq;

		if ( encrypt )
		{
				start = 0;
				end_  = 16;
				incr  = 1;
		}

		TRANSFORM( text, L, IP );

		if ( work[0] != key[0] ||   /* if the key in the work area is the same as */
				 work[1] != key[1] ||   /* the supplied key, the subkeys in the sork  */
				 work[2] != key[2] ||   /* area have already been calculated          */
				 work[3] != key[3] )
		{
				 work[0] = key[0];
				 work[1] = key[1];
				 work[2] = key[2];
				 work[3] = key[3];

				 SETKS( key, &work[4] );
		}

		i = start;
		do
		{
				TRANSFORM( &L[2], TEMP1, E );
				xq = 3*i + 4;
				TEMP2[0] = TEMP1[0] ^ work[xq++];
				TEMP2[1] = TEMP1[1] ^ work[xq++];
				TEMP2[2] = TEMP1[2] ^ work[xq++];

				SELECT( TEMP2, TEMP1 );
				TRANSFORM( TEMP1, TEMP2, P );

				TEMP1[0] = TEMP2[0] ^ L[0];
				TEMP1[1] = TEMP2[1] ^ L[1];

				L[0] = L[2];         /* L ':=' R for 2 */
				L[1] = L[3];
				L[2] = TEMP1[0];     /* R ':=' TEMP1 for 2 */
				L[3] = TEMP1[1];

				i += incr;
		}
		while (i != end_);

		TEMP1[2] = L[0];        /* TEMP1[2] ':=' L for 2 */
		TEMP1[3] = L[1];

		TRANSFORM( TEMP1, text, IP_1 );
}

int DESWeakKey(unsigned short  key[4])
{
	unsigned short  i;

	unsigned short  weakkey[4][4] = {
		{ 0x0000, 0x0000, 0x0000, 0x0000 },
		{ 0x0000, 0x0000, 0xFFFF, 0xFFFF },
		{ 0xFFFF, 0xFFFF, 0x0000, 0x0000 },
		{ 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF },
	};

	for (i = 0 ; i < 4 ; i++)
		{
		if ((weakkey[i][0] == key[0]) && (weakkey[i][1] == key[1]) &&
				(weakkey[i][2] == key[2]) && (weakkey[i][3] == key[3]) )
			return(TRUE);
		}

	return(FALSE);


}


