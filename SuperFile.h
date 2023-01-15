#pragma once

#include <stdio.h>
#include "Supermarket.h"


int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
			const char* customersFileName);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
			const char* customersFileName);

//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);

//Compressed
int		saveSuperMarketToFileCompressed(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName);

int saveAdressToFileCompressed(const SuperMarket* pMarket, const char* fileName);

int saveProductToFileCompressed(const Product* pProd, const char* fileName);