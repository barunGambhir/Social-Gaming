#include "Expression.h"
#include "GameLogic.h"
#include <iostream>
#include <string>
#include <regex>

// This is a barebones implementation. Will need to do type checking, exception throwing and more complex behavior later.

// Control --------------------------------------------------------------------------------------------------------------

std::any WhileLoopExp::evaluate(Environment& e, GameLogic& game) {
    while (std::any_cast<bool>(condition->evaluate(e, game))) {
        for (auto& exp : body) {
            exp->evaluate(e, game);
        }
    }
    
    return std::any();
}



std::any IfExp::evaluate(Environment& e, GameLogic& game) {
    if (std::any_cast<bool>(condition->evaluate(e, game))) {
        for (auto& exp : body) {
            exp->evaluate(e, game);
        }
    }
    
    return std::any();
}



std::any ScopeChangeExp::evaluate(Environment& e, GameLogic& game) {
    Environment enclosed(&e); // Create a new environment on the stack enclosed by the original

    for (auto& exp : body) {
        exp->evaluate(enclosed, game);
    }
    
    return std::any();
}





// Evaluaters --------------------------------------------------------------------------------------------------------------

std::any LogicalBinaryExp::evaluate(Environment& e, GameLogic& game) {
    std::any leftVal =  leftChild->evaluate(e, game);
    std::any rightVal = rightChild->evaluate(e, game);

    switch(binaryOp) {
        case AND:
            return std::any_cast<bool>(leftVal) && std::any_cast<bool>(rightVal);
            break;
        case OR:
            return std::any_cast<bool>(leftVal) || std::any_cast<bool>(rightVal);
            break;
    }

    return std::any(); // Unreachable
}



std::any ArithmeticBinaryExp::evaluate(Environment& e, GameLogic& game) {
    std::any leftVal =  leftChild->evaluate(e, game);
    std::any rightVal = rightChild->evaluate(e, game);
    
    switch(binaryOp) {
        case ADDITION:
            return std::any_cast<int>(leftVal) + std::any_cast<int>(rightVal);
            break;
        case SUBTRACTION:
            return std::any_cast<int>(leftVal) - std::any_cast<int>(rightVal);
            break;
        case MULTIPLICATION:
            return std::any_cast<int>(leftVal) * std::any_cast<int>(rightVal);
            break;
        case DIVISION:
            return std::any_cast<int>(leftVal) / std::any_cast<int>(rightVal);
            break;
    }

    return std::any(); // Unreachable
}



std::any ComparisonBinaryExp::evaluate(Environment& e, GameLogic& game) {
    std::any leftVal =  leftChild->evaluate(e, game);
    std::any rightVal = rightChild->evaluate(e, game);
    
    switch(binaryOp) {
        case EQUALITY:
            return std::any_cast<int>(leftVal) == std::any_cast<int>(rightVal);
            break;
        case LEQ:
            return std::any_cast<int>(leftVal) <= std::any_cast<int>(rightVal);
            break;
        case GEQ:
            return std::any_cast<int>(leftVal) >= std::any_cast<int>(rightVal);
            break;
        case LESS_THAN:
            return std::any_cast<int>(leftVal) < std::any_cast<int>(rightVal);
            break;
        case GREATER_THAN:
            return std::any_cast<int>(leftVal) > std::any_cast<int>(rightVal);
            break;
        case NEQ:
            return std::any_cast<int>(leftVal) != std::any_cast<int>(rightVal);
            break;
    }

    return std::any(); // Unreachable
}





// Storage types --------------------------------------------------------------------------------------------

std::any LiteralExp::evaluate(Environment& e, GameLogic& game) {
    return value;
}



std::any VariableDeclarationExp::evaluate(Environment& e, GameLogic& game) {
    e.init(name);
    return std::any();
}



std::any VariableAssignmentExp::evaluate(Environment& e, GameLogic& game) {
    e.set(name, child->evaluate(e, game));
    return std::any();
}



std::any VariableAccessExp::evaluate(Environment& e, GameLogic& game) {
    return e.get(name);
}





// Function Calls ---------------------------------------------------------------------------------------------------

std::any PrintExp::evaluate(Environment& e, GameLogic& game) {
    std::cout << std::any_cast<int>(child->evaluate(e, game)) << std::endl; // Need to add type handling, example on https://en.cppreference.com/w/cpp/utility/any/type 
    return std::any();
}


std::any MessageExp::evaluate(Environment& e, GameLogic& game) {
    std::string rawMes = std::string(this->message);

    std::regex regex("\\{([^\\}]+)\\}");
    std::sregex_iterator it(rawMes.begin(), rawMes.end(), regex);
    std::sregex_iterator end;

    std::string finalMes;
    size_t lastPos = 0;

    while (it != end) {
        std::smatch match = *it;
        finalMes += rawMes.substr(lastPos, match.position() - lastPos);

        std::string placeholder = match[1].str();
        finalMes += game.getStateAsString(placeholder);

        lastPos = match.position() + match.length();
        ++it;
    }

    finalMes += rawMes.substr(lastPos);

    // Output the modified string
    std::cout << finalMes << std::endl;
    //TODO: replace cout with result << so it prints to Game browser

    return std::any();
}


std::any DiscardExp::evaluate(Environment& e, GameLogic& game) {
    std::cout << "\ndiscard what:\n"; 
    discardWhat.print(); 

    std::cout << "discard from:\n"; 
    discardFrom.print(); 
    return std::any();
}

std::any ScoresExp::evaluate(Environment& e, GameLogic& game) {
    std::cout << "\nScores expr detected!\n";
    for(auto i : expressionList) {
        i.print(); 
    }
    return std::any(); 
}




ForExp::ForExp(std::string var, int min, int max) {
    loopVarName = var;
    from = min;
    to = max;
    // this->rules = std::move(rules);
}

std::any ForExp::evaluate(Environment& e, GameLogic& game) {
    std::cout << "\nfor expression detected!\n";
    std::cout << "Loop variable: " << loopVarName << std::endl;
    std::cout << "Loop from: " << from << std::endl;
    std::cout << "Loop to: " << to << std::endl;

    //this part below does not work yet because no nested rules are put into for exp:
    for (auto const& expression : rules) {
        expression->evaluate(game.getEnvironment(), game);
        // Will add try/catch to handle SocialGaming runtime exceptions wihtout crashing the entire program.
    }
    return std::any();
}