#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "map"

enum InputStates{
    NUMBER_OF_STATES,
    NUMBER_OF_VARIABLES,
    STATES,
    GOAL_STATES,
    VARIABLES,
    TRANSITIONS,
    STRINGS
};

bool checkDFA(int number_of_states, int number_of_variables, std::vector<std::string> &states,
              std::vector<std::string> &goal_states, std::vector<std::string> &variables,
              std::map<std::pair<std::string, std::string>,std::string> &transitions,
              std::vector<std::vector<std::string>> &strings) {
    bool goal_state_found;

    //State Number Check
    if (number_of_states != states.size()){
        std::cout << "The provided number of states is inconsistent with the given states." << std::endl;
        exit(EXIT_FAILURE);
    }

    //Variable Number Check

    if (number_of_variables != variables.size()){
        std::cout << "The provided number of variables is inconsistent with the given variables." << std::endl;
        exit(EXIT_FAILURE);
    }


    //Goal State Check
    for (auto &goalState: goal_states) {
        goal_state_found = false;
        for (auto &state: states) {
            if (goalState == state) {
                goal_state_found = true;
                break;
            }
        }
        if (!goal_state_found) {
            std::cout << "At least one of the final states is not among the states of the DFA." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    //Transition Check
    bool transition_from_state_found;
    bool transition_character_found;
    bool transition_to_state_found;

    for (auto &transition: transitions) {
        transition_from_state_found = false;
        transition_character_found = false;
        transition_to_state_found = false;
        for (auto &state: states) {
            if (transition.first.first == state) {
                transition_from_state_found = true;
            }
            if (transition.second == state) {
                transition_to_state_found = true;
            }
            if (transition_to_state_found && transition_from_state_found) {
                break;
            }
        }
        if (!(transition_from_state_found && transition_to_state_found)) {
            std::cout << "Invalid state in the transition." << std::endl;
            exit(EXIT_FAILURE);
        }
        for (auto &character: variables) {
            if (transition.first.second == character) {
                transition_character_found = true;
            }
        }
        if (!transition_character_found) {
            std::cout << "Invalid character in the transition." << std::endl;
            exit(EXIT_FAILURE);
        }

    }
    //String Check

    bool character_found;
    for (auto &string: strings) {
        if (string.size()==1 && string[0].empty()){
            string.pop_back();
            continue;
        }
        for (auto &character: string) {
            character_found = false;
            for (auto &variable: variables) {
                if (character == variable) {
                    character_found = true;
                    break;
                }
            }
            if (!character_found) {
                std::cout << "Invalid character in one of the strings." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    //Check if all transitions are available
    for(auto& state: states){
        for (auto& variable: variables){
            auto transition_pair = std::pair(state, variable);
            if(transitions[transition_pair].empty()){
                std::cout << "Missing state, variable pair in the transitions. This contradicts with the definition of DFA." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    return true;
}
void evaluateDFA(int number_of_states, int number_of_variables, std::vector<std::string> &states,
                 std::vector<std::string> &goal_states, std::vector<std::string> &variables,
                 std::map<std::pair<std::string, std::string>,std::string> &transitions,
                 std::vector<std::vector<std::string>> &strings){
    checkDFA(number_of_states, number_of_variables, states, goal_states, variables, transitions, strings);
    auto routes = std::vector<std::vector<std::string>>();
    std::string nextState;
    for(auto& string: strings){
        auto route = std::vector<std::string>();
        route.push_back(states[0]);
        for(auto& character: string){
            auto stateCharacterPair = std::pair<std::string, std::string>(route[route.size()-1], character);
            route.push_back(transitions[stateCharacterPair]);
        }
        routes.push_back(route);
    }
    std::ofstream outputFile;
    outputFile.open("output.txt");

    for(auto& current_route: routes){
        bool accepted = false;
        auto line = std::string("Route Taken: ");
        for(auto i = 0; i < current_route.size(); i++){
            if(i < current_route.size() -1){
                line += (current_route[i] + ", ");
            } else{
                line += current_route[i];

            }
        }
        outputFile << line << std::endl;
        for (auto& goal_state: goal_states){
            if (current_route[current_route.size()-1] == goal_state){
                outputFile << "Accepted" << std::endl << std::endl;
                accepted = true;
                break;
            }
        }
        if(!accepted){
            outputFile << "Rejected" << std::endl << std::endl;
        }
    }
    outputFile.close();
}

int main() {
    std::string file_loc;
    std::cout << "Please enter the loaction of the input file:  ";
    std::cin >> file_loc;
    std::ifstream inputFile(file_loc);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }
    std::string line;
    auto row_index = 0;

    int number_of_states;
    int number_of_variables;
    int state_start = 0;
    int state_length = 0;
    auto inputState = InputStates();
    auto states = std::vector<std::string>();
    auto goal_states = std::vector<std::string>();
    auto variables = std::vector<std::string>();
    auto transitions = std::map<std::pair<std::string, std::string>,std::string>();
    auto strings = std::vector<std::vector<std::string>>();

    std::string transition_state_from;
    std::string transition_character;
    std::string transition_state_to;
    inputState = NUMBER_OF_STATES;
    while (std::getline(inputFile, line)) {
        switch (inputState) {
            case NUMBER_OF_STATES:
                number_of_states = std::stoi(line);
                inputState = NUMBER_OF_VARIABLES;
                break;
            case NUMBER_OF_VARIABLES:
                number_of_variables = std::stoi(line);
                inputState = STATES;
                break;
            case STATES:
                state_start = 0;
                state_length = 0;
                for(auto i = 0; i<=line.length(); i++){
                    if(i == line.length() || line[i] == ',' ){
                        states.push_back(line.substr(state_start, state_length));
                        state_start = i+1;
                        state_length = 0;
                    }
                    else{
                        state_length++;
                    }
                }
                inputState = GOAL_STATES;
                break;
            case GOAL_STATES:
                state_start = 0;
                state_length = 0;
                for(auto i = 0; i<=line.length(); i++){
                    if(i == line.length() || line[i] == ','){
                        goal_states.push_back(line.substr(state_start, state_length));
                        state_start = i+1;
                        state_length = 0;
                    }
                    else{
                        state_length++;
                    }
                }
                inputState = VARIABLES;
                break;
            case VARIABLES:
                state_start = 0;
                state_length = 0;
                for(auto i = 0; i<=line.length(); i++){
                    if(i == line.length() || line[i] == ','){
                        variables.push_back(line.substr(state_start, state_length));
                        state_start = i+1;
                        state_length = 0;
                    }
                    else{
                        state_length++;
                    }
                }
                inputState = TRANSITIONS;
                break;
            case TRANSITIONS:
                state_start = 0;
                state_length = 0;
                if (line == "END_OF_TRANSITIONS"){
                    inputState = STRINGS;
                    break;
                }
                for(auto i = 0; i<=line.length(); i++){
                    if(i == line.length()){
                        transition_state_to = line.substr(state_start, state_length);
                        state_start = i+1;
                        state_length = 0;
                        auto transition_pair = std::pair(transition_state_from, transition_character);
                        if(transitions[transition_pair].empty()){
                            transitions[transition_pair] = transition_state_to;
                        }
                        else{
                            std::cout << "There are multiple transitions corresponding to at least one state, character pair. This contradicts with the definition of DFA." <<std::endl;
                            exit(EXIT_FAILURE);
                        }

                    }

                    else if(line[i] == '-'){
                        transition_state_from = line.substr(state_start, state_length);
                        state_start = i+1;
                        state_length = 0;
                    }
                    else if(line[i] == '>'){
                        transition_character = line.substr(state_start, state_length);
                        state_start = i+1;
                        state_length = 0;
                    }

                    else{
                        state_length++;
                    }
                }
                break;
            case STRINGS:
                state_start = 0;
                state_length = 0;
                auto current_string = std::vector<std::string>();
                for(auto i = 0; i<=line.length(); i++){
                    if(i == line.length() || line[i] == ','){
                        current_string.push_back(line.substr(state_start, state_length));
                        state_start = i+1;
                        state_length = 0;
                    }
                    else{
                        state_length++;
                    }
                }
                strings.push_back(current_string);
                break;
        }

    }
    evaluateDFA(number_of_states, number_of_variables, states, goal_states, variables, transitions, strings);
    std::cout << "Successfully written the results on the file: output.txt" << std::endl;
    return 0;
}

