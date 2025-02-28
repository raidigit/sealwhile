#!/bin/bash
# Sealwhile Auto-Update Script
# GitHub: https://github.com/raidigit/tuskwm

log() {
    echo "[$(date '+%Y-%m-%d %H:%M-%S')] $1"
}

check_deps() {
    for cmd in curl tar make sudo; do
        if ! command -v $cmd &> /dev/null; then
            log "ERROR: Required command missing: $cmd"
            exit 1
        fi
    done
}

select_version() {
    PS3="Select version type: "
    select type in "User (stable)" "Dev (pre-release)"; do
        case $type in
            "User (stable)") 
                RELEASE_TYPE="stable"
                API_ENDPOINT="https://api.github.com/repos/raidigit/sealwhile/releases/latest"
                break
                ;;
            "Dev (pre-release)")
                RELEASE_TYPE="pre"
                API_ENDPOINT="https://api.github.com/repos/raidigit/sealwhile/releases"
                break
                ;;
            *) echo "Invalid option";;
        esac
    done
}

get_latest_tag() {
    if [ "$RELEASE_TYPE" = "stable" ]; then
        curl -s $API_ENDPOINT | grep '"tag_name":' | sed 's/.*"\([^"]*\)".*/\1/'
    else
        curl -s $API_ENDPOINT | grep -B 10 '"prerelease": true' | grep '"tag_name":' | head -1 | sed 's/.*"\([^"]*\)".*/\1/'
    fi
}

main() {
    log "=== Starting sealwhile Update ==="
    
    # 1. Version selection
    select_version
    log "Selected $RELEASE_TYPE version channel"

    # 2. Dependency check
    log "Checking system dependencies..."
    check_deps

    # 3. Version detection
    CURRENT_VER=$(git describe --tags 2>/dev/null || echo "v0.0.0")
    log "Current version: $CURRENT_VER"

    # 4. Get latest release info
    log "Querying GitHub API..."
    LATEST_VER=$(get_latest_tag)
    
    if [ -z "$LATEST_VER" ]; then
        log "ERROR: No releases found"
        exit 2
    fi
    log "Latest available version: $LATEST_VER"

    # 5. Version comparison
    if [ "$CURRENT_VER" = "$LATEST_VER" ]; then
        log "Already up-to-date"
        exit 0
    fi

    # 6. Download package
    DOWNLOAD_URL="https://github.com/raidigit/sealwhile/archive/refs/tags/$LATEST_VER.tar.gz"
    log "Downloading $DOWNLOAD_URL"
    curl -L -o update_temp.tar.gz "$DOWNLOAD_URL" || {
        log "ERROR: Download failed"
        exit 3
    }

    # 7. Extract archive
    log "Extracting files..."
    tar -xzf update_temp.tar.gz || {
        log "ERROR: Extraction failed"
        exit 4
    }

    # 8. Preserve update script
    log "Backing up update.sh..."
    [ -f ../update.sh ] && cp ../update.sh .

    # 9. Compile and install
    log "Building project..."
    cd "sealwhile-${LATEST_VER#v}" || {
        log "ERROR: Source directory missing"
        exit 5
    }

    make clean && make || {
        log "ERROR: Build failed"
        exit 6
    }

    log "Installing..."
    sudo make install || {
        log "ERROR: Installation failed"
        exit 7
    }

    # 10. Restore update script
    log "Restoring update.sh..."
    [ -f update.sh ] && cp update.sh ..

    # 11. Cleanup
    log "Cleaning temporary files..."
    cd ..
    rm -rf "sealwhile-${LATEST_VER#v}" update_temp.tar.gz

    log "=== Successfully updated to $LATEST_VER ==="
}

main
