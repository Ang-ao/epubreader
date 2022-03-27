#include "epubView.h"

EpubView::EpubView(QWidget* parent)
    : QWidget(parent)
{
    this->loadFile(QString("file:d:/GitHub/epubreader/res/Mastering.epub"));
}

EpubView::~EpubView()
{

}

auto EpubView::loadFile(const QString& path) -> void
{
    /*
    this->_playEpub = new EPUB::Converter();
    const QString ebookFile = urie.toLocalFile();
    qDebug() << "ebook file: " << ebookFile;
    this->_activeEpub = this->_playEpub->convert(ebookFile, _ZIPCACHEBOOK_);
    qDebug() << "file size: " << this->_activeEpub.size();
    if(this->_activeEpub.size() > 0)
    {
        EpubToc fox = this->_activeEpub.at(0);
        QFileInfo fileInfo(fox.jumpUrl);
        this->jumpEpub(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        qDebug() << "file url: " << fox.jumpUrl;
        emit this->paintMenuEpub(true);
    }
    */
}

auto EpubView::scroll(int amount) -> void
{
    int offset = this->_offset + amount;
    offset = qMin(static_cast<int>(this->_document->size().height() - this->_document->pageSize().height()), offset);
    this->_offset = qMax(0, offset);
    this->update();
}

auto EpubView::scrollPage(int amount) -> void
{
    int currentPage = this->_offset / this->_document->pageSize().height();
    currentPage += amount;
    int offset = currentPage * this->_document->pageSize().height();
    offset = qMin(static_cast<int>(this->_document->size().height() - this->_document->pageSize().height()), offset);
    this->_offset = qMax(0, offset);
    this->update();
}

void EpubView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
    if(!this->_document)
    {
        painter.drawText(rect(), Qt::AlignCenter, "Loading...");
        return;
    }

    QAbstractTextDocumentLayout::PaintContext paintContext;
    paintContext.clip = rect();
    paintContext.clip.translate(0, this->_offset);

    paintContext.palette = palette();
    for(int group = 0; group < 3; ++group)
    {
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::WindowText, Qt::black);
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Light, Qt::black);
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Text, Qt::black);
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Base, Qt::black);

        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Background, Qt::white);
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Window, Qt::white);
        paintContext.palette.setColor(QPalette::ColorGroup(group), QPalette::Button, Qt::white);
    }

    painter.translate(0, -this->_offset);
    painter.setClipRect(paintContext.clip);
    this->_document->documentLayout()->draw(&painter, paintContext);
}

void EpubView::keyPressEvent(QKeyEvent* e) 
{
    switch (e->key())
    {
    case Qt::Key_Up:
        this->scroll(-40);
        break;
    case Qt::Key_Down:
        this->scroll(40);
        break;
    case Qt::Key_Left:
        this->scrollPage(-1);
        break;
    case Qt::Key_Right:
        this->scrollPage(1);
        break;
    case Qt::Key_End:
        this->_offset = this->_document->size().height() - this->_document->pageSize().height();
        update();
    default:
        break;
    }
}
void EpubView::resizeEvent(QResizeEvent*)
{
    this->_document->setPageSize(this->size());
    this->update();
}