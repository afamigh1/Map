SkipList (and RandomHeight) implementation taken and modified from http://www.drdobbs.com/cpp/skip-lists-in-c/184403579?pgno=1

To my knowledge, some functions do not work. Notably the clear() function in map segfaults occassionally.
I commented out lines 173 and 240 as they were crashing my program. Not sure
why that's so as the rest of the insert/erase calls work fine.
I included the test.cpp file and it is just the test1 code with the above
changes made.
