#include "AEScrypt.h"

void AEScrypt::subBytes(unsigned char original[16])
{
	for (int i = 0; i < 16; i++)
	{
		original[i] = sBox[(int)original[i]];
	}
}
void AEScrypt::subBytesInv(unsigned char original[16])
{
	for (int i = 0; i < 16; i++)
	{
		original[i] = sBoxInv[(int)original[i]];
	}
}

void AEScrypt::mixColumnCore(unsigned char c[4], unsigned char ret[4])
{
	unsigned char c2[4];
	unsigned char c3[4];

	// loop to get 2 * c and 3* c values in a arrray
	for (int i = 0; i < 4; i++)
	{
		if (c[i] >= 0x80) // if the left most bit is a 1, bitwise shift then xor with 0x1B
		{
			c2[i] = c[i] << 1;
			c2[i] = c2[i] ^ 0x1b;
			c3[i] = c2[i] ^ c[i];    // 3 * c = 2 * c + c
		}
		else
		{
			c2[i] = c[i] << 1;  // left most bit is 0, bitwise shift is enough
			c3[i] = c2[i] ^ c[i];
		}
	}

	/*
* matrix rows:      2  3  1  1
*					1  2  3  1
*					1  1  2  3
*					3  1  1  2
*/

	ret[0] = c2[0] ^ c3[1] ^ c[2] ^ c[3];
	ret[1] = c[0] ^ c2[1] ^ c3[2] ^ c[3];
	ret[2] = c[0] ^ c[1] ^ c2[2] ^ c3[3];
	ret[3] = c3[0] ^ c[1] ^ c[2] ^ c2[3];


}
void AEScrypt::GfDouble(unsigned char *original)
{
	if (*original >= 0x80) // if the left most bit is a 1, bitwise shift then xor with 0x1B
	{
		*original = *original << 1;
		*original = *original ^ 0x1b;   
	}
	else
	{
		*original = *original << 1;  // left most bit is 0, bitwise shift is enough
	}
}
void AEScrypt::mixColumnCoreInv(unsigned char c[4], unsigned char ret[4])
{
	unsigned char nine[4];
	unsigned char eleven[4];
	unsigned char thirteen[4];
	unsigned char fourteen[4];

	for (int i = 0; i < 4; i++)
	{
		nine[i] = c[i];
		GfDouble(&nine[i]); GfDouble(&nine[i]); GfDouble(&nine[i]); // 2, 4, 8
		nine[i] = nine[i] ^ c[i]; // 8 + 1
		
		eleven[i] = c[i];
		GfDouble(&eleven[i]); GfDouble(&eleven[i]); // 2, 4
		eleven[i] = eleven[i] ^ c[i]; // 4 + 1
		GfDouble(&eleven[i]); // 5 x 2
		eleven[i] = eleven[i] ^ c[i];

		thirteen[i] = c[i];
		GfDouble(&thirteen[i]); // x 2
		thirteen[i] = thirteen[i] ^ c[i]; // 2 + 1
		GfDouble(&thirteen[i]); GfDouble(&thirteen[i]); // 3 x 2 x 2
		thirteen[i] = thirteen[i] ^ c[i]; // 12 + 1

		fourteen[i] = c[i];
		GfDouble(&fourteen[i]); // x2
		fourteen[i] = fourteen[i] ^ c[i]; // 2 + 1
		GfDouble(&fourteen[i]); // 3 x 2
		fourteen[i] = fourteen[i] ^ c[i]; // 6 + 1
		GfDouble(&fourteen[i]); // 7 x 2
		
	}
	/*
	*    14  11  13  9
	*    9   14  11  13
	*    13  9   14  11
	*    11  13  9   14
	*/
	
	ret[0] = fourteen[0] ^ eleven[1] ^ thirteen[2] ^ nine[3];
	ret[1] = nine[0] ^ fourteen[1] ^ eleven[2] ^ thirteen[3];
	ret[2] = thirteen[0] ^ nine[1] ^ fourteen[2] ^ eleven[3];
	ret[3] = eleven[0] ^ thirteen[1] ^ nine[2] ^ fourteen[3];
	
}


void AEScrypt::mixColumn(unsigned char original[16])
{
	unsigned char col[4];
	unsigned char ret[4];
	
	//column 1
	int z = 0;
	for (int i = 0; i < 13;)
	{
		col[z] = original[i];
		i =	i + 4;
		z = z + 1;
	}
	mixColumnCore(col, ret);
	z = 0;
	for (int i = 0; i < 13;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 2
	z = 0;
	for (int i = 1; i < 14;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCore(col, ret);
	z = 0;
	for (int i = 1; i < 14;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 3
	z = 0;
	for (int i = 2; i < 15;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCore(col, ret);
	z = 0;
	for (int i = 2; i < 15;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 4
	z = 0;
	for (int i = 3; i < 16;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCore(col, ret);
	z = 0;
	for (int i = 3; i < 16;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}




}
void AEScrypt::mixColumnInv(unsigned char original[16])
{
	unsigned char col[4];
	unsigned char ret[4];

	//column 1
	int z = 0;
	for (int i = 0; i < 13;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCoreInv(col, ret);
	z = 0;
	for (int i = 0; i < 13;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 2
	z = 0;
	for (int i = 1; i < 14;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCoreInv(col, ret);
	z = 0;
	for (int i = 1; i < 14;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 3
	z = 0;
	for (int i = 2; i < 15;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCoreInv(col, ret);
	z = 0;
	for (int i = 2; i < 15;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}

	// column 4
	z = 0;
	for (int i = 3; i < 16;)
	{
		col[z] = original[i];
		i = i + 4;
		z = z + 1;
	}
	mixColumnCoreInv(col, ret);
	z = 0;
	for (int i = 3; i < 16;)
	{
		original[i] = ret[z];
		i = i + 4;
		z = z + 1;
	}




}

void AEScrypt::shiftRows(unsigned char original[16])
{
	unsigned char buffer[16];
	buffer[0] = original[0];
	buffer[1] = original[1];
	buffer[2] = original[2];
	buffer[3] = original[3];

	buffer[4] = original[7];
	buffer[5] = original[4];
	buffer[6] = original[5];
	buffer[7] = original[6];

	buffer[8] = original[10];
	buffer[9] = original[11];
	buffer[10] = original[8];
	buffer[11] = original[9];

	buffer[12] = original[13];
	buffer[13] = original[14];
	buffer[14] = original[15];
	buffer[15] = original[12];

	for (int i = 0; i < 16; i++)
	{
		original[i] = buffer[i];
	}
}
void AEScrypt::shiftRowsInv(unsigned char original[16])
{
	unsigned char buffer[16];
	buffer[0] = original[0];
	buffer[1] = original[1];
	buffer[2] = original[2];
	buffer[3] = original[3];

	buffer[4] = original[5];
	buffer[5] = original[6];
	buffer[6] = original[7];
	buffer[7] = original[4];

	buffer[8] = original[10];
	buffer[9] = original[11];
	buffer[10] = original[8];
	buffer[11] = original[9];

	buffer[12] = original[15];
	buffer[13] = original[12];
	buffer[14] = original[13];
	buffer[15] = original[14];

	for (int i = 0; i < 16; i++)
	{
		original[i] = buffer[i];
	}
}

void AEScrypt::keySchedule(unsigned char key[16], unsigned char roundKeys[11][16])
{
	for (int i = 0; i < 16; i++)
	{
		roundKeys[0][i] = key[i];
	}

	for (int z = 1; z < 11; z++)
	{
		// rotate
		roundKeys[z][1] = roundKeys[z-1][16];
		roundKeys[z][5] = roundKeys[z - 1][4];
		roundKeys[z][9] = roundKeys[z - 1][8];
		roundKeys[z][13] = roundKeys[z - 1][12];

		// sub bytes
		for (int i = 1; i < 14;)
		{
			roundKeys[z][i] = sBox[(int)roundKeys[z][i]];
			i = i + 4;
		}

		// xor with i-4 and rcon1
		int y = 0;
		for (int i = 1; i < 14;)
		{
			roundKeys[z][i] = roundKeys[z][i] ^ roundKeys[z-1][i] ^ rcon[0][y];
			y++;
			i = i + 4;
		}

		// xor next 3 columns, W i - 1 ^ W i - 4
		for (int i = 2; i < 16; i++)
		{
			if (i == 1 || i == 5 || i == 9 || i == 13)
				continue;
			roundKeys[z][i] = roundKeys[z][i-1] ^ roundKeys[z-1][i];
		}

	}
}
void AEScrypt::addRoundKey(unsigned char original[16], unsigned char key[16])
{
	for (int i = 0; i < 16; i++)
	{
		original[i] = original[i] ^ key[i];
	}
}

void AEScrypt::encryptCore(unsigned char original[16], unsigned char key[16])
{
	// create key schedule
	unsigned char roundKeys[11][16];
	keySchedule(key, roundKeys);

	// first round, add round, xor with 1st key
	addRoundKey(original, roundKeys[0]);

	// beginning 9 loops now. subBytes, shiftRows, mixColumns, addRoundKey
	for (int i = 1; i < 10; i++)
	{
		subBytes(original);
		shiftRows(original);
		mixColumn(original);
		addRoundKey(original, roundKeys[i]);
	}

	// final round: subBytes, shiftRow, addRoundkey
	subBytes(original);
	shiftRows(original);
	addRoundKey(original, roundKeys[10]);

}

void AEScrypt::decryptCore(unsigned char ciphered[16], unsigned char key[16])
{
	// make key schedule
	unsigned char roundKeys[11][16];
	keySchedule(key, roundKeys);

	// final round reverse, add final round key - > shiftRows inv -> subBytes inv
	addRoundKey(ciphered, roundKeys[10]);
	shiftRowsInv(ciphered);
	subBytesInv(ciphered);

	// 9 loops in reverse addRoundKey -> mixColumnInv -> shiftRowsInv -> subBytesInv
	for (int i = 9; i > 0; i--)
	{
		addRoundKey(ciphered, roundKeys[i]);
		mixColumnInv(ciphered);
		shiftRowsInv(ciphered);
		subBytesInv(ciphered);
	}

	// initial round in reverse addRoundKey
	addRoundKey(ciphered, roundKeys[0]);

}


void AEScrypt::encrypt(unsigned char* original, int length, unsigned char* ciphered)
{

	// encryption for 128bit block sizes using Cipher block chaining
	// using rand_s function as PRNG to generate the Initiliaztion vector


	// generate a random Initiliaztion vector
	unsigned char IV[16];
	unsigned int iRand;
	for (int i = 0; i < 16; i++)
	{
		rand_s(&iRand);
		IV[i] = (unsigned char)iRand;
	}

	// check if padding required
	int noBlocks = length / 16;
	int bytesPad = 0;
	if (length % 16 != 0)
	{
		noBlocks++;
		bytesPad = 16 - (length % 16);
	}

	// dynamically allocate buffer memory
	unsigned char* buffer = new unsigned char[noBlocks * 16];

	// copy over to buffer
	for (int i = 0; i < noBlocks * 16; i++)
	{
			buffer[i] = original[i];
	}

	// pad if required
	if (bytesPad > 0)
	{
		for (int i = noBlocks * 16 - bytesPad; i < noBlocks*  16; i++)
			buffer[i] = bytesPad;

	}

	// intialize first block with IV
	for (int i = 0; i < 16; i++)
		buffer[i] = buffer[i] ^ IV[i];

	// encrypt blocks - CBC
	encryptCore(buffer, key);
	for (int i = 16; i < noBlocks * 16; i++)
	{
		for (int z = 0; z < 16; z++)
			buffer[i + z] = buffer[i + z] ^ buffer[i - 16 + z];
		encryptCore((buffer+ i), key);
		i = i + 16;
	}
	
	for(int i = 0; i < noBlocks * 16; i++)
		ciphered[i] = buffer[i];
}


/*
void AEScrypt::encrypt(char* oString, char* eString)
{
	int length = 0;
	// find length of string
	bool found = false;
	while (found == false)
	{
		if (oString[length] == '\0')
		{
			found = true;
		}
		length++;
	}

	// get number of 16 byte blocks required
	int noBlocks = 0;
	if (length % 16 == 0)
		noBlocks = length / 16;
	else
	{
		noBlocks = (length / 16) + 1;
	}

	unsigned char** buffer = new unsigned char* [noBlocks];
	for (int i = 0; i < noBlocks; i++)
		buffer[i] = new unsigned char[16];

	// move oString into buffer
	for (int i = 0; i < noBlocks; i++)
	{
		for (int z = 0; z < 16; z++)
		{
			buffer[i][z] = oString[z + i * 16];
		}
	}

	// encrypt each block and append to oString buffer
	for (int i = 0; i < noBlocks; i++)
	{
		encryptCore(buffer[i], key);
	}


	// copy from buffer to oString
	for (int i = 0; i < noBlocks; i++)
	{
		for (int z = 0; z < 16; z++)
		{
			buffer[i][z] = oString[z + i * 16];
		}
	}



	/*
	//
	bool nullFound = false;
	int cBlocks = 0;
	while (nullFound == false)
	{
		for (int z = 0; z < 16; z++)
		{
			oString[cBlocks * 16 + z] = buffer[cBlocks][z];
			if (buffer[cBlocks][z] == '\0')
			{
				nullFound = true;
				break;
			}
		}
		cBlocks++;
	}

	*/

	
