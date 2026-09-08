// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "yuno/ui/settings/settings_main.h"

#include "settings/sections/settings_main.h"
#include "lang_auto.h"
#include "yuno/yuno_settings.h"
#include "yuno/ui/yuno_logo.h"
#include "yuno/ui/settings/settings_appearance.h"
#include "yuno/ui/settings/settings_yuno.h"
#include "yuno/ui/settings/settings_chats.h"
#include "yuno/ui/settings/settings_filters.h"
#include "yuno/ui/settings/settings_general.h"
#include "yuno/ui/settings/settings_other.h"
#include "core/version.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_yuno_settings.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "window/window_session_controller_link_info.h"

#include <QDesktopServices>

namespace Settings {

using namespace Builder;

namespace {

void BuildLogo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto logo = object_ptr<Ui::RpWidget>(ctx.container);
		const auto logoRaw = logo.data();
		logoRaw->resize(
			QSize(st::settingsCloudPasswordIconSize,
				st::settingsCloudPasswordIconSize));
		logoRaw->setNaturalWidth(st::settingsCloudPasswordIconSize);
		logoRaw->paintRequest(
		) | rpl::on_next([=] {
			auto p = QPainter(logoRaw);
			const auto image = YunoAssets::currentAppLogoPad();
			if (!image.isNull()) {
				const auto size = st::settingsCloudPasswordIconSize;
				const auto scaled = image.scaled(
					size * style::DevicePixelRatio(),
					size * style::DevicePixelRatio(),
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation);
				p.drawImage(QRect(0, 0, size, size), scaled);
			}
		}, logoRaw->lifetime());
		return { .widget = std::move(logo), .align = style::al_top };
	});
}

void BuildVersionInfo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				rpl::single(
					QString("Yunogram Desktop v")
					+ QString::fromLatin1(AppVersionStr)),
				st::boxTitle),
			.align = style::al_top,
		};
	});

	builder.addSkip();

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				tr::yuno_SettingsDescription(),
				st::centeredBoxLabel),
			.align = style::al_top,
		};
	});
}

void BuildCategories(SectionBuilder &builder) {
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::yuno_CategoriesHeader());

	builder.addSectionButton({
		.title = rpl::single(QString("TG Yunogram")),
		.targetSection = YunoGhost::Id(),
		.icon = { &st::menuIconGroupReactions },
	});
	builder.addSectionButton({
		.title = tr::yuno_CategoryFilters(),
		.targetSection = YunoFilters::Id(),
		.icon = { &st::menuIconTagFilter },
	});
	builder.addSectionButton({
		.title = tr::yuno_CategoryGeneral(),
		.targetSection = YunoGeneral::Id(),
		.icon = { &st::menuIconShowAll },
	});
	builder.addSectionButton({
		.title = tr::yuno_CategoryAppearance(),
		.targetSection = YunoAppearance::Id(),
		.icon = { &st::menuIconPalette },
	});
	builder.addSectionButton({
		.title = tr::yuno_CategoryChats(),
		.targetSection = YunoChats::Id(),
		.icon = { &st::menuIconChatBubble },
	});
	builder.addSectionButton({
		.title = tr::yuno_CategoryOther(),
		.targetSection = YunoOther::Id(),
		.icon = { &st::menuIconFave },
	});
}

void BuildLinks(SectionBuilder &builder) {
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::yuno_LinksHeader());

	const auto controller = builder.controller();

	builder.addButton({
		.id = u"yuno/channel"_q,
		.title = tr::yuno_LinksChannel(),
		.icon = { &st::menuIconChannel },
		.label = rpl::single(QString("@yunogram")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("yunogram"),
			});
		},
	});
	builder.addButton({
		.id = u"yuno/chat"_q,
		.title = tr::yuno_LinksChats(),
		.icon = { &st::menuIconChats },
		.label = rpl::single(QString("@yunogramchat")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("yunogramchat"),
			});
		},
	});
	builder.addButton({
		.id = u"yuno/crowdin"_q,
		.title = tr::yuno_LinksTranslate(),
		.icon = { &st::menuIconTranslate },
		.label = rpl::single(QString("Crowdin")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://translate.yunogram.one"));
		},
	});
	builder.addButton({
		.id = u"yuno/website"_q,
		.title = tr::yuno_LinksDocumentation(),
		.icon = { &st::menuIconIpAddress },
		.label = rpl::single(QString("docs.yunogram.one")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://docs.yunogram.one"));
		},
	});

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = YunoMain::Id(),
	.parentId = MainId(),
	.title = &tr::yuno_YunoPreferences,
	.icon = &st::menuIconPremium,
}, [](SectionBuilder &builder) {
	BuildLogo(builder);
	builder.addSkip();
	BuildVersionInfo(builder);
	BuildCategories(builder);
	BuildLinks(builder);
});

} // namespace

rpl::producer<QString> YunoMain::title() {
	return rpl::single(QString(""));
}

YunoMain::YunoMain(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void YunoMain::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type YunoMainId() {
	return YunoMain::Id();
}

} // namespace Settings
