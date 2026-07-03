#!/bin/bash

# WiFi selector for Arch - works alongside NetworkManager
# Uses nmcli (part of NetworkManager) - already on your system

# Check if running as root
if [[ $EUID -ne 0 ]]; then
    echo "Please run with: sudo $0"
    exit 1
fi

# Function to check internet connectivity
check_internet() {
    # Try multiple methods to verify internet connectivity
    if ping -c 1 -W 2 8.8.8.8 >/dev/null 2>&1; then
        return 0
    elif ping -c 1 -W 2 1.1.1.1 >/dev/null 2>&1; then
        return 0
    elif nmcli -t -f STATE general status 2>/dev/null | grep -q "connected"; then
        return 0
    else
        return 1
    fi
}

# Function to run the ISO creator
run_iso_creator() {
    echo ""
    echo "═══════════════════════════════════════════════════════════"
    echo "  Internet connection detected! Launching..."
    echo "═══════════════════════════════════════════════════════════"
    echo ""
    sudo ./claudemods-distro-iso-creator
    exit $?
}

# Check if already connected to internet
echo "Checking internet connectivity..."
if check_internet; then
    echo "Already connected to the internet!"
    run_iso_creator
fi

echo "No internet connection detected. Launching WiFi selector..."
echo ""

# Check if NetworkManager is running
if ! systemctl is-active --quiet NetworkManager; then
    echo "NetworkManager is not running. Starting it..."
    systemctl start NetworkManager
    sleep 2
fi

# Refresh WiFi scan
echo "Scanning for networks..."
nmcli device wifi rescan >/dev/null 2>&1
sleep 2

# Get available networks
mapfile -t networks < <(nmcli -t -f SSID,SIGNAL,SECURITY device wifi list | sort -t: -k2 -rn | uniq)

if [[ ${#networks[@]} -eq 0 ]]; then
    echo "No WiFi networks found!"
    exit 1
fi

# Build selection array
declare -a display=()
declare -a ssids=()
declare -a securities=()

for network in "${networks[@]}"; do
    IFS=':' read -r ssid signal security <<< "$network"
    [[ -z "$ssid" ]] && continue

    # Signal bars
    if [[ $signal -ge 80 ]]; then bars="████"
    elif [[ $signal -ge 60 ]]; then bars="███░"
    elif [[ $signal -ge 40 ]]; then bars="██░░"
    elif [[ $signal -ge 20 ]]; then bars="█░░░"
    else bars="░░░░"; fi

    display+=("$(printf "%-30s %s %3s%%  %s" "$ssid" "$bars" "$signal" "$security")")
    ssids+=("$ssid")
    securities+=("$security")
done

# Interactive menu with arrow keys
selected=0
total=${#display[@]}

while true; do
    clear
    echo "═══════════════════════════════════════════════════════════"
    echo "  WiFi Network Selector (↑↓ arrows, Enter to select, q to quit)"
    echo "═══════════════════════════════════════════════════════════"
    echo ""

    for i in "${!display[@]}"; do
        if [[ $i -eq $selected ]]; then
            echo -e "  \033[7;1m > ${display[$i]} \033[0m"
        else
            echo "    ${display[$i]}"
        fi
    done

    echo ""
    echo "═══════════════════════════════════════════════════════════"

    # Read key
    read -rsn1 key
    if [[ $key == $'\e' ]]; then
        read -rsn2 key
        case $key in
            '[A') ((selected--)); [[ $selected -lt 0 ]] && selected=$((total - 1)) ;;
            '[B') ((selected++)); [[ $selected -ge $total ]] && selected=0 ;;
        esac
    elif [[ $key == 'q' || $key == 'Q' ]]; then
        echo -e "\nExiting..."
        exit 0
    elif [[ $key == '' ]]; then
        break
    fi
done

clear
echo "Selected: ${ssids[$selected]}"
echo ""

# Connect
if [[ "${securities[$selected]}" == *"WPA"* ]] || [[ "${securities[$selected]}" == *"WEP"* ]]; then
    echo -n "Enter password: "
    read password
    echo ""

    if [[ -z "$password" ]]; then
        echo "No password entered. Exiting."
        exit 1
    fi

    echo "Connecting to ${ssids[$selected]}..."
    nmcli device wifi connect "${ssids[$selected]}" password "$password"
else
    echo "Connecting to ${ssids[$selected]} (open network)..."
    nmcli device wifi connect "${ssids[$selected]}"
fi

# Check result
if [[ $? -eq 0 ]]; then
    echo ""
    echo "════════════════════════════════════════════════"
    echo "  ✓ Connected successfully!"
    echo "════════════════════════════════════════════════"

    # Verify internet connectivity before launching ISO creator
    sleep 3  # Give connection time to fully establish
    echo ""
    if check_internet; then
        run_iso_creator
    else
        echo "⚠ Warning: Connected to WiFi but no internet access detected."
        echo "  Please check your connection and run the ISO creator manually:"
        echo "  sudo ./claudemods-distro-iso-creator"
        exit 1
    fi
else
    echo ""
    echo "✗ Connection failed. Check password and try again."
    exit 1
fi
