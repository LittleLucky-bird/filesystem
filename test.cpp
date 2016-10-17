#include "recordmanager/PageHead.h"
#include "common/RID.h"

int main(int argc, char const *argv[]) {
  char a = 0;
  for (int i = 0; i < 128; i++) {
    std::cout << a++ <<" "<<i<< std::endl;
  }
  return 0;
}
