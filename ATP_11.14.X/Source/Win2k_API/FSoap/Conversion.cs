using System;

namespace FSOAP
{
	public class Conversion
	{
		private static readonly byte[] ebcdicChart = 
		{
			(byte)0xF0, (byte)0xF1, (byte)0xF2, (byte)0xF3, (byte)0xF4, 
			(byte)0xF5, (byte)0xF6, (byte)0xF7, (byte)0xF8, (byte)0xF9,
			(byte)0x81, (byte)0x82, (byte)0x83, (byte)0x84, (byte)0x85, 
			(byte)0x86, (byte)0x87, (byte)0x88, (byte)0x89, (byte)0x91,
			(byte)0x92, (byte)0x93, (byte)0x94, (byte)0x95, (byte)0x96, 
			(byte)0x97, (byte)0x98, (byte)0x99, (byte)0xA2, (byte)0xA3,
			(byte)0xA4, (byte)0xA5, (byte)0xA6, (byte)0xA7, (byte)0xA8, 
			(byte)0xA9, (byte)0xC1, (byte)0xC2, (byte)0xC3, (byte)0xC4,
			(byte)0xC5, (byte)0xC6, (byte)0xC7, (byte)0xC8, (byte)0xC9, 
			(byte)0xD1, (byte)0xD2, (byte)0xD3, (byte)0xD4, (byte)0xD5,
			(byte)0xD6, (byte)0xD7, (byte)0xD8, (byte)0xD9, (byte)0xE2, 
			(byte)0xE3, (byte)0xE4, (byte)0xE5, (byte)0xE6, (byte)0xE7,
			(byte)0xE8, (byte)0xE9, (byte)0x40, (byte)0x4B, (byte)0x4C, 
			(byte)0x4D, (byte)0x4E, (byte)0x4F, (byte)0x50, (byte)0x5A,
			(byte)0x5B, (byte)0x5C, (byte)0x5D, (byte)0x5E, (byte)0x60, 
			(byte)0x61, (byte)0x6B, (byte)0x6C, (byte)0x6D, (byte)0x6E,
			(byte)0x6F, (byte)0x7A, (byte)0x7B, (byte)0x7C, (byte)0x7D, 
			(byte)0x7E, (byte)0x7F, (byte)0x5F, (byte)0x9A
		};

		private static readonly byte[] asciiChart = 
		{
			(byte)0x30, (byte)0x31, (byte)0x32, (byte)0x33, (byte)0x34, 
			(byte)0x35, (byte)0x36, (byte)0x37, (byte)0x38, (byte)0x39,
			(byte)0x61, (byte)0x62, (byte)0x63, (byte)0x64, (byte)0x65,
			(byte)0x66, (byte)0x67, (byte)0x68, (byte)0x69, (byte)0x6A,
			(byte)0x6B, (byte)0x6C, (byte)0x6D, (byte)0x6E, (byte)0x6F, 
			(byte)0x70, (byte)0x71, (byte)0x72, (byte)0x73, (byte)0x74,
			(byte)0x75, (byte)0x76, (byte)0x77, (byte)0x78, (byte)0x79, 
			(byte)0x7A, (byte)0x41, (byte)0x42, (byte)0x43, (byte)0x44,
			(byte)0x45, (byte)0x46, (byte)0x47, (byte)0x48, (byte)0x49, 
			(byte)0x4A, (byte)0x4B, (byte)0x4C, (byte)0x4D, (byte)0x4E,
			(byte)0x4F, (byte)0x50, (byte)0x51, (byte)0x52, (byte)0x53, 
			(byte)0x54, (byte)0x55, (byte)0x56, (byte)0x57, (byte)0x58,
			(byte)0x59, (byte)0x5A, (byte)0x20, (byte)0x2E, (byte)0x3C, 
			(byte)0x28, (byte)0x2B, (byte)0x7C, (byte)0x26, (byte)0x21,
			(byte)0x24, (byte)0x2A, (byte)0x29, (byte)0x3B, (byte)0x2D, 
			(byte)0x2F, (byte)0x2C, (byte)0x25, (byte)0x5F, (byte)0x3E,
			(byte)0x3F, (byte)0x3A, (byte)0x23, (byte)0x40, (byte)0x27, 
			(byte)0x3D, (byte)0x22, (byte)0x5E, (byte)0x5E
		};

		public static byte[] Ebcdic2Ascii(byte[] ebcdic)
		{

			bool found;
			byte[] ascii = new byte[ebcdic.Length];
			int numOfCharacters = ebcdicChart.Length;
      
			for (int j = 0; j < ebcdic.Length; j++)
			{
				found = false;
				/* CONVERT EBCDIC TO ASCII */
				for(int i = 0; i < numOfCharacters  &&  !found; i++ )
				{
					if ( ebcdic[j] == ebcdicChart[i] )
					{
						ascii[j] = asciiChart[i];
						found = true;
					}
				}
				if (found == false)
					ascii[j] = 0x00;
			}
			return ascii;
		}
   
		public static byte[] Ascii2Ebcdic(byte[] ascii)
		{
			bool found = false;
			byte[]  ebcdic = new byte[ascii.Length];
    
			for (int j = 0; j < ascii.Length; j++)
			{
				found = false;
				/* CONVERT ASCII TO EBCDIC */
				for(int i = 0; i < asciiChart.Length  &&  !found; i++ )
				{
					if ( ascii[j] == asciiChart[i] )
					{
						ebcdic[j] = ebcdicChart[i];
						found = true;
					}
				}
				if ( found == false)
					ebcdic[j] = 0x00;
			}
			return ebcdic;
		}

		
		/** This function converts an ASCII string (containing characters '0' to '9' or 'A' to 'F' or 
		'a' to 'f' or characters 0x3A to 0x3F)
		to a packed byte format based on the length of the string
		Depending on the mode, the characters are converted  starting with the first character or the last
		character. For example if the mode is 2, it starts from the first character, such that
		"1234" will be converted to 0x12 0x34 - and "123" will be converted to 0x12 0x30,
		if the mode is 1, it starts from the last character, such that
		"1234" will be converted to 0x12 0x34 - but "123" will be converted to 0x01 0x23
		if the mode is 1, it starts from the last character, such that
		"12=34" will be converted to 0x01 0x2d 0x34 
		**/

		public static byte[] Ascii2PackedByte (byte[] src, int mode) 
		{
			int i;
			if (mode != 1 && mode != 2)
				return null;
			int len = src.Length;
			byte[] dest = new byte[(int)(Math.Ceiling(len/2.0))];
			if (mode == 2)
			{
				for (i = 0; i < len; i+=2)
				{
					if ((src[i] >= '0' && src[i] <= '9') || (src[i] >= 0x3A && src[i] <= 0x3F))
						dest[i/2] = (byte)((src[i] - '0') << 4);
					else if (src[i] >= 'A' && src[i] <= 'F')
						dest[i/2] = (byte)(((src[i] - 'A') + 0x0A) << 4);
					else if (src[i] >= 'a' && src[i] <= 'f')
						dest[i/2] = (byte)(((src[i] - 'a') + 0x0A) << 4);
					else return null;

					if ((i + 1) < len) 
					{
						if ((src[i+1] >= '0' && src[i+1] <= '9') || (src[i+1] >= 0x3A && src[i+1] <= 0x3F))
							dest[i/2] |= (byte)(src[i+1] - '0');
						else if (src[i+1] >= 'A' && src[i+1] <= 'F')
							dest[i/2] |= (byte)((src[i+1] - 'A') + 0x0A);
						else if  (src[i+1] >= 'a' && src[i+1] <= 'f')
							dest[i/2] |= (byte)((src[i+1] - 'a') + 0x0A);
						else return null;
					}
				}
			}
			else if (mode == 1)
			{
				for (i = len-1; i > 0; i-=2)
				{
					if ((src[i] >= '0' && src[i] <= '9') || (src[i] >= 0x3A && src[i] <= 0x3F))
						dest[i/2] = (byte)((src[i] - '0') << 4);
					else if (src[i] >= 'A' && src[i] <= 'F')
						dest[i/2] = (byte)(((src[i] - 'A') + 0x0A) << 4);
					else if (src[i] >= 'a' && src[i] <= 'f')
						dest[i/2] = (byte)(((src[i] - 'a') + 0x0A) << 4);

					if ((i - 1) > 0) 
					{
						if ((src[i-1] >= '0' && src[i-1] <= '9') || (src[i-1] >= 0x3A && src[i-1] <= 0x3F))
							dest[i/2] |= (byte)(src[i-1] - '0');
						else if (src[i-1] >= 'A' && src[i-1] <= 'F')
							dest[i/2] |= (byte)((src[i+1] - 'A') + 0x0A);
						else if  (src[i-1] >= 'a' && src[i-1] <= 'f')
							dest[i/2] |= (byte)((src[i-1] - 'a') + 0x0A);
					}
				}
			}
			return dest;
		}

		/**
		* This function will convert from raw binary to binary that represent
		* ascii characters, for example, if the raw binary is represented as
		* 0x013C, the function will generate the binary 0x30313343, i.e. we go
		* from 2 bytes to 4 bytes - this will handle not only BCD values but also
		* alpha values
		*/
		public static byte[] PackedByte2Ascii(byte[] src) 
		{
			int i, j;
			int len = src.Length * 2;
			byte[] dest = new byte[len];

			for (i = 0; i < len; i+=2)
			{
				j =  src[i/2];
				// Note: j >>> 4 is the same as / 16
				if ((j >> 4) <= 9) /* BCD */ /* Dividing by 16 moves 4 bits to the >> */
					dest[i] = (byte)((j >> 4) + 0x30); // '0' is encoded as 0x30
				else 
					dest[i] = (byte)(((j >> 4) - 0x0A) + 0x41); // 'A' is encoded as 0x41

				if ((j & 0x0F) <= 9)
					dest[i+1] = (byte)((j & 0x0F) + 0x30);
				else  
					dest[i+1] = (byte)(((j & 0x0F) - 0x0A) + 0x41);
			}
			return dest;
		}

		/* This function converts a byte array containing binary data to a long integer
		 * - example if the byte array contains {0x12,0x34}, it is converted to 1234
		 * - if non bcd values are passed to the function, an exception is thrown 
		 */
		public static long PackedByte2Long(byte[] bcdData)
		{
			int numDigits = bcdData.Length*2;
			if (numDigits > 20)
			{
				throw new Exception("PackedByte2Long: BCD number is too large for unsigned long.  Max 20 digits.");
			}
			long result = 0;
			for (int i=0, j=(numDigits-1); i < bcdData.Length; i++)
			{
				if ((((bcdData[i] & 0xF0) >> 4) > 9) || ((bcdData[i] & 0x0F) > 9))
					throw new Exception("PackedByte2Long: Invalid data passed to function");

				result += ((bcdData[i] & 0xF0) >> 4) * ((long)Math.Pow(10, j--));
				result +=  (bcdData[i] & 0x0F) * ((long)Math.Pow(10, j--));
			}
			return result;
		}

		public static byte[] Long2PackedByte(long n)
		{
			int len = n.ToString().Length;
			int length = (int) Math.Ceiling(len/2.0);
			byte[] packed = new byte[length];

			for (int i = length-1; i >= 0; i--)
			{
				packed[i] = (byte)(n % 10);
				n /= 10;
				packed[i] |= (byte)((n%10) << 4);
				n /= 10;
			}
			return packed;
		}

		// RJ 0 filled
		public static byte[] Long2PackedByte(long n, int length)
		{
			byte[] temp = Long2PackedByte(n);
			byte[] packed = new byte[length];
			if (length < temp.Length)
				return null;
			Array.Copy(temp, 0, packed, length - temp.Length, temp.Length);
			for (int i = 0; i < (length - temp.Length); i++)
				packed[i] = 0x00;
			return packed;
		}

		public static string ByteArray2HexStringWithHeader(byte[] data)
		{
			if (data.Length == 0) 
				return string.Empty;
			string s = "" + Environment.NewLine + "Data Length = " + data.Length + " bytes" + Environment.NewLine;
			return s + ByteArray2HexString(data);
		}

		public static string ByteArray2HexStringNoFormat(byte[] data)
		{
			if (data.Length == 0) 
				return string.Empty;
			string s = null;
			for (int i=0; i < data.Length; i++)
			{
				if (data[i] < 16)
					s += "0";
				s +=  Convert.ToString(data[i], 16).ToUpper() + " ";
			}
			return s;
		}

		public static string ByteArray2HexString(byte[] data)
		{
			if (data.Length == 0) 
				return string.Empty;
			string s = null;
			for (int i=0; i < data.Length; i++)
			{
				if (i != 0 && (i % 16) == 0)
					s += Environment.NewLine;
				if (data[i] < 16)
					s += "0";
				s +=  Convert.ToString(data[i], 16).ToUpper() + " ";
			}
			return s;
		}

		/**
		* convert a signed byte to an integer without extending the sign bit.
		* java treats a byte whose msb is set as a 2's compliment signed
		* byte.  Thus, when the byte is cast to an integer, java sign extends
		* the msb.
		* This function treats the byte argument as an unsigned byte and therefore
		* does not sign extend the bit.
		*/
		/*
		public static int SignedByteToUnsignedInt(byte b)
		{
			return (((b & (byte)0x80) == 0) ? b : (256 +  ((byte)b)));
		}
		*/

		public enum Justification : int
		{
			Left,
			Right
		}

		public static string FillString(object o, int length, Justification justification, char fillByte)
		{
			string value = o.ToString();
			if (value.Length > length)
				value = value.Substring(0,length);
			if (justification == Justification.Left)
				value = value.PadRight(length,fillByte);
			else
				value = value.PadLeft(length,fillByte);

			return value;
		}
  
	}
}
