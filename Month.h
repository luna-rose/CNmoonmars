#ifndef __MONTH__
#define __MONTH__


class Month {
public:
	Month();
	Month(char* str);
	
	void Set(char* str);
	
	enum MonthEnum { Invalid = 0,
		Jan = 1, Feb = 2,  Mar = 3,  Apr = 4, 
		May = 5, Jun = 6,  Jul = 7,  Aug = 8, 
		Sep = 9, Oct = 10, Nov = 11, Dec = 12 } value;
};


#endif