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
    std::cout << "claudemods distribution iso creator v1.01 05-07-2026" << std::endl;
    resetColor();
    std::cout << std::endl;
}

// Function to display the main menu
void displayMenu() {
    std::cout << std::endl;
    setColor("\033[38;2;0;255;255m"); // Your cyan color
    std::cout << "=== DISTRIBUTIONS MENU ===" << std::endl;
    std::cout << "1. claudemods Desktop Distributions" << std::endl;
    std::cout << "2. Cachyos Desktop Distributions" << std::endl;
    std::cout << "3. Arch Distributions" << std::endl;
    std::cout << "4. Guides" << std::endl;
    std::cout << "5. Changelog" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "==========================" << std::endl;
    resetColor();
}

// Function to display the guides submenu
void displayGuidesMenu() {
    std::cout << std::endl;
    setColor("\033[38;2;0;255;255m"); // Cyan color
    std::cout << "=== GUIDES MENU ===" << std::endl;
    std::cout << "1. claudemods Guide" << std::endl;
    std::cout << "2. Cachyos Guide" << std::endl;
    std::cout << "3. Arch Guide" << std::endl;
    std::cout << "4. Return to Main Menu" << std::endl;
    std::cout << "===================" << std::endl;
    resetColor();
}

// Function to handle guide selection
void handleGuideSelection(int guideChoice) {
    switch(guideChoice) {
        case 1:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Opening claudemods Guide..." << std::endl;
            resetColor();
            system("nano ./guides/claudemodsguide.md");
            break;
        case 2:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Opening Cachyos Guide..." << std::endl;
            resetColor();
            system("nano ./guides/cachyosguide.md");
            break;
        case 3:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Opening Arch Guide..." << std::endl;
            resetColor();
            system("nano ./guides/archguide.md");
            break;
        case 4:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Returning to Main Menu..." << std::endl;
            resetColor();
            break;
        default:
            setColor("\033[38;2;0;255;255m");
            std::cout << "Invalid selection! Please choose 1-4." << std::endl;
            resetColor();
            break;
    }
}

// Function to handle main menu selection
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
            std::cout << "You selected: Cachyos Desktop Distributions" << std::endl;
            std::cout << "Starting Cachyos ISO creation process..." << std::endl;
            resetColor();

            // Launch the Cachyos installer
            {
                CachyosInstaller installer;
                installer.run();
            }
            break;
        case 3:
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
        case 4:
            // Guides menu
        {
            int guideChoice;
            do {
                #ifdef _WIN32
                system("cls");
                #else
                system("clear");
                #endif

                displayHeader();
                displayGuidesMenu();

                setColor("\033[38;2;0;255;255m");
                std::cout << "Enter your choice (1-4): ";
                resetColor();

                if (!(std::cin >> guideChoice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "\033[31m" << "Invalid input! Please enter a number 1-4." << "\033[0m" << std::endl;
                    std::cout << std::endl;
                    setColor("\033[38;2;0;255;255m");
                    std::cout << "Press Enter to continue...";
                    resetColor();
                    std::cin.get();
                    continue;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                handleGuideSelection(guideChoice);

                if (guideChoice >= 1 && guideChoice <= 3) {
                    std::cout << std::endl;
                    setColor("\033[38;2;0;255;255m");
                    std::cout << "Guide closed. Press Enter to return to Guides Menu...";
                    resetColor();
                    std::cin.get();
                }

            } while (guideChoice != 4);
        }
        break;
        case 5:
            // Changelog
            setColor("\033[38;2;0;255;255m");
            std::cout << "Opening Changelog..." << std::endl;
            std::cout << "Opening nano editor with changelog.md..." << std::endl;
            resetColor();

            // Open changelog.md with nano
            system("nano ./changelog.md");

            std::cout << std::endl;
            setColor("\033[38;2;0;255;255m");
            std::cout << "Changelog closed. Press Enter to return to main menu...";
            resetColor();
            std::cin.get();
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

        if (choice != 6 && choice >= 1 && choice <= 3) {
            std::cout << std::endl;
            setColor("\033[38;2;0;255;255m");
            std::cout << "Press Enter to return to main menu...";
            resetColor();
            std::cin.get();  // Only one get() needed now
        }

    } while (choice != 6);

    return 0;
}
