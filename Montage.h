#ifndef _MONTAGE__H_
#define _MONTAGE__H_

#include <vector>
#include <string>
using namespace std;

class Montage
{
public:
  static void compose(const vector<string> &images, int cols, int rows, const string& composedImagePath);
};

#endif //_MONTAGE__H_
