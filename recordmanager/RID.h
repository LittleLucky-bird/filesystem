#ifndef RID_H_
#define RID_H_

class RID {
public:
	RID() : page(-1), slot(-1) {}     // Default constructor
	RID(int pageNum, int slotNum) : page(pageNum), slot(slotNum) {}
	~RID(){}                                        // Destructor

	int GetPageNum(int &pageNum) const          // Return page number
	{ pageNum = page; return 0; }
	int GetSlotNum(int &slotNum) const         // Return slot number
	{ slotNum = slot; return 0; }

	int Page() const          // Return page number
	{ return page; }
	int Slot() const          // Return slot number
	{ return slot; }
	void Copy(RID _rid){
	  int Page = 0;
	  int Slot = 0;
          _rid.GetPageNum(Page);
	  _rid.GetSlotNum(Slot);
	  this->page = Page;
	  this->slot = Slot;
	}
	bool operator==(const RID & rhs) const
	{
		int p;
		int s;
		rhs.GetPageNum(p);
		rhs.GetSlotNum(s);
		return (p == page && s == slot);
	}

private:
	int page;
	int slot;
};

#endif
