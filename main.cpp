#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <map>
#include <random>
#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>

const std::string VERSION = "Enterprise 1.0";
const std::string DEFAULT_USER = "user";
const std::string DEFAULT_HOST = "shitOS-host";


namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
}


class FileSystem {
private:
    struct File {
        std::string name;
        std::string content;
    };
    
    std::vector<File> files;
    
public:
    FileSystem() 
        create_file("README.txt", "Welcome to ShitOS Enterprise Edition\n");
        create_file("TODOS.txt", "1. Improve system stability\n2. Add more features\n");
    }
    
    void create_file(const std::string& filename, const std::string& content = "") {
        files.push_back({filename, content});
    }
    
    bool file_exists(const std::string& filename) const {
        return std::any_of(files.begin(), files.end(), 
            [&filename](const File& f) { return f.name == filename; });
    }
    
    std::string read_file(const std::string& filename) const {
        auto it = std::find_if(files.begin(), files.end(), 
            [&filename](const File& f) { return f.name == filename; });
        
        if (it != files.end()) {
            return it->content;
        }
        throw std::runtime_error("File not found: " + filename);
    }
    
    void write_file(const std::string& filename, const std::string& content) {
        auto it = std::find_if(files.begin(), files.end(), 
            [&filename](const File& f) { return f.name == filename; });
        
        if (it != files.end()) {
            it->content = content;
        } else {
            create_file(filename, content);
        }
    }
    
    std::vector<std::string> list_files() const {
        std::vector<std::string> result;
        for (const auto& file : files) {
            result.push_back(file.name);
        }
        return result;
    }
};

 
class CalculatorService {
public:
    double calculate(double a, double b, char op) const {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': 
                if (b == 0) throw std::runtime_error("Division by zero");
                return a / b;
            default:
                throw std::invalid_argument("Invalid operator");
        }
    }
};

class SystemInfo {
private:
    time_t boot_time;
    
public:
    SystemInfo() : boot_time(time(nullptr)) {}
    
    std::string get_uptime() const {
        double seconds = difftime(time(nullptr), boot_time);
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        
        return std::to_string(hours) + "h " + 
               std::to_string(minutes) + "m " + 
               std::to_string(secs) + "s";
    }
    
    std::string get_os_info() const {
        return "ShitOS " + VERSION + " (C++ " + std::to_string(__cplusplus) + ")";
    }
};

class ShitOS {
private:
    FileSystem fs;
    CalculatorService calculator;
    SystemInfo sys_info;
    std::string current_user;
    std::string hostname;
    
    void clear_screen() {
        std::cout << "\x1b[H\x1b[2J\x1b[3J";
    }
    
    void print_prompt() {
        std::cout << Colors::BOLD << current_user << "@" << hostname 
                  << Colors::RESET << ":$ ";
    }
    
    void show_help() {
        std::cout << "\nAvailable commands:\n"
                  << "  help       - Show this help\n"
                  << "  clear      - Clear screen\n"
                  << "  info       - Show system info\n"
                  << "  calc       - Start calculator\n"
                  << "  notepad    - Simple text editor\n"
                  << "  files      - File manager\n"
                  << "  exit       - Shutdown system\n";
    }
    
    void show_info() {
        std::cout << "\nSystem Information:\n"
                  << "------------------\n"
                  << "OS:        " << sys_info.get_os_info() << "\n"
                  << "Uptime:    " << sys_info.get_uptime() << "\n"
                  << "User:      " << current_user << "\n"
                  << "Host:      " << hostname << "\n";
    }
    
    void run_calculator() {
        std::cout << "\nCalculator (enter 'exit' to quit)\n";
        
        while (true) {
            std::cout << "calc> ";
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "exit") break;
            
            try {
                size_t op_pos = input.find_first_of("+-*/");
                if (op_pos != std::string::npos) {
                    double a = std::stod(input.substr(0, op_pos));
                    double b = std::stod(input.substr(op_pos + 1));
                    char op = input[op_pos];
                    
                    double result = calculator.calculate(a, b, op);
                    std::cout << "= " << Colors::GREEN << result 
                              << Colors::RESET << "\n";
                } else {
                    std::cout << Colors::RED << "Invalid input format\n" 
                              << Colors::RESET;
                }
            } catch (const std::exception& e) {
                std::cout << Colors::RED << "Error: " << e.what() << "\n"
                          << Colors::RESET;
            }
        }
    }
    
    void run_notepad() {
        std::cout << "\nNotepad (enter 'EOL' on empty line to save and exit)\n";
        std::cout << "Enter filename: ";
        
        std::string filename;
        std::getline(std::cin, filename);
        
        std::cout << "Enter text (EOL to finish):\n";
        std::string content;
        std::string line;
        
        while (true) {
            std::getline(std::cin, line);
            if (line == "EOL") break;
            content += line + "\n";
        }
        
        fs.write_file(filename, content);
        std::cout << Colors::GREEN << "File saved successfully\n" 
                  << Colors::RESET;
    }
    
    void show_files() {
        auto files = fs.list_files();
        std::cout << "\nFile System:\n";
        
        if (files.empty()) {
            std::cout << "  No files found\n";
            return;
        }
        
        for (const auto& file : files) {
            std::cout << "  " << file << "\n";
        }
        
        std::cout << "\nEnter filename to view or 'back' to return: ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input != "back") {
            try {
                std::cout << "\nFile content:\n"
                          << fs.read_file(input) << "\n";
            } catch (const std::exception& e) {
                std::cout << Colors::RED << e.what() << "\n" 
                          << Colors::RESET;
            }
        }
    }
    
public:
    ShitOS() : current_user(DEFAULT_USER), hostname(DEFAULT_HOST) {}
    
    void run() {
        clear_screen();
        std::cout << Colors::BOLD << Colors::GREEN 
                  << "Welcome to ShitOS " << VERSION 
                  << Colors::RESET << "\n\n";
        
        while (true) {
            print_prompt();
            
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            if (command == "help") {
                show_help();
            } else if (command == "clear") {
                clear_screen();
            } else if (command == "info") {
                show_info();
            } else if (command == "calc") {
                run_calculator();
            } else if (command == "notepad") {
                run_notepad();
            } else if (command == "files") {
                show_files();
            } else if (command == "exit") {
                break;
            } else {
                std::cout << Colors::RED << "Unknown command: " << command 
                          << Colors::RESET << "\n";
            }
        }
        
        std::cout << "\nShutting down...\n";
    }
};

int main() {
    ShitOS os;
    os.run();
    return 0;
}
