#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject* object, QEvent* event);
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;

    void loadPreferences();
    void savePreferences();
};

#endif // MAINWINDOW_H
