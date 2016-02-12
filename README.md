# spellchecker
An university project containing a library implementing a spellchecker - the software is supposed to create a real-life dictionary, stored in a TRIE tree. It also provides methods to save(read) our compressed dictionary in(from) a file.

Developed using pure C with CMake for building, plus a cmocka library for unit testing, and also doxygen documentation (in polish; english version in progress).

It also contains a simple GTK text editor (written for linux, not made by me), which uses the library's methods and provides a real-life testing enviroment.

Simply compile it using make; make tests requires that you have cmocka installed, you can download it from here: https://cmocka.org/

make doc creates a full doxygen documentation.

In the future, the code will be refactorized to be completly in english - so far, many files, including the whole documentation, is written in polish (my native language).
