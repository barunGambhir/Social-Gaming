# 1. Document purpose

The purpose of this document is to give the reader a high level overview of our interpreter. As such, it leaves some lower level implementation details undetermined such as exact class structure, methods, data structures, etc.

# 2. What even is an interpreter?

A computer only understands one programming language, its specific machine language. As you probably found out in 295, writing in this language is difficult which is why we have programming languages like Python, Java or even C. However, this introduces a new problem: how do we translate from a programming language to a machine language? That is the job of interpreters and compilers.

## Compilers vs. Interpreters

- A compiler is a program that translates code from one language to another language.
- An interpreter runs code in a (non-machine) language. It may compile code as part of this process, but doesn’t have to. The input is code but there is no output, the program is simply run.

In a compiler, the input is code and so is the output. In an interpreter the input is code but there is no output, the program is simply run.

## What are we making?

We are making an interpreter for SocialGaming. The interpreter is written in C++. The interpreter will understand and execute SocialGaming programs.


# 3. Our interpreter’s structure

Interpreters and compilers generally use a pipeline architecture. Our interpreter will be no different. The initial input is a text file in SocialGaming. The text file is effectively an ordered list of individual characters. Each stage transforms it into a more useful format for the next stage.

## TreeSitter

TreeSitter is a general purpose program that accomplishes two of the stages in our pipeline. You provide it with a grammar which defines the structure of the language. It then produces a program to “understand” that grammar. The grammar for SocialGaming can be found [here](https://github.com/nsumner/tree-sitter-socialgaming/blob/main/grammar.js).

### Scanning
This stage’s purpose is to group characters into tokens. You can think of a token as a basic unit that cannot be split any further while still maintaining meaning. They might be a single character such as ```=```, ```+``` or ```(```. They could be longer such as ```if```, ```int``` or ```while```. They could even be user defined such as in the case of variables or functions, for instance, ```calculateTax``` or ```salesPrice```.

**Input:** A list of characters.
![Image of list of characters](/images/character_list.jpg)

**Output:** A list of tokens.
![Image of list of tokens](/images/token_list.jpg)


### Parsing

This stage’s purpose is to organize the tokens into a tree. This tree represents the nested structure of the language. For instance, a single statement may be composed of multiple expressions, which are themselves composed of expressions, literals, variables, etc. This also is the stage where any syntax errors will be detected.

**Input:** A list of tokens.
![Image of list of tokens](/images/token_list.jpg)

**Output:** A syntax tree.
![Image of syntax tree](/images/syntax_tree.jpg)


## Game Builder

The syntax tree is still not very useful to us. While it shows us the structure of the language, it doesn’t carry any information about the semantics. In other words, it doesn’t tell us how to execute the code. This stage will traverse the syntax tree and output a game.

### Game
The game object will be responsible for storing the data concerning the game. This will be data defined at creation such as the rules for the game and constant values. It will also store information about the current state of the game such as whose turn it is, player’s scores, etc.

### Rules
The game will store a list of rules. The rules are the behaviors that will actually be executed when the game is run. They are in the same tree organization as output by TreeSitter. However, they are now nodes which capture their semantics (their behavior).

### Nodes
A node can be evaluated. Based on the type of node it is, it will do so in different ways. They may evaluate other nodes, return values or produce side effects. For instance:
- A node corresponding to a literal in the syntax tree will simply return that literal.
- A node containing a variable will return its current value in the game.
- An addition node will evaluate its two children, add their values together, then return the result.
- A variable assignment node will evaluate its expression then set its variable’s value to the result of the expression.
- An if statement node will evaluate a boolean expression and then evaluate its child node if that expression is true.
- A while loop will repeatedly evaluate its child until its boolean expression child node evaluates as false.

**Input:** A syntax tree.
**Output:** A game.

## Interpreter

The interpreter is very simple once all the above work has been done. It just needs to call evaluate on each of the rules stored in the game. Each node will then carry out the work it has been assigned to do. The interpreter will need to handle any runtime errors that occur during execution, such as an operation done on an invalid type.

**Input:** A game.
