#ifndef PAGEHEAD_H
#define PAGEHEAD_H

#include <cstring>
#include <iostream>

class PageHead{
public:
	int usedSlot;//已经使用的槽的个数
	char slotMap[84];//84个byte每一个bit对应之后每个槽的状态。（0为空，1为有记录）
	PageHead(){
		usedSlot = 0;
		memset(slotMap,0,sizeif(slotMap));
	}
	bool getRecordHead(int RecordID){
		char Record slotMap[RecordID >> 3];
		return (Record >> (7 - (RecordID%8))) % 2;
	}
	int setRecordHead(int RecordID,bool IsRecord){
		char Record = slotMap[RecordID>>3];
		if(IsRecord) Record  = Record | (1<<(7-(RecordID%8)));
		else Record = Record & (255-(1<<(7-(RecordID%8))));
		slotMap[RecordID>>3]=Record;
		return 1;
	}
};

#endif