#!/bin/bash -eux

# Setup codesigning
# Thanks https://www.update.rocks/blog/osx-signing-with-travis/
(
    set +x
    KEY_CHAIN=build.keychain

    security create-keychain -p travis $KEY_CHAIN
    security default-keychain -s $KEY_CHAIN
    security unlock-keychain -p travis $KEY_CHAIN

    echo "Test password: '$MAC_CODESIGN_PASSWORD'"
    security import $CODESIGN_SECUREFILEPATH -k $KEY_CHAIN -P $MAC_CODESIGN_PASSWORD -T /usr/bin/codesign
    security set-key-partition-list -S apple-tool:,apple: -s -k travis $KEY_CHAIN

    rm -rf *.p12
)

set +e

export HOMEBREW_NO_AUTO_UPDATE=1
brew install gnu-tar ninja
wget -nv https://github.com/jcelerier/cninja/releases/download/v3.7.4/cninja-v3.7.4-macOS.tar.gz -O cninja.tgz &

wget -nv https://github.com/phracker/MacOSX-SDKs/releases/download/10.15/MacOSX10.15.sdk.tar.xz &

SDK_ARCHIVE=sdk-macOS.tar.gz
wget -nv https://github.com/ossia/score-sdk/releases/download/sdk18/$SDK_ARCHIVE -O $SDK_ARCHIVE
sudo mkdir -p /opt/ossia-sdk
sudo chown -R $(whoami) /opt
sudo chmod -R a+rwx /opt
gtar xhaf $SDK_ARCHIVE --strip-components=2 --directory /opt/ossia-sdk/
sudo rm -rf /Library/Developer/CommandLineTools
sudo rm -rf /usr/local/include/c++

wait wget || true
gtar xhaf cninja.tgz
sudo cp -rf cninja /usr/local/bin/

echo "Copying sdks..."
gtar xhaf MacOSX10.15.sdk.tar.xz
mv MacOSX10.15.sdk /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
sudo cp -rf cninja /usr/local/bin/
set -e
