#pragma once
#ifndef MACROS_H_
#define MACROS_H_
#define CHECK_RETURN_0(ptr) {\
					 if (ptr==NULL)\
				     return 0;\
					}


#define CHECK_MSG_RETURN_0(ptr,msg) {\
							if (ptr==NULL){\
								puts(msg);\
								return 0;}\
							}

#define  FREE_CLOSE_FILE_RETURN_0(ptr,file){\
									fclose(file);\
									free(ptr);}

#define  CLOSE_RETURN_0(file){\
							fclose(file);}

#define	DETAIL_PRINT(str) printf(#str "\n");

#endif /* MACROS_H_ */