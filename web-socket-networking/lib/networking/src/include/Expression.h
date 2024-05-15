// #include "Environment.h"
// #include <any>
// #include <memory>
// #include <vector>

// /*
// An expression is a unit of work that can be evaluated. Subtypes of the interface implement different behavior.
// An expression returns a std::any which is able to hold any type of value or nothing at all.
// */

// class Expression {
// public:
//     virtual std::any evaluate(Environment& e) = 0;
// };





// // Control --------------------------------------------------------------------------------------------------------------

// // Executes its body until its condition evaluates as false
// class WhileLoopExp : public Expression {
// public:
//     WhileLoopExp(std::vector<std::unique_ptr<Expression>> body, std::unique_ptr<Expression> condition) :
//         body{std::move(body)},
//         condition{std::move(condition)} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::vector<std::unique_ptr<Expression>> body;
//     std::unique_ptr<Expression> condition;
// };



// // Executes its body if its condition is true
// class IfExp : public Expression {
// public:
//     IfExp(std::vector<std::unique_ptr<Expression>> body, std::unique_ptr<Expression> condition) :
//         body{std::move(body)},
//         condition{std::move(condition)} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::vector<std::unique_ptr<Expression>> body;
//     std::unique_ptr<Expression> condition;
// };



// // Enters a new scope (for entering blocks)
// class ScopeChangeExp : public Expression {
// public:
//     ScopeChangeExp(std::vector<std::unique_ptr<Expression>> body) : 
//         body{std::move(body)} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::vector<std::unique_ptr<Expression>> body;
// };





// // Evaluaters --------------------------------------------------------------------------------------------------------------

// // Evaluates a logical binary expression, returns a bool
// class LogicalBinaryExp : public Expression {
// public:
//     enum OP {
//         AND,
//         OR
//     };

//     LogicalBinaryExp(std::unique_ptr<Expression> leftChild, std::unique_ptr<Expression> rightChild, OP op) : 
//         leftChild{std::move(leftChild)}, 
//         rightChild{std::move(rightChild)},
//         binaryOp{op} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::unique_ptr<Expression> leftChild;
//     std::unique_ptr<Expression> rightChild;
//     OP binaryOp;
// };



// // An artihmetic binary expression
// class ArithmeticBinaryExp : public Expression {
// public:
//     enum OP {
//         ADDITION,
//         SUBTRACTION,
//         MULTIPLICATION,
//         DIVISION
//     };
    
//     ArithmeticBinaryExp(std::unique_ptr<Expression> leftChild, std::unique_ptr<Expression> rightChild, OP op) : 
//         leftChild{std::move(leftChild)}, 
//         rightChild{std::move(rightChild)}, 
//         binaryOp{op} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::unique_ptr<Expression> leftChild;
//     std::unique_ptr<Expression> rightChild;
//     OP binaryOp;
// };



// // A comparison binary expression
// class ComparisonBinaryExp : public Expression {
// public:
//     enum OP {
//         EQUALITY,       // ==
//         LEQ,            // <=
//         GEQ,            // >=
//         LESS_THAN,      // <
//         GREATER_THAN,   // >
//         NEQ             // !=
//     };
    
//     ComparisonBinaryExp(std::unique_ptr<Expression> leftChild, std::unique_ptr<Expression> rightChild, OP op) : 
//         leftChild{std::move(leftChild)}, 
//         rightChild{std::move(rightChild)}, 
//         binaryOp{op} {}

//     std::any evaluate(Environment& e) override;

// private:
//     std::unique_ptr<Expression> leftChild;
//     std::unique_ptr<Expression> rightChild;
//     OP binaryOp;
// };





// // Storage types ------------------------------------------------------------------------------------------------------

// // A stored ltieral value
// class LiteralExp : public Expression {
// public:
//     LiteralExp(std::any v) : 
//         value{v} {}
    
//     std::any evaluate(Environment& e) override;

// private:
//     std::any value;
// };



// // Create a variable (empty name)
// class VariableDeclarationExp : public Expression {
// public:
//     VariableDeclarationExp(std::string n) : 
//         name{n} {}
    
//     std::any evaluate(Environment& e) override;

// private:
//     std::string name;
// };



// // Assign a value to a variable
// class VariableAssignmentExp : public Expression {
// public:
//     VariableAssignmentExp(std::string n, std::unique_ptr<Expression> c) : 
//         name{n},
//         child{std::move(c)} {}
    
//     std::any evaluate(Environment& e) override;

// private:
//     std::string name;
//     std::unique_ptr<Expression> child;
// };



// // Access a variable that depends upon its value in the environemnt
// class VariableAccessExp : public Expression {
// public:
//     VariableAccessExp(std::string n) : 
//         name{n} {}
    
//     std::any evaluate(Environment& e) override;

// private:
//     std::string name;
// };





// // Function Calls ---------------------------------------------------------------------------------------------------

// // Prints the value of its child
// class PrintExp : public Expression {
// public:
//     PrintExp(std::unique_ptr<Expression> child) : 
//         child{std::move(child)} {}
    
//     std::any evaluate(Environment& e) override;

// private:
//     std::unique_ptr<Expression> child;
// };