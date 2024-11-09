#include "HotKeyWidget.h"
#include "ui_HotKeyWidget.h"

#include <QMessageBox>

HotKeyWidget::HotKeyWidget(HotKey *capture, HotKey *record, QWidget *parent):
    QWidget(parent), ui(new Ui::HotKeyWidget), m_capture(capture), m_record(record) {

    ui->setupUi(this);

    connect(ui->radioButton, &QRadioButton::toggled, this, &HotKeyWidget::switchHotKey);
    connect(ui->cancel, &QPushButton::clicked, this, &HotKeyWidget::hide);
    connect(ui->ok, &QPushButton::clicked, this, &HotKeyWidget::updateHotKey);
}

HotKeyWidget::~HotKeyWidget() {
    delete ui;
}

void HotKeyWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    ui->radioButton->setChecked(true);
    switchHotKey();
}

void HotKeyWidget::switchHotKey() {
    if (ui->radioButton->isChecked()) {
        m_hotkey = *m_capture;
    } else {
        m_hotkey = *m_record;
    }
    ui->control->setChecked(m_hotkey.modifiers & Qt::ControlModifier);
    ui->alt->setChecked(m_hotkey.modifiers & Qt::AltModifier);
    ui->shift->setChecked(m_hotkey.modifiers & Qt::ShiftModifier);
    ui->comboBox->setCurrentIndex(m_hotkey.key - 'A');
    setWindowTitle(QString("修改%1快捷键").arg(ui->radioButton->isChecked() ? "截图" : "GIF"));
}

void HotKeyWidget::updateHotKey() {
    m_hotkey.modifiers = Qt::NoModifier;
    if (ui->control->isChecked()) {
        m_hotkey.modifiers |= Qt::ControlModifier;
    }
    if (ui->alt->isChecked()) {
        m_hotkey.modifiers |= Qt::AltModifier;
    }
    if (ui->shift->isChecked()) {
        m_hotkey.modifiers |= Qt::ShiftModifier;
    }
    m_hotkey.key = 'A' + ui->comboBox->currentIndex();
    if (m_hotkey.key < 'A' || m_hotkey.key > 'Z') {
        m_hotkey.key = 'A';
        ui->comboBox->setCurrentIndex(0);
    }

    if (m_hotkey.modifiers == Qt::NoModifier) {
        if (QMessageBox::question(this, "是否删除快捷键", QString("是否删除%1快捷键").arg(ui->radioButton->isChecked() ? "截图" : "GIF")) != QMessageBox::Yes) {
            return;
        }
    }
    if (ui->radioButton->isChecked()) {
        *m_capture = m_hotkey;
        emit capture();
    } else {
        *m_record = m_hotkey;
        emit record();
    }
    hide();
}