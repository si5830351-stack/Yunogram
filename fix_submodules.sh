#!/usr/bin/env bash
# Запускать из корня репозитория Yunogram (там, где лежит .git и .gitmodules)
set -e

echo "Удаляю пустые/сломанные папки сабмодулей, если они есть..."
declare -A SUBS=(
  ["Telegram/ThirdParty/GSL"]="https://github.com/Microsoft/GSL.git"
  ["Telegram/ThirdParty/xxHash"]="https://github.com/Cyan4973/xxHash.git"
  ["Telegram/ThirdParty/lz4"]="https://github.com/lz4/lz4.git"
  ["Telegram/lib_crl"]="https://github.com/desktop-app/lib_crl.git"
  ["Telegram/lib_rpl"]="https://github.com/desktop-app/lib_rpl.git"
  ["Telegram/lib_base"]="https://github.com/desktop-app/lib_base.git"
  ["Telegram/codegen"]="https://github.com/Yunogram/codegen.git"
  ["Telegram/lib_ui"]="https://github.com/Yunogram/lib_ui.git"
  ["Telegram/lib_lottie"]="https://github.com/desktop-app/lib_lottie.git"
  ["Telegram/lib_tl"]="https://github.com/Yunogram/lib_tl.git"
  ["Telegram/lib_spellcheck"]="https://github.com/desktop-app/lib_spellcheck"
  ["Telegram/lib_storage"]="https://github.com/desktop-app/lib_storage.git"
  ["cmake"]="https://github.com/desktop-app/cmake_helpers.git"
  ["Telegram/ThirdParty/expected"]="https://github.com/TartanLlama/expected"
  ["Telegram/ThirdParty/QR"]="https://github.com/nayuki/QR-Code-generator"
  ["Telegram/lib_qr"]="https://github.com/desktop-app/lib_qr.git"
  ["Telegram/ThirdParty/hunspell"]="https://github.com/hunspell/hunspell"
  ["Telegram/ThirdParty/range-v3"]="https://github.com/ericniebler/range-v3.git"
  ["Telegram/ThirdParty/nimf"]="https://github.com/hamonikr/nimf.git"
  ["Telegram/ThirdParty/hime"]="https://github.com/hime-ime/hime.git"
  ["Telegram/ThirdParty/fcitx5-qt"]="https://github.com/fcitx/fcitx5-qt.git"
  ["Telegram/lib_webrtc"]="https://github.com/desktop-app/lib_webrtc.git"
  ["Telegram/ThirdParty/tgcalls"]="https://github.com/TelegramMessenger/tgcalls.git"
  ["Telegram/lib_webview"]="https://github.com/desktop-app/lib_webview.git"
  ["Telegram/ThirdParty/kimageformats"]="https://github.com/KDE/kimageformats.git"
  ["Telegram/ThirdParty/kcoreaddons"]="https://github.com/KDE/kcoreaddons.git"
  ["Telegram/ThirdParty/cld3"]="https://github.com/google/cld3.git"
  ["Telegram/ThirdParty/libprisma"]="https://github.com/desktop-app/libprisma.git"
  ["Telegram/ThirdParty/xdg-desktop-portal"]="https://github.com/flatpak/xdg-desktop-portal.git"
  ["Telegram/lib_translate"]="https://github.com/desktop-app/lib_translate"
  ["Telegram/lib_icu"]="https://github.com/Yunogram/lib_icu.git"
  ["Telegram/ThirdParty/cmark-gfm"]="https://github.com/desktop-app/cmark-gfm.git"
  ["Telegram/ThirdParty/MicroTeX"]="https://github.com/desktop-app/MicroTeX.git"
  ["Telegram/ThirdParty/TooManyCooks"]="https://github.com/tzcnt/TooManyCooks.git"
  ["Telegram/ThirdParty/libcbor"]="https://github.com/PJK/libcbor.git"
  ["Telegram/ThirdParty/libfido2"]="https://github.com/Yubico/libfido2.git"
)

for path in "${!SUBS[@]}"; do
  url="${SUBS[$path]}"
  if [ -d "$path" ]; then
    echo "Убираю пустую папку: $path"
    rm -rf "$path"
  fi
done

echo ""
echo "Добавляю сабмодули заново (это займёт какое-то время, качаются реальные репозитории)..."
for path in "${!SUBS[@]}"; do
  url="${SUBS[$path]}"
  echo ">>> $path <- $url"
  git submodule add --force "$url" "$path"
done

echo ""
echo "Готово. Проверяю..."
git submodule status | head -40

echo ""
echo "Теперь закоммить и запушить:"
echo "  git add .gitmodules $(printf '"%s" ' "${!SUBS[@]}")"
echo "  git commit -m \"Fix: properly register all submodules\""
echo "  git push"
