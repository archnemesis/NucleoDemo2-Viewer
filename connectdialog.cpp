#include "connectdialog.h"
#include "ui_connectdialog.h"
#include <QSettings>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    QSettings settings;
    int size = settings.beginReadArray("hostHistory");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        ui->hostname->addItem(QString("%1")
                              .arg(settings.value("host").toString()));
    }
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

const QString ConnectDialog::getHostname()
{
    return ui->hostname->currentText();
}

const int ConnectDialog::getPort()
{
    return ui->port->value();
}
