#include "pe/ui/launcher.hpp"

#include <algorithm>

namespace pe::ui
{

using audio::Cue;

namespace
{
constexpr Rect kListPanel{108.0f, 188.0f, 820.0f, 720.0f};
constexpr Rect kDetailPanel{980.0f, 188.0f, 820.0f, 720.0f};
constexpr Rect kWindow{138.0f, 270.0f, 760.0f, 564.0f};
constexpr float kRowHeight = 90.0f;
}

const Translation *Launcher::translation_of(std::uint64_t title_id) const
{
    const auto at = std::find_if(translations_.list.begin(), translations_.list.end(),
                                 [title_id](const Translation &entry)
                                 { return title_id != 0 && entry.title_id == title_id && entry.error.empty(); });
    return at == translations_.list.end() ? nullptr : &*at;
}

bool Launcher::translation_in_library(const Translation &entry) const
{
    return entry.error.empty() && std::any_of(games_.begin(), games_.end(), [&](const Game &game)
    { return !game.remote && game.title_id != 0 && game.title_id == entry.title_id; });
}

const char *Launcher::translation_action(const Translation &entry) const
{
    if (translations_.busy && translations_.active_title == entry.title_id)
        return tr("Working...");
    if (!entry.error.empty())
        return tr("Unavailable");
    if (entry.managed)
        return tr("Remove");
    return entry.installed ? tr("Installed manually") : tr("Install");
}

void Launcher::change_translation(const Translation &entry)
{
    if (translations_.busy || !entry.error.empty() || (entry.installed && !entry.managed))
        return;
    const bool started = entry.managed ? services_.remove_translation(entry.title_id) :
                                         services_.install_translation(entry.title_id);
    if (started)
    {
        const auto next = services_.translations();
        translations_.busy = next.busy;
        translations_.active_title = next.active_title;
        translations_wait_ = 0.0f;
        message_.clear();
        cue(Cue::select);
    }
    else
    {
        say(tr("Could not start the translation operation. Please try again."), true);
        cue(Cue::error);
    }
}

void Launcher::poll_translations(float dt)
{
    translations_wait_ -= dt;
    if (translations_wait_ > 0.0f)
        return;
    translations_wait_ = 0.35f;
    Translations next = services_.translations();
    const bool changed = next.generation != translations_.generation;
    const bool finished = translations_.busy && !next.busy;
    const std::string selected = translation_rows_.selected >= 0 &&
        translation_rows_.selected < static_cast<int>(translations_.list.size()) ?
            translations_.list[static_cast<std::size_t>(translation_rows_.selected)].name : std::string{};
    std::stable_sort(next.list.begin(), next.list.end(), [&](const Translation &a, const Translation &b)
    {
        const bool local_a = translation_in_library(a), local_b = translation_in_library(b);
        return local_a != local_b ? local_a : a.name < b.name;
    });
    int position = 0;
    for (std::size_t i = 0; i < next.list.size(); ++i)
        if (next.list[i].name == selected)
            position = static_cast<int>(i);
    translations_ = std::move(next);
    if (changed || finished || position != translation_rows_.selected)
        translation_rows_.reset(static_cast<int>(translations_.list.size()), position);
    if (!changed && !finished)
        return;
    for (Game &game : games_)
        count_mods(game, services_.mods(game.title_id));
    read_home();
    if ((modal_ == Modal::game || modal_ == Modal::mods) && !games_.empty())
    {
        Game &game = games_[static_cast<std::size_t>(library_.selected)];
        const int selected_mod = mod_rows_.selected;
        read_mods(game);
        mod_rows_.reset(static_cast<int>(mod_list_.size()), selected_mod);
        const int selected_row = game_rows_.selected;
        game_rows_.reset(dialog_rows(Modal::game), selected_row);
        option_ = game_rows_.selected;
    }
    if (finished)
    {
        say(translations_.error.empty() ? tr("Translation updated. Applies on next launch.") :
                                          translations_.error,
            !translations_.error.empty());
        cue(translations_.error.empty() ? Cue::saved : Cue::error);
    }
}

void Launcher::open_translations(std::uint64_t title_id)
{
    translations_return_ = screen_;
    translations_modal_ = modal_;
    modal_ = modal_shown_ = Modal::none;
    translations_wait_ = 0.0f;
    poll_translations(0.0f);
    std::stable_sort(translations_.list.begin(), translations_.list.end(), [&](const Translation &a,
                                                                            const Translation &b)
    {
        const bool local_a = translation_in_library(a), local_b = translation_in_library(b);
        return local_a != local_b ? local_a : a.name < b.name;
    });
    int position = 0;
    for (std::size_t i = 0; i < translations_.list.size(); ++i)
        if (translations_.list[i].title_id == title_id && translations_.list[i].error.empty())
            position = static_cast<int>(i);
    translation_rows_.visible = 6;
    translation_rows_.pitch = 94.0f;
    translation_rows_.reset(static_cast<int>(translations_.list.size()), position);
    open(Screen::translations, true);
}

void Launcher::press_translations(Key key)
{
    switch (key)
    {
    case Key::circle:
        open(translations_return_, false);
        if (translations_modal_ != Modal::none)
        {
            modal_ = modal_shown_ = translations_modal_;
            if (!games_.empty())
            {
                read_mods(games_[static_cast<std::size_t>(library_.selected)]);
                mod_rows_.reset(static_cast<int>(mod_list_.size()), mod_rows_.selected);
                game_rows_.reset(dialog_rows(Modal::game), game_rows_.selected);
                option_ = game_rows_.selected;
            }
        }
        return;
    case Key::up:
    case Key::down:
        if (translation_rows_.move(key == Key::down ? 1 : -1))
        {
            message_.clear();
            cue(Cue::focus);
        }
        return;
    case Key::cross:
        if (translation_rows_.selected >= 0 &&
            translation_rows_.selected < static_cast<int>(translations_.list.size()))
            change_translation(translations_.list[static_cast<std::size_t>(translation_rows_.selected)]);
        return;
    default:
        return;
    }
}

void Launcher::draw_translations(Canvas &c)
{
    auto &list = c.list;
    draw_frame(c, tr("PT-BR translations"), tr("Community translations from the CNX list"));
    glass(c, kListPanel, 26.0f, theme::kPanel.with_alpha(0.80f), theme::kPanelEdge.with_alpha(0.55f));
    glass(c, kDetailPanel, 26.0f, theme::kPanel.with_alpha(0.80f), theme::kPanelEdge.with_alpha(0.55f));
    const std::string heading = fill(tr("{0} translations"), {std::to_string(translations_.list.size())});
    text(c, heading, 150.0f, baseline(214.0f, 32.0f, theme::kText24), theme::kText24, theme::kLimePale);
    const Translation *entry = translation_rows_.selected >= 0 &&
        translation_rows_.selected < static_cast<int>(translations_.list.size()) ?
            &translations_.list[static_cast<std::size_t>(translation_rows_.selected)] : nullptr;
    list.push_clip({kWindow.x - 12.0f, kWindow.y, kWindow.w + 24.0f, kWindow.h});
    if (entry != nullptr)
        plate_focus(c, kRowPlate, {150.0f, kWindow.y + translation_rows_.cursor() - translation_rows_.scroll(),
                                  736.0f, kRowHeight}, 1.0f);
    for (int row = translation_rows_.first_row(); row <= translation_rows_.last_row(); ++row)
    {
        const auto &item = translations_.list[static_cast<std::size_t>(row)];
        const float top = kWindow.y + static_cast<float>(row) * translation_rows_.pitch - translation_rows_.scroll();
        const bool local = translation_in_library(item);
        list.push_opacity(translation_rows_.row_alpha(row, kRowHeight));
        text_fit(c, item.name, 176.0f, baseline(top + 10.0f, 36.0f, theme::kText24), theme::kText24,
                 local ? theme::kLimePale : theme::kValue, 684.0f);
        const std::string state = item.installed ? tr("Installed") : tr("Not installed");
        const std::string summary = (local ? std::string(tr("In your library")) + " · " : "") +
                                    item.size + " · " + (!item.error.empty() ? tr("Invalid metadata") : state);
        text_shrink(c, summary, 176.0f, baseline(top + 49.0f, 30.0f, theme::kSmall), theme::kSmall,
                    !item.error.empty() ? theme::kWarning : theme::kMeta, 684.0f);
        list.pop_opacity();
    }
    list.pop_clip();
    scrollbar(c, translation_rows_, 908.0f, kWindow.y, kWindow.h);
    if (entry == nullptr)
        text_block(c, translations_.loading ? tr("Reading the CNX list...") : tr("The translation list is unavailable."),
                   150.0f, baseline(300.0f, 38.0f, theme::kText24), theme::kText24, 38.0f,
                   theme::kCopy, 736.0f, 3, kShrink);
    if (translations_.loading || !translations_.list.empty())
        text_shrink(c, translations_.loading ? tr("Updating the list...") :
                        translations_.cached ? tr("Saved list. Updated once a day.") : tr("List updated."),
                    150.0f, baseline(856.0f, 28.0f, theme::kSmall), theme::kSmall, theme::kMeta, 736.0f);
    if (entry != nullptr)
    {
        text_fit(c, entry->name, 1016.0f, baseline(214.0f, 62.0f, theme::kLead), theme::kLead,
                 theme::kTitle, 748.0f);
        text_shrink(c, !entry->error.empty() ? tr("Unavailable") :
                      entry->installed ? tr("PT-BR translation installed (CNX)") :
                                         tr("PT-BR translation available (CNX)"),
                    1016.0f, baseline(290.0f, 34.0f, theme::kText24), theme::kText24,
                    !entry->error.empty() ? theme::kWarning : theme::kLimePale, 748.0f);
        text(c, entry->size, 1016.0f, baseline(338.0f, 34.0f, theme::kText24), theme::kText24, theme::kValue);
        text_shrink(c, translation_action(*entry), 1764.0f, baseline(338.0f, 34.0f, theme::kText24),
                    theme::kText24, translations_.busy || !entry->error.empty() ||
                        (entry->installed && !entry->managed) ? theme::kMeta : theme::kLime,
                    520.0f, Align::right);
        if (!entry->error.empty())
            text_block(c, entry->error, 1016.0f, baseline(384.0f, 30.0f, theme::kSmall),
                       theme::kSmall, 30.0f, theme::kWarning, 748.0f, 2, kShrink);
        else if (entry->installed && !entry->managed)
            text_block(c, tr("Installed outside the app. This folder is kept; only PT-BR CNX can be removed here."),
                       1016.0f, baseline(384.0f, 30.0f, theme::kSmall), theme::kSmall, 30.0f,
                       theme::kCopy, 748.0f, 2, kShrink);
    }
    list.rounded_rect({1016.0f, 454.0f, 748.0f, 1.0f}, 0.0f, theme::kRule);
    text_block(c, tr("Some translations require a specific game version or extra steps. Check the release instructions and the update in updates/."),
               1016.0f, baseline(478.0f, 32.0f, theme::kSmall), theme::kSmall, 32.0f,
               theme::kWarning, 748.0f, 4, kShrink);
    text_block(c, "Lista do CNX (CostelaCNX, gamemoddesignbr/nx-links), arquivos de AMSNX/Traducoes-BR-NX, feitos pela comunidade",
               1016.0f, baseline(624.0f, 30.0f, theme::kSmall), theme::kSmall, 30.0f,
               theme::kCopy, 748.0f, 4, kShrink);
    static constexpr const char *kSites[] = {"jumpmanclubbrasil.com.br", "traducoespkg.tumblr.com", "nintendonxbr.blogspot.com"};
    for (int i = 0; i < 3; ++i)
        text(c, kSites[i], 1016.0f, baseline(756.0f + 32.0f * static_cast<float>(i), 30.0f, theme::kSmall),
             theme::kSmall, theme::kLimePale);
    const std::string status = !message_.empty() ? message_ : translations_.error;
    if (!status.empty())
        notice(c, status, 110.0f, baseline(916.0f, 28.0f, theme::kSmall), theme::kSmall,
               message_warning_ || !translations_.error.empty() ? theme::kWarning : theme::kLimePale,
               1700.0f, message_warning_ || !translations_.error.empty());
    const Hint hints[] = {{Pad::cross, entry == nullptr ? "" : translation_action(*entry)},
                          {Pad::circle, tr("Back")}, {Pad::updown, tr("Browse translations")}};
    draw_footer(c, entry != nullptr && entry->error.empty() && !(entry->installed && !entry->managed) &&
                       !translations_.busy ? hints : hints + 1,
                entry != nullptr && entry->error.empty() && !(entry->installed && !entry->managed) &&
                    !translations_.busy ? 3 : 2);
}

}
