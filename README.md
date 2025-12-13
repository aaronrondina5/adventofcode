# adventofcode problems
[adventofcode.com](https://adventofcode.com)

## Setup
1. Install g++ (C++ compiler):
   - On macOS: `brew install gcc`
   - On Ubuntu: `sudo apt install g++`

## Running
Add your solution to `main.cpp`.

### From Command Line
```
mkdir -p ./build && g++-15 -std=c++23 main.cpp -o ./build/${fileBasenameNoExtension}.tsk && ./build/${fileBasenameNoExtension}.tsk < ./io/input0.txt > ./io/output0.txt
```

### From VSCode:
Terminal > Run Task > Execute Task > Execute main
