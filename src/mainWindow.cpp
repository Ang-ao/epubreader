#include "mainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _stackedWidget(new QStackedWidget(this))
    , _treeWidget(new QTreeWidget(this))
    , _epubView(new EPUB::EpubView(this))
{
    this->resize(1000, 800);
    this->setWindowTitle(tr("Epub Reader"));

    this->initLayout();
}

MainWindow::~MainWindow()
{

}

auto MainWindow::initLayout() -> void
{
    this->setMenu();
    this->setHomeWidge();
    this->setViewWidget();

    this->setCentralWidget(this->_stackedWidget);
    this->gotoStackedWidgetPage(0);
}

auto MainWindow::setMenu() -> void
{
    // set menubar
    auto menuBar = new QMenuBar(this);
    this->setMenuBar(menuBar);
    // option menu
    auto optionMenu = new QMenu(menuBar);
    menuBar->addAction(optionMenu->menuAction());
    optionMenu->setTitle(tr("Option(&O)"));
    // to home page
    auto homeAction = new QAction(this);
    homeAction->setText(tr("Home(&H)"));
    homeAction->setStatusTip(tr("go to home page"));
    this->connect(homeAction, SIGNAL(triggered()), this, SLOT(gotoStackedWidgetPage()));
    // open file
    auto openAction = new QAction(this);
    openAction->setText(tr("Open(&O)"));
    openAction->setStatusTip(tr("open an existing file"));
    this->connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    // close window
    auto closeAction = new QAction(this);
    closeAction->setText(tr("Close(&E)"));
    closeAction->setStatusTip(tr("clost the window"));
    this->connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    // set to menu
    optionMenu->addAction(homeAction);
    optionMenu->addSeparator();
    optionMenu->addAction(openAction);
    optionMenu->addSeparator();
    optionMenu->addAction(closeAction);
}

auto MainWindow::setHomeWidge() -> void
{
    QWidget* homeWidget = new QWidget(this);
    QLabel* label = new QLabel(homeWidget);
    label->setText(tr("Hello epub!!!"));
    label->setAlignment(Qt::AlignCenter);

    QHBoxLayout* mainLayout = new QHBoxLayout(homeWidget);
    mainLayout->addWidget(label);
    homeWidget->setLayout(mainLayout);
    this->_stackedWidget->addWidget(homeWidget);
}

auto MainWindow::setViewWidget() -> void
{
    QMainWindow* viewWidget = new QMainWindow(this);
    QDockWidget* tocWidget = new QDockWidget(tr("TOC"), viewWidget);
    tocWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    tocWidget->setFeatures(QDockWidget::DockWidgetMovable);
    tocWidget->setWidget(this->_treeWidget);
    viewWidget->setCentralWidget(this->_epubView);
    viewWidget->addDockWidget(Qt::RightDockWidgetArea, tocWidget);

    this->_stackedWidget->addWidget(viewWidget);
}

auto MainWindow::gotoStackedWidgetPage(int page) -> void
{
    Q_ASSERT(page >= 0);
    this->_stackedWidget->setCurrentIndex(page);
}

auto MainWindow::openFile() -> void
{
    QFileDialog fileGet;
    auto fileName = fileGet.getOpenFileName(this, tr("Open File"), tr("./"), tr("EPUBS(*.epub)"));
    if("" == fileName) return;

    this->_epubView->loadFile(fileName);
    this->setToc();
    this->gotoStackedWidgetPage(1);
}

auto MainWindow::setToc() -> void
{
    this->_treeWidget->clear();
    auto tocData = this->_epubView->getToc();

}