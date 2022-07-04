To compile all code, type:
```
$ make
```

To run the resulting code on directory test1:
```
$ ./dirstats 5 test1
```

## Test files:
```
$ ./dirstats 5 test1
--------------------------------------------------------------
Largest file:      "test1/dir1/down.png"
Largest file size: 202
Number of files:   7
Number of dirs:    9
Total file size:   253
Most common file types:
  - "ASCII text" x 5
  - "PNG image data" x 1
  - "empty" x 1
Most common words:
  - "contents" x 4
  - "test" x 2
  - "ihdr" x 1
  - "exdu" x 1
  - "png" x 1
Duplicate files - group 1:
  - "test1/a/b/c/d/f"
  - "test1/dir1/file2.txt"
  - "test1/dir1/file1.txt"
--------------------------------------------------------------

$ ./dirstats 5 test4
--------------------------------------------------------------
Largest file:      "test4/words.txt"
Largest file size: 71
Number of files:   1
Number of dirs:    0
Total file size:   71
Most common file types:
  - "ASCII text" x 1
Most common words:
  - "word" x 5
  - "newline" x 1
  - "without" x 1
  - "last" x 1
  - "wordone" x 1
--------------------------------------------------------------

$ ./dirstats 5 test5
--------------------------------------------------------------
Largest file:      "test5/f1.txt"
Largest file size: 29
Number of files:   1
Number of dirs:    0
Total file size:   29
Most common file types:
  - "ASCII text" x 1
Most common words:
  - "abcd" x 6
--------------------------------------------------------------



