#include "pocketform.h"
#include "filetree/filemodel.h"
#include "gcodepropertiesform.h"
#include "tooldatabase/tooldatabase.h"
#include "ui_pocketform.h"
#include <QDockWidget>
#include <QMessageBox>
#include <QSettings>
#include <gccreator.h>
#include <gcfile.h>
#include <gcpocket.h>
#include <myclipper.h>
#include <scene.h>
#include <tooldatabase/tooleditdialog.h>

const int QPairID = qRegisterMetaType<QPair<Tool, Tool>>("QPair<Tool,Tool>");

enum {
    Offset,
    Raster,
};

PocketForm::PocketForm(QWidget* parent)
    : FormsUtil("PocketForm", new GCode::PocketCreator, parent)
    , ui(new Ui::PocketForm)
{
    ui->setupUi(this);
    ui->toolName->setTool(tool);
    ui->toolName_2->setTool(tool2);

    connect(ui->rbClimb, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->rbConventional, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->rbInside, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->rbOffset, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->rbOutside, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->rbRaster, &QRadioButton::clicked, this, &PocketForm::rb_clicked);
    connect(ui->chbxUseTwoTools, &QCheckBox::clicked, this, &PocketForm::rb_clicked);

    QSettings settings;
    settings.beginGroup("PocketForm");
    ui->cbxPass->setCurrentIndex(settings.value("cbxPass").toInt());
    ui->chbxUseTwoTools->setChecked(settings.value("chbxUseTwoTools").toBool());
    ui->dsbxAngle->setValue(settings.value("dsbxAngle").toDouble());
    ui->rbClimb->setChecked(settings.value("rbClimb").toBool());
    ui->rbConventional->setChecked(settings.value("rbConventional").toBool());
    ui->rbInside->setChecked(settings.value("rbInside").toBool());
    ui->rbOffset->setChecked(settings.value("rbOffset").toBool());
    ui->rbOutside->setChecked(settings.value("rbOutside").toBool());
    ui->rbRaster->setChecked(settings.value("rbRaster").toBool());
    settings.endGroup();

    ui->pbEdit->setIcon(QIcon::fromTheme("document-edit"));
    ui->pbSelect->setIcon(QIcon::fromTheme("view-form"));
    ui->pbEdit_2->setIcon(QIcon::fromTheme("document-edit"));
    ui->pbSelect_2->setIcon(QIcon::fromTheme("view-form"));
    ui->pbClose->setIcon(QIcon::fromTheme("window-close"));
    ui->pbCreate->setIcon(QIcon::fromTheme("document-export"));

    for (QPushButton* button : findChildren<QPushButton*>())
        button->setIconSize({ 16, 16 });

    ui->sbxSteps->setSuffix(tr(" - Infinity"));

    updateArea();
    rb_clicked();

    parent->setWindowTitle(ui->label->text());
}

PocketForm::~PocketForm()
{
    QSettings settings;
    settings.beginGroup("PocketForm");
    settings.setValue("cbxPass", ui->cbxPass->currentIndex());
    settings.setValue("chbxUseTwoTools", ui->chbxUseTwoTools->isChecked());
    settings.setValue("dsbxAngle", ui->dsbxAngle->value());
    settings.setValue("rbClimb", ui->rbClimb->isChecked());
    settings.setValue("rbConventional", ui->rbConventional->isChecked());
    settings.setValue("rbInside", ui->rbInside->isChecked());
    settings.setValue("rbOffset", ui->rbOffset->isChecked());
    settings.setValue("rbOutside", ui->rbOutside->isChecked());
    settings.setValue("rbRaster", ui->rbRaster->isChecked());
    settings.endGroup();
    delete ui;
}

void PocketForm::on_pbSelect_clicked()
{
    ToolDatabase tdb(this, { Tool::EndMill, Tool::Engraving, Tool::Laser });
    if (tdb.exec()) {
        //        tool = tdb.tool();
        //        ui->toolName->setText(tool.name);
        //        updateName();
        Tool mpTool(tdb.tool());
        if (ui->chbxUseTwoTools->isChecked() && tool2.id() > -1 && tool2.diameter() <= mpTool.diameter()) {
            QMessageBox::warning(this, tr("Warning"), tr("The diameter of the second tool must be greater than the first!"));
        } else {
            tool = mpTool;
            ui->toolName->setTool(tool);
            updateArea();
        }
        rb_clicked();
    }
}
void PocketForm::on_pbSelect_2_clicked()
{
    ToolDatabase tdb(this, { Tool::EndMill, Tool::Engraving });
    if (tdb.exec()) {
        Tool mpTool(tdb.tool());
        if (tool.id() > -1 && tool.diameter() >= mpTool.diameter()) {
            QMessageBox::warning(this, tr("Warning"), tr("The diameter of the second tool must be greater than the first!"));
        } else {
            tool2 = mpTool;
            ui->toolName_2->setTool(tool2);
        }
    }
}

void PocketForm::on_pbEdit_clicked()
{
    ToolEditDialog d;
    d.setTool(tool);
    if (d.exec()) {
        if (ui->chbxUseTwoTools->isChecked() && tool2.id() > -1 && tool2.diameter() <= d.tool().diameter()) {
            QMessageBox::warning(this, tr("Warning"), tr("The diameter of the second tool must be greater than the first!"));
            return;
        }
        updateArea();
        tool = d.tool();
        tool.setId(-1);
        ui->toolName->setTool(tool);
        updateName();
    }
}

void PocketForm::on_pbEdit_2_clicked()
{
    ToolEditDialog d;
    d.setTool(tool2);
    if (d.exec()) {
        if (tool.id() > -1 && tool.diameter() >= d.tool().diameter()) {
            QMessageBox::warning(this, tr("Warning"), tr("The diameter of the second tool must be greater than the first!"));
            return;
        }
        tool2 = d.tool();
        tool.setId(-1);
        ui->toolName_2->setTool(tool2);
        updateName();
    }
}

void PocketForm::on_pbCreate_clicked()
{
    createFile();
}

void PocketForm::on_pbClose_clicked()
{
    if (parent())
        if (auto* w = dynamic_cast<QWidget*>(parent()); w)
            w->close();
}

void PocketForm::createFile()
{
    if (!tool.isValid()) {
        tool.errorMessageBox(this);
        return;
    }
    if (ui->chbxUseTwoTools->isChecked() && !tool2.isValid()) {
        tool2.errorMessageBox(this);
        return;
    }

    Paths wPaths;
    Paths wRawPaths;
    AbstractFile const* file = nullptr;

    for (auto* item : Scene::selectedItems()) {
        GraphicsItem* gi = dynamic_cast<GraphicsItem*>(item);
        switch (item->type()) {
        case GiGerber:
        case GiAperturePath:
            if (!file) {
                file = gi->file();
                boardSide = file->side();
            } else if (file != gi->file()) {
                QMessageBox::warning(this, tr("Warning"), tr("Working items from different files!"));
                return;
            }
            if (item->type() == GiGerber)
                wPaths.append(gi->paths());
            else
                wRawPaths.append(gi->paths());
            m_usedItems[gi->file()->id()].append(gi->id());
            break;
        case GiShapeC:
            wRawPaths.append(gi->paths());
            //m_used[gi->file()->id()].append(gi->id());
            break;
        case GiDrill:
            wPaths.append(gi->paths());
            m_usedItems[gi->file()->id()].append(gi->id());
            break;
        default:
            break;
        }

        //        if (item->type() == GerberItemType) {
        //            GerberItem* gi = static_cast<GerberItem*>(item);
        //            if (!file)
        //                file = gi->typedFile<Gerber::File>();
        //            if (file != gi->file()) {
        //                QMessageBox::warning(this, tr("Warning"), tr("Working items from different files!"));
        //                return;
        //            }
        //            boardSide = gi->file()->side();
        //        }
        //        if (item->type() == GerberItemType || item->type() == DrillItemType)
        //            wPaths.append(static_cast<GraphicsItem*>(item)->paths());
    }

    if (wRawPaths.isEmpty() && wPaths.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("No selected items for working..."));
        return;
    }

    GCode::GCodeParams gcp;
    gcp.convent = ui->rbConventional->isChecked();
    gcp.side = side;
    gcp.tool.append(tool);
    gcp.tool.append(tool2);

    gcp.dParam[GCode::UseAngle] = ui->dsbxAngle->value();
    gcp.dParam[GCode::Depth] = ui->dsbxDepth->value();
    gcp.dParam[GCode::Pass] = ui->cbxPass->currentIndex();
    gcp.dParam[GCode::UseRaster] = ui->rbRaster->isChecked();
    gcp.dParam[GCode::Steps] = ui->sbxSteps->value();
    gcp.dParam[GCode::TwoTools] = ui->chbxUseTwoTools->isChecked();
    gcp.dParam[GCode::MinArea] = ui->dsbxMinArea->value();

    m_tpc->setGcp(gcp);
    m_tpc->addPaths(wPaths);
    m_tpc->addRawPaths(wRawPaths);
    if (ui->chbxUseTwoTools->isChecked())
        fileCount = 2;
    createToolpath();
}

void PocketForm::on_sbxSteps_valueChanged(int arg1)
{
    ui->sbxSteps->setSuffix(!arg1 ? tr(" - Infinity") : "");
}

void PocketForm::updateName()
{
    static const QStringList name = { tr("Pocket On"), tr("Pocket Outside"), tr("Pocket Inside"), tr("Raster On"), tr("Raster Outside"), tr("Raster Inside") };
    ui->leName->setText(name[side + type * 3]);
}

void PocketForm::updatePixmap()
{
    static const QStringList pixmapList = {
        QStringLiteral(":/toolpath/pock_offs_climb.svg"),
        QStringLiteral(":/toolpath/pock_rast_climb.svg"),
        QStringLiteral(":/toolpath/pock_offs_conv.svg"),
        QStringLiteral(":/toolpath/pock_rast_conv.svg"),
    };
    int size = qMin(ui->lblPixmap->height(), ui->lblPixmap->width());
    ui->lblPixmap->setPixmap(QIcon(pixmapList[type + direction * 2]).pixmap(QSize(size, size)));
}

void PocketForm::updateArea()
{
    //    if (qFuzzyIsNull(ui->dsbxMinArea->value()))
    ui->dsbxMinArea->setValue((tool.getDiameter(ui->dsbxDepth->value() * 0.5)) * (tool.getDiameter(ui->dsbxDepth->value() * 0.5)) * M_PI * 0.5);
}

void PocketForm::rb_clicked()
{
    const bool rasterIsChecked = ui->rbRaster->isChecked();

    if (ui->rbOutside->isChecked())
        side = GCode::Outer;
    else if (ui->rbInside->isChecked())
        side = GCode::Inner;

    if (ui->rbOffset->isChecked()) {
        type = Offset;
    } else if (rasterIsChecked) {
        type = Raster;
        ui->chbxUseTwoTools->setChecked(false);
    }

    if (tool.type() == Tool::Laser)
        ui->chbxUseTwoTools->setChecked(false);

    if (ui->rbClimb->isChecked())
        direction = GCode::Climb;
    else if (ui->rbConventional->isChecked())
        direction = GCode::Conventional;

    {
        ui->cbxPass->setVisible(rasterIsChecked);
        ui->labelPass->setVisible(rasterIsChecked);
        ui->dsbxAngle->setVisible(rasterIsChecked);
        ui->labelAngle->setVisible(rasterIsChecked);
    }
    {
        ui->chbxUseTwoTools->setVisible(!rasterIsChecked && tool.type() != Tool::Laser);
        ui->sbxSteps->setVisible(!rasterIsChecked && tool.type() != Tool::Laser);
        ui->labelSteps->setVisible(!rasterIsChecked && tool.type() != Tool::Laser);
    }
    {
        const bool checked = ui->chbxUseTwoTools->isChecked();
        ui->chbxUseTwoTools->setChecked(checked);

        ui->labelSteps->setVisible(!checked && type != Raster);
        ui->sbxSteps->setVisible(!checked && type != Raster);

        ui->dsbxMinArea->setVisible(checked);
        ui->labelMinArea->setVisible(checked);

        ui->labelToolName2->setVisible(checked);
        ui->toolName_2->setVisible(checked);

        ui->pbEdit_2->setVisible(checked);
        ui->pbSelect_2->setVisible(checked);
    }

    updateName();
    updatePixmap();
}

void PocketForm::resizeEvent(QResizeEvent* event)
{
    updatePixmap();
    QWidget::resizeEvent(event);
}

void PocketForm::showEvent(QShowEvent* event)
{
    updatePixmap();
    QWidget::showEvent(event);
}

void PocketForm::on_leName_textChanged(const QString& arg1) { m_fileName = arg1; }

void PocketForm::editFile(GCode::File* /*file*/)
{
}
