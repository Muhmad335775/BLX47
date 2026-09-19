#!/bin/bash
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

APK_PATH="android/app/build/outputs/apk/release/app-release.apk"
AAB_PATH="android/app/build/outputs/bundle/release/app-release.aab"

pass() { echo -e "${GREEN}✅ $1${NC}"; }
fail() { echo -e "${RED}❌ $1${NC}"; exit 1; }

echo "== BLX47 Build Verification =="

[ -f "$APK_PATH" ] && pass "APK file exists" || fail "APK file missing"
[ -f "$AAB_PATH" ] && pass "AAB file exists" || fail "AAB file missing"

apksigner verify --print-certs "$APK_PATH" > /tmp/sig_check.txt 2>&1
if grep -q "blx47" /tmp/sig_check.txt; then
    pass "APK signed with BLX47 release key"
else
    fail "APK NOT signed with the correct release key"
fi

SIZE=$(stat -c%s "$APK_PATH" 2>/dev/null || stat -f%z "$APK_PATH")
if [ "$SIZE" -gt 1000000 ]; then
    pass "APK size OK ($((SIZE / 1024 / 1024)) MB)"
else
    fail "APK too small — likely broken build"
fi

echo -e "${GREEN}== ALL CHECKS PASSED — SAFE TO UPLOAD ==${NC}"
