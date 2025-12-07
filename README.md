# adventofcode problems
[adventofcode.com](https://adventofcode.com)

## Setup
1. Install g++ (C++ compiler):
   - On macOS: `brew install gcc`
   - On Ubuntu: `sudo apt install g++`

## Running
Add your solution to `A.cpp`.

### From Command Line
```
mkdir -p ${fileDirname}/build && g++-15 -std=c++23 A.cpp -o ${fileDirname}/build/${fileBasenameNoExtension}.tsk && ${fileDirname}/build/${fileBasenameNoExtension}.tsk < input.txt > output.txt
```

### From VSCode:
Terminal > Run Task > Execute Task > Execute File A
