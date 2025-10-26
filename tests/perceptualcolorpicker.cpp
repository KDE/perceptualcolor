// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include <colordialog.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <rgbcolorspacefactory.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto myColorSpace = PerceptualColor::RgbColorSpaceFactory::createSrgb();
    PerceptualColor::ColorDialog m_colorDialog(myColorSpace);
    m_colorDialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
    m_colorDialog.show();
    return app.exec();
}
