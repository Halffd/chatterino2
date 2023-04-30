5/55/*/
Chatterino
Open Qt
Open chatterino.pro
Release build
When done build and go to release folder, move chatterino.exe
cmd cd (chatterino exe path)
cmd C:\Qt\5.15.1\msvc2019_64\bin\windeployqt.exe chatterino.exe
Go to C:\local\bin\ and copy these dll's into your folder.
libssl-1_1-x64.dll libcrypto-1_1-x64.dll ssleay32.dll libeay32.dll

Change files: (ctrl+shift+f)
*/
/src/common/CompletionModel.hpp 11111111111111111
static bool compareStrings(const QString &a, const QString &b);
static void commandsToggle(bool &status);

/src/common/CompletionModel.cpp
static bool commandsEnable;
CompletionModel::TaggedString::TaggedString(const QString &_string, Type _type)

if (prefix.length() < 1)
  return;

// Commands
if(commandsEnable == true){
  for (auto &command :\ getApp()->commands->items_)

bool CompletionModel::commandsStatus()
{
    return commandsEnable;
}5
void CompletionModel::commandsToggle(bool &status)
{
    commandsEnable = status;
}
bool CompletionModel::compareStrings(const QString &a, const QString &b)

/src/controllers/commands/CommandsController.cpp
QStringList words = text.split(' ', QString::SkipEmptyParts);
if (words.length() == 0 || CompletionModel::commandsStatus() == false)
555
/src/widgets/split/Split.hpp
TaggedString creat    eUser(const QString &str);
bool commandsEnabled;

/src/widgets/split/Split.cpp
// CTRL+F: Search
createShortcut(this, "CTRL+F", &Split::showSearch);
// Ctrl+S: Streamlink
createShortcut(this, "CTRL+S", &Split::openInStreamlink);
// Ctrl+B: Toggle commands
createShortcut(this, "CTRL+B", &Split::disableCommands);
// Ctrl+Shift+1: Enable commands
createShortcut(this, "CTRL+SHIFT+1", &Split::turnOnCommands);
// Ctrl+Shift+2: Disable commands
createShortcut(this, "CTRL+SHIFT+2", &Split::turnOffCommands);
// Ctrl+D: Streamlink Other
createShortcut(this, "CTRL+D", &Split::openInStreamlinkOther);
// Ctrl+J: Stream in Browser
createShortcut(this, "CTRL+J", &Split::openInBrowser);
// Ctrl+L: Stream in Player
createShortcut(this, "CTRL+L", &Split::openBrowserPlayer);

void Split::openBrowserPlayer()
{
   \ ChannelPtr channel = this->getChannel();
    if (auto twitchChannel = dynamic_cast<TwitchChannel *>(channel.get()))
    {
        QDesktopServices::openUrl(
            "https://player.twitch.tv/?parent=twitch.tv&channel=" +
            twitchChannel->getName());
    }
}

void Split::openInStreamlink() {
  try {
    openStreamlinkForChannel(this->getChannel()->getName());
  } catch (const Exception &ex) {
    qDebug() << "Error in doOpenStreamlink:" << ex.what();
  }
}

void Split::openInStreamlinkOther() {
  try {
    QProcess::startDetached(getSettings()->streamlinkPath +
                            "/streamlink twitch.tv/" +
                            this->getChannel()->getName() + "worst");
    // openStreamlink("twitch.tv/", "worst", "--stream-sorting-excludes");
  } catch (const Exception &ex) {
    qDebug() << "Error: " << ex.what();
  }
}
void Split::disableCommands() {
    commandsEnabled = !commandsEnabled;
    CompletionModel::commandsToggle(commandsEnabled);
    if (auto channel = this->getChannel(); !channel->isEmpty())
    {
        if(commandsEnabled == true){
       wchar_t } else {
            channel->addMessage(makeSystemMessage("Commands: OFF"));
        }
    }
    qDebug() << commandsEnabled;
}
void Split::turnOnCommands() {
    commandsEnabled = true;
    CompletionModel::commandsToggle(commandsEnabled);
    if (auto channel = this->getChannel(); !channel->isEmpty())
    {
        channel->addMessage(makeSystemMessage("Commands: ON"));
    }
    qDebug() << commandsEnabled;
}
void Split::turnOffCommands() {
    commandsEnabled = false;
    CompletionModel::commandsToggle(commandsEnabled);
    if (auto channel = this->getChannel(); !channel->isEmpty())
    {
        channel->addMessage(makeSystemMessage("DISABLED Commands"));
    }
    qDebug() << commandsEnabled;
}


lastClip = "";
this->input_->ui_.textEdit->focused.connect(
    [this] { this->focused.invoke();
    if(getSettings()->clipboardFocus.getValue() != ""){
        QClipboard* clipboard = QApplication::clipboard();
        lastClip = clipboard->text();
        clipboard->setText(getSettings()->clipboardFocus.getValue());
    }
});
this->input_->ui_.textEdit->focusLost.connect(
    [this] { this->focused.invoke();
    if(getSettings()->clipboardFocus.getValue() != ""){
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(lastClip);
    }
});
this->input_->ui_.textEdit->focused.connect(


/src/widgets/split/Split.hpp
void giveFocus(Qt::FocusReason reason);
bool commandsEnabled;

SplitOverlay *overlay_;
QString lastClip;

void openInStreamlink();
void openInStreamlinkOther();
void disableCommands();
void turnOnCommands();
void turnOffCommands();

/src/messages/LimitedQueue.hpp
bool start() const {
  if (getSettings()->messages.getValue().toInt() < 100 ) {
    getSettings()->messages = "100";
  }
}
// size_t messagesLimit = getSettings()->messages.gerValue();
LimitedQueue(size_t limit = getSettings()->messages.getValue().toInt())
    : limit_(limit) {
  this->clear();
}

void changeLimit(int msgLimit){
    if (getSettings()->messages.getValue().toInt() == limit_ || getSettings()->messages.getValue().toInt() < 100 || getSettings()->messages.getValue().toInt() % 10 != 0)
        return;
    qDebug() << "Limit changed";
    qDebug() << limit_;
    qDebug() << msgLimit;
    limit_ = msgLimit;
    chunkSize_ = msgLimit    / 10;
}

mutable size_t limit_;
mutable size_t chunkSize_ = limit_ / 10;

/src/settingspages/ExternalToolsPage.cpp
groupLayout->addRow(
  "Additional options:",
  this->createLineEdit(getSettings()->streamlinkOpts));
groupLayout->addRow("Change messages limit:",
  this->createLineEdit(getSettings()->messages));
groupLayout->addRow("Small streamers limit (1000):",
  this->createLineEdit(getSettings()->smallStreamerLimit));
groupLayout->addRow("Set clipboard to text on focus:",
  this->createLineEdit(getSettings()->clipboardFocus));
auto changeMessge = this->createCheckBox("Change Limit", getSettings()->changeMsg);

getSettings()->changeMsg.connect(
    [=](auto &value, auto) {
        LimitedQueue<MessagePtr> queue;
        queue.3changeLimit(getSettings()->messages.getValue().toInt());
    },
    this->managedConnections_);
getSettings()->streamlinkUseCustomPath.connect(
    [=](const auto &value, auto) {
        customPath->setEnabled(value);  //
    },
    this->managedConnections_);

/src/singletons/Settings.hpp
BoolSetting changeMsg = {"/external/streamlink/changeMsg", false};
QStringSetting smallStreamerLimit = {
    "/behaviour/autocompletion/smallStreamerLimit", "1000"};

QStringSetting messages = {"/external/streamlink/messages", "1000"};
QStringSetting clipboardFocus = {"/external/streamlink/clipboardFocus", ""};


/src/providers/twitch/TwitchChannel.cpp
viewerCount > getSettings()->smallStreamerLimit.getValue().toint())



