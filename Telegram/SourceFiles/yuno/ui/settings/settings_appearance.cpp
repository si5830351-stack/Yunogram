// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "yuno/ui/settings/settings_appearance.h"

#include "lang_auto.h"
#include "yuno/yuno_settings.h"
#include "yuno/ui/boxes/font_selector.h"
#include "yuno/ui/components/avatar_corners_preview.h"
#include "yuno/ui/components/icon_picker.h"
#include "yuno/ui/settings/yuno_builder.h"
#include "yuno/ui/settings/settings_yuno_utils.h"
#include "yuno/ui/settings/settings_main.h"
#include "inline_bots/bot_attach_web_view.h"
#include "main/main_session.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_yuno_icons.h"
#include "styles/style_yuno_styles.h"
#include "styles/style_dialogs.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace YunoBuilder;

namespace {

bool HasDrawerBots(not_null<Window::SessionController*> controller) {
	// todo: maybe iterate through all accounts
	const auto bots = &controller->session().attachWebView();
	for (const auto &bot : bots->attachBots()) {
		if (!bot.inMainMenu || !bot.media) {
			continue;
		}
		return true;
	}
	return false;
}

void BuildAppIcon(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle({
		.id = u"yuno/appIcon"_q,
		.title = tr::yuno_AppIconHeader(),
	});

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<IconPicker>(ctx.container),
			.margin = st::settingsButtonNoIcon.padding,
		};
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	builder.addDivider();
	builder.addSkip();
	yuno.addSettingToggle({
		.id = u"yuno/hideNotificationBadge"_q,
		.title = tr::yuno_HideNotificationBadge(),
		.getter = &YunoSettings::hideNotificationBadge,
		.setter = &YunoSettings::setHideNotificationBadge,
	});
	builder.addSkip();
	builder.addDividerText(tr::yuno_HideNotificationBadgeDescription());
	builder.addSkip();
#else
    builder.addDivider();
    builder.addSkip();
#endif
}

void BuildAvatarCorners(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();
	const auto controller = builder.controller();

	const auto mapRadius = [](int val)
	{
		if (val == 0) {
			return tr::yuno_AvatarCornersSquare(tr::now).toUpper();
		} else if (val == 23) {
			return tr::yuno_AvatarCornersCircle(tr::now).toUpper();
		}
		return QString::number(val);
	};

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		const auto container = ctx.container;
		auto title = object_ptr<Ui::FlatLabel>(
			container,
			tr::yuno_AvatarCorners(),
			st::defaultSubsectionTitle);
		const auto titleRaw = title.data();

		const auto badge = Ui::CreateChild<Ui::PaddingWrap<Ui::FlatLabel>>(
			container,
			object_ptr<Ui::FlatLabel>(
				container,
				settings->avatarCornersValue() | rpl::map(mapRadius),
				st::settingsPremiumNewBadge),
			st::yunoBetaBadgePadding);
		badge->show();
		badge->setAttribute(Qt::WA_TransparentForMouseEvents);
		badge->paintRequest() | rpl::on_next([=] {
			auto p = QPainter(badge);
			auto hq = PainterHighQualityEnabler(p);
			p.setPen(Qt::NoPen);
			p.setBrush(st::windowBgActive);
			const auto r = st::yunoBetaBadgePadding.left();
			p.drawRoundedRect(badge->rect(), r, r);
		}, badge->lifetime());

		titleRaw->geometryValue() | rpl::on_next([=](QRect geometry) {
			badge->moveToLeft(
				geometry.x()
					+ titleRaw->textMaxWidth()
					+ st::settingsPremiumNewBadgePosition.x(),
				geometry.y()
					+ (geometry.height() - badge->height()) / 2);
		}, badge->lifetime());

		return {
			.widget = std::move(title),
			.margin = st::defaultSubsectionTitlePadding,
		};
	}, [] {
		return SearchEntry{
			.id = u"yuno/avatarCorners"_q,
			.title = tr::yuno_AvatarCorners(tr::now),
		};
	});

	auto *previewRaw = static_cast<AvatarCornersPreview*>(nullptr);
	builder.add([&](const Builder::WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<AvatarCornersPreview>(
			ctx.container,
			controller);
		previewRaw = preview.data();
		const auto vMargin = st::settingsButtonNoIcon.padding
			- st::defaultDialogRow.padding;
		return {
			.widget = std::move(preview),
			.margin = QMargins(0, vMargin.top(), 0, vMargin.bottom()),
		};
	});

	yuno.addSlider({
		.id = u"yuno/avatarCornersSlider"_q,
		.title = rpl::single(QString()),
		.showTitle = false,
		.steps = 24,
		.current = settings->avatarCorners(),
		.onChanged = [=](int val) {
			YunoSettings::getInstance().setAvatarCorners(val);
			if (previewRaw) {
				previewRaw->update();
			}
		},
		.onFinalChanged = [=](int val) {
			YunoSettings::getInstance().setAvatarCorners(val);
			ShowRestartPrompt(controller);
		},
	});

	yuno.addSettingToggle({
		.id = u"yuno/singleCornerRadius"_q,
		.title = tr::yuno_SingleCornerRadius(),
		.getter = &YunoSettings::singleCornerRadius,
		.setter = &YunoSettings::setSingleCornerRadius,
	});

	builder.addSkip();
	builder.addDividerText(tr::yuno_SingleCornerRadiusDescription());
	builder.addSkip();
}

void BuildAppearance(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();

	builder.addSubsectionTitle(tr::yuno_CategoryAppearance());

	yuno.addSettingToggle({
		.id = u"yuno/materialSwitches"_q,
		.altIds = { u"yuno/newSwitchStyle"_q },
		.title = tr::yuno_MaterialSwitches(),
		.getter = &YunoSettings::materialSwitches,
		.setter = &YunoSettings::setMaterialSwitches,
	});
	yuno.addSettingToggle({
		.id = u"yuno/disableCustomBackgrounds"_q,
		.altIds = { u"yuno/customThemes"_q },
		.title = tr::yuno_DisableCustomBackgrounds(),
		.getter = &YunoSettings::disableCustomBackgrounds,
		.setter = &YunoSettings::setDisableCustomBackgrounds,
	});
	yuno.addSettingToggle({
		.id = u"yuno/hidePremiumStatuses"_q,
		.title = tr::yuno_HidePremiumStatuses(),
		.getter = &YunoSettings::hidePremiumStatuses,
		.setter = &YunoSettings::setHidePremiumStatuses,
	});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"yuno/monoFont"_q,
		.title = tr::yuno_MonospaceFont(),
		.st = &st::settingsButtonNoIcon,
		.label = rpl::single(
			settings->monoFont().isEmpty()
				? tr::yuno_FontDefault(tr::now)
				: settings->monoFont()),
		.onClick = [=] {
			YunoUi::FontSelectorBox::Show(
				controller,
				[=](const QString &font) {
					YunoSettings::getInstance().setMonoFont(font);
				});
		},
	});

	yuno.addSectionDivider();
}

void BuildChatFolders(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::yuno_ChatFoldersHeader());

	yuno.addSettingToggle({
		.id = u"yuno/hideNotificationCounters"_q,
		.altIds = { u"yuno/tabCounter"_q },
		.title = tr::yuno_HideNotificationCounters(),
		.getter = &YunoSettings::hideNotificationCounters,
		.setter = &YunoSettings::setHideNotificationCounters,
	});
	yuno.addSettingToggle({
		.id = u"yuno/hideAllChatsFolder"_q,
		.altIds = { u"yuno/hideAllChats"_q },
		.title = tr::yuno_HideAllChats(),
		.getter = &YunoSettings::hideAllChatsFolder,
		.setter = &YunoSettings::setHideAllChatsFolder,
	});

	yuno.addSectionDivider();
}

void BuildTrayElements(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::yuno_TrayElementsHeader());

	yuno.addSettingToggle({
		.id = u"yuno/showGhostToggleInTray"_q,
		.title = tr::yuno_EnableGhostModeTray(),
		.getter = &YunoSettings::showGhostToggleInTray,
		.setter = &YunoSettings::setShowGhostToggleInTray,
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	yuno.addSettingToggle({
		.id = u"yuno/showStreamerToggleInTray"_q,
		.title = tr::yuno_EnableStreamerModeTray(),
		.getter = &YunoSettings::showStreamerToggleInTray,
		.setter = &YunoSettings::setShowStreamerToggleInTray,
	});
#endif

	yuno.addSectionDivider();
}

void BuildDrawerElements(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::yuno_DrawerElementsHeader());

	yuno.addSettingToggle({
		.id = u"yuno/showMyProfileInDrawer"_q,
		.title = tr::lng_menu_my_profile(),
		.getter = &YunoSettings::showMyProfileInDrawer,
		.setter = &YunoSettings::setShowMyProfileInDrawer,
		.icon = { &st::menuIconProfile },
	});

	const auto controller = builder.controller();
	if (controller && HasDrawerBots(controller)) {
		yuno.addSettingToggle({
			.id = u"yuno/showBotsInDrawer"_q,
			.title = tr::lng_filters_type_bots(),
			.getter = &YunoSettings::showBotsInDrawer,
			.setter = &YunoSettings::setShowBotsInDrawer,
			.icon = { &st::menuIconBot },
		});
	}

	yuno.addSettingToggle({
		.id = u"yuno/showNewGroupInDrawer"_q,
		.title = tr::lng_create_group_title(),
		.getter = &YunoSettings::showNewGroupInDrawer,
		.setter = &YunoSettings::setShowNewGroupInDrawer,
		.icon = { &st::menuIconGroups },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showNewChannelInDrawer"_q,
		.title = tr::lng_create_channel_title(),
		.getter = &YunoSettings::showNewChannelInDrawer,
		.setter = &YunoSettings::setShowNewChannelInDrawer,
		.icon = { &st::menuIconChannel },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showContactsInDrawer"_q,
		.title = tr::lng_menu_contacts(),
		.getter = &YunoSettings::showContactsInDrawer,
		.setter = &YunoSettings::setShowContactsInDrawer,
		.icon = { &st::menuIconUserShow },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showCallsInDrawer"_q,
		.title = tr::lng_menu_calls(),
		.getter = &YunoSettings::showCallsInDrawer,
		.setter = &YunoSettings::setShowCallsInDrawer,
		.icon = { &st::menuIconPhone },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showSavedMessagesInDrawer"_q,
		.title = tr::lng_saved_messages(),
		.getter = &YunoSettings::showSavedMessagesInDrawer,
		.setter = &YunoSettings::setShowSavedMessagesInDrawer,
		.icon = { &st::menuIconSavedMessages },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showLReadToggleInDrawer"_q,
		.title = tr::yuno_LReadMessages(),
		.getter = &YunoSettings::showLReadToggleInDrawer,
		.setter = &YunoSettings::setShowLReadToggleInDrawer,
		.icon = { &st::yunoLReadMenuIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showSReadToggleInDrawer"_q,
		.title = tr::yuno_SReadMessages(),
		.getter = &YunoSettings::showSReadToggleInDrawer,
		.setter = &YunoSettings::setShowSReadToggleInDrawer,
		.icon = { &st::yunoSReadMenuIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showNightModeToggleInDrawer"_q,
		.title = tr::lng_menu_night_mode(),
		.getter = &YunoSettings::showNightModeToggleInDrawer,
		.setter = &YunoSettings::setShowNightModeToggleInDrawer,
		.icon = { &st::menuIconNightMode },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showGhostToggleInDrawer"_q,
		.title = tr::yuno_GhostModeToggle(),
		.getter = &YunoSettings::showGhostToggleInDrawer,
		.setter = &YunoSettings::setShowGhostToggleInDrawer,
		.icon = { &st::yunoGhostIcon },
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	yuno.addSettingToggle({
		.id = u"yuno/showStreamerToggleInDrawer"_q,
		.title = tr::yuno_StreamerModeToggle(),
		.getter = &YunoSettings::showStreamerToggleInDrawer,
		.setter = &YunoSettings::setShowStreamerToggleInDrawer,
		.icon = { &st::yunoStreamerModeMenuIcon },
	});
#endif

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = YunoAppearance::Id(),
	.parentId = YunoMain::Id(),
	.title = &tr::yuno_CategoryAppearance,
	.icon = &st::menuIconPalette,
}, [](SectionBuilder &builder) {
	auto yuno = YunoSectionBuilder(builder);

	builder.addSkip();
	BuildAppIcon(builder, yuno);
	BuildAvatarCorners(builder, yuno);
	BuildAppearance(builder, yuno);
	BuildChatFolders(builder, yuno);
	BuildTrayElements(builder, yuno);
	BuildDrawerElements(builder, yuno);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> YunoAppearance::title() {
	return tr::yuno_CategoryAppearance();
}

YunoAppearance::YunoAppearance(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void YunoAppearance::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type YunoAppearanceId() {
	return YunoAppearance::Id();
}

} // namespace Settings
