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
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp, "Falied opening file");
	BYTE data[2] = { 0 };
	int numOfProd = getNumOfProductsInList(pMarket);
	int superMarketNameLen = strlen(pMarket->name);
	data[0] = numOfProd >> 2;
	data[1] = (numOfProd & 0x3) << 6 | superMarketNameLen&0x3F;
	if (fwrite(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	if (fwrite(pMarket->name, sizeof(char), superMarketNameLen, fp) != superMarketNameLen)
		return 0;

	//save Adress
	
	CHECK_RETURN_0(saveAdressToFileCompressed(pMarket, fp));
	CHECK_RETURN_0(saveProductArrayToFileCompressed(fp, &pMarket->productList, getNumOfProductsInList(pMarket)));
	CHECK_RETURN_0(fclose(fp));
	return 1;
}

int saveAdressToFileCompressed(const SuperMarket* pMarket, FILE* fileName)
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

int saveProductToFileCompressed(const Product* pProd, FILE* fileName)
{
	
	BYTE data1[6] = { 0 };
	BYTE data2[3] = { 0 };
	//Make the barcode available for compress
	char* barcode = pProd->barcode;
	int name_len = strlen(pProd->name);
	
	decodeBarcode(barcode);
	
	//barcode type and name length.
	data1[0] = (barcode[0] << 2) |( barcode[1] >> 4);
	data1[1] = ((barcode[1]&0xF) << 4) | (barcode[2] >> 2);
	data1[2] = ((barcode[2]& 0x3) << 6) | barcode[3];
	data1[3] = (barcode[4] << 2 )|| (barcode[5] >> 4);
	data1[4] = ((barcode[5] & 0xF) << 4) | (barcode[6] >> 2);
	data1[5] = ((barcode[6] & 0x3) << 6) | ((name_len & 0xF) << 2) | (pProd->type & 0x3);
	
	if (fwrite(&data1, sizeof(BYTE), 6, fileName) != 6)
		return 0;

	if (fwrite(pProd->name, sizeof(char), name_len, fileName) != name_len)
		return 0;
//Count and price
	int integerPrice = (int) pProd->price;
	int priceAfterDot = (int)((pProd->price - integerPrice) * 100);
	data2[0] = pProd->count&0xFF;
	data2[1] = ((priceAfterDot) << 1) | ((integerPrice >> 8));
	data2[2] = integerPrice & 0xFF;
	

	if (fwrite(&data2, sizeof(BYTE), 3, fileName) != 3)
		return 0;
	return 1;
}

int saveProductArrayToFileCompressed(FILE* file, LIST* pList, int count)
{

	if (!file || !pList)
		return 0;
	if (fwrite(&count, sizeof(int), 1, file) != 1)
	{
		fclose(file);
		return 0;
	}
	if (count > 0) {
		NODE* tmp;
		tmp = pList->head.next;
		while (tmp != NULL)
		{
			saveProductToFileCompressed(tmp->key, file);
			tmp = tmp->next;
		}
	}
	return 1;
}

int initSuperMarketCompressed(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp = fopen(fileName, "rb");
	CHECK_MSG_RETURN_0(fp, "Failed opening file");
	BYTE data[2];
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	int numOfProd = (data[0] << 2) | (data[1] >> 6);
	int superMarketNameLen = data[1] & 0x3F;
	pMarket->name = (char*)malloc(sizeof(char) * (superMarketNameLen + 1));
	CHECK_RETURN_0(pMarket->name);
	if (fread(pMarket->name, sizeof(char), superMarketNameLen, fp) != superMarketNameLen)
		return 0;
	pMarket->name[superMarketNameLen] = '\0';
	CHECK_RETURN_0(readAdressFromFileCompressed(pMarket, fp));
	CHECK_RETURN_0(readeProductArrayFromFileCompressed(fp, pMarket));
	fclose(fp);
	return 1;
}

int readAdressFromFileCompressed(SuperMarket* pMarket, FILE* fileName)
{
	BYTE houseNumber;
	if (fread(&houseNumber, sizeof(BYTE), 1, fileName) != 1)
		return 0;
	pMarket->location.num = houseNumber;
	int len_street;
	if (fread(&len_street, sizeof(int), 1, fileName) != 1)
		return 0;
	pMarket->location.street = (char*)malloc(len_street * sizeof(char));
	CHECK_RETURN_0(pMarket->location.street);
	if (fread(pMarket->location.street, sizeof(char), len_street, fileName) != len_street)
		return 0;
	int len_city;
	if (fread(&len_city, sizeof(int), 1, fileName) != 1)
		return 0;
	pMarket->location.city = (char*)malloc(len_city * sizeof(char));
	CHECK_RETURN_0(pMarket->location.city);
	if (fread(pMarket->location.city, sizeof(char), len_city, fileName) != len_city)
		return 0;
	return 1;
}

int readProductFromFileCompressed(Product* pProd, FILE* fileName)
{
	BYTE data1[6] = { 0 };
	BYTE data2[3] = { 0 };
	char* barcode = malloc(sizeof(char) * 8);
	if (fread(&data1, sizeof(BYTE), 6, fileName) != 6)
		return 0;

	// Extract barcode type and name length
	barcode[0] = data1[0] >> 2;
	barcode[1] = ((data1[0] & 0x3) << 4) | (data1[1] >> 4);
	barcode[2] = ((data1[1] & 0xF) << 2) | (data1[2] >> 6);
	barcode[3] = data1[2] & 0x3F;
	barcode[4] = data1[3] >> 2;
	barcode[5] = ((data1[3] & 0x3) << 4) | (data1[4] >> 4);
	barcode[6] = ((data1[4] & 0xF) << 2) | (data1[5] >> 6);
	int name_len = data1[5] & 0xF;
	pProd->type = data1[5] & 0x3;
	barcode[7] = '\0';
	encodeBarcode(barcode);
	printf(barcode);
	strcpy(pProd->barcode, barcode);
	printf("\n");
	// Read the name of the product
	if (fread(pProd->name, sizeof(char), name_len, fileName) != name_len)
		return 0;
	pProd->name[name_len] = '\0';

	if (fread(&data2, sizeof(BYTE), 3, fileName) != 3)
		return 0;

	// Extract count and price
	pProd->count = data2[0]  & 0xFF;
	int integerPrice = ((data2[1] & 0x1)<< 8) | data2[2];
	float priceAfterDot = (data2[1] >> 1) / 100;
	printf("integer - %d price after dot - %f \n", integerPrice, priceAfterDot);
	pProd->price = integerPrice + priceAfterDot;

	return 1;
}

int readeProductArrayFromFileCompressed(FILE* file, SuperMarket* pMarket)
{
	int count = 0;
	if (!file || !pMarket)
		return 0;

	if (fread(&count, sizeof(int), 1, file) != 1)
	{
		CLOSE_RETURN_0(file);
	}
	printf(" Count - %d\n", count);
	for (int i = 0; i < count - 1; i++)
	{
		Product* pProd = (Product*)malloc(sizeof(Product));
		CHECK_RETURN_0(pProd);
		
		if (!readProductFromFileCompressed(pProd, file))
		{
			FREE_CLOSE_FILE_RETURN_0(pProd, file);
		}
		printProduct(pProd);
		CHECK_RETURN_0(L_insert(&pMarket->productList.head, pProd, compareProductByBarcode));
	}

	return 1;
}

