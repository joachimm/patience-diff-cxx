# C++ Patience Diff
The Patience Diff in C++14
##Algorithm
The Patience Diff algorithm is best explained by [Alfedenzo][] and [Bram Cohen][] who invented the algorithm. My additions are to try to make the diff structure updateable. This is work in progress.
##Build
Build:

	c++ -std=c++1y -stdlib=libc++ src/*.cc -o diff
## Run
The following examples use the example files in the root folder. I found them on [Alfedenzo][] page, they seem to be used in various places when discussing diff algorithms so I thought I would continue the tradition. I added line endings on the last row to get around an issue with c++ std::getline. The main.cc binary is a test-bed for developing the algorithm and should probably not be used as a diff tool for any other purpose.

	./diff file1.txt file2.txt
### Incremental
It is possible to update the first file after the fact. The format is the following:

	operator:point:sourceFile:start:length

operator is + for add, - for remove. sourceFile is the file frow which the added/removd lines are picked. start is the line position whre to read in the source file, and length is the amount of lines.
Be careful when using the - operator. The line picked from the source need to be the same as in the structure. Remember that the relative insertion point will change with each add/remove.

	./diff file1.txt file1.txt -:2:file1.txt:2:1 -:23:file1.txt:24:1 +:23:file2.txt:23:1
	
## Future Work
The Patience Diff works by finding lines that are equal and only exist once in both files. Such a sequence of line-index pairs is compiled. The Patience Sort algorithm is then applied to this sequence of equal pairs, to find the longest sequence of lines that maintains order in both files. Maintains order, means that line 1 comes before line 3, but does not mean that all unique across both files will be part of the resulting sequence.

This algorithm is then recursively performed between the lines in the sequence, with the hope of yielding pairs that are now unique due to the more limited range. If no new ranges are found, try to find identical but no unique pairs by comparing line by line from the front or back.

Suffice to say the algorithm performs a lot of recursion. However in the case where a diff is updated, it should be possible to limit recursion substantially.

What to look for?
Vocabulary: modification = position/line-index and length of an insertion/deletion.

1. We know that an modification has been performed on only one of the files. The diff needs to be updated between modifications for this to remain true.
2. We know that all lines around this modification range are unchanged. If not the modification would have happened somewhere else!
3. We know that the un-manipulated file is un-manipulated! This means that all its line-indexes still represent a distinct line in that file.

How does this help us?

During the first level of the recursion, the sequence of lines yielded could be saved.
The next time the diff structure is updated with a modification. Again during the first level of recursion a comparison could be made.

Compare the sequences of the two files using the line-index of the un-modified file. All ranges with unchanged line-index is identical to the previous update of the structure, with the exception of the range containing the manipulation. Of course to find that range the line-index of the _modified_ file should be used. It's probably not worth it to use anything other than a prefix/suffix comparison.

Example:

	[(file1-line-index:file2-line-index), ...]
	[(1:2),(3:7),(5:9)]

Lets say we have an insertion of 2 lines at line-index 4 in file1. Lets pretend that this insertion yield the following sequence for the first iteration.

	[(1:2),(3:7),(4:8),(7:9)]

(5:9) and (7:9) represent the same line due to rule 1 and 2, file2 was unmodified and the line-index are the same. So no need to check from (7:9) to last line-index.
We can also skip recursing between first line-index and (3:7).

Only recursion between (3:7),(4:8),(7:9) for correct result.

Lets say that the insertion did not contain any uniques and the first recursion returned the same sequence as the previous update of the structure.

	[(1:2),(3:7),(5:9)]

There would still be a need to recurse between (3:7),(5:9), since thats the range where the modification happened.


## License
GPLv3 due to inclusion of basic_tree.h from the TextMate source.

[Alfedenzo]:	http://alfedenzo.livejournal.com/170301.html
[Bram Cohen]: http://bramcohen.livejournal.com/73318.html
