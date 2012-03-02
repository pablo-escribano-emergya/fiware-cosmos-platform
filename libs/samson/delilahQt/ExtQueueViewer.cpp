/* ****************************************************************************
*
* FILE                     ExtQueueViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson queues' data
*
*/

#include "ExtQueueViewer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

bool QueueData::operator==(const QueueData &other) const 
{
    if(name != other.name) return false;
    if(kvs != other.kvs) return false;
    if(size != other.size) return false;
    if(key != other.key) return false;
    if(value != other.value) return false;
    if(total_kvs != other.total_kvs) return false;
    if(total_size != other.total_size) return false;
    if(kvs_s != other.kvs_s) return false;
    if(bytes_s != other.bytes_s) return false;
    if(blocks != other.blocks) return false;
    if(rate_size != other.rate_size) return false;
    if(on_memory != other.on_memory) return false;
    if(on_disk != other.on_disk) return false;
    if(locked != other.locked) return false;
    if(time_from != other.time_from) return false;
    if(time_to != other.time_to) return false;
    return true;
}

ExtQueueViewer::ExtQueueViewer(std::string _title, QWidget* parent): QWidget(parent)
{
    //QVBoxLayout* layout;
    
    title = _title;
    data.name = _title;
    
    mainLayout = new QVBoxLayout(this);
    
    generalLayout = new QHBoxLayout();
    sizeLayout = new QHBoxLayout();
    formatLayout = new QHBoxLayout();
    rateLayout = new QHBoxLayout();
    blocksLayout = new QHBoxLayout();
    connectButtonLayout = new QHBoxLayout();

    sizeBox = new QGroupBox("Size", this);
    formatBox = new QGroupBox("Format", this);
    rateBox = new QGroupBox("Rate", this);
    blocksBox = new QGroupBox("Blocks", this);

    QFont bigFont;
    QFont boldFont;
    
    name = new QLabel("Empty", sizeBox);
    boldFont = name->font();
    boldFont.setBold(true);
    name->setFont(boldFont);
    kvsLabel = new QLabel("Kvs:", sizeBox);
    kvs = new QLabel("0", sizeBox);
    kvs->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    sizeLabel = new QLabel("Size:", sizeBox);
    size = new QLabel("0", sizeBox);
    size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    keyLabel = new QLabel("key:", formatBox);
    key = new QLabel("", formatBox);
    key->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    valueLabel = new QLabel("Value:", formatBox);
    value = new QLabel("", formatBox);
    value->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    total_kvsLabel = new QLabel("Total Kvs:", rateBox);
    total_kvs = new QLabel("0", rateBox);
    total_kvs->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    total_sizeLabel = new QLabel("Total Size:", rateBox);
    total_size = new QLabel("0", rateBox);
    total_size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    kvs_sLabel = new QLabel("Kvs/s:", rateBox);
    kvs_s = new QLabel("0", rateBox);
    kvs_s->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    bytes_sLabel = new QLabel("Bytes/s:", rateBox);
    bytes_s = new QLabel("0", rateBox);
    bytes_s->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    blocksLabel = new QLabel("Blocks:", blocksBox);
    blocks = new QLabel("0", blocksBox);
    blocks->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    rate_sizeLabel = new QLabel("Size:", blocksBox);
    rate_size = new QLabel("0", blocksBox);
    rate_size->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    on_memoryLabel = new QLabel("On Memory:", blocksBox);
    on_memory = new QLabel("0", blocksBox);
    on_memory->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    on_diskLabel = new QLabel("On Disk:", blocksBox);
    on_disk = new QLabel("0", blocksBox);
    on_disk->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    lockedLabel = new QLabel("Locked:", blocksBox);
    locked = new QLabel("0", blocksBox);
    locked->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    time_fromLabel = new QLabel("Time from", blocksBox);
    time_from = new QLabel("0", blocksBox);
    time_from->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    time_toLabel = new QLabel("to", blocksBox);
    time_to = new QLabel("0", blocksBox);
    time_to->setFrameStyle(QFrame::Panel | QFrame::Sunken);


    //Feed connection
    connectButton = new QPushButton("Connect to this queue", this);
    connectButton->setCheckable(true);
    queueFeed = new QTextEdit(this);
    queueFeed->setReadOnly(true);
    
    
    mainLayout->addWidget(name);
    mainLayout->addLayout(generalLayout);
    mainLayout->addWidget(rateBox);
    mainLayout->addWidget(sizeBox);
    mainLayout->addWidget(blocksBox);
    mainLayout->addLayout(connectButtonLayout);
    mainLayout->addWidget(queueFeed);
    mainLayout->addStretch();
    
    connectButtonLayout->addWidget(connectButton);
    connectButtonLayout->addStretch();

    generalLayout->addWidget(sizeBox);
    generalLayout->addWidget(formatBox);
    generalLayout->addStretch();

    sizeLayout->addWidget(kvsLabel, 0, Qt::AlignRight);
    sizeLayout->addWidget(kvs);
    sizeLayout->addWidget(sizeLabel, 0, Qt::AlignRight);
    sizeLayout->addWidget(size);
    sizeLayout->addStretch();
    sizeBox->setLayout(sizeLayout);
    
    formatLayout->addWidget(keyLabel, 0, Qt::AlignRight);
    formatLayout->addWidget(key);
    formatLayout->addWidget(valueLabel, 0, Qt::AlignRight);
    formatLayout->addWidget(value);
    formatLayout->addStretch();
    formatBox->setLayout(formatLayout);
    
    rateLayout->addWidget(total_kvsLabel, 0, Qt::AlignRight);
    rateLayout->addWidget(total_kvs);
    rateLayout->addWidget(total_sizeLabel, 0, Qt::AlignRight);
    rateLayout->addWidget(total_size);
    rateLayout->addWidget(kvs_sLabel, 0, Qt::AlignRight);
    rateLayout->addWidget(kvs_s);
    rateLayout->addWidget(bytes_sLabel, 0, Qt::AlignRight);
    rateLayout->addWidget(bytes_s);
    rateLayout->addStretch();
    rateBox->setLayout(rateLayout);

    blocksLayout->addWidget(blocksLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(blocks);
    blocksLayout->addWidget(rate_sizeLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(rate_size);
    blocksLayout->addWidget(on_memoryLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(on_memory);
    blocksLayout->addWidget(on_diskLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(on_disk);
    blocksLayout->addWidget(lockedLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(locked);
    blocksLayout->addWidget(time_fromLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(time_from);
    blocksLayout->addWidget(time_toLabel, 0, Qt::AlignRight);
    blocksLayout->addWidget(time_to);
    blocksLayout->addStretch();
    blocksBox->setLayout(blocksLayout);
    
}

void ExtQueueViewer::setData(QueueData* newData)
{
        data = *newData;
        name->setText(QString(data.name.c_str() + QString(": ")));
        kvs->setText(QString(data.kvs.c_str()));
        size->setText(QString(au::str(strtoul(data.size.c_str(), NULL, 0)).c_str()));
        key->setText(QString(data.key.c_str()));
        value->setText(QString(data.value.c_str()));
        total_kvs->setText(QString(au::str(strtoul(data.total_kvs.c_str(), NULL, 0)).c_str()));
        total_size->setText(QString(au::str(strtoul(data.total_size.c_str(), NULL, 0)).c_str()));
        kvs_s->setText(QString(au::str(strtoul(data.kvs_s.c_str(), NULL, 0)).c_str()));
        bytes_s->setText(QString(au::str(strtoul(data.bytes_s.c_str(), NULL, 0)).c_str()));
        blocks->setText(QString(au::str(strtoul(data.blocks.c_str(), NULL, 0)).c_str()));
        rate_size->setText(QString(au::str(strtoul(data.rate_size.c_str(), NULL, 0)).c_str()));
        on_memory->setText(QString(au::str(strtoul(data.on_memory.c_str(), NULL, 0)).c_str()));
        on_disk->setText(QString(au::str(strtoul(data.on_disk.c_str(), NULL, 0)).c_str()));
        locked->setText(QString(au::str(strtoul(data.locked.c_str(), NULL, 0)).c_str()));
        time_from->setText(QString(au::str_time(strtoul(data.time_from.c_str(), NULL, 0)).c_str()));
        time_to->setText(QString(au::str_time(strtoul(data.time_to.c_str(), NULL, 0)).c_str()));
}
void ExtQueueViewer::updateFeed(std::string line)
{
    queueFeed->append(QString(line.c_str()) + QString("\n"));
    queueFeed->ensureCursorVisible();    
}

} //namespace