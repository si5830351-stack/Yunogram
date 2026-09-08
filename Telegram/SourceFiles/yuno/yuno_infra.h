// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "base/basic_types.h"

namespace Window {
class SessionController;
} // namespace Window

namespace YunoInfra {

void init();
void applyDefaultTheme();
void applyDefaultSubscriptions(
	not_null<Window::SessionController*> controller);

}
