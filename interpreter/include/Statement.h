/*
A statement produces side effects.
*/

#pragma once
#include "Expression.h"
#include "Environment.h"
#include <optional>
#include <memory>
#include <vector>
#include <deque>
#include <string>

class Statement {
public:
    class Resumable {
    public:
        Resumable() : 
            toBeResumed(nullptr) 
            {}
        Resumable(Statement* toBeResumed) : 
            toBeResumed(toBeResumed)
            {}
        
        [[nodiscard]] 
        std::optional<Resumable> resume() {
            if (!toBeResumed) {
                throw new std::runtime_error("Resumable is invalid");
            }
            
            std::optional<Resumable> ret = toBeResumed->resumeImpl();
            toBeResumed = nullptr;
            return ret;
        }

        void abort() {
            if (!toBeResumed) {
                throw new std::runtime_error("Resumable is invalid");
            }
            
            toBeResumed->abortImpl();
        }
    private:
        Statement* toBeResumed;
    };
    friend Resumable;


    Statement() = default;
    Statement(const Statement&) = delete;
    Statement(Statement&&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement& operator=(Statement&&) = delete;
    virtual ~Statement() = default;


    [[nodiscard]] 
    std::optional<Resumable> start(std::shared_ptr<Environment>& env) { return this->startImpl(env); }
    
private:
    [[nodiscard]] 
    virtual std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) = 0;

    [[nodiscard]] 
    virtual std::optional<Resumable> resumeImpl() = 0;

    virtual void abortImpl() = 0;
};
using StmtUptr = std::unique_ptr<Statement>;





/*---------------------------------------------------------------------------------------------------
||                                              Loops                                              ||
---------------------------------------------------------------------------------------------------*/

class ForEachStmt final : public Statement {
public:
    ForEachStmt(std::string freshVarName, ExprUptr list, std::vector<StmtUptr> loopBody) :
        freshVarName(freshVarName),
        list(std::move(list)),
        loopBody(std::move(loopBody))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;
    
private:
    std::string freshVarName;
    ExprUptr list;
    std::vector<StmtUptr> loopBody;
};



class WhileStmt final : public Statement {
public:
    WhileStmt(ExprUptr condition, std::vector<StmtUptr> loopBody) :
        condition(std::move(condition)),
        loopBody(std::move(loopBody))
        {}

    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;
    
private:
    ExprUptr condition;
    std::vector<StmtUptr> loopBody;

    struct State {
        std::shared_ptr<Environment> currentEnv;
        Resumable pausedStatement;
        size_t statementIndex;
    };
    State executionState;
    std::optional<Resumable> executeLoop();
};



class ParallelForEachStmt final : public Statement {
public:
    ParallelForEachStmt(std::string freshVarName, ExprUptr list, std::vector<StmtUptr> loopBody) :
        freshVarName(freshVarName),
        list(std::move(list)),
        loopBody(std::move(loopBody))
        {}

    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;
        
private:
    std::string freshVarName;
    ExprUptr list;
    std::vector<StmtUptr> loopBody;
};

/*---------------------------------------------------------------------------------------------------
||                                         List Operations                                         ||
---------------------------------------------------------------------------------------------------*/

class ExtendStmt final : public Statement {
public:
    ExtendStmt(ExprUptr extendableList, ExprUptr toExtendWith) :
        extendableList(std::move(extendableList)),
        toExtendWith(std::move(toExtendWith))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr extendableList;
    ExprUptr toExtendWith;
};



class ReverseStmt final : public Statement {
public:
    ReverseStmt(ExprUptr list) :
        list(std::move(list))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr list;
};



class ShuffleStmt final : public Statement {
public:
    ShuffleStmt(ExprUptr list) :
        list(std::move(list))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr list;
};



class SortStmt final : public Statement {
public:
    SortStmt(ExprUptr list) :
        list(std::move(list))
        {}
    
    SortStmt(ExprUptr list, std::string key) :
        list(std::move(list)),
        key(key)
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr list;
    std::string key; // Optional
};



class DealStmt final : public Statement {
public:
    DealStmt(ExprUptr numToDeal, ExprUptr playerList, ExprUptr fromList) :
        numToDeal(std::move(numToDeal)),
        playerList(std::move(playerList)),
        fromList(std::move(fromList))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr numToDeal;
    ExprUptr playerList;
    ExprUptr fromList;
};



class DiscardStmt final : public Statement {
public:
    DiscardStmt(ExprUptr numToDiscard, ExprUptr list) :
        numToDiscard(std::move(numToDiscard)),
        list(std::move(list))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr numToDiscard;
    ExprUptr list;
};

/*---------------------------------------------------------------------------------------------------
||                                              Input                                              ||
---------------------------------------------------------------------------------------------------*/

class InputChoiceStmt final : public Statement {
public:
    InputChoiceStmt(std::string targetPlayer, std::string prompt, ExprUptr choiceList, std::string selectionStorage, ExprUptr timeoutLimit) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        choiceList(std::move(choiceList)),
        selectionStorage(selectionStorage),
        timeoutLimit(std::move(timeoutLimit))
        {}

    InputChoiceStmt(std::string targetPlayer, std::string prompt, ExprUptr choiceList, std::string selectionStorage) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        choiceList(std::move(choiceList)),
        selectionStorage(selectionStorage),
        timeoutLimit(nullptr)
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    std::string targetPlayer;
    std::string prompt;
    ExprUptr choiceList;
    std::string selectionStorage;
    ExprUptr timeoutLimit; // Optional
};



class InputTextStmt final : public Statement {
public:
    InputTextStmt(std::string targetPlayer, std::string prompt, std::string selectionStorage, ExprUptr timeoutLimit) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        selectionStorage(selectionStorage),
        timeoutLimit(std::move(timeoutLimit))
        {}
    InputTextStmt(std::string targetPlayer, std::string prompt, std::string selectionStorage) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        selectionStorage(selectionStorage),
        timeoutLimit(nullptr)
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    std::string targetPlayer;
    std::string prompt;
    std::string selectionStorage;
    ExprUptr timeoutLimit; // Optional
};



class InputRangeStmt final : public Statement {
public:
    InputRangeStmt(std::string targetPlayer, std::string prompt, ExprUptr range, std::string selectionStorage, ExprUptr timeoutLimit) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        range(std::move(range)),
        selectionStorage(selectionStorage),
        timeoutLimit(std::move(timeoutLimit))
        {}
    InputRangeStmt(std::string targetPlayer, std::string prompt, ExprUptr range, std::string selectionStorage) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        range(std::move(range)),
        selectionStorage(selectionStorage),
        timeoutLimit(nullptr)
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    std::string targetPlayer;
    std::string prompt;
    ExprUptr range;
    std::string selectionStorage;
    ExprUptr timeoutLimit; // Optional
};



class InputVoteStmt final : public Statement {
public:
    InputVoteStmt(std::string targetPlayer, std::string prompt, ExprUptr choiceList, std::string selectionStorage, ExprUptr timeoutLimit) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        choiceList(std::move(choiceList)),
        selectionStorage(selectionStorage),
        timeoutLimit(std::move(timeoutLimit))
        {}
    InputVoteStmt(std::string targetPlayer, std::string prompt, ExprUptr choiceList, std::string selectionStorage) :
        targetPlayer(targetPlayer),
        prompt(prompt),
        choiceList(std::move(choiceList)),
        selectionStorage(selectionStorage),
        timeoutLimit(nullptr)
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    std::string targetPlayer;
    std::string prompt;
    ExprUptr choiceList;
    std::string  selectionStorage;
    ExprUptr timeoutLimit; // Optional
};

/*---------------------------------------------------------------------------------------------------
||                                              Output                                             ||
---------------------------------------------------------------------------------------------------*/

class MessageStmt final : public Statement {
public:
    MessageStmt(ExprUptr players, ExprUptr message) :
        players(std::move(players)),
        message(std::move(message))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr players;
    ExprUptr message;
};



class ScoreStmt final : public Statement {
public:
    ScoreStmt(ExprUptr keyList) :
        keyList(std::move(keyList))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr keyList;
};

/*---------------------------------------------------------------------------------------------------
||                                               Misc                                              ||
---------------------------------------------------------------------------------------------------*/

class InparallelStmt final : public Statement {
public:
    InparallelStmt(std::vector<StmtUptr> body) : 
        body(std::move(body)) 
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    std::vector<StmtUptr> body;
};



class MatchStmt final : public Statement {
public:
    MatchStmt(ExprUptr toBeMatched, std::vector< std::pair<ExprUptr, std::vector<StmtUptr>> > matches) :
        toBeMatched(std::move(toBeMatched)),
        matches(std::move(matches))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr toBeMatched;
    std::vector< std::pair<ExprUptr, std::vector<StmtUptr>> > matches;
};



class TimerStmt final : public Statement {
public:
    enum TimerPolicy {
        AT_MOST,
        EXACTLY
    };
    
    TimerStmt(ExprUptr timeLimit, TimerPolicy policy, ExprUptr inVar, std::vector<StmtUptr> body) :
        timeLimit(std::move(timeLimit)),
        policy(policy),
        inVar(std::move(inVar)),
        body(std::move(body))
        {}
    
    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

private:
    ExprUptr timeLimit;
    TimerPolicy policy;
    ExprUptr inVar;
    std::vector<StmtUptr> body;
};



// Enters a new scope (for entering blocks)
class ScopeChangeStmt final : public Statement {
public:
    ScopeChangeStmt(std::vector<ExprUptr> body) : 
        body{std::move(body)} {}

    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;
    
private:
    std::vector<ExprUptr> body;
};



class VariableAssignmentStmt final : public Statement {
public:
    VariableAssignmentStmt(std::string varName, ExprUptr newValue) :
        varName(varName),
        newValue(std::move(newValue))
        {}

    [[nodiscard]] std::optional<Resumable> startImpl(std::shared_ptr<Environment>& env) override;
    [[nodiscard]] std::optional<Resumable> resumeImpl() override;
    void abortImpl() override;

public:
    std::string varName;
    ExprUptr newValue;
};