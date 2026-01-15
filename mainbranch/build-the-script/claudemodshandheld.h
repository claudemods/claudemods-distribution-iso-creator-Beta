#ifndef CLAUDEMODSHANDHELD_H
#define CLAUDEMODSHANDHELD_H

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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

extern const std::string COLOR_CYAN;
extern const std::string COLOR_RED;
extern const std::string COLOR_GREEN;
extern const std::string COLOR_YELLOW;
extern const std::string COLOR_ORANGE;
extern const std::string COLOR_PURPLE;
extern const std::string COLOR_RESET;

class ClaudemodsHandheldInstaller {
private:
    std::string new_username;
    std::string root_password;
    std::string user_password;
    std::string timezone;
    std::string keyboard_layout;
    std::string current_distro_name;
    std::string extra_packages;

    struct termios oldt, newt;

    std::string getConfigFilePath() {
        return getCurrentDir() + "/configurationclaudemods.txt";
    }

    void saveConfiguration() {
        std::string configFile = getConfigFilePath();
        std::ofstream file(configFile);
        if (file.is_open()) {
            file << "username=" << new_username << std::endl;
            file << "root_password=" << root_password << std::endl;
            file << "user_password=" << user_password << std::endl;
            file << "timezone=" << timezone << std::endl;
            file << "keyboard_layout=" << keyboard_layout << std::endl;
            file << "current_distro=" << current_distro_name << std::endl;
            file << "extra_packages=" << extra_packages << std::endl;
            file.close();
            std::cout << COLOR_GREEN << "Configuration saved to " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cerr << COLOR_RED << "Failed to save configuration to " << configFile << COLOR_RESET << std::endl;
        }
    }

    void loadConfiguration() {
        std::string configFile = getConfigFilePath();
        std::ifstream file(configFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t delimiter = line.find('=');
                if (delimiter != std::string::npos) {
                    std::string key = line.substr(0, delimiter);
                    std::string value = line.substr(delimiter + 1);

                    if (key == "username") new_username = value;
                    else if (key == "root_password") root_password = value;
                    else if (key == "user_password") user_password = value;
                    else if (key == "timezone") timezone = value;
                    else if (key == "keyboard_layout") keyboard_layout = value;
                    else if (key == "current_distro") current_distro_name = value;
                    else if (key == "extra_packages") extra_packages = value;
                }
            }
            file.close();
            std::cout << COLOR_GREEN << "Configuration loaded from " << configFile << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_YELLOW << "No existing configuration found. Starting with default settings." << COLOR_RESET << std::endl;
        }
    }

    std::string getCurrentDir() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            return std::string(cwd);
        }
        return ".";
    }

    std::string getTargetFolder() {
        return getCurrentDir() + "/claudemods-distro";
    }

    std::string getCalamaresFolder() {
        return getCurrentDir() + "/calamares-claudemods";
    }

    bool directoryExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
    }

    bool fileExists(const std::string& path) {
        struct stat info;
        return (stat(path.c_str(), &info) == 0);
    }

    bool extractRequiredFiles() {
        std::cout << COLOR_CYAN << "Checking for required folders..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::string calamaresFolder = getCalamaresFolder();

        bool buildImageExists = directoryExists(calamaresFolder + "/build-image-arch-img");
        bool calamaresFilesExists = directoryExists(calamaresFolder + "/calamares-files");
        bool workingHooksExists = directoryExists(calamaresFolder + "/working-hooks-btrfs-ext4");

        if (buildImageExists && calamaresFilesExists && workingHooksExists) {
            std::cout << COLOR_GREEN << "All required folders already exist." << COLOR_RESET << std::endl;
            return true;
        }

        if (!directoryExists(calamaresFolder)) {
            std::cout << COLOR_CYAN << "Creating calamares-claudemods folder..." << COLOR_RESET << std::endl;
            std::string createCmd = "sudo mkdir -p " + calamaresFolder;
            execute_command(createCmd);
        }

        std::string sourceDir = currentDir + "/calamares-per-distro/claudemods";
        std::vector<std::string> zipFiles = {
            "calamares-claudemods.zip",
            "claudemods.zip",
            "build-image-claudemods.zip"
        };

        for (const auto& zipFile : zipFiles) {
            std::string sourcePath = sourceDir + "/" + zipFile;

            if (fileExists(sourcePath)) {
                std::cout << COLOR_CYAN << "Extracting " << zipFile << "..." << COLOR_RESET << std::endl;
                std::string extractCmd = "sudo unzip -q " + sourcePath + " -d " + calamaresFolder + " >/dev/null 2>&1";
                execute_command(extractCmd);
                std::cout << COLOR_GREEN << "Done." << COLOR_RESET << std::endl;
            }
        }

        std::cout << COLOR_GREEN << "Extraction process completed." << COLOR_RESET << std::endl;
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
        std::cout << COLOR_CYAN << "claudemods handheld distribution iso creator Beta v1.01 14-01-2026" << COLOR_RESET << std::endl;
        std::cout << std::endl;
    }

    void setup_terminal() {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    void restore_terminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    int get_arrow_key() {
        int ch = getchar();
        if (ch == 27) {
            getchar();
            ch = getchar();
            return ch;
        }
        return ch;
    }

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

                std::string menu_line;
                if (title == "claudemods handheld distribution iso creator") {
                    std::string setting_value;
                    switch(i) {
                        case 0: setting_value = "current directory as claudemods-distro"; break;
                        case 1: setting_value = new_username.empty() ? "[Not Set]" : new_username; break;
                        case 2: setting_value = root_password.empty() ? "[Not Set]" : root_password; break;
                        case 3: setting_value = user_password.empty() ? "[Not Set]" : user_password; break;
                        case 4: setting_value = timezone.empty() ? "[Not Set]" : timezone; break;
                        case 5: setting_value = keyboard_layout.empty() ? "[Not Set]" : keyboard_layout; break;
                        case 6: setting_value = current_distro_name.empty() ? "[Not Set]" : current_distro_name; break;
                        case 7: setting_value = extra_packages.empty() ? "[Not Set]" : extra_packages; break;
                        default: setting_value = ""; break;
                    }

                    menu_line = options[i] + " " + setting_value;
                } else {
                    menu_line = options[i];
                }

                if (menu_line.length() > 80) {
                    menu_line = menu_line.substr(0, 77) + "...";
                }

                std::cout << std::left << std::setw(56) << menu_line;
                std::cout << "║" << std::endl;
            }

            std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << COLOR_RESET;
            std::cout << COLOR_YELLOW << "Use ↑↓ arrows to navigate, Enter to select" << COLOR_RESET << std::endl;

            int key = get_arrow_key();
            if (key == 'A') {
                selected = (selected - 1 + options.size()) % options.size();
            } else if (key == 'B') {
                selected = (selected + 1) % options.size();
            } else if (key == 10) {
                restore_terminal();
                return selected;
            }
        }
    }

    std::string get_input(const std::string& prompt) {
        std::cout << COLOR_CYAN << prompt << COLOR_RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    int execute_command(const std::string& cmd) {
        std::cout << COLOR_CYAN << std::flush;
        int status = system(cmd.c_str());
        std::cout << COLOR_RESET << std::flush;
        return status;
    }

    void display_current_settings() {
        std::cout << COLOR_YELLOW << "\nCurrent Settings:" << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << "Installation Path: " << COLOR_RESET << "current directory as claudemods-distro" << std::endl;
        std::cout << COLOR_CYAN << "Username: " << COLOR_RESET
        << (new_username.empty() ? "[Not Set]" : new_username) << std::endl;
        std::cout << COLOR_CYAN << "Root Password: " << COLOR_RESET
        << (root_password.empty() ? "[Not Set]" : root_password) << std::endl;
        std::cout << COLOR_CYAN << "User Password: " << COLOR_RESET
        << (user_password.empty() ? "[Not Set]" : user_password) << std::endl;
        std::cout << COLOR_CYAN << "Timezone: " << COLOR_RESET
        << (timezone.empty() ? "[Not Set]" : timezone) << std::endl;
        std::cout << COLOR_CYAN << "Keyboard Layout: " << COLOR_RESET
        << (keyboard_layout.empty() ? "[Not Set]" : keyboard_layout) << std::endl;
        std::cout << COLOR_CYAN << "Current Distro: " << COLOR_RESET
        << (current_distro_name.empty() ? "[Not Set]" : current_distro_name) << std::endl;
        std::cout << COLOR_CYAN << "Extra Packages: " << COLOR_RESET
        << (extra_packages.empty() ? "[Not Set]" : extra_packages) << std::endl;
        std::cout << std::endl;
    }

    bool setup_target_directory(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Creating target directory: " << target_folder << COLOR_RESET << std::endl;
        execute_command("sudo mkdir -p " + target_folder);

        struct stat info;
        if (stat(target_folder.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "Failed to create target directory: " << target_folder << COLOR_RESET << std::endl;
            return false;
        }

        if (!(info.st_mode & S_IFDIR)) {
            std::cerr << COLOR_RED << "Target path is not a directory: " << target_folder << COLOR_RESET << std::endl;
            return false;
        }

        std::cout << COLOR_GREEN << "Target directory created successfully!" << COLOR_RESET << std::endl;

        execute_command("sudo mkdir -p " + target_folder + "/usr/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/");
        execute_command("sudo mkdir -p " + target_folder + "/usr/lib/initcpio/udev/");
        execute_command("sudo mkdir -p " + target_folder + "/etc/sysctl.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules " + target_folder + "/usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/11-dm-initramfs.rules /usr/lib/initcpio/udev/11-dm-initramfs.rules");
        execute_command("sudo cp -r " + currentDir + "/needed-files/80-gamecompatibility.conf " + target_folder + "/etc/sysctl.d/80-gamecompatibility.conf");
        execute_command("sudo mkdir -p " + target_folder + "/etc/pacman.d");
        
        // SYSTEMD-BOOT DIRECTORIES ONLY - NO GRUB SHIT
        execute_command("sudo mkdir -p " + target_folder + "/boot/loader/entries");
        execute_command("sudo mkdir -p " + target_folder + "/efi/EFI/systemd");
        
        execute_command("sudo mkdir -p " + target_folder + "/usr/share/plymouth/themes");
        execute_command("sudo mkdir -p " + target_folder + "/usr/local/bin");
        execute_command("sudo mkdir -p " + target_folder + "/etc/systemd/system");
        execute_command("sudo mkdir -p " + target_folder + "/etc/sddm.conf.d");

        return true;
    }

    void setup_pacman_and_files(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();

        execute_command("sudo cp -r " + currentDir + "/needed-files/vconsole.conf " + target_folder + "/etc/vconsole.conf");
        execute_command("sudo cp -r /etc/resolv.conf " + target_folder + "/etc/resolv.conf");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d " + target_folder + "/etc/pacman.d");
        execute_command("sudo unzip -o " + currentDir + "/needed-files/pacman.d.zip -d /etc/pacman.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf " + target_folder + "/etc/pacman.conf");
        execute_command("sudo cp -r " + currentDir + "/needed-files/pacman.conf /etc/pacman.conf");

        execute_command("sudo pacman -Sy");
    }

    bool verify_pacstrap_success(const std::string& target_folder) {
        struct stat info;
        std::string test_bin = target_folder + "/bin/bash";
        if (stat(test_bin.c_str(), &info) != 0) {
            std::cerr << COLOR_RED << "pacstrap failed! /bin/bash not found in target." << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void setup_boot_directory(const std::string& target_folder) {
        // SYSTEMD-BOOT ONLY - NO GRUB
        execute_command("sudo mkdir -p " + target_folder + "/boot/loader/entries");
        execute_command("sudo mkdir -p " + target_folder + "/efi/EFI/systemd");
    }

    void create_user_home_structure(const std::string& target_folder) {
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.config/fish");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("sudo mkdir -p " + target_folder + "/home/" + new_username + "/.local/share");

        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");
    }

    void fix_user_places_xbel(const std::string& target_folder) {
        std::string cmd = "sudo ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
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
                std::string sed_cmd = "sudo sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }
    }

    void install_calamares(const std::string& target_folder) {
        std::string currentDir = getCurrentDir();

        std::cout << COLOR_CYAN << "Installing Calamares installer..." << COLOR_RESET << std::endl;

        execute_command("sudo cp -r " + currentDir + "/calamares-claudemods/calamares-files/calamares " + target_folder + "/etc/");
        execute_command("sudo cp -r " + currentDir + "/calamares-claudemods/calamares-files/claudemods " + target_folder + "/usr/share/calamares/branding/");
        execute_command("sudo cp -r " + currentDir + "/calamares-claudemods/calamares-files/extras/* " + target_folder + "/");
        execute_command("sudo cp -r " + currentDir + "/calamares-claudemods/working-hooks-btrfs-ext4/* /etc/initcpio");
        execute_command("sudo cp " + currentDir + "/calamares-claudemods/calamares-files/mount.conf " + target_folder + "/usr/share/calamares/modules");

        execute_command("sudo cp " + currentDir + "/needed-files/Calamares " + target_folder + "/home/" + new_username + "/Desktop/Calamares");
        execute_command("sudo cp " + currentDir + "/needed-files/rsync-installer " + target_folder + "/home/" + new_username + "/Desktop/rsync-installer");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/Calamares");
        execute_command("sudo chmod +x " + target_folder + "/home/" + new_username + "/Desktop/rsync-installer");
        execute_command("sudo mkdir -p " + target_folder + "/opt/rsync-installer");
        execute_command("sudo tar xzf " + currentDir + "/needed-files/rsync-installer.tar.gz -C " + target_folder + "/opt/rsync-installer");
        execute_command("sudo cp -r " + currentDir + "/needed-files/wireless-regdom " + target_folder + "/etc/conf.d/wireless-regdom");

        execute_command("sudo rm -rf " + target_folder + "/usr/share/calamares/branding/manjaro");

        std::cout << COLOR_GREEN << "Calamares installation completed!" << COLOR_RESET << std::endl;
    }

    void create_squashfs_image(const std::string& distro_name) {
        std::cout << COLOR_CYAN << "Creating squashfs image..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::string target_folder = getTargetFolder();

        std::cout << COLOR_CYAN << "Cleaning pacman cache..." << COLOR_RESET << std::endl;
        std::string cache_clean_cmd = "sudo rm -rf " + target_folder + "/var/cache/pacman/pkg/*";
        if (execute_command(cache_clean_cmd) == 0) {
            std::cout << COLOR_GREEN << "Pacman cache cleaned successfully!" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to clean pacman cache!" << COLOR_RESET << std::endl;
        }

        std::string squashfs_cmd = "sudo mksquashfs " + target_folder + " " + currentDir + "/calamares-claudemods/build-image-arch-img/LiveOS/rootfs.img -noappend -comp xz -b 256K -Xbcj x86 -e etc/udev/rules.d/70-persistent-cd.rules -e etc/udev/rules.d/70-persistent-net.rules -e etc/mtab -e etc/fstab -e dev/* -e proc/* -e sys/* -e tmp/* -e run/* -e mnt/* -e media/* -e lost+found";

        std::cout << COLOR_CYAN << "Executing: " << squashfs_cmd << COLOR_RESET << std::endl;

        if (execute_command(squashfs_cmd) == 0) {
            std::cout << COLOR_GREEN << "Squashfs image created successfully!" << COLOR_RESET << std::endl;

            std::cout << COLOR_CYAN << "Cleaning up target folder..." << COLOR_RESET << std::endl;
            std::string cleanup_cmd = "sudo rm -rf " + target_folder;
            if (execute_command(cleanup_cmd) == 0) {
                std::cout << COLOR_GREEN << "Target folder deleted successfully: " << target_folder << COLOR_RESET << std::endl;
            } else {
                std::cout << COLOR_RED << "Failed to delete target folder: " << target_folder << COLOR_RESET << std::endl;
            }

            std::cout << COLOR_CYAN << "Searching for kernel images..." << COLOR_RESET << std::endl;

            std::vector<std::string> kernel_files;
            std::string find_cmd = "find /boot -name 'vmlinuz-*' -type f 2>/dev/null | sort";
            FILE* pipe = popen(find_cmd.c_str(), "r");
            if (pipe) {
                char buffer[256];
                while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    std::string file_path = buffer;
                    file_path.erase(std::remove(file_path.begin(), file_path.end(), '\n'), file_path.end());
                    if (!file_path.empty()) kernel_files.push_back(file_path);
                }
                pclose(pipe);
            }

            if (kernel_files.empty()) {
                std::cout << COLOR_RED << "No kernel images found!" << COLOR_RESET << std::endl;
            } else {
                std::cout << COLOR_CYAN << "Available kernels:" << COLOR_RESET << std::endl;
                for (size_t i = 0; i < kernel_files.size(); i++) {
                    std::cout << "[" << i + 1 << "] " << kernel_files[i] << std::endl;
                }

                std::cout << COLOR_CYAN << "Select kernel (1-" << kernel_files.size() << "): " << COLOR_RESET;
                std::string input;
                std::getline(std::cin, input);

                try {
                    int choice = std::stoi(input);
                    if (choice >= 1 && choice <= kernel_files.size()) {
                        std::string copy_cmd = "sudo cp " + kernel_files[choice - 1] + " " + currentDir + "/calamares-claudemods/build-image-arch-img/boot/vmlinuz-x86_64";
                        if (execute_command(copy_cmd) == 0) {
                            std::cout << COLOR_GREEN << "Kernel copied successfully!" << COLOR_RESET << std::endl;
                        } else {
                            std::cout << COLOR_RED << "Failed to copy kernel!" << COLOR_RESET << std::endl;
                        }
                    }
                } catch (...) {
                    std::cout << COLOR_RED << "Invalid selection!" << COLOR_RESET << std::endl;
                }
            }

            std::cout << COLOR_CYAN << "Generating initramfs..." << COLOR_RESET << std::endl;
            std::string initramfs_cmd = "cd " + currentDir + "/calamares-claudemods/build-image-arch-img && sudo mkinitcpio -c mkinitcpio.conf -g " + currentDir + "/calamares-claudemods/build-image-arch-img/boot/initramfs-x86_64.img";
            if (execute_command(initramfs_cmd) == 0) {
                std::cout << COLOR_GREEN << "Initramfs generated successfully!" << COLOR_RESET << std::endl;
                create_iso_image(distro_name);
            } else {
                std::cout << COLOR_RED << "Failed to generate initramfs!" << COLOR_RESET << std::endl;
            }

        } else {
            std::cout << COLOR_RED << "Failed to create squashfs image!" << COLOR_RESET << std::endl;
        }
    }

    void create_iso_image(const std::string& distro_name) {
        std::cout << COLOR_CYAN << "Creating ISO image with XORRISO..." << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();

        // Build the XORRISO command with the distro name as ISO filename
        std::string xorriso_cmd = "sudo xorriso -as mkisofs "
        "--modification-date=\"$(date +%Y%m%d%H%M%S00)\" "
        "--protective-msdos-label "
        "-volid \"2025\" "
        "-appid \"claudemods Linux Live/Rescue CD\" "
        "-publisher \"claudemods claudemods101@gmail.com >\" "
        "-preparer \"Prepared by user\" "
        "-r -graft-points -no-pad "
        "--sort-weight 0 / "
        "--sort-weight 1 /boot "
        "--grub2-mbr " + currentDir + "/calamares-claudemods/build-image-arch-img/boot/grub/i386-pc/boot_hybrid.img "
        "-partition_offset 16 "
        "-b boot/grub/i386-pc/eltorito.img "
        "-c boot.catalog "
        "-no-emul-boot -boot-load-size 4 -boot-info-table --grub2-boot-info "
        "-eltorito-alt-boot "
        "-append_partition 2 0xef " + currentDir + "/calamares-claudemods/build-image-arch-img/boot/efi.img "
        "-e --interval:appended_partition_2:all:: "
        "-no-emul-boot "
        "-iso-level 3 "
        "-o \"" + currentDir + "/" + distro_name + ".iso\" " +
        currentDir + "/calamares-claudemods/build-image-arch-img/";

        std::cout << COLOR_CYAN << "Executing: " << xorriso_cmd << COLOR_RESET << std::endl;

        if (execute_command(xorriso_cmd) == 0) {
            std::cout << COLOR_GREEN << "ISO image created successfully: " << distro_name + ".iso" << COLOR_RESET << std::endl;
        } else {
            std::cout << COLOR_RED << "Failed to create ISO image!" << COLOR_RESET << std::endl;
        }
    }

    void set_username() {
        new_username = get_input("Enter username: ");
        saveConfiguration();
    }

    void set_root_password() {
        root_password = get_input("Enter root password: ");
        saveConfiguration();
    }

    void set_user_password() {
        user_password = get_input("Enter user password: ");
        saveConfiguration();
    }

    void set_timezone() {
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
        saveConfiguration();
    }

    void set_keyboard_layout() {
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
        saveConfiguration();
    }

    void set_extra_packages() {
        extra_packages = get_input("Enter extra packages (space separated): ");
        saveConfiguration();
    }

    bool check_settings_configured() {
        if (new_username.empty()) {
            std::cout << COLOR_RED << "Error: Username not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (root_password.empty()) {
            std::cout << COLOR_RED << "Error: Root password not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (user_password.empty()) {
            std::cout << COLOR_RED << "Error: User password not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (timezone.empty()) {
            std::cout << COLOR_RED << "Error: Timezone not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (keyboard_layout.empty()) {
            std::cout << COLOR_RED << "Error: Keyboard layout not set!" << COLOR_RESET << std::endl;
            return false;
        }
        if (current_distro_name.empty()) {
            std::cout << COLOR_RED << "Error: No distribution selected!" << COLOR_RESET << std::endl;
            return false;
        }
        return true;
    }

    void mount_system_dirs() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo mkdir -p " + target_folder + "/dev");
        execute_command("sudo mkdir -p " + target_folder + "/dev/pts");
        execute_command("sudo mkdir -p " + target_folder + "/proc");
        execute_command("sudo mkdir -p " + target_folder + "/sys");
        execute_command("sudo mkdir -p " + target_folder + "/run");
        execute_command("sudo mkdir -p " + target_folder + "/etc");

        execute_command("sudo mount --bind /dev " + target_folder + "/dev");
        execute_command("sudo mount --bind /dev/pts " + target_folder + "/dev/pts");
        execute_command("sudo mount --bind /proc " + target_folder + "/proc");
        execute_command("sudo mount --bind /sys " + target_folder + "/sys");
        execute_command("sudo mount --bind /run " + target_folder + "/run");
    }

    void unmount_system_dirs() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo umount " + target_folder + "/dev/pts");
        execute_command("sudo umount " + target_folder + "/dev");
        execute_command("sudo umount " + target_folder + "/proc");
        execute_command("sudo umount " + target_folder + "/sys");
        execute_command("sudo umount " + target_folder + "/run");
    }

    void create_user() {
        std::string target_folder = getTargetFolder();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");
    }

    void apply_timezone_keyboard_settings() {
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Setting timezone to: " << timezone << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"ln -sf /usr/share/zoneinfo/" + timezone + " /etc/localtime\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"hwclock --systohc\"");

        std::cout << COLOR_CYAN << "Setting keyboard layout to: " << keyboard_layout << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'KEYMAP=" + keyboard_layout + "' > /etc/vconsole.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'LANG=en_US.UTF-8' > /etc/locale.conf\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"locale-gen\"");
    }

    void start_installation() {
        if (!check_settings_configured()) {
            std::cout << COLOR_RED << "Cannot proceed with installation. Please configure all settings first." << COLOR_RESET << std::endl;
            return;
        }

        if (current_distro_name == "Spitfire-CKGE-Minimal") {
            install_spitfire_ckge_minimal();
        } else {
            std::cout << COLOR_RED << "No valid distribution selected!" << COLOR_RESET << std::endl;
        }
    }

    void install_spitfire_ckge_minimal() {
        std::cout << COLOR_ORANGE << "Installing Spitfire CKGE Minimal..." << COLOR_RESET << std::endl;
        std::string target_folder = getTargetFolder();
        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Minimal installation in: " << target_folder << COLOR_RESET << std::endl;

        std::string currentDir = getCurrentDir();
        std::cout << COLOR_CYAN << "Working from directory: " << currentDir << COLOR_RESET << std::endl;

        if (!setup_target_directory(target_folder)) return;
        setup_pacman_and_files(target_folder);

        std::cout << COLOR_CYAN << "Installing base system with pacstrap..." << COLOR_RESET << std::endl;
        // ADD SYSTEMD-BOOT AND EFIBOOTMGR PACKAGES
        std::string pacstrap_cmd = "sudo pacstrap " + target_folder + " claudemods-handheld calamares-fix lutris hhd adjustor hhd-ui openrgb";
        if (!extra_packages.empty()) {
            pacstrap_cmd += " " + extra_packages;
        }
        execute_command(pacstrap_cmd);

        if (!verify_pacstrap_success(target_folder)) return;
        
        // NO NEED FOR SETUP_BOOT_DIRECTORY HERE - ALREADY DONE IN SETUP_TARGET_DIRECTORY

        mount_system_dirs();
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");
        
        // INSTALL SYSTEMD-BOOT PROPERLY
        std::cout << COLOR_CYAN << "Installing systemd-boot..." << COLOR_RESET << std::endl;
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"bootctl install\"");
        
        // CREATE SYSTEMD-BOOT CONFIG FILES
        std::cout << COLOR_CYAN << "Creating systemd-boot configuration..." << COLOR_RESET << std::endl;
        
        // Create loader.conf
        std::string loader_conf = target_folder + "/boot/loader/loader.conf";
        std::ofstream loader_file(loader_conf);
        if (loader_file.is_open()) {
            loader_file << "default arch.conf\n";
            loader_file << "timeout 5\n";
            loader_file << "console-mode max\n";
            loader_file << "editor no\n";
            loader_file.close();
        }
        
        // Create arch.conf boot entry
        std::string arch_conf = target_folder + "/boot/loader/entries/arch.conf";
        std::ofstream arch_file(arch_conf);
        if (arch_file.is_open()) {
            arch_file << "title   Arch Linux\n";
            arch_file << "linux   /vmlinuz-linux\n";
            arch_file << "initrd  /initramfs-linux.img\n";
            arch_file << "options root=LABEL=arch_iso rw quiet splash\n";
            arch_file.close();
        }
        
        // Create arch-fallback.conf boot entry
        std::string fallback_conf = target_folder + "/boot/loader/entries/arch-fallback.conf";
        std::ofstream fallback_file(fallback_conf);
        if (fallback_file.is_open()) {
            fallback_file << "title   Arch Linux (fallback)\n";
            fallback_file << "linux   /vmlinuz-linux\n";
            fallback_file << "initrd  /initramfs-linux-fallback.img\n";
            fallback_file << "options root=LABEL=arch_iso rw quiet splash\n";
            fallback_file.close();
        }

        apply_timezone_keyboard_settings();

        // REMOVE ALL GRUB REFERENCES - USE SYSTEMD-BOOT FILES INSTEAD
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.sh " + target_folder + "/usr/local/bin/term.sh");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/term.service");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        create_user();
        create_user_home_structure(target_folder);

        execute_command("cd " + target_folder);
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-full-v1.01.zip");
        execute_command("sudo wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool-v1.01.zip");
        execute_command("sudo unzip -o " + currentDir + "/Arch-Systemtool-v1.01.zip -d " + target_folder + "/opt");
        execute_command("sudo unzip -o " + currentDir + "/spitfire-full-v1.01.zip -d " + target_folder + "/home/" + new_username + "/");
        execute_command("sudo mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d/kde_settings.conf");
        execute_command("sudo cp " + currentDir + "/needed-files/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("sudo chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("sudo chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/konsolerc");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes/SpitFireLogin");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole/claudemods-cyan.colorscheme");
        execute_command("sudo cp -r " + currentDir + "/needed-files/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole/claudemods-cyan.profile");
        execute_command("sudo rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        fix_user_places_xbel(target_folder);

        install_calamares(target_folder);

        unmount_system_dirs();
        std::cout << COLOR_GREEN << "Spitfire CKGE Minimal installation completed in: " << target_folder << COLOR_RESET << std::endl;

        create_squashfs_image("Spitfire-CKGE-Minimal");
    }

    void show_distro_selection() {
        std::vector<std::string> distro_options = {
            "Install Spitfire CKGE Minimal",
            "Back to Main Menu"
        };

        int selected = 0;
        while (true) {
            selected = show_menu(distro_options, "Select Distribution to Install", selected);

            switch(selected) {
                case 0:
                    current_distro_name = "Spitfire-CKGE-Minimal";
                    saveConfiguration();
                    std::cout << COLOR_GREEN << "Spitfire CKGE Minimal selected. Use 'Start Installation' to begin." << COLOR_RESET << std::endl;
                    return;
                case 1:
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

    void show_main_menu() {
        std::vector<std::string> main_options = {
            "Installation Path: ",
            "Set Username",
            "Set Root Password",
            "Set User Password",
            "Set Timezone",
            "Set Keyboard Layout",
            "Select Distro to Install",
            "Install Extra Packages",
            "Start Installation",
            "Exit"
        };

        int selected = 0;
        while (true) {
            system("clear");
            display_header();
            display_current_settings();

            selected = show_menu(main_options, "claudemods handheld distribution iso creator", selected);

            switch(selected) {
                case 0:
                    break;
                case 1:
                    set_username();
                    break;
                case 2:
                    set_root_password();
                    break;
                case 3:
                    set_user_password();
                    break;
                case 4:
                    set_timezone();
                    break;
                case 5:
                    set_keyboard_layout();
                    break;
                case 6:
                    show_distro_selection();
                    break;
                case 7:
                    set_extra_packages();
                    break;
                case 8:
                    start_installation();
                    break;
                case 9:
                    std::cout << COLOR_GREEN << "Exiting. Goodbye!" << COLOR_RESET << std::endl;
                    return;
            }

            std::cout << COLOR_CYAN << "Press Enter to continue..." << COLOR_RESET;
            std::cin.get();
        }
    }

public:
    void run() {
        loadConfiguration();

        if (!extractRequiredFiles()) {
            std::cerr << COLOR_RED << "Failed to extract required files. Cannot continue." << COLOR_RESET << std::endl;
            return;
        }

        display_header();
        show_main_menu();
    }
};

#endif // CLAUDEMODSHANDHELD_H
