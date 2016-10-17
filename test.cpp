#include "recordmanager/RecordManager.h"
// #include "recordmanager/rm_filescan.h"

int main(int argc, char const *argv[]) {
  MyBitMap::initConst();
  FileManager* fm = new FileManager();
  BufPageManager* bpm = new BufPageManager(fm);
  RM_Manager* rm_manager = new RM_Manager(fm,bpm);
  rm_manager->CreateFile("testfile.txt",3);
  RM_FileHandle filehandle;
  rm_manager->OpenFile("testfile.txt",filehandle);
  char a[3] = {'a','b','c'};
  for (int i = 0; i < 1000; i++) {
     RID rid;
     filehandle.InsertRec(a,rid);
     std::cout << rid.Page()<<" "<<rid.Slot() << std::endl;
  }
  bpm->close();
  //以上插入了一千条信息,现在输出信息
  std::cout <<"共有" <<filehandle.getNumber() << "条记录"<<std::endl;

  return 0;
}
