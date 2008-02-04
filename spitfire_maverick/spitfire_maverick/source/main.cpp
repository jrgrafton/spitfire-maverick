// PALib Template Application

// Includes
#include <PA9.h>       // Include for PA_Lib

int main()
{
	PA_Init();    // Initializes PA_Lib
	PA_InitVBL(); // Initializes a standard VBL

	PA_InitText(1, 2);
	PA_OutputSimpleText(1, 1, 2, "Hello World!");

	// Infinite loop to keep the program running
	while (1)
	{
		PA_WaitForVBL();
	}
	
	return 0;
}
