#include <iostream>
#include <string>
#include <limits>
#include "claudemods.h"
#include "cachyos.h"
#include "arch.h"

// Function to set text color
void setColor(const std::string& colorCode) {
    std::cout << colorCode;
}

// Function to reset text color
void resetColor() {
    std::cout << "\033[0m";
}

// Function to display the header
void displayHeader() {
    setColor("\033[31m"); // Red color
    std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░██████╗" << std::endl;
    std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
    std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
    std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
    std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
    std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
    resetColor();

    // Custom text in cyan below ASCII
    setColor("\033[38;2;0;255;255m");
    std::cout << "claudemods distribution iso creator v1.01 12-01-2026" << std::endl;
    resetColor();
    std::cout << std::endl;
}

// Function to display the menu
void displayMenu() {
    std::cout << std::endl;
    setColor("\033[38;2;0;255;255m"); // Your cyan color
    std::cout << "=== DISTRIBUTIONS MENU ===" << std::endl;
    std::cout << "1. claudemods Desktop Distributions" << std::endl;
    std::cout << "2. claudemods Handheld Distributions" << std::endl;  // New option
    std::cout << "3. Cachyos Distributions" << std::endl;
    std::cout << "4. Cachyos Handheld Distributions" << std::endl;  // New option
    std::cout << "5. Arch Distributions" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "==========================" << std::endl;
    resetColor();
}

// Function to handle menu selection
void handleSelection(int choice) {
    switch(choice) {
        case 1:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Claudemods Desktop Distributions" << std::endl;
            std::cout << "Starting Claudemods ISO creation process..." << std::endl;
            resetColor();

            // Launch the Claudemods installer
            {
                ClaudemodsInstaller installer;
                installer.run();
            }
            break;
        case 2:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: claudemods Handheld Distributions" << std::endl;
            std::cout << "Option not yet implemented." << std::endl;
            resetColor();
            break;
        case 3:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Cachyos Distributions" << std::endl;
            std::cout << "Starting Cachyos ISO creation process..." << std::endl;
            resetColor();

            // Launch the Cachyos installer
            {
                CachyosInstaller installer;
                installer.run();
            }
            break;
        case 4:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Cachyos Handheld Distributions" << std::endl;
            std::cout << "Option not yet implemented." << std::endl;
            resetColor();
            break;
        case 5:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Arch Distributions" << std::endl;
            std::cout << "Starting Arch ISO creation process..." << std::endl;
            resetColor();

            // Launch the Arch installer
            {
                ArchInstaller installer;
                installer.run();
            }
            break;
        case 6:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Exiting program. Goodbye!" << std::endl;
            resetColor();
            break;
        default:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Invalid selection! Please choose 1-6." << std::endl;
            resetColor();
            break;
    }
}

int main() {
    int choice;

    do {
        // Clear screen (optional)
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif

        displayHeader();
        displayMenu();

        setColor("\033[38;2;0;255;255m");
        std::cout << "Enter your choice (1-6): ";
        resetColor();

        if (!(std::cin >> choice)) {
            // Clear error state and invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\033[31m" << "Invalid input! Please enter a number 1-6." << "\033[0m" << std::endl;
            continue;
        }

        // Clear the newline character from input buffer immediately
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        handleSelection(choice);

        if (choice != 6 && choice >= 1 && choice <= 5) {
            std::cout << std::endl;
            setColor("\033[38;2;0;255;255m");
            std::cout << "Press Enter to return to main menu...";
            resetColor();
            std::cin.get();  // Only one get() needed now
        }

    } while (choice != 6);

    return 0;
}
