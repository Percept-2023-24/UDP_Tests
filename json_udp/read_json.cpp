#include "rapidjson/document.h" 
#include "rapidjson/filereadstream.h" 
#include <iostream> 

using namespace std;

int main() 
{ 
	// Open the file for reading 
	FILE* fp = fopen("frame_data.json", "r"); 

	// Use a FileReadStream to 
	// read the data from the file 
	char readBuffer[65536]; 
	rapidjson::FileReadStream is(fp, readBuffer, 
								sizeof(readBuffer)); 

	// Parse the JSON data 
	// using a Document object 
	rapidjson::Document d; 
	d.ParseStream(is); 

	// Close the file 
	fclose(fp); 

	// Access the data in the JSON document 
	cout << d["angle"].GetInt() << endl; 
	cout << d["range"].GetInt() << endl; 

	return 0; 
}