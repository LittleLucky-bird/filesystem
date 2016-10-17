#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "../common/RID.h"
#include "PageHead.h"
#include <cstring>
#include <iostream>

class RM_Record{
public:
	RM_Record():recordSize(-1), data(NULL), rid(-1, -1){}
	~RM_Record(){
		if (data != NULL)
			delete [] data;
	}

	int GetData(char *&pData) const{
		if (data != NULL && recordSize != -1){
			pData = data;
			return 1;
		}
		else
			return 0;
	}

	int Set(char *pData,int size,RID rid_){
		if(recordSize != -1 && (size != recordSize))
			return 0;
		recordSize = size;
	  	this->rid.Copy(rid_);
		if (data == NULL){
			data = new char[recordSize];
			memset(data, 0, sizeof(data));
		}
	  	memcpy(data, pData, size);
		return 1;
	}

	int GetRid(RID &rid) const{
		if (data != NULL && recordSize != -1){
			rid.Copy(this->rid);
			return 1;
	  }
		else
			return 0;
	}

	int recordSize;
	char *data;
	RID rid;
};

class FileHead{
public:
	int recordSize;//记录长度
	int pageNumber;//页的个数
	int recordPerPage;//每页记录的个数
	int recordNumber;//记录的总数

	FileHead(int recordSize){
		int pageHead = 96;
		int emptyHead = 12;
		int byteNumber = pageHead - emptyHead;
		int bitPerByte = 8;
		int bitPerPage = 8196;
		this->recordSize = recordSize;
		this->pageNumber = 0;
		if (recordSize * byteNumber * bitPerByte <= (bitPerPage - pageHead))
			this->recordPerPage = byteNumber * bitPerByte;
		else
			this->recordPerPage = (bitPerPage - pageHead)/recordSize;
		this->recordNumber = 0;
	}

};

class RM_FileHandle{

private:
	BufPageManager *bpm;
	int fileID;
	FileHead *fileHead;
	int recordSize;
	int recordPerPage;


public:
	RM_FileHandle()：bpm(null),fileID(-1){}
	RM_FileHandle(BufPageManager *bpm_,int fileID_){
		Open(bpm_,fileID_);
	}
	~RM_FileHandle(){}

	void Open(BufPageManager *bpm_,int fileID_){
		bpm = bpm_;
		fileID = fileID_;
		int index;
		fileHead = (FileHead*)(bpm->getPage(fileID,0,index));
		recordSize = fileHead->recordSize;
		recordPerPage = fileHead->recordPerPage;
		bpm->access(index);
	}

	int getFileID() const{
		return fileID;
	}

	int GetRec(const RID &rid,RM_Record &rec) const{
		int page = rid.Page();
		int slot = rid.Slot();
		if (page > fileHead->pageNumber)
			return 0;
		int index;
		BufType b = bpm->getPage(fileID, page, index);
		PageHead *head = (PageHead*)(b);
		if (head->getRecordHead(slot))
		{
			char *data;
			data = (char*)b;
			data += 96 + recordSize * slot;
			rec.Set(data, recordSize, rid);
			return 1;
		}
		else
			return 0;
	}

	int InsertRec(const char*pData,RID &rid){
		int headindex, pageindex;
		fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
		bool ok = false;
		for (int i = 1; i <= fileHead->pageNumber; i++)
		{
			BufType b = bpm->getPage(fileID, i, pageindex);
			PageHead *head = (PageHead*)(b);
			if (head->usedSlot < recordPerPage)
			{
				for (int j = 0; j < recordPerPage; j++)
				{
					if (head->getRecordHead(j) == 0)
					{
						char *data;
						data = (char*)b;
						data += 96 + recordSize * j;
						memcpy(data, pData, recordSize);
						head->usedSlot++;
						head->setRecordHead(j, true);
						bpm->markDirty(pageindex);
						rid = RID(i, j);
						ok = true;
						break;
					}
				}
				break;
			}
		}
		if (!ok)
		{
			fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
			int pageid = (++fileHead->pageNumber);
			bpm->markDirty(headindex);
			BufType b = bpm->allocPage(fileID, pageid, pageindex, false);
			memset(b, 0, 8 * 1024);
			PageHead head = PageHead();
			head.usedSlot++;
			head.setRecordHead(0, true);
			char *data;
			data = (char*)b;
			memcpy(data, &head, sizeof(PageHead));
			data += 96;
			memcpy(data, pData, recordSize);
			bpm->markDirty(pageindex);
			rid = RID(pageid, 0);
			ok = true;
		}
		fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
		fileHead->recordNumber++;
		bpm->markDirty(headindex);
		return ok;
	}
	
	int DeleteRec(const RID &rid){
		int page = rid.Page();
		int slot = rid.Slot();
		if (page > fileHead->pageNumber)
			return 0;
		int pageindex, headindex;
		BufType b = bpm->getPage(fileID, page, pageindex);
		PageHead *head = (PageHead*)(b);
		if (head->getRecordHead(slot))
		{
			head->usedSlot--;
			head->setRecordHead(slot, false);
			bpm->markDirty(pageindex);
			fileHead = (FileHead*)(bpm->getPage(fileID, 0, headindex));
			fileHead->recordNumber--;
			bpm->markDirty(headindex);
			return 1;
		}
		return 0;
	}
	int UpdateRec(const RM_Record &rec){
		RID rid;
		rec.GetRid(rid);
		int page = rid.Page();
		int slot = rid.Slot();
		if (page > fileHead->pageNumber)
			return 0;
		int index;
		BufType b = bpm->getPage(fileID, page, index);
		PageHead *head = (PageHead*)(b);
		if (head->getRecordHead(slot))
		{
			char *data;
			data = (char*)b;
			data += 96 + recordSize * slot;
			char *pdata;
			rec.GetData(pdata);
			memcpy(data, pdata, recordSize);
			bpm->markDirty(index);
			return 1;
		}
		return 0;
	}
};

class PFS{
public:
	int pageNumber;
	char head[100];
	char used[8088]; // 0-unused 1-used
	PFS(){
		pageNumber = 1;
		for(int i=0;i<8088;i++)
			used[i] = 0;
	}
	~PFS();
};



class RM_Manager
{
	FileManager* myFileManager;
	BufPageManager* bpm;
public:
	RM_Manager(FileManager* pfm,BufPageManager* bpm){
		myFileManager = pfm;
		this->bpm = bpm;
	}
	~RM_Manager();
	FileManager* getFileManager(){
		return myFileManager;
	}
	~RM_Manager();
	FileManager* getFileManager(){
		return myFileManager;
	}
	BufPageManager* getBufPageManager(){
		return bpm;
	}
	int CreateFile(const char *fileName,int recordSize){
		int index;
		int fileID;
		int pageID = 0;
		if(myFileManager = NULL){
			cout<<"file can't found"<<endl;
		}
		myFileManager->createFile(fileName);
		myFileManager->openFile(fileName,fileID);
		cout<<"fileID"<<fileID<<endl;
		BufType b = bpm->allocPage(fileID,pageID,index,false);
		memset(b,0,8*1024);
		bpm->markDirty(index);
		cout<<"recordSize "<<recordSize<<endl;
		FileHead *newPage = new FileHead(recordSize);
		memcpy(b,newPage,sizeof(FileHead));
		cout<<"after memcpy"<<b[0]<<endl;
		b = bpm->getPage(fileID,pageID,index);
		cout<<"test page head"<<b[0]<<endl;
		bpm->close();
		myFileManager->closeFile(fileID);
		return 0;
	}
	int DestroyFile(const char *fileName){
		myFileManager->destroyFile(fileName);
		return 0;
	}
	int OpenFile(const char *fileName,RM_FileHandle &fileHandle){
		int fileID;
		myFileManager->openFile(fileName,fileID);
		fileHandle.Open(bpm,fileID);
		return 0;
	}
	int CloseFile(RM_FileHandle &fileHandle){
		int fileID = fileHandle.getFileID();
		bpm->close();
		myFileManager->closeFile(fileID);
		return 0;
	}
};

#endif
