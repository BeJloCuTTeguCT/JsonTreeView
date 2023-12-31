#include "widget.h"
#include "jsontreemodel.h"
#include "jsonobjectmodel.h"

#include <qboxlayout.h>
#include <qtreeview.h>
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>

#include <qdebug.h>

void Widget::ReadJsonFile(QString path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString jsonString = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QJsonObject obj = doc.object();
    QJsonArray array = obj["objects"].toArray();
    JsonObjectModel *model;
    foreach (QVariant element, array.toVariantList())
    {
        model = nullptr;
        QJsonObject object = element.toJsonObject();
        QList<int> parent_ids, child_ids;
        foreach (QVariant id, object[JSON_PARENTS].toArray().toVariantList())
            parent_ids.append(id.toInt());
        foreach (QVariant id, object[JSON_CHILDS].toArray().toVariantList())
            child_ids.append(id.toInt());
        foreach (JsonObjectModel *parentObj, m_objects) {
            foreach (int id, parentObj->Child_ids()) {
                if (object[JSON_ID].toInt() == id) {
                    model = new JsonObjectModel(object[JSON_ID].toInt(),
                                                object[JSON_NAME].toString(),
                                                parent_ids,
                                                child_ids,
                                                parentObj);
                    model->SetPathToIconImage(":/" + QString::number(object[JSON_ID].toInt()));
                    break;
                } // if
            } // foreach
        } // foreach
        if (model == nullptr) {
            model = new JsonObjectModel(object[JSON_ID].toInt(),
                                        object[JSON_NAME].toString(),
                                        parent_ids,
                                        child_ids);
            model->SetPathToIconImage(":/" + QString::number(object[JSON_ID].toInt()));
            // model->SetPathToIconImage("C:/bPO-211/PD/JsonTreeView/Resources/icons/16.png");
        } // if
        m_objects.push_back(model);
    } // foreach
}

Widget::Widget(QWidget *parent)
    : QWidget(parent), m_mainLayout(new QVBoxLayout(this)),
      m_treeView(new QTreeView(this)), m_jsonTreeModel(new JsonTreeModel(this))
{
    m_mainLayout->addWidget(m_treeView);
    this->setLayout(m_mainLayout);
    this->resize(350, 350);
    this->setWindowTitle("[Proto] Прототип отображения древовидных структур JSON");
    this->ReadJsonFile("..\\JsonTreeView\\json.json");
    foreach (JsonObjectModel *obj, m_objects){
        if (obj->Parent_ids().count() == 0)
            m_jsonTreeModel->addItem(obj, QModelIndex());
    } // foreach
    m_jsonTreeModel->setColumns(QStringList() << JSON_NAME << JSON_ID);
    m_treeView->setModel(m_jsonTreeModel);
}

Widget::~Widget()
{
    delete m_jsonTreeModel;
    delete m_treeView;
}
