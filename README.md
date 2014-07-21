#C++ Patience Diff
The Patience Diff in C++14
##Algorithm
The Patience Diff algorithm is best explained by [Alfedenzo][] and [Bram Cohen][] who invented the algorithm. My additions are to try to make the diff structure updateable. This is work in progress.
##Build
Build:

	c++ -std=c++1y -stdlib=libc++ src/*.cc -o diff
##Run
The following examples use the example files in the root folder. I found them on [Alfedenzo][] page, they seem to be used in various places when discussing diff algorithms so I thought I would continue the tradition. I added line endings on the last row to get around an issue with c++ std::getline. The main.cc binary is a test-bed for developing the algorithm and should probably not be used as a diff tool for any other purpose.

	./diff file1.txt file2.txt
###Incremental
It is possible to update the first file after the fact. The format is the following:

	operator:point:sourceFile:start:length

operator is + for add, - for remove. sourceFile is the file frow which the added/removd lines are picked. start is the line position whre to read in the source file, and length is the amount of lines.
Be careful when using the - operator. The line picked from the source need to be the same as in the structure. Remember that the relative insertion point will change with each add/remove.

	./diff file1.txt file1.txt -:2:file1.txt:2:1 -:23:file1.txt:24:1 +:23:file2.txt:23:1
##License
GPLv3 due to inclusion of basic_tree.h from the TextMate source.

[Alfedenzo]:	http://alfedenzo.livejournal.com/170301.html
[Bram Cohen]: http://bramcohen.livejournal.com/73318.html