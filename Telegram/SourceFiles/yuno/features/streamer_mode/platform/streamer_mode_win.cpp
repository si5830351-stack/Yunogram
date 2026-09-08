// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026

#include "yuno/features/streamer_mode/platform/streamer_mode_win.h"

#include "core/application.h"
#include "window/window_controller.h"

namespace YunoFeatures::StreamerMode::Impl {

void enableHook() {
	Core::App().enumerateWindows([&](not_null<Window::Controller*> w) {
		SetWindowDisplayAffinity(w->widget()->psHwnd(), WDA_EXCLUDEFROMCAPTURE);
	});
}

void disableHook() {
	Core::App().enumerateWindows([&](not_null<Window::Controller*> w) {
		SetWindowDisplayAffinity(w->widget()->psHwnd(), WDA_NONE);
	});
}

void hideWidgetWindow(QWidget *widget) {
	auto handle = reinterpret_cast<HWND>(widget->window()->winId());
	SetWindowDisplayAffinity(handle, WDA_EXCLUDEFROMCAPTURE);
}

void showWidgetWindow(QWidget *widget) {
	auto handle = reinterpret_cast<HWND>(widget->window()->winId());
	SetWindowDisplayAffinity(handle, WDA_NONE);
}

}
