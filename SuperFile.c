#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Falied opening file");

	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		CLOSE_RETURN_0(fp);
	}

	if (!saveAddressToFile(&pMarket->location, fp))
	{
		CLOSE_RETURN_0(fp);
	}
	int count = getNumOfProductsInList(pMarket);

	if (!writeIntToFile(count, fp, "Error write product count\n"))
	{
		CLOSE_RETURN_0(fp);
	}

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToFile(pTemp, fp))
		{
			CLOSE_RETURN_0(fp);
		}
		pN = pN->next;
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);

	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	
	}

	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				free(pMarket->name);
				fclose(fp);
				return 0;
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
				return 0;
			}
		}
	}


	fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName,&pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp, "Falied to open file.");

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		CLOSE_RETURN_0(fp);
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	CHECK_MSG_RETURN_0(fp, "Falied to open file");
	int count;
	fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}

int saveSuperMarketToFileCompressed(const SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	BYTE data[2] = { 0 };
	int numOfProd = getNumOfProductsInList(pMarket);
	int superMarketNameLen = sizeof(pMarket->name);
	data[0] = numOfProd >> 2;
	data[1] = (numOfProd & 0x3) << 6 | superMarketNameLen;
	if (fwrite(&data, sizeof(BYTE), 2, fileName) != 2)
		return 0;
	if (fwrite(pMarket->name, sizeof(char), superMarketNameLen, fileName) != superMarketNameLen)
		return 0;

	//save Adress

	return 1;
}

int saveAdressToFileCompressed(const SuperMarket* pMarket, const char* fileName)
{
	BYTE houseNumber = pMarket->location.num;
	if (fwrite(&houseNumber, sizeof(BYTE), 1, fileName) != 1)
		return 0;
	int len_street = (int)strlen(pMarket->location.street) + 1;
	if (fwrite(&len_street, sizeof(int), 1, fileName) != 1)
		return 0;
	if (fwrite(pMarket->location.street, sizeof(char), len_street, fileName) != len_street)
		return 0;
	int len_city = (int)strlen(pMarket->location.city) + 1;
	if (fwrite(&len_city, sizeof(int), 1, fileName) != 1)
		return 0;
	if (fwrite(pMarket->location.city, sizeof(char), len_city, fileName) != len_city)
		return 0;
	return 1;
}

int saveProductToFileCompressed(const Product* pProd, const char* fileName)
{
	BYTE data[3] = { 0 };
	BYTE data2[3] = { 0 };
	unsigned int barcode =(unsigned int) pProd->barcode;
	data[0] = ((barcode >> 2) & 0xFF)| ((barcode>>6) & 0x3);
	data[1] = ((barcode >> 8) & 0xF) << 4 | ((barcode >> 12) & 0xF);
	data[2] = (((barcode >> 14) & 0x3) << 6) | ((barcode >> 20) & 0x3F);

	return 0;
}
