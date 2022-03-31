#ifndef __EPUB_VIEW__H
#define __EPUB_VIEW__H

#include <QDebug>
#include <QString>
#include <QTextBrowser>

#include "edocument.h"

namespace EPUB {

class EpubView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit EpubView(QWidget* parent = 0);
    ~EpubView();

    auto loadFile(const QString& path) -> void;
    auto setPos(const QUrl& url) -> void;
    auto getToc() -> QList<EpubToc> { return this->_document->getToc(); }
    auto getMetaInfo() -> Document::MetaInfo { return this->_document->getMeta(); }
    auto setFile(const QString& fileName) -> void { this->_document->setFile(fileName); this->update(); }
    auto getFile() -> QString { return this->_document->openedFile(); }

private slots:
    void gotoToc(const QUrl& url);

private:
    Document* _document;
    int _offset;
};

}

#endif // __EPUB__VIEW__H
