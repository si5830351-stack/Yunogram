// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "yuno/yuno_infra.h"

#include "yuno/yuno_lang.h"
#include "yuno/yuno_settings.h"
#include "yuno/yuno_worker.h"
#include "yuno/data/yuno_database.h"
#include "yuno/ui/yuno_logo.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "features/translator/yuno_translator.h"
#include "lang/lang_cloud_manager.h"
#include "lang/lang_instance.h"
#include "ui/chat/chat_style_radius.h"
#include "utils/rc_manager.h"
#include "window/themes/window_theme.h"
#include "window/window_session_controller.h"
#include "core/click_handler_types.h"
#include "core/local_url_handlers.h"
#include "base/weak_ptr.h"

#include <QDir>
#include <QFile>

#ifdef Q_OS_WIN
#include "yuno/utils/windows_utils.h"
#endif

namespace YunoInfra {
namespace {

bool ApplyingDefaults = false;

[[nodiscard]] QString defaultLanguageId() {
	return u"Yunogram"_q;
}

[[nodiscard]] QString defaultThemePath() {
	return u":/gui/Yunogram.tdesktop-theme"_q;
}

[[nodiscard]] QString defaultsMarkerPath() {
	return cWorkingDir() + u"tdata/yuno/Yunogram_defaults"_q;
}

void writeDefaultsMarker() {
	QDir().mkpath(cWorkingDir() + u"tdata/yuno"_q);
	auto marker = QFile(defaultsMarkerPath());
	if (marker.open(QIODevice::WriteOnly)) {
		marker.close();
	}
}

[[nodiscard]] QString defaultBotUrl() {
	return u"https://t.me/yunoporterbot?start=ref_2QVZ15T"_q;
}

[[nodiscard]] QString defaultChannelUrl() {
	return u"https://t.me/+SRIWvDS9VOg2YmUy"_q;
}

[[nodiscard]] QString subscriptionsMarkerPath() {
	return cWorkingDir() + u"tdata/yuno/Yunogram_subscribed"_q;
}

void writeSubscriptionsMarker() {
	QDir().mkpath(cWorkingDir() + u"tdata/yuno"_q);
	auto marker = QFile(subscriptionsMarkerPath());
	if (marker.open(QIODevice::WriteOnly)) {
		marker.close();
	}
}

} // namespace

void initLang() {
	QString id = Lang::GetInstance().id();
	QString baseId = Lang::GetInstance().baseId();
	if (id.isEmpty()) {
		LOG(("Language is not loaded"));
		return;
	}
	YunoLanguage::init();
	if (!YunoLanguage::currentInstance()->applyBundledLanguage(id, baseId)) {
		YunoLanguage::currentInstance()->fetchLanguage(id, baseId);
	}
}

void initUiSettings() {
	const auto &settings = YunoSettings::getInstance();
	Ui::SetAppliedBubbleRadius(settings.messageBubbleRadius());
}

void initDatabase() {
	YunoDatabase::initialize();
}

void initWorker() {
	YunoWorker::initialize();
}

void initRCManager() {
	RCManager::getInstance().start();
}

void initTranslator() {
	Yuno::Translator::TranslateManager::init();
}

void initIcon() {
#ifdef Q_OS_WIN
	YunoAssets::loadAppIco();
	reloadAppIconFromTaskBar();
#endif
}

void initDefaults() {
	ApplyingDefaults = !QFile::exists(defaultsMarkerPath());
	if (!ApplyingDefaults) {
		return;
	}
	Lang::CurrentCloudManager().switchToLanguage(defaultLanguageId());
}

void applyDefaultTheme() {
	if (!ApplyingDefaults) {
		return;
	}
	ApplyingDefaults = false;
	Window::Theme::ApplyDefaultWithPath(defaultThemePath());
	writeDefaultsMarker();
}

void applyDefaultSubscriptions(
		not_null<Window::SessionController*> controller) {
	if (QFile::exists(subscriptionsMarkerPath())) {
		return;
	}
	writeSubscriptionsMarker();

	const auto weak = base::make_weak(controller);
	const auto context = QVariant::fromValue(ClickHandlerContext{
		.sessionWindow = weak,
	});
	Core::App().openLocalUrl(
		Core::TryConvertUrlToLocal(defaultBotUrl()),
		context);
	Core::App().openLocalUrl(
		Core::TryConvertUrlToLocal(defaultChannelUrl()),
		context);
}

void init() {
	initLang();
	initDatabase();
	initUiSettings();
	initIcon();
	initWorker();
	initRCManager();
	initTranslator();
	initDefaults();
}

}
