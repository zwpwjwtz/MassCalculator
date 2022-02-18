#ifndef FRAMESETTINGS_H
#define FRAMESETTINGS_H

#include <QFrame>


namespace Ui {
class FrameSettings;
}

class FrameSettings : public QFrame
{
    Q_OBJECT

public:
    explicit FrameSettings(QWidget *parent = nullptr);
    ~FrameSettings();

    void loadSettings();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FrameSettings *ui;

private slots:
    void on_comboLanguage_currentIndexChanged(int index);
    void on_spinMassPrecisionAveraged_valueChanged(int arg1);
    void on_spinMassPrecisionMonoisotopic_valueChanged(int arg1);
    void on_spinMassPrecisionIsotopicPattern_valueChanged(int arg1);
    void on_spinBinningWidth_valueChanged(double arg1);
    void on_spinMaxIsotopicCount_valueChanged(int arg1);
};

#endif // FRAMESETTINGS_H
