#include "process_arguments.h"

int main(int argc, char* argv[])
{
	PNG2AssetData png2AssetData(argc, argv);

	int proccesArgValue = png2AssetData.errorCode;

	// Return the error code if the function returns non-zero
	if(proccesArgValue != 0) {
		return proccesArgValue;
	}

	return png2AssetData.Execute();
}
