/*****************************************************************************/
/* SCommon.cpp                            Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Common functions for StormLib, used by all SFile*** modules               */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 24.03.03  1.00  Lad  The first version of SFileCommon.cpp                 */
/* 19.11.03  1.01  Dan  Big endian handling                                  */
/* 12.06.04  1.01  Lad  Renamed to SCommon.cpp                               */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "SCommon.h"

#include "stormlib/misc/crc32.h"
#include "stormlib/misc/md5.h"

char StormLibCopyright[] = "StormLib v 4.50 Copyright Ladislav Zezula 1998-2003";

//-----------------------------------------------------------------------------
// The buffer for decryption engine.

TMPQArchive * pFirstOpen = NULL;        // The first member of MPQ archives chain
LCID    lcLocale  = LANG_NEUTRAL;       // File locale
USHORT  wPlatform = 0;                  // File platform

//-----------------------------------------------------------------------------
// Compression types

// 
// Data compressions
//
// Can be combination of MPQ_COMPRESSION_PKWARE, MPQ_COMPRESSION_BZIP2
// and MPQ_COMPRESSION_ZLIB. Some newer compressions are not supported
// by older games. The table of supported compressions is here:
//
//   MPQ_COMPRESSION_PKWARE - All games since Diablo I
//   MPQ_COMPRESSION_ZLIB   - Games since Starcraft
//   MPQ_COMPRESSION_BZIP2  - Games since World of Warcraft
//

static int nDataCmp = MPQ_COMPRESSION_ZLIB;

//
// WAVE compressions by quality level
//

static int uWaveCmpLevel[] = {-1, 4, 2};
static int uWaveCmpType[] = {MPQ_COMPRESSION_PKWARE, 0x81, 0x81};

//-----------------------------------------------------------------------------
// Storm buffer functions

// Buffer for the decryption engine
#define STORM_BUFFER_SIZE   0x500
static DWORD StormBuffer[STORM_BUFFER_SIZE];
static BOOL  bStormBufferCreated = FALSE;

int PrepareStormBuffer()
{
    DWORD dwSeed = 0x00100001;
    DWORD index1 = 0;
    DWORD index2 = 0;
    int   i;

    // Initialize the decryption buffer.
    // Do nothing if already done.
    if(bStormBufferCreated == FALSE)
    {
        for(index1 = 0; index1 < 0x100; index1++)
        {
            for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
            {
                DWORD temp1, temp2;

                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp1  = (dwSeed & 0xFFFF) << 0x10;

                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp2  = (dwSeed & 0xFFFF);

                StormBuffer[index2] = (temp1 | temp2);
            }
        }
        bStormBufferCreated = TRUE;
    }
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// Encrypting and decrypting hash table

void EncryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// Encrypting and decrypting block table

void EncryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *pdwTable ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// Functions tries to get file decryption key. The trick comes from block
// positions which are stored at the begin of each compressed file. We know the
// file size, that means we know number of blocks that means we know the first
// DWORD value in block position. And if we know encrypted and decrypted value,
// we can find the decryption key !!!
//
// hf    - MPQ file handle
// block - DWORD array of block positions
// ch    - Decrypted value of the first block pos

DWORD DetectFileSeed(DWORD * block, DWORD decrypted)
{
    DWORD saveSeed1;
    DWORD temp = *block ^ decrypted;    // temp = seed1 + seed2
    temp -= 0xEEEEEEEE;                 // temp = seed1 + StormBuffer[0x400 + (seed1 & 0xFF)]

    for(int i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD (We exactly know the value)
        seed1  = temp - StormBuffer[0x400 + i];
        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[0] ^ (seed1 + seed2);

        if(ch != decrypted)
            continue;

        // Add 1 because we are decrypting block positions
        saveSeed1 = seed1 + 1;

        // If OK, continue and test the second value. We don't know exactly the value,
        // but we know that the second one has lower 16 bits set to zero
        // (no compressed block is larger than 0xFFFF bytes)
        seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
        seed2  = ch + seed2 + (seed2 << 5) + 3;

        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[1] ^ (seed1 + seed2);

        if((ch & 0xFFFF0000) == 0)
            return saveSeed1;
    }
    return 0;
}

// Function tries to detect file seed. It expectes at least two uncompressed bytes
DWORD DetectFileSeed2(DWORD * pdwBlock, UINT nDwords, ...)
{
    va_list argList;
    DWORD dwDecrypted[0x10];
    DWORD saveSeed1;
    DWORD dwTemp;
    DWORD i, j;
    
    // We need at least two DWORDS to detect the seed
    if(nDwords < 0x02 || nDwords > 0x10)
        return 0;
    
    va_start(argList, nDwords);
    for(i = 0; i < nDwords; i++)
        dwDecrypted[i] = va_arg(argList, DWORD);
    va_end(argList);
    
    dwTemp = (*pdwBlock ^ dwDecrypted[0]) - 0xEEEEEEEE;
    for(i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD
        seed1  = dwTemp - StormBuffer[0x400 + i];
        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = pdwBlock[0] ^ (seed1 + seed2);

        if(ch != dwDecrypted[0])
            continue;

        saveSeed1 = seed1;

        // If OK, continue and test all bytes.
        for(j = 1; j < nDwords; j++)
        {
            seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
            seed2  = ch + seed2 + (seed2 << 5) + 3;

            seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
            ch     = pdwBlock[j] ^ (seed1 + seed2);

            if(ch == dwDecrypted[j] && j == nDwords - 1)
                return saveSeed1;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
// Encrypting and decrypting MPQ blocks

void EncryptMPQBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block;
        *block++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptMPQBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *block++ = ch;
    }
}


DWORD DecryptHashIndex(TMPQArchive * ha, const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x000 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return (dwSeed1 & (ha->pHeader->dwHashTableSize - 1));
}

DWORD DecryptName1(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x100 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

DWORD DecryptName2(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    int    ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x200 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

DWORD DecryptFileSeed(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;          // EBX
    DWORD  dwSeed2 = 0xEEEEEEEE;          // ESI
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);           // ECX

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

TMPQHash * GetHashEntry(TMPQArchive * ha, const char * szFileName)
{
    TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash0;                      // File hash entry (start)
    TMPQHash * pHash;                       // File hash entry (current)
    DWORD dwIndex = (DWORD)(DWORD_PTR)szFileName;
    DWORD dwName1;
    DWORD dwName2;

    // If filename is given by index, we have to search all hash entries for the right index.
    if(dwIndex <= ha->pHeader->dwBlockTableSize)
    {
        // Pass all the hash entries and find the 
        for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
        {
            if(pHash->dwBlockIndex == dwIndex)
                return pHash;
        }
        return NULL;
    }

    // Decrypt name and block index
    dwIndex = DecryptHashIndex(ha, szFileName);
    dwName1 = DecryptName1(szFileName);
    dwName2 = DecryptName2(szFileName);
    pHash   = pHash0 = ha->pHashTable + dwIndex;
    
    // Look for hash index
    while(pHash->dwBlockIndex != HASH_ENTRY_FREE)
    {
        if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2 && pHash->dwBlockIndex != HASH_ENTRY_DELETED)
            return pHash;

        // Move to the next hash entry
        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            break;
    }

    // File was not found
    return NULL;
}

// Retrieves the locale-specific hash entry
TMPQHash * GetHashEntryEx(TMPQArchive * ha, const char * szFileName, LCID lcLocale)
{
    TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash0 = NULL;           // Language-neutral hash entry
    TMPQHash * pHashX = NULL;           // Language-speficic
    TMPQHash * pHash = GetHashEntry(ha, szFileName);

    if(pHash != NULL)
    {
        TMPQHash * pHashStart = pHash;
        DWORD dwName1 = pHash->dwName1;
        DWORD dwName2 = pHash->dwName2;

        while(pHash->dwBlockIndex != HASH_ENTRY_FREE)
        {
            if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2)
            {
                if(pHash->lcLocale == LANG_NEUTRAL)
                    pHash0 = pHash;
                if(pHash->lcLocale == lcLocale)
                    pHashX = pHash;

                // If both found, break the loop
                if(pHash0 != NULL && pHashX != NULL)
                    break;
            }

            if(++pHash >= pHashEnd)
                pHash = ha->pHashTable;
            if(pHash == pHashStart)
                return NULL;
        }

        if(lcLocale != LANG_NEUTRAL && pHashX != NULL)
            return pHashX;
        if(pHash0 != NULL)
            return pHash0;
        return NULL;
    }

    return pHash;
}

// Encrypts file name and gets the hash entry
// Returns the hash pointer, which is always within the allocated array
TMPQHash * FindFreeHashEntry(TMPQArchive * ha, const char * szFileName)
{
    TMPQHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPQHash * pHash0;                    // File hash entry (search start)
    TMPQHash * pHash;                     // File hash entry
    DWORD dwIndex = DecryptHashIndex(ha, szFileName);
    DWORD dwName1 = DecryptName1(szFileName);
    DWORD dwName2 = DecryptName2(szFileName);
    DWORD dwBlockIndex = 0xFFFFFFFF;

    // Save the starting hash position
    pHash = pHash0 = ha->pHashTable + dwIndex;

    // Look for the first free hash entry. Can be also a deleted entry
    while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
    {
        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            return NULL;
    }

    // Fill the hash entry with the informations about the file name
    pHash->dwName1   = dwName1;
    pHash->dwName2   = dwName2;
    pHash->lcLocale  = (USHORT)lcLocale;
    pHash->wPlatform = wPlatform;

    // Now we have to find a free block entry
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
        TMPQBlock * pBlock = ha->pBlockTable + dwIndex;

        if((pBlock->dwFlags & MPQ_FILE_EXISTS) == 0)
        {
            dwBlockIndex = dwIndex;
            break;
        }
    }

    // If no free block entry found, we have to use the index
    // at the end of the current block table
    if(dwBlockIndex == 0xFFFFFFFF)
        dwBlockIndex = ha->pHeader->dwBlockTableSize;
    pHash->dwBlockIndex = dwBlockIndex;
    return pHash;
}

//-----------------------------------------------------------------------------
// Checking for valid archive handle and valid file handle

BOOL IsValidMpqHandle(TMPQArchive * ha)
{
    if(ha == NULL || IsBadReadPtr(ha, sizeof(TMPQArchive)))
        return FALSE;
    if(ha->pHeader == NULL || IsBadReadPtr(ha->pHeader, sizeof(TMPQHeader)))
        return FALSE;
    
    return (ha->pHeader->dwID == ID_MPQ);
}

BOOL IsValidFileHandle(TMPQFile * hf)
{
    if(hf == NULL || IsBadReadPtr(hf, sizeof(TMPQFile)))
        return FALSE;

    if(hf->hFile != INVALID_HANDLE_VALUE)
        return TRUE;

    return IsValidMpqHandle(hf->ha);
}

int AddInternalFile(TMPQArchive * ha, const char * szFileName)
{
    TMPQBlockEx * pBlockEx;
    TMPQBlock * pBlockEnd;
    TMPQBlock * pBlock;
    TMPQHash * pHash;
    BOOL bFoundFreeEntry = FALSE;
    int nError = ERROR_SUCCESS;

    // Check if the file already exists in the archive
    pHash = GetHashEntryEx(ha, szFileName, LANG_NEUTRAL);
    if(pHash == NULL)
    {
        pHash = FindFreeHashEntry(ha, szFileName);
        if(pHash != NULL)
        {
            pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
            pBlockEx = ha->pExtBlockTable;
            for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++, pBlockEx++)
            {
                if((pBlock->dwFlags & MPQ_FILE_EXISTS) == 0)
                {
                    bFoundFreeEntry = TRUE;
                    break;
                }
            }

            // If the block is out of the available entries, return error
            if(pBlock >= (ha->pBlockTable + ha->pHeader->dwHashTableSize))
                return ERROR_DISK_FULL;    

            // If we had to add the file at the end, increment the block table
            if(bFoundFreeEntry == FALSE)
                ha->pHeader->dwBlockTableSize++;

            // Fill the block entry
            pBlockEx->wFilePosHigh = (USHORT)ha->HashTablePos.HighPart;
            pBlock->dwFilePos = ha->HashTablePos.LowPart;
            pBlock->dwFSize   = 0;
            pBlock->dwCSize   = 0;
            pBlock->dwFlags   = MPQ_FILE_EXISTS;

            // Add the node for the file name
            return SListFileAddNode(ha, szFileName);
        }
        else
        {
            nError = ERROR_HANDLE_DISK_FULL;
        }
    }

    return nError;
}

// This function writes a local file into the MPQ archive.
// Returns 0 if OK, otherwise error code.
int AddFileToArchive(
    TMPQArchive * ha,
    HANDLE hFile,
    const char * szArchivedName,
    DWORD dwFlags,
    DWORD dwQuality,
    int nFileType,
    BOOL * pbReplaced)
{
    LARGE_INTEGER TempPos;              // For various file offset calculations
    TMPQBlock * pBlockEnd;              // Pointer to end of the block table
    TMPQFile * hf = NULL;               // File structure for newly added file
    BYTE * pbCompressed = NULL;         // Compressed (target) data
    BYTE * pbToWrite = NULL;            // Data to write to the file
    DWORD dwBlockPosLen = 0;            // Length of the file block offset (in bytes)
    DWORD dwTransferred = 0;            // Number of bytes read or written
    DWORD dwFileSizeHigh = 0;           // High 32 bits of the file size
    DWORD dwFileSize = 0;               // Low 32-bits of the file size
    BOOL bReplaced = FALSE;             // TRUE if replaced, FALSE if added
    int nCmpFirst = nDataCmp;           // Compression for the first data block
    int nCmpNext  = nDataCmp;           // Compression for the next data blocks
    int nCmp      = nDataCmp;           // Current compression
    int nCmpLevel = -1;                 // Compression level
    int nError = ERROR_SUCCESS;

    // Set the correct compression types
    if(dwFlags & MPQ_FILE_IMPLODE)
        nCmpFirst = nCmpNext = MPQ_COMPRESSION_PKWARE;
    pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;

    if(dwFlags & MPQ_FILE_COMPRESS)
    {
        if(nFileType == SFILE_TYPE_DATA)
            nCmpFirst = nCmpNext = nDataCmp;
    
        if(nFileType == SFILE_TYPE_WAVE)
        {
            nCmpNext  = uWaveCmpType[dwQuality];
            nCmpLevel = uWaveCmpLevel[dwQuality];
        }
    }

    // Get the size of the file to be added
    if(nError == ERROR_SUCCESS)
    {
        dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

        // Adjust file flags for too-small files
        if(dwFileSize < 0x04)
            dwFlags &= ~(MPQ_FILE_ENCRYPTED | MPQ_FILE_FIXSEED);
        if(dwFileSize < 0x20)
            dwFlags &= ~MPQ_FILE_COMPRESSED;

        // File in MPQ cannot be greater than 4GB
        if(dwFileSizeHigh != 0)
            nError = ERROR_PARAMETER_QUOTA_EXCEEDED;
    }

    // Allocate the TMPQFile entry for newly added file
    if(nError == ERROR_SUCCESS)
    {
        hf = (TMPQFile *)ALLOCMEM(BYTE, sizeof(TMPQFile) + strlen(szArchivedName));
        if(hf == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Reset the TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
        memset(hf, 0, sizeof(TMPQFile));
        strcpy(hf->szFileName, szArchivedName);
        hf->ha = ha;
		hf->hFile = INVALID_HANDLE_VALUE;

        // Check if the file already exists in the archive
        if((hf->pHash = GetHashEntryEx(ha, szArchivedName, lcLocale)) != NULL)
        {
            if(hf->pHash->lcLocale == lcLocale)
            {
                if((dwFlags & MPQ_FILE_REPLACEEXISTING) == 0)
                {
                    nError = ERROR_ALREADY_EXISTS;
                    hf->pHash = NULL;
                }
                else
                {
                    hf->pBlockEx = ha->pExtBlockTable + hf->pHash->dwBlockIndex;
                    hf->pBlock = ha->pBlockTable + hf->pHash->dwBlockIndex;
                    bReplaced = TRUE;
                }
            }
            else
                hf->pHash = NULL;
        }

        if(nError == ERROR_SUCCESS && hf->pHash == NULL)
        {
            hf->pHash = FindFreeHashEntry(ha, szArchivedName);
            if(hf->pHash == NULL)
                nError = ERROR_HANDLE_DISK_FULL;
        }

        // Set the hash index
        hf->dwHashIndex = (DWORD)(hf->pHash - ha->pHashTable);
    }

    // Find a block table entry for the file
    if(nError == ERROR_SUCCESS)
    {
        TMPQBlockEx * pBlockEx = NULL;      // Entry in the extended block table
        TMPQBlock * pBlock = NULL;          // Entry in the block table

        // Get the position of the first file
        hf->MpqFilePos.QuadPart = ha->pHeader->dwHeaderSize;

        // Search the entire block table and find a free block.
        // Also find MPQ offset at which the file data will be stored
        pBlockEx = ha->pExtBlockTable;
        for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++, pBlockEx++)
        {
            if(pBlock->dwFlags & MPQ_FILE_EXISTS)
            {
                TempPos.HighPart  = pBlockEx->wFilePosHigh;
                TempPos.LowPart   = pBlock->dwFilePos;
                TempPos.QuadPart += pBlock->dwCSize;

                if(TempPos.QuadPart > hf->MpqFilePos.QuadPart)
                    hf->MpqFilePos = TempPos;
            }
            else
            {
                if(hf->pBlock == NULL)
                {
                    hf->pBlockEx = pBlockEx;
                    hf->pBlock = pBlock;
                }
            }
        }

        // Calculate the raw file offset
        hf->RawFilePos.QuadPart = hf->MpqFilePos.QuadPart + ha->MpqPos.QuadPart;

        // If no free block in the middle of the block table,
        // use the one after last used block
        if(hf->pBlock == NULL)
        {
            hf->pBlockEx = pBlockEx;
            hf->pBlock = pBlock;
        }

        // When format V1, we cannot exceed 4 GB
        if(ha->pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1)
        {
            TempPos.QuadPart  = hf->RawFilePos.QuadPart + dwFileSize;
            TempPos.QuadPart += ha->pHeader->dwHashTableSize * sizeof(TMPQHash);
            TempPos.QuadPart += ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);

            if(TempPos.HighPart != 0)
                nError = ERROR_DISK_FULL;
        }

        // If the block offset exceeds number of hash entries,
        // we cannot add new file to the MPQ
        hf->dwBlockIndex = (DWORD)(hf->pBlock - ha->pBlockTable);
        if(hf->dwBlockIndex >= ha->pHeader->dwHashTableSize)
            nError = ERROR_HANDLE_DISK_FULL;
    }

    // Create seed1 for file encryption
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_ENCRYPTED))
    {
        const char * szTemp = strrchr(szArchivedName, '\\');

        // Create seed1 for file encryption
        if(szTemp != NULL)
            szArchivedName = szTemp + 1;

        hf->dwSeed1 = DecryptFileSeed(szArchivedName);
        if(dwFlags & MPQ_FILE_FIXSEED)
            hf->dwSeed1 = (hf->dwSeed1 + hf->MpqFilePos.LowPart) ^ dwFileSize;
    }

    // Resolve CRC32 and MD5 entry for the file
    // Only do it when the MPQ archive has attributes
    if(nError == ERROR_SUCCESS && ha->pAttributes != NULL)
    {
        if(ha->pAttributes->pCrc32 != NULL)
            hf->pCrc32 = ha->pAttributes->pCrc32 + hf->dwBlockIndex;
        if(ha->pAttributes->pFileTime != NULL)
            hf->pFileTime = ha->pAttributes->pFileTime + hf->dwBlockIndex;
        if(ha->pAttributes->pMd5 != NULL)
            hf->pMd5 = ha->pAttributes->pMd5 + hf->dwBlockIndex;
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS)
    {
        hf->nBlocks = (dwFileSize / ha->dwBlockSize) + 1;
        if(dwFileSize % ha->dwBlockSize)
            hf->nBlocks++;

        if((hf->pbFileBuffer = ALLOCMEM(BYTE, ha->dwBlockSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = hf->pbFileBuffer;
    }

    // For compressed files, allocate buffer for block positions and for the compressed data
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_COMPRESSED))
    {
        hf->pdwBlockPos = ALLOCMEM(DWORD, hf->nBlocks + 1);
        pbCompressed = ALLOCMEM(BYTE, ha->dwBlockSize * 2);
        if(hf->pdwBlockPos == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbCompressed;
    }

    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
        // Set the file pointer to file data position
        if(hf->RawFilePos.QuadPart != ha->FilePointer.QuadPart)
        {
            DWORD pointer3 = SetFilePointer(ha->hFile, hf->RawFilePos.LowPart, &hf->RawFilePos.HighPart, FILE_BEGIN);
            ha->FilePointer = hf->RawFilePos;
        }

        // Initialize the hash entry for the file
        hf->pHash->dwBlockIndex = hf->dwBlockIndex;

        // Initialize the block table entry for the file
        hf->pBlockEx->wFilePosHigh = (USHORT)hf->MpqFilePos.HighPart;
        hf->pBlock->dwFilePos = hf->MpqFilePos.LowPart;
        hf->pBlock->dwFSize   = dwFileSize;
        hf->pBlock->dwCSize   = 0;
        hf->pBlock->dwFlags   = dwFlags | MPQ_FILE_EXISTS;
    }

    // Write block positions (if the file will be compressed)
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_COMPRESSED))
    {
        dwBlockPosLen = hf->nBlocks * sizeof(DWORD);
        if(dwFlags & MPQ_FILE_HAS_EXTRA)
            dwBlockPosLen += sizeof(DWORD);

        memset(hf->pdwBlockPos, 0, dwBlockPosLen);
        hf->pdwBlockPos[0] = dwBlockPosLen;
        
        // Write the block positions. Only swap the first item, rest is zeros.
        BSWAP_INT32_UNSIGNED(hf->pdwBlockPos);
        WriteFile(ha->hFile, hf->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        BSWAP_INT32_UNSIGNED(hf->pdwBlockPos);

        // From this position, the archive is considered changed,
        // so the hash table and block table will be written when the archive is closed.
        ha->FilePointer.QuadPart += dwTransferred;
        hf->pBlock->dwCSize = dwTransferred;
        ha->dwFlags |= MPQ_FLAG_CHANGED;

        if(dwTransferred == dwBlockPosLen)
		{
            hf->pBlock->dwCSize += dwBlockPosLen;
		}
        else
            nError = GetLastError();
    }

    // Write all file blocks
    if(nError == ERROR_SUCCESS)
    {
        crc32_context crc32_ctx;
        md5_context md5_ctx;
        DWORD nBlock;       

        // Initialize CRC32 and MD5 processing
        CRC32_Init(&crc32_ctx);
        MD5_Init(&md5_ctx);
        nCmp = nCmpFirst;

        // Move the file pointer to the begin of the file
        DWORD pointer5 = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        for(nBlock = 0; nBlock < hf->nBlocks-1; nBlock++)
        {
            DWORD dwInLength  = ha->dwBlockSize;
            DWORD dwOutLength = ha->dwBlockSize;

            // Load the block from the file
            ReadFile(hFile, hf->pbFileBuffer, ha->dwBlockSize, &dwInLength, NULL);
            if(dwInLength == 0)
                break;

            // Update CRC32 and MD5 for the file
            if(hf->pCrc32 != NULL)
                CRC32_Update(&crc32_ctx, hf->pbFileBuffer, dwInLength);
            if(hf->pMd5 != NULL)
                MD5_Update(&md5_ctx, hf->pbFileBuffer, dwInLength);

            // Compress the block, if necessary
            dwOutLength = dwInLength;
            if(hf->pBlock->dwFlags & MPQ_FILE_COMPRESSED)
            {
                // Should be enough for compression
                int nOutLength = ha->dwBlockSize * 2;
                int nCmpType = 0;

                if(hf->pBlock->dwFlags & MPQ_FILE_IMPLODE)
                    Compress_pklib((char *)pbCompressed, &nOutLength, (char *)hf->pbFileBuffer, dwInLength, &nCmpType, 0);

                if(hf->pBlock->dwFlags & MPQ_FILE_COMPRESS)
                    SCompCompress((char *)pbCompressed, &nOutLength, (char *)hf->pbFileBuffer, dwInLength, nCmp, 0, nCmpLevel);

                // The compressed block size must NOT be the same or greater like
                // the original block size. If yes, do not compress the block
                // and store the data as-is.
                if(nOutLength >= (int)dwInLength)
                {
                    memcpy(pbCompressed, hf->pbFileBuffer, dwInLength);
                    nOutLength = dwInLength;
                }

                // Update block positions
                dwOutLength = nOutLength;
                hf->pdwBlockPos[nBlock+1] = hf->pdwBlockPos[nBlock] + dwOutLength;
                nCmp = nCmpNext;
            }

            // Encrypt the block, if necessary
            if(hf->pBlock->dwFlags & MPQ_FILE_ENCRYPTED)
            {
                BSWAP_ARRAY32_UNSIGNED((DWORD *)pbToWrite, dwOutLength / sizeof(DWORD));
                EncryptMPQBlock((DWORD *)pbToWrite, dwOutLength, hf->dwSeed1 + nBlock);
                BSWAP_ARRAY32_UNSIGNED((DWORD *)pbToWrite, dwOutLength / sizeof(DWORD));
            }
            
            // Write the block
            WriteFile(ha->hFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
            if(dwTransferred != dwOutLength)
            {
                nError = ERROR_DISK_FULL;
                break;
            }

            // Update the hash table position and the compressed file size
            ha->FilePointer.QuadPart += dwTransferred;
            hf->pBlock->dwCSize += dwTransferred;
            ha->dwFlags |= MPQ_FLAG_CHANGED;
        }

        // Finish calculating of CRC32 and MD5
        if(hf->pCrc32 != NULL)
            CRC32_Finish(&crc32_ctx, (unsigned long *)&hf->pCrc32->dwValue);
        if(hf->pMd5 != NULL)
            MD5_Finish(&md5_ctx, hf->pMd5->Value);
    }

    // Now save the block positions
    if(nError == ERROR_SUCCESS && (hf->pBlock->dwFlags & MPQ_FILE_COMPRESSED))
    {
        if(dwFlags & MPQ_FILE_HAS_EXTRA)
            hf->pdwBlockPos[hf->nBlocks] = hf->pdwBlockPos[hf->nBlocks-1];

        // If file is encrypted, block positions are also encrypted
        if(dwFlags & MPQ_FILE_ENCRYPTED)
            EncryptMPQBlock(hf->pdwBlockPos, dwBlockPosLen, hf->dwSeed1 - 1);
        
        // Set the position back to the block table
        DWORD pointer4 = SetFilePointer(ha->hFile, hf->RawFilePos.LowPart, &hf->RawFilePos.HighPart, FILE_BEGIN);
		
        // Write block positions to the archive
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pdwBlockPos, hf->nBlocks);
        WriteFile(ha->hFile, hf->pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        if(dwTransferred != dwBlockPosLen)
            nError = ERROR_DISK_FULL;

        // Update the file position in the archive
        ha->FilePointer.QuadPart = hf->RawFilePos.QuadPart + dwTransferred;
    }

    // If success, we have to change the settings
    // in MPQ header. If failed, we have to clean hash entry
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwTableSize;

        ha->pLastFile  = NULL;
        ha->dwBlockPos = 0;
        ha->dwBuffPos  = 0;

        // Add new entry to the block table (if needed)
        if(hf->dwBlockIndex >= ha->pHeader->dwBlockTableSize)
            ha->pHeader->dwBlockTableSize++;

        // Calculate positions of all tables
        ha->HashTablePos.QuadPart = hf->RawFilePos.QuadPart + hf->pBlock->dwCSize;
        TempPos.QuadPart = hf->MpqFilePos.QuadPart + hf->pBlock->dwCSize;

        // Set the position of hash table of the archive
        ha->pHeader->dwHashTablePos = TempPos.LowPart;
        ha->pHeader->wHashTablePosHigh = (USHORT)TempPos.HighPart;
        dwTableSize = ha->pHeader->dwHashTableSize * sizeof(TMPQHash);

        // Update block table pos
        TempPos.QuadPart += dwTableSize;
        ha->BlockTablePos.QuadPart = ha->HashTablePos.QuadPart + dwTableSize;
        ha->pHeader->wBlockTablePosHigh = (USHORT)TempPos.HighPart;
        ha->pHeader->dwBlockTablePos = TempPos.LowPart;
        dwTableSize = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);

        // If the archive size exceeded 4GB, we have to use extended block table
        TempPos.QuadPart += dwTableSize;
        if(TempPos.HighPart != 0 || ha->pHeader->ExtBlockTablePos.QuadPart != 0)
        {
            ha->ExtBlockTablePos.QuadPart = ha->BlockTablePos.QuadPart + dwTableSize;
            ha->pHeader->ExtBlockTablePos = TempPos;
            TempPos.QuadPart += ha->pHeader->dwBlockTableSize * sizeof(TMPQBlockEx);
        }

        // Update archive size (only valid for version V1)
        ha->MpqSize = TempPos;
        ha->pHeader->dwArchiveSize = TempPos.LowPart;
    }
    else
    {
        // Clear the hash table entry
        if(hf != NULL && hf->pHash != NULL)
            memset(hf->pHash, 0xFF, sizeof(TMPQHash));
    }

    // Cleanup
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    if(pbReplaced != NULL)
        *pbReplaced = bReplaced;
    FreeMPQFile(hf);
    return nError;
}

/*
int AddFileToArchive(
    TMPQArchive * ha,
    HANDLE hFile,
    const char * szArchivedName,
    DWORD dwFlags,
    DWORD dwQuality,
    int nFileType,
    BOOL * pbReplaced)
{
    LARGE_INTEGER RelativePos = {0};
    LARGE_INTEGER FilePos = {0};
    LARGE_INTEGER TempPos;
    TMPQBlockEx * pBlockEx = NULL;      // Entry in the extended block table
    TMPQCRC32 * pCrc32 = NULL;          // CRC32 entry belonging to newly added file
    TMPQBlock * pBlock = NULL;          // Entry in the block table
    TMPQHash * pHash = NULL;            // Entry in the hash table
    TMPQMD5 * pMd5 = NULL;              // MD5 entry belonging to newly added file
    DWORD * pdwBlockPos = NULL;         // Block position table (compressed files only)
    BYTE  * pbFileData = NULL;          // Uncompressed (source) data
    BYTE  * pbCompressed = NULL;        // Compressed (target) data
    BYTE  * pbToWrite = NULL;           // Data to write to the file
    DWORD   dwBlockPosLen = 0;          // Length of the block table positions
    DWORD   dwTransferred = 0;          // Number of bytes written into archive file
    DWORD   dwFileSize = 0;             // Size of the file to add
    DWORD   dwSeed1   = 0;              // Encryption seed
    DWORD   nBlocks   = 0;              // Number of file blocks
    DWORD   nBlock = 0;                 // Index of the currently written block
    BOOL    bReplaced = FALSE;          // TRUE if replaced, FALSE if added
    int     nCmpFirst = nDataCmp;       // Compression for the first data block
    int     nCmpNext  = nDataCmp;       // Compression for the next data blocks
    int     nCmp      = nDataCmp;       // Current compression
    int     nCmpLevel = -1;             // Compression level
    int     nError = ERROR_SUCCESS;

    // Set the correct compression types
    if(dwFlags & MPQ_FILE_COMPRESS_PKWARE)
        nCmpFirst = nCmpNext = MPQ_COMPRESSION_PKWARE;

    if(dwFlags & MPQ_FILE_COMPRESS_MULTI)
    {
        if(nFileType == SFILE_TYPE_DATA)
            nCmpFirst = nCmpNext = nDataCmp;
    
        if(nFileType == SFILE_TYPE_WAVE)
        {
            nCmpNext  = uWaveCmpType[dwQuality];
            nCmpLevel = uWaveCmpLevel[dwQuality];
        }
    }

    // Check if the file already exists in the archive
    if(nError == ERROR_SUCCESS)
    {
        if((pHash = GetHashEntryEx(ha, szArchivedName, lcLocale)) != NULL)
        {
            if(pHash->lcLocale == lcLocale)
            {
                if((dwFlags & MPQ_FILE_REPLACEEXISTING) == 0)
                {
                    nError = ERROR_ALREADY_EXISTS;
                    pHash = NULL;
                }
                else
                    bReplaced = TRUE;
            }
            else
                pHash = NULL;
        }

        if(nError == ERROR_SUCCESS && pHash == NULL)
        {
            pHash = FindFreeHashEntry(ha, szArchivedName);
            if(pHash == NULL)
                nError = ERROR_HANDLE_DISK_FULL;
        }
    }

    // Get the block table entry for the file
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwFileSizeHigh = 0;

        // Get the size of the added file
        dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
        if(dwFileSizeHigh != 0)
            nError = ERROR_PARAMETER_QUOTA_EXCEEDED;

        // Fix the flags, if the file is too small
        if(dwFileSize < 0x04)
            dwFlags &= ~(MPQ_FILE_ENCRYPTED | MPQ_FILE_FIXSEED);
        if(dwFileSize < 0x20)
            dwFlags &= ~MPQ_FILE_COMPRESSED;

        if(pHash->dwBlockIndex == HASH_ENTRY_FREE)
            pHash->dwBlockIndex = ha->pHeader->dwBlockTableSize;

        // The block table index cannot be larger than hash table size
        if(pHash->dwBlockIndex >= ha->pHeader->dwHashTableSize)
            nError = ERROR_HANDLE_DISK_FULL;
    }

    // Resolve CRC32 and MD5 entry for the file
    // Only do it when the MPQ archive has attributes
    if(nError == ERROR_SUCCESS && ha->pAttributes != NULL)
    {
        if(ha->pAttributes->pCrc32 != NULL)
            pCrc32 = ha->pAttributes->pCrc32 + pHash->dwBlockIndex;
        if(ha->pAttributes->pMd5 != NULL)
            pMd5 = ha->pAttributes->pMd5 + pHash->dwBlockIndex;
    }

    // The file will be stored after the end of the last archived file
    // (i.e. at old position of archived file
    if(nError == ERROR_SUCCESS)
    {
        TMPQBlock * pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
        const char * szTemp = strrchr(szArchivedName, '\\');

        // Get the position of the first file
        RelativePos.QuadPart = ha->pHeader->dwHeaderSize;

        // Find the position of the last file. It has to be after the last archived file
        // (Do not use the dwArchiveSize here, because it may or may not
        // include the hash table at the end of the file
        pBlockEx = ha->pExtBlockTable;
        for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++, pBlockEx++)
        {
            if(pBlock->dwFlags & MPQ_FILE_EXISTS)
            {
                TempPos.HighPart = pBlockEx->wFilePosHigh;
                TempPos.LowPart = pBlock->dwFilePos;
                TempPos.QuadPart += pBlock->dwCSize;

                if(TempPos.QuadPart > RelativePos.QuadPart)
                    RelativePos = TempPos;
            }
        }

        // When format V1, we cannot exceed 4 GB
        if(ha->pHeader->wFormatVersion == MPQ_FORMAT_VERSION_1)
        {
            TempPos.QuadPart = ha->MpqPos.QuadPart + RelativePos.QuadPart;
            TempPos.QuadPart += ha->pHeader->dwHashTableSize * sizeof(TMPQHash);
            TempPos.QuadPart += ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);
            TempPos.QuadPart += dwFileSize;

            if(TempPos.HighPart != 0)
                nError = ERROR_DISK_FULL;
        }

        // Get pointers to both block entries of the file
        pBlockEx = ha->pExtBlockTable + pHash->dwBlockIndex;
        pBlock = ha->pBlockTable + pHash->dwBlockIndex;
        
        // Save the file size info
        pBlockEx->wFilePosHigh = (USHORT)RelativePos.HighPart;
        pBlock->dwFilePos = RelativePos.LowPart;
        pBlock->dwFSize   = GetFileSize(hFile, NULL);
        pBlock->dwFlags   = dwFlags | MPQ_FILE_EXISTS;

        // Create seed1 for file encryption
        if(szTemp != NULL)
            szArchivedName = szTemp + 1;

        if(dwFlags & MPQ_FILE_ENCRYPTED)
        {
            dwSeed1 = DecryptFileSeed(szArchivedName);
            
            if(dwFlags & MPQ_FILE_FIXSEED)
                dwSeed1 = (dwSeed1 + pBlock->dwFilePos) ^ pBlock->dwFSize;
        }
    }

    // Allocate buffer for the input data
    if(nError == ERROR_SUCCESS)
    {
        nBlocks = (pBlock->dwFSize / ha->dwBlockSize) + 1;
        if(pBlock->dwFSize % ha->dwBlockSize)
            nBlocks++;

        pBlock->dwCSize = 0;
        if((pbFileData = ALLOCMEM(BYTE, ha->dwBlockSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbFileData;
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_COMPRESSED))
    {
        pdwBlockPos  = ALLOCMEM(DWORD, nBlocks + 1);
        pbCompressed = ALLOCMEM(BYTE, ha->dwBlockSize * 2);
        if(pdwBlockPos == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbCompressed;
    }

    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
        // Set the file pointer to file data position
        FilePos.QuadPart = ha->MpqPos.QuadPart + RelativePos.QuadPart;
        if(FilePos.QuadPart != ha->FilePointer.QuadPart)
        {
            SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
            ha->FilePointer = FilePos;
        }
    }

    // Write block positions (if the file will be compressed)
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_COMPRESSED))
    {
        dwBlockPosLen = nBlocks * sizeof(DWORD);
        if(dwFlags & MPQ_FILE_HAS_EXTRA)
            dwBlockPosLen += sizeof(DWORD);

        memset(pdwBlockPos, 0, dwBlockPosLen);
        pdwBlockPos[0] = dwBlockPosLen;
        
        // Write the block positions
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pdwBlockPos, nBlocks);
        WriteFile(ha->hFile, pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pdwBlockPos, nBlocks);

        if(dwTransferred == dwBlockPosLen)
            pBlock->dwCSize += dwBlockPosLen;
        else
            nError = GetLastError();

        // Update the current position in the file
        ha->HashTablePos.QuadPart = FilePos.QuadPart + dwTransferred;
    }

    // Write all file blocks
    if(nError == ERROR_SUCCESS)
    {
        crc32_context crc32_ctx;
        md5_context md5_ctx;

        // Initialize CRC32 and MD5 processing
        CRC32_Init(&crc32_ctx);
        MD5_Init(&md5_ctx);
        nCmp = nCmpFirst;

        // Move the file pointer to the begin of the file
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        for(nBlock = 0; nBlock < nBlocks-1; nBlock++)
        {
            DWORD dwInLength  = ha->dwBlockSize;
            DWORD dwOutLength = ha->dwBlockSize;

            // Load the block from the file
            ReadFile(hFile, pbFileData, ha->dwBlockSize, &dwInLength, NULL);
            if(dwInLength == 0)
                break;

            // Update CRC32 and MD5 for the file
            if(pCrc32 != NULL)
                CRC32_Update(&crc32_ctx, pbFileData, dwInLength);
            if(pMd5 != NULL)
                MD5_Update(&md5_ctx, pbFileData, dwInLength);

            // Compress the block, if necessary
            dwOutLength = dwInLength;
            if(pBlock->dwFlags & MPQ_FILE_COMPRESSED)
            {
                // Should be enough for compression
                int nOutLength = ha->dwBlockSize * 2;
                int nCmpType = 0;

                if(pBlock->dwFlags & MPQ_FILE_COMPRESS_PKWARE)
                    Compress_pklib((char *)pbCompressed, &nOutLength, (char *)pbFileData, dwInLength, &nCmpType, 0);

                if(pBlock->dwFlags & MPQ_FILE_COMPRESS_MULTI)
                    SCompCompress((char *)pbCompressed, &nOutLength, (char *)pbFileData, dwInLength, nCmp, 0, nCmpLevel);

                // The compressed block size must NOT be the same or greater like
                // the original block size. If yes, do not compress the block
                // and store the data as-is.
                if(nOutLength >= (int)dwInLength)
                {
                    memcpy(pbCompressed, pbFileData, dwInLength);
                    nOutLength = dwInLength;
                }

                // Update block positions
                dwOutLength = nOutLength;
                pdwBlockPos[nBlock+1] = pdwBlockPos[nBlock] + dwOutLength;
                nCmp = nCmpNext;
            }

            // Encrypt the block, if necessary
            if(pBlock->dwFlags & MPQ_FILE_ENCRYPTED)
            {
                BSWAP_ARRAY32_UNSIGNED((DWORD *)pbToWrite, dwOutLength / sizeof(DWORD));
                EncryptMPQBlock((DWORD *)pbToWrite, dwOutLength, dwSeed1 + nBlock);
                BSWAP_ARRAY32_UNSIGNED((DWORD *)pbToWrite, dwOutLength / sizeof(DWORD));
            }
            
            // Write the block
            WriteFile(ha->hFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
            if(dwTransferred != dwOutLength)
            {
                nError = ERROR_DISK_FULL;
                break;
            }

            // Update the hash table position and the compressed file size
            ha->HashTablePos.QuadPart += dwTransferred;
            ha->FilePointer.QuadPart += dwTransferred;
            pBlock->dwCSize += dwOutLength;
        }

        // Finish calculating of CRC32 and MD5
        if(pCrc32 != NULL)
            CRC32_Finish(&crc32_ctx, &pCrc32->dwValue);
        if(pMd5 != NULL)
            MD5_Finish(&md5_ctx, pMd5->Value);
    }

    // Now save the block positions
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPQ_FILE_COMPRESSED))
    {
        if(dwFlags & MPQ_FILE_HAS_EXTRA)
            pdwBlockPos[nBlocks] = pdwBlockPos[nBlocks-1];

        // If file is encrypted, block positions are also encrypted
        if(dwFlags & MPQ_FILE_ENCRYPTED)
            EncryptMPQBlock(pdwBlockPos, dwBlockPosLen, dwSeed1 - 1);
        
        // Set the position back to the block table
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        
        // Write block positions to the archive
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pdwBlockPos, nBlocks);
        WriteFile(ha->hFile, pdwBlockPos, dwBlockPosLen, &dwTransferred, NULL);
        if(dwTransferred != dwBlockPosLen)
            nError = ERROR_DISK_FULL;

        ha->FilePointer.QuadPart = ha->FilePointer.QuadPart + dwTransferred;
    }

    // If success, we have to change the settings
    // in MPQ header. If failed, we have to clean hash entry
    if(nError == ERROR_SUCCESS)
    {
        ha->pLastFile  = NULL;
        ha->dwBlockPos = 0;
        ha->dwBuffPos  = 0;
        ha->dwFlags   |= MPQ_FLAG_CHANGED;

        // Add new entry to the block table (if needed)
        if(pHash->dwBlockIndex >= ha->pHeader->dwBlockTableSize)
            ha->pHeader->dwBlockTableSize++;

        // Hash table size in the TMPQArchive is already set at this point
        RelativePos.QuadPart = ha->HashTablePos.QuadPart - ha->MpqPos.QuadPart;
        ha->pHeader->dwHashTablePos = RelativePos.LowPart;
        ha->pHeader->wHashTablePosHigh = (USHORT)RelativePos.HighPart;

        // Update block table pos
        RelativePos.QuadPart += (ha->pHeader->dwHashTableSize * sizeof(TMPQHash));
        ha->pHeader->wBlockTablePosHigh = (USHORT)RelativePos.HighPart;
        ha->pHeader->dwBlockTablePos = RelativePos.LowPart;
        ha->BlockTablePos.QuadPart = RelativePos.QuadPart + ha->MpqPos.QuadPart;

        // If the archive size exceeded 4GB, we have to use extended block table pos
        RelativePos.QuadPart += (ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock));
        if(RelativePos.HighPart != 0)
        {
            ha->pHeader->ExtBlockTablePos = RelativePos;
            ha->ExtBlockTablePos.QuadPart = RelativePos.QuadPart + ha->MpqPos.QuadPart;

            RelativePos.QuadPart += (ha->pHeader->dwBlockTableSize * sizeof(TMPQBlockEx));
        }

        // Update archive size (only valid for version V1)
        ha->MpqSize = RelativePos;
        ha->pHeader->dwArchiveSize = ha->MpqSize.LowPart;
    }
    else
    {
        // Clear the hash table entry
        if(pHash != NULL)
            memset(pHash, 0xFF, sizeof(TMPQHash));
    }

    // Cleanup
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    if(pdwBlockPos != NULL)
        FREEMEM(pdwBlockPos);
    if(pbFileData != NULL)
        FREEMEM(pbFileData);
    if(pbReplaced != NULL)
        *pbReplaced = bReplaced;
    return nError;
}
*/

int SetDataCompression(int nDataCompression)
{
    nDataCmp = nDataCompression;
    return 0;
}

// This method saves MPQ header, hash table and block table.
int SaveMPQTables(TMPQArchive * ha)
{
    BYTE * pbBuffer = NULL;
    DWORD dwBytes;
    DWORD dwWritten;
    DWORD dwBuffSize = max(ha->pHeader->dwHashTableSize, ha->pHeader->dwBlockTableSize);
    int   nError = ERROR_SUCCESS;

    // Allocate buffer for encrypted tables
    if(nError == ERROR_SUCCESS)
    {
        // Allocate temporary buffer for tables encryption
        pbBuffer = ALLOCMEM(BYTE, sizeof(TMPQHash) * dwBuffSize);
        if(pbBuffer == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Write the MPQ Header
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwHeaderSize = ha->pHeader->dwHeaderSize;

        // Write the MPQ header
        DWORD pointer1 = SetFilePointer(ha->hFile, ha->MpqPos.LowPart, &ha->MpqPos.HighPart, FILE_BEGIN);
		
        // Convert to little endian for file save
        BSWAP_TMPQHEADER(ha->pHeader);
        WriteFile(ha->hFile, ha->pHeader, dwHeaderSize, &dwWritten, NULL);
        BSWAP_TMPQHEADER(ha->pHeader);

        if(dwWritten != ha->pHeader->dwHeaderSize)
            nError = ERROR_DISK_FULL;
    }

    // Write the hash table
    if(nError == ERROR_SUCCESS)
    {
        // Copy the hash table to temporary buffer
        dwBytes = ha->pHeader->dwHashTableSize * sizeof(TMPQHash);
        memcpy(pbBuffer, ha->pHashTable, dwBytes);

        // Convert to little endian for file save
        EncryptHashTable((DWORD *)pbBuffer, (BYTE *)"(hash table)", dwBytes >> 2);
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pbBuffer, dwBytes / sizeof(DWORD));

        // Set the file pointer to the offset of the hash table and write it
        DWORD pointer2 = SetFilePointer(ha->hFile, ha->HashTablePos.LowPart, (PLONG)&ha->HashTablePos.HighPart, FILE_BEGIN);
		WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
        if(dwWritten != dwBytes)
            nError = ERROR_DISK_FULL;
    }

    // Write the block table
    if(nError == ERROR_SUCCESS)
    {
        // Copy the block table to temporary buffer
        dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlock);
        memcpy(pbBuffer, ha->pBlockTable, dwBytes);

        // Encrypt the block table and write it to the file
        EncryptBlockTable((DWORD *)pbBuffer, (BYTE *)"(block table)", dwBytes >> 2);
        
        // Convert to little endian for file save
        BSWAP_ARRAY32_UNSIGNED((DWORD *)pbBuffer, dwBytes / sizeof(DWORD));
        WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
        if(dwWritten != dwBytes)
            nError = ERROR_DISK_FULL;
    }

    // Write the extended block table
    if(nError == ERROR_SUCCESS && ha->pHeader->ExtBlockTablePos.QuadPart != 0)
    {
        // We expect format V2 or newer in this case
        assert(ha->pHeader->wFormatVersion >= MPQ_FORMAT_VERSION_2);

        // Copy the block table to temporary buffer
        dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TMPQBlockEx);
        memcpy(pbBuffer, ha->pExtBlockTable, dwBytes);

        // Convert to little endian for file save
        BSWAP_ARRAY16_UNSIGNED((USHORT *)pbBuffer, dwBytes / sizeof(USHORT));
        WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
        if(dwWritten != dwBytes)
            nError = ERROR_DISK_FULL;
    }

    // Set end of file here
    if(nError == ERROR_SUCCESS)
    {
        SetEndOfFile(ha->hFile);
    }

    // Cleanup and exit
    if(pbBuffer != NULL)
        FREEMEM(pbBuffer);
    return nError;
}

// Frees the MPQ archive
void FreeMPQArchive(TMPQArchive *& ha)
{
    if(ha != NULL)
    {
        FREEMEM(ha->pbBlockBuffer);
        FREEMEM(ha->pBlockTable);
        FREEMEM(ha->pExtBlockTable);
        FREEMEM(ha->pHashTable);
        SListFileFreeListFile(ha);
        FreeMPQAttributes(ha->pAttributes);

        if(ha->hFile != INVALID_HANDLE_VALUE)
            CloseHandle(ha->hFile);
        FREEMEM(ha);
        ha = NULL;
    }
}
