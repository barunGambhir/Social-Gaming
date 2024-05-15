/*
An expression produces SGData for consumption by a statement.
*/

#pragma once
#include "SGData.h"
#include "Environment.h"

class Expression {
public:
    Expression() = default;
    Expression(const Expression&) = delete;
    Expression(Expression&&) = delete;
    Expression& operator=(const Expression&) = delete;
    Expression& operator=(Expression&&) = delete;
    virtual ~Expression() = default;
    
    virtual SGData evaluate(Environment&) = 0;
};
using ExprUptr = std::unique_ptr<Expression>;



/*---------------------------------------------------------------------------------------------------
||                                           Evaluaters                                            ||
---------------------------------------------------------------------------------------------------*/

class LogicalBinaryExp final : public Expression {
public:
    enum Operator {
        AND,
        OR
    };

    LogicalBinaryExp(ExprUptr leftChild, ExprUptr rightChild, Operator op) : 
        leftChild{std::move(leftChild)}, 
        rightChild{std::move(rightChild)},
        operation{op} 
        {}

    SGData evaluate(Environment&) override;

private:
    ExprUptr leftChild;
    ExprUptr rightChild;
    Operator operation;
};



class ArithmeticBinaryExp final : public Expression {
public:
    enum Operator {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
        DIVISION
    };
    
    ArithmeticBinaryExp(ExprUptr leftChild, ExprUptr rightChild, Operator op) : 
        leftChild{std::move(leftChild)}, 
        rightChild{std::move(rightChild)}, 
        operation{op} 
        {}

    SGData evaluate(Environment&) override;    

private:
    ExprUptr leftChild;
    ExprUptr rightChild;
    Operator operation;
};



class ComparisonBinaryExp final : public Expression {
public:
    enum Operator {
        EQUALITY,       // ==
        LEQ,            // <=
        GEQ,            // >=
        LESS_THAN,      // <
        GREATER_THAN,   // >
        NEQ             // !=
    };
    
    ComparisonBinaryExp(ExprUptr leftChild, ExprUptr rightChild, Operator op) : 
        leftChild{std::move(leftChild)}, 
        rightChild{std::move(rightChild)}, 
        operation{op} 
        {}

    SGData evaluate(Environment&) override;

private:
    ExprUptr leftChild;
    ExprUptr rightChild;
    Operator operation;
};

/*---------------------------------------------------------------------------------------------------
||                                         List Operations                                         ||
---------------------------------------------------------------------------------------------------*/

class UpFromExp final : public Expression {
public:
    UpFromExp(ExprUptr max, ExprUptr min) :
        max(std::move(max)),
        min(std::move(min))
        {}

    SGData evaluate(Environment&) override;

private:
    ExprUptr max;
    ExprUptr min;
};



class ListContainsExp final : public Expression {
public:
    ListContainsExp(ExprUptr list, ExprUptr contains) : 
        list{std::move(list)},
        contains{std::move(contains)}
        {}

    SGData evaluate(Environment&) override;
    
private:
    ExprUptr list;
    ExprUptr contains;
};



class ListSizeExp final : public Expression {
public:
    ListSizeExp(ExprUptr list) : 
        list{std::move(list)} 
        {}

    SGData evaluate(Environment&) override;
    
private:
    ExprUptr list;
};



class ListCollectExp final : public Expression {
public:
    ListCollectExp(ExprUptr outerList, ExprUptr innerList, ExprUptr condition) : 
        outerList(std::move(outerList)),
        innerList(std::move(innerList)),
        condition(std::move(condition))
        {}

    SGData evaluate(Environment&) override;
    
private:
    ExprUptr outerList;
    ExprUptr innerList;
    ExprUptr condition;
};


/*---------------------------------------------------------------------------------------------------
||                                             Storage                                             ||
---------------------------------------------------------------------------------------------------*/

class LiteralExp final : public Expression {
public:
    LiteralExp(SGData v) : 
        value{v} 
        {}

    SGData evaluate(Environment&) override;
    
private:
    SGData value;
};



// Create a variable (empty name)
class VariableDeclarationExp final : public Expression {
public:
    VariableDeclarationExp(std::string n) : 
        name{n} 
        {}
    
    SGData evaluate(Environment&) override;

private:
    std::string name;
};



class VariableAccessExp final : public Expression {
public:
    VariableAccessExp(std::string n) : 
        name{n} 
        {}
    
    SGData evaluate(Environment&) override;

private:
    std::string name;
};



class DotAccessExp final : public Expression {
public:
    DotAccessExp(ExprUptr left, std::string right) : 
        left(std::move(left)),
        right(right)
        {}

    SGData evaluate(Environment&) override;

private:
    ExprUptr left;
    std::string right;    
};