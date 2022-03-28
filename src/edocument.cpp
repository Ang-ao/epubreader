#include "edocument.h"

namespace EPUB {

Document::Document(const QString& fileName, QObject* parent)
    : QTextDocument(parent)
    , _fileName(fileName)
    , _coverPager("")
    , _useBaseRef(false)
    , _opened(false)
{
    QFileInfo fileInfo(fileName);
    QString name = fileInfo.baseName();
    this->_bookPath = QCoreApplication::applicationDirPath() + "/books/" + name + "/";
    QDir dir(this->_bookPath);
    if(not dir.exists()) dir.mkpath(this->_bookPath);
    EPUBDEBUG() << "fileName: " << name;
    EPUBDEBUG() << "bookPath: " << this->_bookPath;
}

Document::~Document() { }

auto Document::open() -> bool
{
    UnZipStream* unZip = new UnZipStream(this->_fileName);

    if(unZip->canRead())
    {
        // save files to local, if don't on ram
        const QStringList entries = unZip->fileList();
        DataMap allFiles = unZip->listData();
        for(auto it = allFiles.begin(); it != allFiles.end(); ++it)
        {
            QByteArray chunk = it.value();

            QString name(it.key());
            EPUBDEBUG() << "file zip in: " << name;
            QString mimeType = MimeinFile(name);
            if(QLatin1String("text/html") == mimeType or QLatin1String("application/xhtml+xml") == mimeType)
            {
                this->_uniqueuris.insert(name);
                this->_textData.insert(name, it.value());
            }
            else if(QLatin1String("image/jpeg") == mimeType)
            {
               this->_imageData.insert(name, it.value()); 
            }
            else if(QLatin1String("text/css") == mimeType)
            {
                this->_styleData.insert(name, it.value());
            }
            else 
            {
                this->_imageData.insert(name, it.value());
            }

            if(METAINFOCONTAINERFILE == name)
            {
                if(chunk.size() < 4) return false;
                if(this->metaReader(chunk))
                {
                    this->_opened = true;
                    return true;
                }
            }
            this->_cache.insert(name, chunk);
            // this->saveFile(name, it);
        }
    }
    delete unZip;
    unZip = nullptr;
    return false;
}

auto Document::getDomElementFromXml(const QByteArray& xml) -> QDomElement
{
    QDomDocument root;
    if(xml.size() < 5) return root.createElement("root");

    QDomDocument document;
    if(document.setContent(xml, true));
    {
        QDomElement node = document.documentElement();
        EPUBDEBUG() << "sucess getDomElementFromXml first tagName: " << node.tagName();
        return node;
    }

    return root.createElement("root");
}

auto Document::metaReader(QByteArray& xml) -> bool
{
    QString containerFile{"META-INF/container.xml"};
    bool ret{false};
    QDomElement root = this->getDomElementFromXml(this->_metaData[containerFile]);
    QDomNodeList der = root.elementsByTagName("rootfile");
    if(0 == der.count())
    {
        EPUBWARNING() << "unable to get place from file content.opf, inside META-INF/container.xml";
        return ret;
    }
    this->_metaData.remove(containerFile);
    QString contentFile{""};

    for(int i{0}; i < der.count(); ++i) 
    {
        QDomElement node = der.at(i).toElement();
        contentFile = node.attribute("full-path"); // content.opf
        EPUBDEBUG() << "Start on file:" << contentFile << __LINE__;
    }

    if(0 == contentFile.size())
    {
        EPUBWARNING() << "unable to get place from file content.opf, inside META-INF/container.xml";
        return false;
    }
    if(contentFile.contains("/"))
    {
        this->_baseRefDir = contentFile.left(contentFile.lastIndexOf("/")) + "/";
    }

    QDomNodeList metaList = this->getPageName(contentFile, QString("metadata"));
    QDomNodeList list = metaList.at(0).toElement().childNodes();
    for(int i{0}; i < list.size(); ++i)
    {
        QDomNode&& tmp = list.at(i);
        if("dc" == tmp.prefix())
        {
            this->_metaInfo[tmp.localName()] << tmp.firstChild().toText().data();
        }
    }
    QDomNodeList guideList = this->getPageName(contentFile, QString("guide"));
    this->_coverPager = guideList.at(0).toElement().firstChildElement().attribute("htef");

    QString tocFile{""};
    QDomNodeList itemlist = this->getPageName(contentFile, QString("item"));
    for(int i{0}; i < itemlist.count(); ++i) 
    {
        QDomElement nodepager = itemlist.at(i).toElement();
        if("ncx" == nodepager.attribute("id"))
        {
            tocFile = nodepager.attribute("href");
        }
    }
    EPUBDEBUG() << "get toc from  " << tocFile << __LINE__;

    QDomNodeList navitom = this->getPageName(tocFile, "navMap");

    if (1 == navitom.count())
    {
        QDomElement elem = navitom.at(0).toElement();
        ret = this->readMenu(elem);
    }
    this->_metaData.remove(tocFile);

    if(this->_coverPager.isEmpty())
    {
        EPUBWARNING() << "Coverpage or StartPage variable is not full! from file " << this->_nextFileToReadCrono;
    }
    else ret = true;

    return ret;
}

auto Document::readMenu(const QDomElement& element) -> bool
{
    QDomElement child = element.firstChildElement();

    while(not child.isNull())
    {
        
        child = child.nextSiblingElement();
    }
    return true;
}

auto Document::getPageName(const QString fileName, const QString tag) -> QDomNodeList
{
    EPUBDEBUG() << "Request GetPageName: file_name/tag" << fileName << " : " << tag << " current actioncycle.";

    QDomNodeList defineterror;

    QByteArray chunx = this->_metaData[fileName]; 
    if(0 == chunx.size()) return defineterror;

    QDomElement e = this->getDomElementFromXml(chunx);
    QDomNodeList der = e.elementsByTagName(tag);
    QDomNodeList dera = e.elementsByTagName(e.tagName());

    if(der.count() > 0) return der;
    else if(dera.count() > 0) return dera;

    EPUBDEBUG() << "Request Error: " << fileName << ":" << tag << " export FAIL!....";
    return defineterror;
}

auto Document::saveFile(const QString& name, DataMap::iterator& data) -> void
{
    QString savePath = this->_bookPath + name;
    QString dirPath = savePath.left(savePath.lastIndexOf("/")) + "/";
    QDir dir(dirPath);
    if(not dir.exists()) dir.mkpath(dirPath);

    QFile file(savePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data.value());
    }
}

} // namespace EPUB