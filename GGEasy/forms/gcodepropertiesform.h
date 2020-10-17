/*******************************************************************************
*                                                                              *
* Author    :  Bakiev Damir                                                    *
* Version   :  na                                                              *
* Date      :  01 February 2020                                                *
* Website   :  na                                                              *
* Copyright :  Bakiev Damir 2016-2020                                          *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
*******************************************************************************/
#pragma once
#include <QWidget>

namespace Ui {
class GCodePropertiesForm;
}

class GCodePropertiesForm : public QWidget {
    Q_OBJECT

public:
    explicit GCodePropertiesForm(QWidget* parent);
    ~GCodePropertiesForm() override;

    void updatePosDsbxs();
    void updateAll();

    inline static double safeZ;
    inline static double boardThickness;
    inline static double copperThickness;
    inline static double clearence;
    inline static double plunge;
    inline static double glue;

private slots:
    void on_pbResetHome_clicked();
    void on_pbResetZero_clicked();

private:
    Ui::GCodePropertiesForm* ui;
};
