// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include <colordialog.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qglobal.h>
#include <rgbcolorspacefactory.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <qcoreapplication.h>
#include <qnamespace.h>
#endif

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Prepare configuration before instantiating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
    auto myColorSpace = PerceptualColor::RgbColorSpaceFactory::createSrgb();
    PerceptualColor::ColorDialog m_colorDialog(myColorSpace);
    m_colorDialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
    m_colorDialog.show();
    return app.exec();
}
