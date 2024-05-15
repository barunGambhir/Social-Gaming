#include <memory>
#include <string>
#include <iostream>
#include "Expression.h"

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

int main() {
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

    return 0;
}