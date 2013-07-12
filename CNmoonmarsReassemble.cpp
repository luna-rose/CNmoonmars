#include <string>
#include <getopt.h>

struct Params {
	std::string resultsDir;
	
	std::string inputFilename;
	std::string limitsFilename;
	std::string abcdDistFilename;
	std::string possibleHotspotsFilename;
};

Params DefaultParams() {
	Params params;
	
	params.resultsDir = "output/";
	
	params.inputFilename = "input-observedhotspots.txt";
	params.limitsFilename = "limits.txt";
	params.abcdDistFilename = "abcdspaceprob.txt";
	params.possibleHotspotsFilename = "possiblehotspots.txt";
	
	return params;
}

void ParseArguments(int argc, char* argv[], Params &params) {
	static struct option long_options[] =
	{
		{"outputDir",					required_argument, NULL, 130},
		{"inputFilename",				required_argument, NULL, 131},
		{"limitsFilename",				required_argument, NULL, 132},
		{"abcdDistFilename",			required_argument, NULL, 133},
		{"possibleHotspotsFilename",	required_argument, NULL, 134},
		{0, 0, 0, 0}
	};
	
	int option_index;
	int c;
	while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1) {
		switch (c)
		{
			case 130: params.resultsDir = optarg; break;
			case 131: params.inputFilename = optarg; break;
			case 132: params.inputFilename = optarg; break;
			case 133: params.abcdDistFilename = optarg; break;
			case 134: params.possibleHotspotsFilename = optarg; break;
			default: 
				fprintf (stderr, "Error: Could not parse arguments.\n");
				exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char* argv[]) {
	Params params = DefaultParams();	
	ParseArguments(argc, argv, params);
	
	// TODO: put code to reassemble files here
	
	return EXIT_SUCCESS;
}
