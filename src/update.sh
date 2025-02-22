#!/bin/bash
# TuskWM Update Script
# GitHub: https://github.com/raidigit/tuskwm
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Check Deps
check_deps() {
    for cmd in curl tar make sudo; do
        if ! command -v $cmd &> /dev/null; then
            log "Error: please, install $cmd"
            exit 1
        fi
    done
}

# Main
main() {
    log "=== Starting update service TuskWM ==="
    
    # 1. Check deps
    log "Checking system deps..."
    check_deps

    # 2. Version check
    CURRENT_VER=$(git describe --tags 2>/dev/null || echo "v0.0.0")
    log "Current version: $CURRENT_VER"

    # 3. Get latest release info
    log "Response GitHub API..."
    API_RESPONSE=$(curl -s https://api.github.com/repos/raidigit/tuskwm/releases/latest)
    
    LATEST_VER=$(echo "$API_RESPONSE" | grep '"tag_name":' | sed 's/.*"\([^"]*\)".*/\1/')
    log "Latest version: $LATEST_VER"

    # 4. Version need
    if [ "$CURRENT_VER" = "$LATEST_VER" ]; then
        log "You have latest update"
        exit 0
    fi

    # 5. Download archive
    DOWNLOAD_URL="https://github.com/raidigit/tuskwm/archive/refs/tags/$LATEST_VER.tar.gz"
    log "Downloading $DOWNLOAD_URL"
    curl -L -o update_temp.tar.gz "$DOWNLOAD_URL" || {
        log "Error: Unable to download archive"
        exit 2
    }

    # 6. Unarchive
    log "Unarchive..."
    tar -xzf update_temp.tar.gz || {
        log "Error: unable to unarchive"
        exit 3
    }

    # 7. Change dir
    cd "tuskwm-${LATEST_VER#v}" || {
        log "Error: temp directory not found"
        exit 4
    }

    # 8. Save update.sh
    log "Save old update.sh..."
    [ -f ../update.sh ] && cp ../update.sh .

    # 9. Compiling
    log "Compiling..."
    make clean && make || {
        log "Error: compile error"
        exit 5
    }

    log "Installling..."
    sudo make install || {
        log "Error: installation error"
        exit 6
    }

    # 10. Restore update.sh
    log "Restore update.sh..."
    [ -f update.sh ] && cp update.sh ..

    # 11. Cleaning
    log "Deleting temp dirs..."
    cd ..
    rm -rf "tuskwm-${LATEST_VER#v}" update_temp.tar.gz

    log "=== Updated to $LATEST_VER success! ==="
}

# start
main
