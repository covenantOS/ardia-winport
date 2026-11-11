#include "chrome/browser/ardia/pip/ac_pip_mac.h"

std::optional<gfx::Rect> LoadArdiaPictureInPictureBounds() {
  return std::nullopt;
}

void SaveArdiaPictureInPictureBounds(const gfx::Rect&) {}

void FadeOutArdiaPictureInPicture(gfx::NativeWindow, base::TimeDelta) {}

void ResetArdiaPictureInPictureFade(gfx::NativeWindow) {}

bool PresentArdiaPictureInPictureControls(
    gfx::NativeWindow,
    const std::vector<ArdiaPipNativeControl>&) {
  return false;
}
