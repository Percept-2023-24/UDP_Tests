#include "rapidjson/document.h" 
#include "rapidjson/filewritestream.h" 
#include "rapidjson/writer.h" 
#include <fstream> 
#include <iostream> 

using namespace rapidjson; 

int main() 
{ 
	// Create the JSON document 
	Document d; 
	d.SetObject(); 

	// Add data to the JSON document 
	d.AddMember("name", "Geek", d.GetAllocator()); 
	d.AddMember("age", 30, d.GetAllocator()); 

	// Open the output file 
	FILE* fp = fopen("example.json", "w"); 

	// Write the JSON data to the file 
	char writeBuffer[65536]; 
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer)); 
    Writer<FileWriteStream> writer(os); 
    d.Accept(writer);

	fclose(fp);

	return 0; 

}