#Ben's guide on how to run the parser demo found at https://github.com/nsumner/cpp-tree-sitter:
1) From the social-gaming/parser-example directory, run

source /usr/shared/CMPT/faculty/wsumner/base/env373/bin/activate      (only run this line if you're in csil)
- mkdir build
- cd build
- cmake ..
- make
- ./demo

If everything is working, Expect the demo to read in rockPaperScissors and print it as a tree

code logic to explore is in socialgaming_parser_demo.cpp, 
TODO: feel free to try and access the inner nodes like configuration!

After source code changes: 
Rerun *make* to recompile.
