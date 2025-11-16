#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>
#include <algorithm>

// Include your resource manager
#include "resources.h"

// Color definitions
const std::string COLOR_CYAN = "\033[38;2;0;255;255m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_ORANGE = "\033[38;5;208m";
const std::string COLOR_PURPLE = "\033[38;5;93m";
const std::string COLOR_RESET = "\033[0m";

class ClaudemodsInstaller {
private:
    std::string target_folder;
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    
    // Terminal control for arrow keys
    struct termios oldt, newt;

    // ADD ZIP EXTRACTION FUNCTION
    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Extracting required files..." << COLOR_RESET << std::endl;
        
        // Use your existing resource manager functions
        if (!ResourceManager::extractEmbeddedZip("")) {
            std::cerr << COLOR_RED << "Failed to extract embedded ZIP files!" << COLOR_RESET << std::endl;
            return false;
        }
        
        if (!ResourceManager::extractCalamaresResources("")) {
            std::cerr << COLOR_RED << "Failed to extract Calamares resources!" << COLOR_RESET << std::endl;
            return false;
        }
        
        std::cout << COLOR_GREEN << "All required files extracted successfully!" << COLOR_RESET << std::endl;
        return true;
    }

    void display_header() {
        std::cout << COLOR_RED;
        std::cout << "░█████╗░██╗░░░░░░█████╗░██║░░░██╗██████╗░███████╗███╗░░░███╗░█████╗░██████╗░░██████╗" << std::endl;
        std::cout << "██╔══██╗██║░░░░░██╔══██╗██║░░░██║██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗██╔════╝" << std::endl;
        std::cout << "██║░░╚═╝██║░░░░░███████║██║░░░██║██║░░██║█████╗░░██╔████╔██║██║░░██║██║░░██║╚█████╗░" << std::endl;
        std::cout << "██║░░██╗██║░░░░░██╔══██║██║░░░██║██║░░██║██╔══╝░░██║╚██╔╝██║██║░░██║██║░░██║░╚═══██╗" << std::endl;
        std::cout << "╚█████╔╝███████╗██║░░██║╚██████╔╝██████╔╝███████╗██║░╚═╝░██║╚█████╔╝██████╔╝██████╔╝" << std::endl;
        std::cout << "░╚════╝░╚══════╝╚═╝░░░░░░╚═════╝░╚═════╝░╚══════╝╚═╝░░░░░╚═╝░╚════╝░╚═════╝░╚═════╝░" << std::endl;
        std::cout << COLOR_CYAN << "claudemods distribution installer Beta DevBranch v1.01 15-11-2025" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Supports Ext4 And Btrfs filesystems" << COLOR_RESET << std::endl;
        std::cout << std::endl;
    }

    // Function to setup terminal for arrow key reading
    void setup_terminal() {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    // Function to restore terminal
    void restore_terminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    // Function to get arrow key input
    int get_arrow_key() {
        int ch = getchar();
        if (ch == 27) { // ESC
            getchar(); // Skip [
            ch = getchar(); // Actual key
            return ch;
        }
        return ch;
    }

    // Function to display menu with selection
    int show_menu(const std::vector<std::string>& options, const std::string& title, int selected = 0) {
        setup_terminal();

        while (true) {
            system("clear");
            display_header();

            std::cout << COLOR_CYAN;
            std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
            std::cout << "║ " << std::left << std::setw(60) << title << "║" << std::endl;
            std::cout << "╠══════════════════════════════════════════════════════════════╣" << std::endl;

            for (int i = 0; i < options.size(); i++) {
                std::cout << "║ ";
                if (i == selected) {
                    std::cout << COLOR_GREEN << "> " << COLOR_RESET << COLOR_CYAN;
                } else {
                    std::cout << "  ";
                }
                std::cout << std::left << std::setw(58) << options[i] << "║" << std::endl;
            }

            std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << COLOR_RESET;
            std::cout << COLOR_YELLOW << "Use ↑↓ arrows to navigate, Enter to select" << COLOR_RESET << std::endl;

            int key = get_arrow_key();
            if (key == 'A') { // Up arrow
                selected = (selected - 1 + options.size()) % options.size();
            } else if (key == 'B') { // Down arrow
                selected = (selected + 1) % options.size();
            } else if (key == 10) { // Enter
                restore_terminal();
                return selected;
            }
        }
    }

    // Function to get text input with prompt
    std::string get_input(const std::string& prompt) {
        std::cout << COLOR_CYAN << prompt << COLOR_RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    int execute_command(const std::string& cmd) {
        std::cout << COLOR_CYAN << "[EXEC] " << cmd << COLOR_RESET << std::endl;
        int status = system(cmd.c_str());
        if (status != 0) {
            std::cerr << COLOR_RED << "Error: " << cmd << COLOR_RESET << std::endl;
        }
        return status;
    }

    bool create_directory(const std::string& path) {
        return system(("mkdir -p " + path).c_str()) == 0;
    }

    void get_user_input() {
        target_folder = get_input("Enter target folder path: ");
        new_username = get_input("Enter username: ");
        root_password = get_input("Enter root password: ");
        user_password = get_input("Enter user password: ");
        
        // Get timezone and keyboard layout
        std::vector<std::string> timezone_options = {
            "America/New_York (US English)",
            "Europe/London (UK English)",
            "Europe/Berlin (German)",
            "Europe/Paris (French)",
            "Europe/Madrid (Spanish)",
            "Europe/Rome (Italian)",
            "Asia/Tokyo (Japanese)",
            "Other (manual entry)"
        };

        int timezone_choice = show_menu(timezone_options, "Select Timezone");
        switch(timezone_choice) {
            case 0: timezone = "America/New_York"; break;
            case 1: timezone = "Europe/London"; break;
            case 2: timezone = "Europe/Berlin"; break;
            case 3: timezone = "Europe/Paris"; break;
            case 4: timezone = "Europe/Madrid"; break;
            case 5: timezone = "Europe/Rome"; break;
            case 6: timezone = "Asia/Tokyo"; break;
            case 7: timezone = get_input("Enter timezone (e.g., Europe/Berlin): "); break;
        }

        std::vector<std::string> keyboard_options = {
            "us (US English)",
            "uk (UK English)",
            "de (German)",
            "fr (French)",
            "es (Spanish)",
            "it (Italian)",
            "jp (Japanese)",
            "Other (manual entry)"
        };

        int keyboard_choice = show_menu(keyboard_options, "Select Keyboard Layout");
        switch(keyboard_choice) {
            case 0: keyboard_layout = "us"; break;
            case 1: keyboard_layout = "uk"; break;
            case 2: keyboard_layout = "de"; break;
            case 3: keyboard_layout = "fr"; break;
            case 4: keyboard_layout = "es"; break;
            case 5: keyboard_layout = "it"; break;
            case 6: keyboard_layout = "jp"; break;
            case 7: keyboard_layout = get_input("Enter keyboard layout (e.g., br, ru, pt): "); break;
        }
    }

    void mount_system_dirs() {
        execute_command("mount --bind /dev " + target_folder + "/dev");
        execute_command("mount --bind /dev/pts " + target_folder + "/dev/pts");
        execute_command("mount --bind /proc " + target_folder + "/proc");
        execute_command("mount --bind /sys " + target_folder + "/sys");
        execute_command("mount --bind /run " + target_folder + "/run");
    }

    void unmount_system_dirs() {
        execute_command("umount " + target_folder + "/dev/pts");
        execute_command("umount " + target_folder + "/dev");
        execute_command("umount " + target_folder + "/proc");
        execute_command("umount " + target_folder + "/sys");
        execute_command("umount " + target_folder + "/run");
    }

    void create_user() {
        execute_command("chroot " + target_folder + " /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");
    }

    void apply_timezone_keyboard_settings() {
        std::cout << COLOR_CYAN << "Setting timezone to: " << timezone << COLOR_RESET << std::endl;
        execute_command("chroot " + target_folder + " /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"hwclock --systohc\"");

        std::cout << COLOR_CYAN << "Setting keyboard layout to: " << keyboard_layout << COLOR_RESET << std::endl;
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"locale-gen\"");
    }

    // SPITFIRE CKGE MINIMAL - EXACT COMMANDS
    void install_spitfire_ckge_minimal() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Minimal..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Minimal installation in: " << target_folder << COLOR_RESET << std::endl;

        // EXACT SPITFIRE MINIMAL COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop");
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub " + target_folder + "/etc/default");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub.cfg " + target_folder + "/boot/grub");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes");
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.sh " + target_folder + "/usr/local/bin");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal.zip");
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/spitfire-minimal.zip -d " + target_folder + "/home/" + new_username + "/");
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt");
        execute_command("chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/spitfire-minimal.zip");
        execute_command("rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        // Fix user-places.xbel
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Spitfire CKGE Minimal installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // SPITFIRE CKGE FULL - DIFFERENT COMMANDS
    void install_spitfire_ckge_full() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Full..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Full installation in: " << target_folder << COLOR_RESET << std::endl;

        // DIFFERENT FULL COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-full"); // DIFFERENT PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub " + target_folder + "/etc/default");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub.cfg " + target_folder + "/boot/grub");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes");
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/termfull.sh " + target_folder + "/usr/local/bin"); // DIFFERENT TERM SCRIPT
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/termfull.service " + target_folder + "/etc/systemd/system/"); // DIFFERENT SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full.zip"); // DIFFERENT ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/spitfire-full.zip -d " + target_folder + "/home/" + new_username + "/"); // DIFFERENT ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt");
        execute_command("chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/spitfire-full.zip"); // DIFFERENT ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        // Fix user-places.xbel
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Spitfire CKGE Full installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // SPITFIRE CKGE MINIMAL DEV - DIFFERENT COMMANDS
    void install_spitfire_ckge_minimal_dev() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Minimal Dev..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Minimal Dev installation in: " << target_folder << COLOR_RESET << std::endl;

        // DEV COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-dev"); // DEV PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub " + target_folder + "/etc/default");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub.cfg " + target_folder + "/boot/grub");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes");
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.sh " + target_folder + "/usr/local/bin");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal.zip");
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/spitfire-minimal.zip -d " + target_folder + "/home/" + new_username + "/");
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt");
        execute_command("chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/spitfire-minimal.zip");
        execute_command("rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        // Fix user-places.xbel
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Spitfire CKGE Minimal Dev installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // SPITFIRE CKGE FULL DEV - DIFFERENT COMMANDS
    void install_spitfire_ckge_full_dev() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Full Dev..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Full Dev installation in: " << target_folder << COLOR_RESET << std::endl;

        // FULL DEV COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-fulldev"); // FULL DEV PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub " + target_folder + "/etc/default");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub.cfg " + target_folder + "/boot/grub");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes");
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/termfull.sh " + target_folder + "/usr/local/bin"); // FULL DEV TERM
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/termfull.service " + target_folder + "/etc/systemd/system/"); // FULL DEV SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full.zip"); // FULL ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/spitfire-full.zip -d " + target_folder + "/home/" + new_username + "/"); // FULL ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt");
        execute_command("chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/spitfire-full.zip"); // FULL ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        // Fix user-places.xbel
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Spitfire CKGE Full Dev installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // APEX CKGE MINIMAL - COMPLETELY DIFFERENT COMMANDS
    void install_apex_ckge_minimal() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Minimal..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Apex CKGE Minimal installation in: " << target_folder << COLOR_RESET << std::endl;

        // APEX SPECIFIC COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop"); // SAME PACKAGE BUT DIFFERENT THEMES
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        // APEX SPECIFIC FILES
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub " + target_folder + "/etc/default"); // DIFFERENT GRUB
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub.cfg " + target_folder + "/boot/grub"); // DIFFERENT GRUB.CFG
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes"); // DIFFERENT THEME
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/term.sh " + target_folder + "/usr/local/bin"); // APEX TERM SCRIPT
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/"); // APEX SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        // APEX THEMES AND TWEAKS
        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal.zip"); // APEX ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/apex-minimal.zip -d " + target_folder + "/home/" + new_username + "/"); // APEX ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d"); // APEX SETTINGS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/tweaksapex.sh " + target_folder + "/opt"); // APEX TWEAKS
        execute_command("chmod +x " + target_folder + "/opt/tweaksapex.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\""); // APEX TWEAKS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/"); // APEX KONSOLERC
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/ApexLogin2 " + target_folder + "/usr/share/sddm/themes"); // APEX LOGIN
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/apex-minimal.zip"); // APEX ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksapex.sh"); // APEX TWEAKS

        // Fix user-places.xbel for APEX
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/apex/" + home_folder + "/g' " + user_places_file; // DIFFERENT SEARCH PATTERN
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Apex CKGE Minimal installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // APEX CKGE FULL - DIFFERENT COMMANDS
    void install_apex_ckge_full() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Full..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Apex CKGE Full installation in: " << target_folder << COLOR_RESET << std::endl;

        // APEX FULL COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-full"); // FULL PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        // APEX FULL FILES
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub " + target_folder + "/etc/default"); // APEX GRUB
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub.cfg " + target_folder + "/boot/grub"); // APEX GRUB.CFG
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes"); // APEX THEME
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/termfull.sh " + target_folder + "/usr/local/bin"); // APEX FULL TERM
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/termfull.service " + target_folder + "/etc/systemd/system/"); // APEX FULL SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        // APEX FULL THEMES AND TWEAKS
        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full.zip"); // APEX FULL ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/apex-full.zip -d " + target_folder + "/home/" + new_username + "/"); // APEX FULL ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d"); // APEX SETTINGS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/tweaksapex.sh " + target_folder + "/opt"); // APEX TWEAKS
        execute_command("chmod +x " + target_folder + "/opt/tweaksapex.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\""); // APEX TWEAKS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/"); // APEX KONSOLERC
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/ApexLogin2 " + target_folder + "/usr/share/sddm/themes"); // APEX LOGIN
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/apex-full.zip"); // APEX FULL ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksapex.sh"); // APEX TWEAKS

        // Fix user-places.xbel for APEX
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/apex/" + home_folder + "/g' " + user_places_file; // DIFFERENT SEARCH PATTERN
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Apex CKGE Full installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // APEX CKGE MINIMAL DEV - DIFFERENT COMMANDS
    void install_apex_ckge_minimal_dev() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Minimal Dev..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Apex CKGE Minimal Dev installation in: " << target_folder << COLOR_RESET << std::endl;

        // APEX MINIMAL DEV COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-dev"); // DEV PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        // APEX DEV FILES
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub " + target_folder + "/etc/default"); // APEX GRUB
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub.cfg " + target_folder + "/boot/grub"); // APEX GRUB.CFG
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes"); // APEX THEME
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/term.sh " + target_folder + "/usr/local/bin"); // APEX TERM
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/"); // APEX SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        // APEX DEV THEMES AND TWEAKS
        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-minimal.zip"); // APEX MINIMAL ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/apex-minimal.zip -d " + target_folder + "/home/" + new_username + "/"); // APEX MINIMAL ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d"); // APEX SETTINGS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/tweaksapex.sh " + target_folder + "/opt"); // APEX TWEAKS
        execute_command("chmod +x " + target_folder + "/opt/tweaksapex.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\""); // APEX TWEAKS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/"); // APEX KONSOLERC
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/ApexLogin2 " + target_folder + "/usr/share/sddm/themes"); // APEX LOGIN
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/apex-minimal.zip"); // APEX MINIMAL ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksapex.sh"); // APEX TWEAKS

        // Fix user-places.xbel for APEX
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/apex/" + home_folder + "/g' " + user_places_file; // DIFFERENT SEARCH PATTERN
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Apex CKGE Minimal Dev installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    // APEX CKGE FULL DEV - DIFFERENT COMMANDS
    void install_apex_ckge_full_dev() {
        std::cout << COLOR_PURPLE << "Installing Apex CKGE Full Dev..." << COLOR_RESET << std::endl;
        get_user_input();
        std::cout << COLOR_CYAN << "Starting Apex CKGE Full Dev installation in: " << target_folder << COLOR_RESET << std::endl;

        // APEX FULL DEV COMMANDS
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");
        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop-fulldev"); // FULL DEV PACKAGE
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        mount_system_dirs();
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        apply_timezone_keyboard_settings();

        // APEX FULL DEV FILES
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub " + target_folder + "/etc/default"); // APEX GRUB
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/grub.cfg " + target_folder + "/boot/grub"); // APEX GRUB.CFG
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes"); // APEX THEME
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/termfull.sh " + target_folder + "/usr/local/bin"); // APEX FULL TERM
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/termfull.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/termfull.service " + target_folder + "/etc/systemd/system/"); // APEX FULL SERVICE
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable termfull.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        // APEX FULL DEV THEMES AND TWEAKS
        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/apex-full.zip"); // APEX FULL ZIP
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/apex-full.zip -d " + target_folder + "/home/" + new_username + "/"); // APEX FULL ZIP
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d"); // APEX SETTINGS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/tweaksapex.sh " + target_folder + "/opt"); // APEX TWEAKS
        execute_command("chmod +x " + target_folder + "/opt/tweaksapex.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksapex.sh " + new_username + "'\""); // APEX TWEAKS
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/"); // APEX KONSOLERC
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/ApexLogin2 " + target_folder + "/usr/share/sddm/themes"); // APEX LOGIN
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/apex-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/apex-full.zip"); // APEX FULL ZIP
        execute_command("rm -rf " + target_folder + "/opt/tweaksapex.sh"); // APEX TWEAKS

        // Fix user-places.xbel for APEX
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/apex/" + home_folder + "/g' " + user_places_file; // DIFFERENT SEARCH PATTERN
                execute_command(sed_cmd);
            }
        }

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Apex CKGE Full Dev installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }

    void show_main_menu() {
        std::vector<std::string> main_options = {
            "Install Spitfire CKGE Minimal",
            "Install Spitfire CKGE Full", 
            "Install Spitfire CKGE Minimal Dev",
            "Install Spitfire CKGE Full Dev",
            "Install Apex CKGE Minimal",
            "Install Apex CKGE Full",
            "Install Apex CKGE Minimal Dev", 
            "Install Apex CKGE Full Dev",
            "Exit"
        };

        while (true) {
            int choice = show_menu(main_options, "claudemods Distribution Installer");

            switch(choice) {
                case 0:
                    install_spitfire_ckge_minimal();
                    break;
                case 1:
                    install_spitfire_ckge_full();
                    break;
                case 2:
                    install_spitfire_ckge_minimal_dev();
                    break;
                case 3:
                    install_spitfire_ckge_full_dev();
                    break;
                case 4:
                    install_apex_ckge_minimal();
                    break;
                case 5:
                    install_apex_ckge_full();
                    break;
                case 6:
                    install_apex_ckge_minimal_dev();
                    break;
                case 7:
                    install_apex_ckge_full_dev();
                    break;
                case 8:
                    std::cout << COLOR_GREEN << "Exiting. Goodbye!" << COLOR_RESET << std::endl;
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

public:
    void run() {
        // EXTRACT FILES FIRST
        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }
        
        display_header();
        show_main_menu();
    }
};

int main() {
    ClaudemodsInstaller installer;
    installer.run();
    return 0;
}
