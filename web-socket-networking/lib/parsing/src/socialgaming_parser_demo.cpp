#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

#include <cpp-tree-sitter.h>


#include "GameLogic.h"


extern "C" {
TSLanguage* tree_sitter_json();
TSLanguage* tree_sitter_socialgaming();
}


//Helper function to read a file and return its text as a string
std::string readFile(const std::string& filename) {
    // Open input file stream for file
    std::ifstream file = std::ifstream(filename);

    if (!file.is_open()) {
        //if file can't be opened, throw error
        std::cerr << "Error: Couldn't open file " << filename << std::endl;
        return "";
    }
    // Otherwise, read the file into a string
    std::string fileText;
    std::string line;
    //get the file line by line, adding newlines at end of each.
    while (std::getline(file, line)) {
        fileText += line + "\n"; 
    }
    file.close(); // tidy up!

    return fileText;
}

///--------------------- interpreter prototype helper functions -----------------------------------------------------------------------
// This is a helper to add a variable declaration and initialization to a list of expressions
void declareVariableWithValue(std::vector<std::unique_ptr<Expression>>& expList, std::string name, std::any value) {
    auto initVariable = std::make_unique<VariableDeclarationExp>(name);
    expList.push_back(std::move(initVariable));
    auto initLiteral = std::make_unique<LiteralExp>(value);
    auto setInitValue = std::make_unique<VariableAssignmentExp>(name, std::move(initLiteral));
    expList.push_back(std::move(setInitValue));
}

// This is helper that just returns an expression to get the value of a variable
std::unique_ptr<Expression> getVariableValue(std::string name) {
    auto variableAccess = std::make_unique<VariableAccessExp>(name);
    return std::move(variableAccess);
}

///--------------------------------------------------------------------------------------------------
int main() {
    // Create a language and parser.
    ts::Language language = tree_sitter_socialgaming();
    ts::Parser parser{language};
    //Get the rules from a .game file and store in a string
    std::string sourcecode = readFile("rockpaperscissors.game");
    // Parse the string into a syntax tree:
    ts::Tree tree  = parser.parseString(sourcecode);
    // Get the root node of the syntax tree.
    ts::Node root = tree.getRootNode();


    GameLogic gameLogic(root, sourcecode);
    // std::cout << gameLogic.getConfiguration().getGameName() << std::endl;

    // std::cout << gameLogic.getConfiguration().getPlayerRange()[0] << std::endl;
    // std::cout << gameLogic.getConfiguration().getPlayerRange()[1] << std::endl;

    // std::cout << gameLogic.getConfiguration().getHasAudience() << std::endl;


    // std::cout << gameLogic.getConfiguration().getSetups()[0].toString();
    // std::cout << gameLogic.getConfiguration().getSetups()[1].toString();
    // std::cout << gameLogic.getConfiguration().getSetups()[2].toString();
    // std::cout << gameLogic.getConstants().toString() << std::endl;
    // std::cout << gameLogic.getVariables().toString() << std::endl;

    ///======================================= interpreter prototype ======================================================
    std::cout << "\n======================================= interpreter prototype ==================================================\n"; 
    std::vector<std::unique_ptr<Expression>> expressions; // These are the expressions that will be executed in order
    
    // Print the result of 3 + 4 - 2 -------------------------------------------------------------------------------------------------------
    auto three =    std::make_unique<LiteralExp>(3); // Create two literal values. These are leaf nodes of the trees.
    auto four =     std::make_unique<LiteralExp>(4);
    declareVariableWithValue(expressions, "two", 2);

    auto plus =     std::make_unique<ArithmeticBinaryExp>(std::move(three), std::move(four), ArithmeticBinaryExp::OP::ADDITION); // Create an addition expression that adds the two literals created earlier.
    
    // Create a subtraction expression that adds the result of the addition and the variable two.
    auto subtract = std::make_unique<ArithmeticBinaryExp>(std::move(plus),  getVariableValue("two"),  ArithmeticBinaryExp::OP::SUBTRACTION); 

    auto print1 =   std::make_unique<PrintExp>(std::move(subtract)); // Print out the result of the subtraction
    expressions.push_back(std::move(print1));



    // Print the result of 5 * 5 * 5 -------------------------------------------------------------------------------------------------------
    auto five1 =     std::make_unique<LiteralExp>(5); // Create three different literals with the value 5.
    auto five2 =     std::make_unique<LiteralExp>(5);
    auto five3 =     std::make_unique<LiteralExp>(5);

    auto multiply1 = std::make_unique<ArithmeticBinaryExp>(std::move(five1),     std::move(five2), ArithmeticBinaryExp::OP::MULTIPLICATION); // Multiple two of the fives
    auto multiply2 = std::make_unique<ArithmeticBinaryExp>(std::move(multiply1), std::move(five3), ArithmeticBinaryExp::OP::MULTIPLICATION); // Multiply the result of the first multiplication with the third 5

    auto print2 =    std::make_unique<PrintExp>(std::move(multiply2)); // Print the result
    expressions.push_back(std::move(print2));



    // Print 10 fibonacci numbers ------------------------------------------------------------------------------------------------

    // Names to prevent typos
    std::string const loopCounter = "loopCounter";
    std::string const term0 = "term0";
    std::string const term1 = "term1";
    std::string const term2 = "term2";

    // Init loop variables, this uses a helper to make a literal, declare a variable then assign the literal variable to the variable
    declareVariableWithValue(expressions, term0, 0);
    declareVariableWithValue(expressions, term1, 1);
    declareVariableWithValue(expressions, term2, 0);
    declareVariableWithValue(expressions, loopCounter, 1);

    // Loop body ---------------------
    std::vector<std::unique_ptr<Expression>> fibLoopBody;

    // Add i-1th and i-2th terms
    // Get variable value is a helper to creates a Variable access expression
    auto termAddition = std::make_unique<ArithmeticBinaryExp>(getVariableValue(term0), getVariableValue(term1), ArithmeticBinaryExp::OP::ADDITION); 
    
    // Store the result to the ith term
    auto updateIthTerm = std::make_unique<VariableAssignmentExp>(term2, std::move(termAddition));
    fibLoopBody.push_back(std::move(updateIthTerm));

    // Update i-1th and i-2th terms
    auto updateI1thTerm = std::make_unique<VariableAssignmentExp>(term0, getVariableValue(term1));
    fibLoopBody.push_back(std::move(updateI1thTerm));
    auto updateI2thTerm = std::make_unique<VariableAssignmentExp>(term1, getVariableValue(term2));
    fibLoopBody.push_back(std::move(updateI2thTerm));

    // Print the value
    auto printFib = std::make_unique<PrintExp>(getVariableValue(term2));
    fibLoopBody.push_back(std::move(printFib));

    // Increament the loop counter
    auto oneLiteral = std::make_unique<LiteralExp>(std::make_any<int>(1));
    auto counterAddition = std::make_unique<ArithmeticBinaryExp>(getVariableValue(loopCounter), std::move(oneLiteral), ArithmeticBinaryExp::OP::ADDITION);
    auto updateCounter = std::make_unique<VariableAssignmentExp>(loopCounter, std::move(counterAddition));
    fibLoopBody.push_back(std::move(updateCounter));
    // -------------------------------

    // Continuation condition of loop
    auto loopExitValue = std::make_unique<LiteralExp>(std::make_any<int>(10));
    auto continuationCondition = std::make_unique<ComparisonBinaryExp>(getVariableValue(loopCounter), std::move(loopExitValue), ComparisonBinaryExp::OP::LEQ);

    // Declare the loop
    auto whileLoop = std::make_unique<WhileLoopExp>(std::move(fibLoopBody), std::move(continuationCondition));
    expressions.push_back(std::move(whileLoop));


    // Create and run the game ---------------------------------------------------------------------------------------------------------------
    
    Environment env(nullptr);
    //game.setTopLevelExpressions(std::move(expressions)); 
    gameLogic.setGlobalEnv(env); 
    gameLogic.run();



    ///============================== parse message =====================================================
    // auto rulesNode = root.getChildByFieldName("rules"); 
    // auto rulesBodyNode = rulesNode.getChildByFieldName("body");
    // for(uint32_t i=0; i < rulesBodyNode.getNumChildren(); i++) {
    //   auto child = rulesBodyNode.getChild(i);
    //   if(child.getType() == "rule") {
    //     PrintHelper::printFieldInfo(child, sourcecode);
    //     PrintHelper::printChildrenInfo(child, sourcecode);
    //   }
    // }



    return 0;
}
