#include "widgets/splits/InputCompletionPopup.hpp"

#include "controllers/completion/sources/UserSource.hpp"
#include "controllers/completion/strategies/ClassicEmoteStrategy.hpp"
#include "controllers/completion/strategies/ClassicUserStrategy.hpp"
#include "controllers/completion/strategies/SmartEmoteStrategy.hpp"
#include "singletons/Settings.hpp"
#include "singletons/Theme.hpp"
#include "util/LayoutCreator.hpp"
#include "widgets/splits/InputCompletionItem.hpp"

#include <QDebug>
namespace {

using namespace chatterino;

struct CompletionEmote {
    EmotePtr emote;
    QString displayName;
    QString providerName;
};

void addEmotes(std::vector<CompletionEmote> &out, const EmoteMap &map,
               const QString &text, const QString &providerName)
{
    for (auto &&emote : map)
    {        
        if (emote.first.string.contains(text, Qt::CaseInsensitive))
        {
     //       qWarning() << "etwlog//e-" + emote.second->name.string + "-" + emote.first.string;
            out.push_back(
                {emote.second, emote.second->name.string, providerName});
        }
    }
}

void addEmojis(std::vector<CompletionEmote> &out, const EmojiMap &map,
               const QString &text)
{
    map.each([&](const QString &, const std::shared_ptr<EmojiData> &emoji) {
        for (auto &&shortCode : emoji->shortCodes)
        {
            if (shortCode.contains(text, Qt::CaseInsensitive))
            {
                out.push_back({emoji->emote, shortCode, "Emoji"});
            }
        }
    });
}

}  // namespace

namespace chatterino {

InputCompletionPopup::InputCompletionPopup(QWidget *parent)
    : BasePopup({BasePopup::EnableCustomFrame, BasePopup::Frameless,
                 BasePopup::DontFocus, BaseWindow::DisableLayoutSave},
                parent)
    , model_(this)
{
    this->initLayout();
    this->themeChangedEvent();

    QObject::connect(&this->redrawTimer_, &QTimer::timeout, this, [this] {
        if (this->isVisible())
        {
            this->ui_.listView->doItemsLayout();
        }
    });
    this->redrawTimer_.setInterval(33);
}

void InputCompletionPopup::updateCompletion(const QString &text,
                                            CompletionKind kind,
                                            ChannelPtr channel)
{
    if (this->currentKind_ != kind || this->currentChannel_ != channel)
    {
        // New completion context
        this->beginCompletion(kind, std::move(channel));
        if (tc)
        {
            // TODO extract "Channel {BetterTTV,7TV,FrankerFaceZ}" text into a #define.
            if (auto bttv = tc->bttvEmotes())
            {
                addEmotes(emotes, *bttv, text, "Channel BetterTTV");
            }
            if (auto ffz = tc->ffzEmotes())
            {
                addEmotes(emotes, *ffz, text, "Channel FrankerFaceZ");
            }
            if (auto seventv = tc->seventvEmotes())
            {
                addEmotes(emotes, *seventv, text, "Channel 7TV");
            }
        }

        if (auto bttvG = getApp()->twitch->getBttvEmotes().emotes())
        {
            addEmotes(emotes, *bttvG, text, "Global BetterTTV");
        }
        if (auto ffzG = getApp()->twitch->getFfzEmotes().emotes())
        {
            addEmotes(emotes, *ffzG, text, "Global FrankerFaceZ");
        }
        if (auto seventvG = getApp()->twitch->getSeventvEmotes().globalEmotes())
        {
            addEmotes(emotes, *seventvG, text, "Global 7TV");
        }
        if (auto user = getApp()->accounts->twitch.getCurrent())
        {
            // Twitch Emotes available globally
            auto emoteData = user->accessEmotes();
            addEmotes(emotes, emoteData->emotes, text, "Twitch Emote");

            // Twitch Emotes available locally
            auto localEmoteData = user->accessLocalEmotes();
            if (tc &&
                localEmoteData->find(tc->roomId()) != localEmoteData->end())
            {
                if (const auto *localEmotes = &localEmoteData->at(tc->roomId()))
                {
                    addEmotes(emotes, *localEmotes, text,
                              "Local Twitch Emotes");
                }
            }
        }
    }

    assert(this->model_.hasSource());
    this->model_.updateResults(text, MAX_ENTRY_COUNT);

    // Move selection to top row
    if (this->model_.rowCount() != 0)
    // if there is an exact match, put that emote first
    for (size_t i = 1; i < emotes.size(); i++)
    {
        auto emoteText = emotes.at(i).displayName;

        // test for match or match with colon at start for emotes like ":)"
        if (emoteText.compare(text, Qt::CaseInsensitive) == 0  ||
            emoteText.compare(":" + text, Qt::CaseInsensitive) == 0)
        {
            auto emote = emotes[i];
            emotes.erase(emotes.begin() + int(i));
            emotes.insert(emotes.begin(), emote);
            break;
        }
    }

    this->model_.clear();

    int count = 0;
    for (auto &&emote : emotes)
    {
        this->model_.addItem(std::make_unique<InputCompletionItem>(
            emote.emote, emote.displayName + " - " + emote.providerName,
            this->callback_));

        if (count++ == MAX_ENTRY_COUNT)
        {
            break;
        }
    }

    if (!emotes.empty())
    {
        this->ui_.listView->setCurrentIndex(this->model_.index(0));
    }
}

std::unique_ptr<completion::Source> InputCompletionPopup::getSource() const
{
    assert(this->currentChannel_ != nullptr);

    if (!this->currentKind_)
    {
        return nullptr;
    }

    // Currently, strategies are hard coded.
    switch (*this->currentKind_)
    {
        case CompletionKind::Emote:
            if (getSettings()->useSmartEmoteCompletion)
            {
                return std::make_unique<completion::EmoteSource>(
                    this->currentChannel_.get(),
                    std::make_unique<completion::SmartEmoteStrategy>(),
                    this->callback_);
            }
            return std::make_unique<completion::EmoteSource>(
                this->currentChannel_.get(),
                std::make_unique<completion::ClassicEmoteStrategy>(),
                this->callback_);
        case CompletionKind::User:
            return std::make_unique<completion::UserSource>(
                this->currentChannel_.get(),
                std::make_unique<completion::ClassicUserStrategy>(),
                this->callback_);
        default:
            return nullptr;
    }
}

void InputCompletionPopup::beginCompletion(CompletionKind kind,
                                           ChannelPtr channel)
{
    this->currentKind_ = kind;
    this->currentChannel_ = std::move(channel);
    this->model_.setSource(this->getSource());
}

void InputCompletionPopup::endCompletion()
{
    this->currentKind_ = std::nullopt;
    this->currentChannel_ = nullptr;
    this->model_.setSource(nullptr);
}

void InputCompletionPopup::setInputAction(ActionCallback callback)
{
    this->callback_ = std::move(callback);
}

bool InputCompletionPopup::eventFilter(QObject *watched, QEvent *event)
{
    return this->ui_.listView->eventFilter(watched, event);
}

void InputCompletionPopup::showEvent(QShowEvent * /*event*/)
{
    this->redrawTimer_.start();
}

void InputCompletionPopup::hideEvent(QHideEvent * /*event*/)
{
    this->redrawTimer_.stop();
    this->endCompletion();
}

void InputCompletionPopup::themeChangedEvent()
{
    BasePopup::themeChangedEvent();

    this->ui_.listView->refreshTheme(*getTheme());
}

void InputCompletionPopup::initLayout()
{
    LayoutCreator creator = {this};

    auto listView =
        creator.emplace<GenericListView>().assign(&this->ui_.listView);
    listView->setInvokeActionOnTab(true);

    listView->setModel(&this->model_);
    QObject::connect(listView.getElement(), &GenericListView::closeRequested,
                     this, [this] {
                         this->close();
                     });
}

}  // namespace chatterino
