#include <iostream>
#include <App.h>

int main(int argc, char *argv[])
{
    if (argc < 2){
	// We expect at least a file path containing the initial map
	std::cerr << "Error: too few arguments" << std::endl;
	return -1;
    }

    std::string mapFilePath;

    for (size_t i = 1; i < argc; ++i){
	// Set file path
	if (std::string(argv[i]) == "-f"){
	    if (i+1 >= argc){
		std::cerr << "Error: no file path specified afet -f" << std::endl;
		return -1;
	    }
	    else{
		mapFilePath = argv[++i];
	    }
	}
    }

    gl3wInit();
    App app(mapFilePath);
    app.run();
    return 0;
}
