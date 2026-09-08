// This is the source code of Yunogram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "yuno/ui/settings/settings_chats.h"

#include "lang_auto.h"
#include "yuno/yuno_settings.h"
#include "yuno/ui/boxes/edit_mark_box.h"
#include "yuno/ui/components/message_preview.h"
#include "yuno/ui/settings/yuno_builder.h"
#include "yuno/ui/settings/settings_yuno_utils.h"
#include "yuno/ui/settings/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_yuno_icons.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include <memory>

namespace Settings {

using namespace Builder;
using namespace YunoBuilder;

namespace {

struct PreviewState {
	MessagePreview *widget = nullptr;
};

void BuildStickersAndEmoji(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::lng_settings_stickers_emoji());

	yuno.addSettingToggle({
		.id = u"yuno/showOnlyAddedEmojisAndStickers"_q,
		.title = tr::yuno_ShowOnlyAddedEmojisAndStickers(),
		.getter = &YunoSettings::showOnlyAddedEmojisAndStickers,
		.setter = &YunoSettings::setShowOnlyAddedEmojisAndStickers,
	});

	yuno.addCollapsibleToggle({
		.id = u"yuno/hideReactions"_q,
		.title = tr::yuno_HideReactions(),
		.checkboxes = {
			NestedEntry{
				tr::yuno_HideReactionsInChannels(tr::now),
				[] { return !YunoSettings::getInstance().showChannelReactions(); },
				[](bool v) { YunoSettings::getInstance().setShowChannelReactions(!v); }
			},
			NestedEntry{
				tr::yuno_HideReactionsInGroups(tr::now),
				[] { return !YunoSettings::getInstance().showGroupReactions(); },
				[](bool v) { YunoSettings::getInstance().setShowGroupReactions(!v); }
			},
			NestedEntry{
				tr::yuno_HideReactionsInPrivateChats(tr::now),
				[] { return !YunoSettings::getInstance().showPrivateChatReactions(); },
				[](bool v) { YunoSettings::getInstance().setShowPrivateChatReactions(!v); }
			}
		},
		.toggledWhenAll = false,
	});

	yuno.addSectionDivider();
}

void BuildRecentStickersLimit(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();

	yuno.addSlider({
		.id = u"yuno/recentStickersCount"_q,
		.title = tr::yuno_SettingsRecentStickersCount(),
		.steps = 200 + 1,
		.current = settings->recentStickersCount(),
		.indexToValue = [](int index) { return index; },
		.onChanged = nullptr,
		.onFinalChanged = [](int amount) {
			YunoSettings::getInstance().setRecentStickersCount(amount);
		},
		.formatLabel = [](int amount) { return QString::number(amount); },
	});

	yuno.addSectionDivider();
}

void BuildGroupsAndChannels(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();

	builder.addSubsectionTitle(tr::lng_premium_double_limits_subtitle_channels());

	yuno.addChooseButton({
		.id = u"yuno/channelBottomButton"_q,
		.altIds = { u"yuno/bottomButton"_q },
		.title = tr::yuno_ChannelBottomButton(),
		.boxTitle = tr::yuno_ChannelBottomButton(),
		.initialSelection = static_cast<int>(settings->channelBottomButton()),
		.options = {
			tr::yuno_ChannelBottomButtonHide(tr::now),
			tr::yuno_ChannelBottomButtonMute(tr::now),
			tr::yuno_ChannelBottomButtonDiscuss(tr::now),
		},
		.setter = [](int index) {
			YunoSettings::getInstance().setChannelBottomButton(
				static_cast<ChannelBottomButton>(index));
		},
	});

	yuno.addSettingToggle({
		.id = u"yuno/quickAdminShortcuts"_q,
		.title = tr::yuno_QuickAdminShortcuts(),
		.getter = &YunoSettings::quickAdminShortcuts,
		.setter = &YunoSettings::setQuickAdminShortcuts,
	});
	yuno.addSettingToggle({
		.id = u"yuno/disableGreetingSticker"_q,
		.title = tr::yuno_DisableGreetingSticker(),
		.getter = &YunoSettings::disableGreetingSticker,
		.setter = &YunoSettings::setDisableGreetingSticker,
	});
	yuno.addSettingToggle({
		.id = u"yuno/showMessageShot"_q,
		.title = tr::yuno_SettingsShowMessageShot(),
		.getter = &YunoSettings::showMessageShot,
		.setter = &YunoSettings::setShowMessageShot,
	});

	builder.addSkip();
	builder.addDividerText(tr::yuno_SettingsShowMessageShotDescription());
	builder.addSkip();
}

void BuildMarks(
		SectionBuilder &builder,
		YunoSectionBuilder &yuno,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &YunoSettings::getInstance();
	const auto controller = builder.controller();

	builder.addSubsectionTitle(tr::lng_settings_messages());

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<MessagePreview>(ctx.container, controller);
		previewState->widget = preview.data();
		return {
			.widget = std::move(preview),
			.margin = style::margins(
				0,
				st::defaultVerticalListSkip,
				0,
				st::settingsPrivacySkipTop),
		};
	});

	yuno.addSettingToggle({
		.id = u"yuno/replaceBottomInfoWithIcons"_q,
		.altIds = { u"yuno/replaceEditedWithIcon"_q },
		.title = tr::yuno_ReplaceMarksWithIcons(),
		.getter = &YunoSettings::replaceBottomInfoWithIcons,
		.setter = &YunoSettings::setReplaceBottomInfoWithIcons,
	});

	builder.scope([&] {
		builder.addButton({
			.id = u"yuno/deletedMark"_q,
			.title = tr::yuno_DeletedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = YunoSettings::getInstance().deletedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::yuno_DeletedMarkText(),
					settings->deletedMark(),
					QString("🧹"),
					[=](const QString &value) {
						YunoSettings::getInstance().setDeletedMark(value);
					});
				Ui::show(std::move(box));
			},
		});

		builder.addButton({
			.id = u"yuno/editedMark"_q,
			.title = tr::yuno_EditedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = YunoSettings::getInstance().editedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::yuno_EditedMarkText(),
					settings->editedMark(),
					tr::lng_edited(tr::now),
					[=](const QString &value) {
						YunoSettings::getInstance().setEditedMark(value);
					});
				Ui::show(std::move(box));
			},
		});
	}, YunoSettings::getInstance().replaceBottomInfoWithIconsValue()
		| rpl::map([](bool v) { return !v; }));

	yuno.addSettingToggle({
		.id = u"yuno/removeMessageTail"_q,
		.title = tr::yuno_RemoveMessageTail(),
		.getter = &YunoSettings::removeMessageTail,
		.setter = &YunoSettings::setRemoveMessageTail,
	});

	yuno.addSettingToggle({
		.id = u"yuno/hideFastShare"_q,
		.altIds = { u"yuno/hideShareButton"_q },
		.title = tr::yuno_HideShareButton(),
		.getter = &YunoSettings::hideFastShare,
		.setter = &YunoSettings::setHideFastShare,
	});
	yuno.addSettingToggle({
		.id = u"yuno/simpleQuotesAndReplies"_q,
		.altIds = { u"yuno/disableColorfulReplies"_q, u"yuno/replyElements"_q },
		.title = tr::yuno_SimpleQuotesAndReplies(),
		.getter = &YunoSettings::simpleQuotesAndReplies,
		.setter = &YunoSettings::setSimpleQuotesAndReplies,
	});

	const auto semiTransparent = yuno.addSettingToggle({
		.id = u"yuno/semiTransparentDeletedMessages"_q,
		.altIds = { u"yuno/translucentDeletedMessages"_q },
		.title = tr::yuno_SemiTransparentDeletedMessages(),
		.getter = &YunoSettings::semiTransparentDeletedMessages,
		.setter = &YunoSettings::setSemiTransparentDeletedMessages,
	});
	if (semiTransparent) {
		yuno.addBetaBadge(semiTransparent);
	}

	yuno.addSectionDivider();
}

void BuildWideMessagesMultiplier(
		SectionBuilder &builder,
		YunoSectionBuilder &yuno,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &YunoSettings::getInstance();

	constexpr auto kMinSize = 1.00;
	constexpr auto kStep = 0.05;

	const auto valueToIndex = [=](double value) {
		return static_cast<int>(std::round((value - kMinSize) / kStep));
	};

	const auto controller = builder.controller();
	yuno.addSlider({
		.id = u"yuno/messageBubbleRadius"_q,
		.title = tr::yuno_MessageBubbleRadius(),
		.steps = 17,
		.current = settings->messageBubbleRadius(),
		.indexToValue = [](int index) { return index; },
		.onChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
		},
		.onFinalChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
			YunoSettings::getInstance().setMessageBubbleRadius(index);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [](int index) {
			return QString::number(index);
		},
	});

	yuno.addSectionDivider();

	yuno.addSlider({
		.id = u"yuno/wideMultiplier"_q,
		.title = tr::yuno_SettingsWideMultiplier(),
		.steps = 61, // (4.00 - 1.00) / 0.05 + 1
		.current = valueToIndex(settings->wideMultiplier()),
		.indexToValue = [](int index) { return index; },
		.onChanged = nullptr,
		.onFinalChanged = [=](int index) {
			YunoSettings::getInstance().setWideMultiplier(
				kMinSize + index * kStep);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [=](int index) {
			return QString::number(kMinSize + index * kStep, 'f', 2);
		},
	});

	builder.addSkip();
	builder.addDividerText(tr::yuno_SettingsWideMultiplierDescription());
	builder.addSkip();
}

void BuildContextMenuElements(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	auto *settings = &YunoSettings::getInstance();

	builder.addSubsectionTitle(tr::yuno_ContextMenuElementsHeader());

	const auto options = std::vector{
		tr::yuno_SettingsContextMenuItemHidden(tr::now),
		tr::yuno_SettingsContextMenuItemShown(tr::now),
		tr::yuno_SettingsContextMenuItemExtended(tr::now),
	};

	yuno.addChooseButton({
		.id = u"yuno/showReactionsPanelInContextMenu"_q,
		.title = tr::yuno_SettingsContextMenuReactionsPanel(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showReactionsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowReactionsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconReactions },
	});
	yuno.addChooseButton({
		.id = u"yuno/showViewsPanelInContextMenu"_q,
		.title = tr::yuno_SettingsContextMenuViewsPanel(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showViewsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowViewsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconShowInChat },
	});
	yuno.addChooseButton({
		.id = u"yuno/showHideMessageInContextMenu"_q,
		.title = tr::yuno_ContextHideMessage(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showHideMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowHideMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconClear },
	});
	yuno.addChooseButton({
		.id = u"yuno/showUserMessagesInContextMenu"_q,
		.title = tr::yuno_UserMessagesMenuText(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showUserMessagesInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowUserMessagesInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconTTL },
	});
	yuno.addChooseButton({
		.id = u"yuno/showMessageDetailsInContextMenu"_q,
		.title = tr::yuno_MessageDetailsPC(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showMessageDetailsInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowMessageDetailsInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconInfo },
	});
	yuno.addChooseButton({
		.id = u"yuno/showRepeatMessageInContextMenu"_q,
		.title = tr::yuno_RepeatMessage(),
		.boxTitle = tr::yuno_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showRepeatMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { YunoSettings::getInstance().setShowRepeatMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::yunoRepeatMenuIcon },
	});
	if (settings->filtersEnabled()) {
		yuno.addChooseButton({
			.id = u"yuno/showAddFilterInContextMenu"_q,
			.title = tr::yuno_RegexFilterQuickAdd(),
			.boxTitle = tr::yuno_SettingsContextMenuTitle(),
			.initialSelection = static_cast<int>(settings->showAddFilterInContextMenu()),
			.options = options,
			.setter = [](int i) { YunoSettings::getInstance().setShowAddFilterInContextMenu(static_cast<ContextMenuVisibility>(i)); },
			.icon = { &st::menuIconAddToFolder },
		});
	}

	builder.addSkip();
	builder.addDividerText(tr::yuno_SettingsContextMenuDescription());
	builder.addSkip();
}

void BuildMessageFieldElements(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::yuno_MessageFieldElementsHeader());

	yuno.addSettingToggle({
		.id = u"yuno/showAttachButtonInMessageField"_q,
		.title = tr::yuno_MessageFieldElementAttach(),
		.getter = &YunoSettings::showAttachButtonInMessageField,
		.setter = &YunoSettings::setShowAttachButtonInMessageField,
		.icon = { &st::messageFieldAttachIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showCommandsButtonInMessageField"_q,
		.title = tr::yuno_MessageFieldElementCommands(),
		.getter = &YunoSettings::showCommandsButtonInMessageField,
		.setter = &YunoSettings::setShowCommandsButtonInMessageField,
		.icon = { &st::messageFieldCommandsIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showAutoDeleteButtonInMessageField"_q,
		.title = tr::yuno_MessageFieldElementTTL(),
		.getter = &YunoSettings::showAutoDeleteButtonInMessageField,
		.setter = &YunoSettings::setShowAutoDeleteButtonInMessageField,
		.icon = { &st::messageFieldTTLIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showEmojiButtonInMessageField"_q,
		.title = tr::yuno_MessageFieldElementEmoji(),
		.getter = &YunoSettings::showEmojiButtonInMessageField,
		.setter = &YunoSettings::setShowEmojiButtonInMessageField,
		.icon = { &st::messageFieldEmojiIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showMicrophoneButtonInMessageField"_q,
		.title = tr::yuno_MessageFieldElementVoice(),
		.getter = &YunoSettings::showMicrophoneButtonInMessageField,
		.setter = &YunoSettings::setShowMicrophoneButtonInMessageField,
		.icon = { &st::messageFieldVoiceIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showGiftButtonInMessageField"_q,
		.title = tr::lng_profile_action_short_gift(),
		.getter = &YunoSettings::showGiftButtonInMessageField,
		.setter = &YunoSettings::setShowGiftButtonInMessageField,
		.icon = { &st::settingsButtonIconGift },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showAiEditorButtonInMessageField"_q,
		.title = tr::lng_ai_compose_title(),
		.getter = &YunoSettings::showAiEditorButtonInMessageField,
		.setter = &YunoSettings::setShowAiEditorButtonInMessageField,
		.icon = { &st::messageFieldCocoonAiIcon },
	});

	yuno.addSectionDivider();
}

void BuildMessageFieldPopups(SectionBuilder &builder, YunoSectionBuilder &yuno) {
	builder.addSubsectionTitle(tr::yuno_MessageFieldPopupsHeader());

	yuno.addSettingToggle({
		.id = u"yuno/showAttachPopup"_q,
		.title = tr::yuno_MessageFieldElementAttach(),
		.getter = &YunoSettings::showAttachPopup,
		.setter = &YunoSettings::setShowAttachPopup,
		.icon = { &st::messageFieldAttachIcon },
	});
	yuno.addSettingToggle({
		.id = u"yuno/showEmojiPopup"_q,
		.title = tr::yuno_MessageFieldElementEmoji(),
		.getter = &YunoSettings::showEmojiPopup,
		.setter = &YunoSettings::setShowEmojiPopup,
		.icon = { &st::messageFieldEmojiIcon },
	});
}

const auto kMeta = BuildHelper({
	.id = YunoChats::Id(),
	.parentId = YunoMain::Id(),
	.title = &tr::yuno_CategoryChats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	auto yuno = YunoSectionBuilder(builder);
	const auto previewState = std::make_shared<PreviewState>();

	builder.addSkip();
	BuildStickersAndEmoji(builder, yuno);
	BuildRecentStickersLimit(builder, yuno);
	BuildGroupsAndChannels(builder, yuno);
	BuildMarks(builder, yuno, previewState);
	BuildWideMessagesMultiplier(builder, yuno, previewState);
	BuildContextMenuElements(builder, yuno);
	BuildMessageFieldElements(builder, yuno);
	BuildMessageFieldPopups(builder, yuno);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> YunoChats::title() {
	return tr::yuno_CategoryChats();
}

YunoChats::YunoChats(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void YunoChats::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type YunoChatsId() {
	return YunoChats::Id();
}

} // namespace Settings
