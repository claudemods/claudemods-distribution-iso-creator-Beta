#include <iostream>
#include <string>
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
    std::cout << "claudemods distribution iso creator v1.01 01-12-2025" << std::endl;
    resetColor();
    std::cout << std::endl;
}

// Function to display the menu
void displayMenu() {
    std::cout << std::endl;
    setColor("\033[38;2;0;255;255m"); // Your cyan color
    std::cout << "=== DISTRIBUTIONS MENU ===" << std::endl;
    std::cout << "1. Claudemods Distributions" << std::endl;
    std::cout << "2. Cachyos Distributions" << std::endl;  // Now fully implemented
    std::cout << "3. Arch Distributions" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "==========================" << std::endl;
    resetColor();
}

// Function to handle menu selection
void handleSelection(int choice) {
    switch(choice) {
        case 1:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Claudemods Distributions" << std::endl;
            std::cout << "Starting Claudemods ISO creation process..." << std::endl;
            resetColor();

            // Clear input buffer before launching installer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Launch the Claudemods installer
            {
                ClaudemodsInstaller installer;
                installer.run();
            }
            break;
        case 2:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Cachyos Distributions" << std::endl;
            std::cout << "Starting Cachyos ISO creation process..." << std::endl;
            resetColor();

            // Clear input buffer before launching installer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Launch the Cachyos installer
            {
                CachyosInstaller installer;  // Changed from ClaudemodsInstaller to CachyosInstaller
                installer.run();
            }
            break;
        case 3:
            setColor("\033[38;2;0;255;255m");
            std::cout << "You selected: Arch Distributions" << std::endl;
            std::cout << "Starting Arch ISO creation process..." << std::endl;
            resetColor();

            // Clear input buffer before launching installer
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            // Launch the Arch installer
            {
                ArchInstaller installer;
                installer.run();
            }
            break;
        case 4:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Exiting program. Goodbye!" << std::endl;
            resetColor();
            break;
        default:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Invalid selection! Please choose 1-4." << std::endl;
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
        std::cout << "Enter your choice (1-4): ";
        resetColor();

        if (!(std::cin >> choice)) {
            // Clear error state and invalid input
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\033[31m" << "Invalid input! Please enter a number 1-4." << "\033[0m" << std::endl;
            continue;
        }

        handleSelection(choice);

        if (choice != 4 && choice >= 1 && choice <= 3) {
            std::cout << std::endl;
            setColor("\033[38;2;0;255;255m");
            std::cout << "Press Enter to return to main menu...";
            resetColor();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }

    } while (choice != 4);

    return 0;
}
