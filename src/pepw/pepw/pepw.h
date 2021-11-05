#pragma once
#include <string>
#include <Windows.h>
#include <wincrypt.h>
#include "SEED128/KISA_SEED_ECB.h"

BYTE dialogTemplateRawData[] = {
	0x01, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x08, 0xc8, 0x80,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb9, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00,
	0x6e, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x20, 0x00, 0x74, 0x00, 0x68, 0x00, 0x65, 0x00,
	0x20, 0x00, 0x70, 0x00, 0x61, 0x00, 0x73, 0x00, 0x73, 0x00, 0x77, 0x00, 0x6f, 0x00, 0x72, 0x00,
	0x64, 0x00, 0x00, 0x00, 0x08, 0x00, 0x90, 0x01, 0x00, 0x01, 0x4d, 0x00, 0x53, 0x00, 0x20, 0x00,
	0x53, 0x00, 0x68, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x20, 0x00, 0x44, 0x00, 0x6c, 0x00,
	0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x81, 0x50,
	0x07, 0x00, 0x07, 0x00, 0xab, 0x00, 0x0e, 0x00, 0xe9, 0x03, 0x00, 0x00, 0xff, 0xff, 0x81, 0x00,
	0x00, 0x00, 0x00, 0x00
};

DWORD hashGenerate(const std::string& str) {
	DWORD hash = 0;
	DWORD poly = 0xEDB88320;

	for (auto i = 0u; i <= str.size(); i++) {
		poly = (poly << 1) | (poly >> 31); // 1bit Left Shift
		hash = (DWORD)(poly * hash + str[i]);
	}

	return hash;
}

int getSHA256(const std::string& password, BYTE* output) {
	HCRYPTPROV hProv = NULL;
	HCRYPTPROV hHash = NULL;
	DWORD cbHashSize = 32;
	DWORD dwRetn = 0;

	if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
		return 1;
	}

	if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
		dwRetn = 1;
		goto label_exit2;
	}

	if (!CryptHashData(hHash, (BYTE*)password.data(), password.size(), 0)) {
		dwRetn = 1;
		goto label_exit1;
	}

	if (!CryptGetHashParam(hHash, HP_HASHVAL, output, &cbHashSize, 0)) {
		dwRetn = 1;
		goto label_exit1;
	}

label_exit1:
	CryptDestroyHash(hHash);
label_exit2:
	CryptReleaseContext(hProv, 0);

	return dwRetn;
}

void encryptData(BYTE* data, std::size_t size, DWORD* roundKey) {
	auto end = data + size - 16;
	for (; data <= end; data++) {
		SEED_Encrypt(data, roundKey);
	}
}