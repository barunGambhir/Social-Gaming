// #include <any>
// #include <unordered_map>
// #include <string>

// /*
// An environment stores information about variables in a given game. It considers a variable a name.
// To support scope and variable shadowing, an environment has an enclosing environemnt. It will search itself for a variable first.
// If it is not found it will ask its enclosing environment instead. 
// */

// class Environment {
// public:
//     Environment(Environment* enc, std::unordered_map<std::string, std::any> e) : enclosing{enc}, env{e} {};
//     Environment(Environment* enc) : enclosing{enc} {};

//     void init(std::string name);
//     std::any get(const std::string& name);
//     void set(const std::string& name, std::any value);

// private:
//     Environment* enclosing;
//     std::unordered_map<std::string, std::any> env;
// };