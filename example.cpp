#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void createDirectoryIfNotExists(const string& dirPath)
{
  string cmd = "mkdir -p " + dirPath;
  int ret = system(cmd.c_str());
  if (ret == -1)
  {
    //handle error
  }
}

static int counter = 0;
void House::montage(const string& algoName, const string& houseName) const
{
        vector<string> tiles;
        for (size_t row = 0; row < _rows; ++row)
        {
          for (size_t col = 0; col < _cols; ++col)
          {
            if (_house[row][col] == ' ')
              tiles.push_back("0");
            else
              tiles.push_back(string() + _house[row][col]);
          }
        }
        string imagesDirPath = "simulations/" + algoName + "_" + houseName;
        createDirectoryIfNotExists(imagesDirPath);
        string counterStr = to_string(counter++);
        string composedImage = imagesDirPath + "/image" + string(5-counterStr.length(), '0') + counterStr + ".jpg";
        Montage::compose(tiles, _cols, _rows, composedImage);
}



//.... To Encode images into video:
string simulationDir =  "simulations/" + algoName + "_" + houseName + "/";
string imagesExpression = simulationDir + "image%5d.jpg";
Encoder::encode(imagesExpression, algoName + "_" + houseName + ".mpg");
//don't forget to remove the images after the video is created...
