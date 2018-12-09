//Game Handling Class
#include <fstream>

//Function to Save an Array to a file, saving memory
void saveInt(int val, int pos, std::string filename);
void loadInt(int &val, int pos, std::string filename);

//Save a Value in an array to a file in binary
void saveInt(int val, int pos, std::string filename){
  //Open File to Write To:
  std::ofstream file;
  file.open(filename, std::ios::binary);

  //If the file has been succesfully opened:
  if(file.is_open()){
    //Write Value to file, at next free position
    file.seekp(pos*sizeof(int));
    file.write((char*) &val, sizeof(int));
  }
  file.close();
};

//Function to load an Array to a file, saving memory
void loadInt(int &val, int pos, std::string filename){
  //Open File to Read From:
  std::ifstream file;
  file.open(filename, std::ios::binary);

  //If the file has been succesfully opened:
  if(file.is_open()){
    //Iteratively Read Elements from the Array
    file.seekg(pos*sizeof(int));
    file.read((char*) &val, sizeof(int));
  }
  file.close();
};
//Game Handling Class
