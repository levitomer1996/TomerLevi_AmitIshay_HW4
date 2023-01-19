#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"





int menu();


int main(int argc, char* argv[])
{
	SuperMarket	market;
	
	
	int isCompressed;
	char* file = paramsMain(argc, argv, &isCompressed);
	CHECK_MSG_RETURN_0(initSuperMarket(&market, file, CUSTOMER_FILE_NAME, isCompressed), "error init  Super Market")
	
	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;


		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerPay:
			if(!doPayment(&market))
				printf("Error in payment\n");
			break;
		
		case eSortCustomer:
			sortCustomers(&market);
			break;
			
		case eSearchCustomer:
			findCustomer(&market);
			break;
			
		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank", "You", "For", "Shpping", "With", "Us", NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);

	//if (!saveSuperMarketToFile(&market, SUPER_FILE_NAME, CUSTOMER_FILE_NAME))
		//printf("Error saving supermarket to file\n");
	saveSuperMarketToFile(&market,file , CUSTOMER_FILE_NAME,isCompressed);

	freeMarket(&market);

	system("pause");
	return 1;
}

int menu()
{
	int option;
	printf("\n\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

char* paramsMain(int argc, char* argv[], int* isCompressed)
{
	int numOfVars = 0;
	*isCompressed = 0;
	numOfVars += sscanf(argv[1], "%d", &(*isCompressed));
	char* file = getDynStr(argv[2]);
	if (file != NULL) {
		numOfVars++;
	}
	if (numOfVars != 2) {
		return NULL;
	}
	return file;
}

