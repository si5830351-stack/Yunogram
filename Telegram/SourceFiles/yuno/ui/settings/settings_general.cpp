// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "yuno/ui/settings/settings_general.h"

#include "lang_auto.h"
#include "yuno/yuno_settings.h"
#include "yuno/ui/settings/yuno_builder.h"
#include "yuno/ui/settings/settings_yuno_utils.h"
#include "yuno/ui/settings/settings_main.h"
#include "base/platform/base_platform_info.h"
#include "core/application.h"
#include "lang/lang_text_entity.h"
#include "platform/platform_translate_provider.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/toast/toast.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace YunoBuilder;

namespace {

void BuildTranslator(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::lng_translate_settings_subtitle());

	auto *settings = &YunoSettings::getInstance();

	const auto options = std::vector{
		std::pair(TranslationProvider::Telegram, QString("Telegram")),
		std::pair(TranslationProvider::Google, QString("Google")),
		std::pair(TranslationProvider::Yandex, QString("Yandex")),
	};
	const auto nativeAvailable = Platform::IsTranslateProviderAvailable();
	auto availableOptions = options;
	if (nativeAvailable) {
		availableOptions.push_back(std::pair(
			TranslationProvider::Native,
			[] {
				if constexpr (Platform::IsMac()) {
					return QString("macOS");
				} else if constexpr (Platform::IsWindows()) {
					return QString("Windows");
				} else {
					return QString("Linux");
				}
			}()));
	}
	auto optionLabels = std::vector<QString>();
	optionLabels.reserve(availableOptions.size());
	for (const auto &option : availableOptions) {
		optionLabels.push_back(option.second);
	}

	const auto getIndex = [=](TranslationProvider val) {
		const auto i = ranges::find(
			availableOptions,
			val,
			&std::pair<TranslationProvider, QString>::first);
		return (i != end(availableOptions))
			? int(i - begin(availableOptions))
			: 0;
	};

	auto currentVal = YunoSettings::getInstance().translationProviderValue()
		| rpl::map(getIndex)
		| rpl::map([=](int val) { return availableOptions[val].second; });

	const auto button = builder.addButton({
		.id = u"yuno/translationProvider"_q,
		.title = tr::yuno_TranslationProvider(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			if (const auto controller = Core::App().activeWindow()->sessionController()) {
				controller->show(Box(
						[=](not_null<Ui::GenericBox*> box) {
							const auto save = [=](int index) {
								const auto option = availableOptions[index].first;
								YunoSettings::getInstance().setTranslationProvider(option);

								if constexpr (Platform::IsMac()) {
									if (option == TranslationProvider::Native) {
										controller->showToast(Ui::Toast::Config{
											.text = tr::lng_translate_settings_use_platform_mac_about(tr::now, tr::rich),
											.duration = 6 * crl::time(1000)
										});
									}
								}
							};
							SingleChoiceBox(box, {
								.title = tr::yuno_TranslationProvider(),
								.options = optionLabels,
								.initialSelection = getIndex(settings->translationProvider()),
								.callback = save,
							});
						}));
			}
		},
	});
	if (button) {
		yuno.addBetaBadge(button);
	}
}

void BuildShowPeerId(SectionBuilder &builder) {
	auto *settings = &YunoSettings::getInstance();

	const auto options = std::vector{
		QString(tr::yuno_SettingsShowID_Hide(tr::now)),
		QString("Telegram API"),
		QString("Bot API")
	};

	auto currentVal = YunoSettings::getInstance().showPeerIdValue()
		| rpl::map([=](PeerIdDisplay val) {
			return options[static_cast<int>(val)];
		});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"yuno/showPeerId"_q,
		.altIds = { u"yuno/showIdAndDc"_q },
		.title = tr::yuno_SettingsShowID(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			controller->show(Box(
				[=](not_null<Ui::GenericBox*> box) {
					const auto save = [=](int index) {
						YunoSettings::getInstance().setShowPeerId(
							static_cast<PeerIdDisplay>(index));
					};
					SingleChoiceBox(box, {
						.title = tr::yuno_SettingsShowID(),
						.options = options,
						.initialSelection = static_cast<int>(settings->showPeerId()),
						.callback = save,
					});
				}));
		},
	});
}

void BuildQoLToggles(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();

	BuildTranslator(builder, yuno);
	yuno.addSectionDivider();

	builder.addSubsectionTitle(tr::yuno_CategoryGeneral());

	const auto controller = builder.controller();
	yuno.addToggle({
		.id = u"yuno/disableStories"_q,
		.altIds = { u"yuno/hideStories"_q },
		.title = tr::yuno_DisableStories(),
		.getter = [=] { return settings->disableStories(); },
		.setter = [=](bool enabled) {
			YunoSettings::getInstance().setDisableStories(enabled);
			ShowRestartPrompt(controller);
		},
	});

	yuno.addSettingToggle({
		.id = u"yuno/disableOpenLinkWarning"_q,
		.title = tr::yuno_DisableOpenLinkWarning(),
		.getter = &YunoSettings::disableOpenLinkWarning,
		.setter = &YunoSettings::setDisableOpenLinkWarning,
	});

	yuno.addCollapsibleToggle({
		.id = u"yuno/similarChannels"_q,
		.title = tr::yuno_DisableSimilarChannels(),
		.checkboxes = {
			NestedEntry{
				tr::yuno_CollapseSimilarChannels(tr::now),
				[] { return YunoSettings::getInstance().collapseSimilarChannels(); },
				[](bool v) { YunoSettings::getInstance().setCollapseSimilarChannels(v); }
			},
			NestedEntry{
				tr::yuno_HideSimilarChannelsTab(tr::now),
				[] { return YunoSettings::getInstance().hideSimilarChannels(); },
				[](bool v) { YunoSettings::getInstance().setHideSimilarChannels(v); }
			}
		},
		.toggledWhenAll = true,
	});

	yuno.addSettingToggle({
		.id = u"yuno/disableNotificationsDelay"_q,
		.title = tr::yuno_DisableNotificationsDelay(),
		.getter = &YunoSettings::disableNotificationsDelay,
		.setter = &YunoSettings::setDisableNotificationsDelay,
	});

	yuno.addSectionDivider();

	const auto zalgoButton = builder.addButton({
		.id = u"yuno/filterZalgo"_q,
		.title = tr::yuno_FilterZalgo(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->filterZalgo()),
	});
	if (zalgoButton) {
		zalgoButton->toggledValue(
		) | rpl::filter(
			[=](bool enabled) {
				return (enabled != settings->filterZalgo());
			}
		) | on_next(
			[=](bool enabled) {
				YunoSettings::getInstance().setFilterZalgo(enabled);
				ShowRestartPrompt(controller);
			},
			zalgoButton->lifetime());
		yuno.addBetaBadge(zalgoButton);
	}

	yuno.addSettingToggle({
		.id = u"yuno/improveLinkPreviews"_q,
		.title = tr::yuno_ImproveLinkPreviews(),
		.getter = &YunoSettings::improveLinkPreviews,
		.setter = &YunoSettings::setImproveLinkPreviews,
	});
	yuno.addSettingToggle({
		.id = u"yuno/showMessageSeconds"_q,
		.altIds = { u"yuno/formatTimeWithSeconds"_q },
		.title = tr::yuno_SettingsShowMessageSeconds(),
		.getter = &YunoSettings::showMessageSeconds,
		.setter = &YunoSettings::setShowMessageSeconds,
	});

	BuildShowPeerId(builder);

	yuno.addSectionDivider();

	builder.addSubsectionTitle(rpl::single(QString("Webview")));

	yuno.addSettingToggle({
		.id = u"yuno/spoofWebviewAsAndroid"_q,
		.title = tr::yuno_SettingsSpoofWebviewAsAndroid(),
		.getter = &YunoSettings::spoofWebviewAsAndroid,
		.setter = &YunoSettings::setSpoofWebviewAsAndroid,
	});

	yuno.addCollapsibleToggle({
		.id = u"yuno/biggerWindow"_q,
		.title = tr::yuno_SettingsBiggerWindow(),
		.checkboxes = {
			NestedEntry{
				tr::yuno_SettingsIncreaseWebviewHeight(tr::now),
				[] { return YunoSettings::getInstance().increaseWebviewHeight(); },
				[](bool v) { YunoSettings::getInstance().setIncreaseWebviewHeight(v); }
			},
			NestedEntry{
				tr::yuno_SettingsIncreaseWebviewWidth(tr::now),
				[] { return YunoSettings::getInstance().increaseWebviewWidth(); },
				[](bool v) { YunoSettings::getInstance().setIncreaseWebviewWidth(v); }
			}
		},
		.toggledWhenAll = false,
	});

	yuno.addSectionDivider();

	builder.addSubsectionTitle(tr::yuno_ConfirmationsTitle());

	yuno.addSettingToggle({
		.id = u"yuno/stickerConfirmation"_q,
		.title = tr::yuno_StickerConfirmation(),
		.getter = &YunoSettings::stickerConfirmation,
		.setter = &YunoSettings::setStickerConfirmation,
	});
	yuno.addSettingToggle({
		.id = u"yuno/gifConfirmation"_q,
		.title = tr::yuno_GIFConfirmation(),
		.getter = &YunoSettings::gifConfirmation,
		.setter = &YunoSettings::setGifConfirmation,
	});
	yuno.addSettingToggle({
		.id = u"yuno/voiceConfirmation"_q,
		.title = tr::yuno_VoiceConfirmation(),
		.getter = &YunoSettings::voiceConfirmation,
		.setter = &YunoSettings::setVoiceConfirmation,
	});
}

const auto kMeta = BuildHelper({
	.id = YunoGeneral::Id(),
	.parentId = YunoMain::Id(),
	.title = &tr::yuno_CategoryGeneral,
	.icon = &st::menuIconShowAll,
}, [](SectionBuilder &builder) {
	auto yuno = YunoSectionBuilder(builder);

	builder.addSkip();
	BuildQoLToggles(builder, yuno);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> YunoGeneral::title() {
	return tr::yuno_CategoryGeneral();
}

YunoGeneral::YunoGeneral(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void YunoGeneral::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type YunoGeneralId() {
	return YunoGeneral::Id();
}

} // namespace Settings
