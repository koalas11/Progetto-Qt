#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QtCharts>
#include <iostream>
#include "set.hpp"

using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct painting {
    QString school, author, subjectTitle, date, room;
public:
    painting(): school(""), author(""), subjectTitle(""), date(""), room("") {}

    painting(const QString &school, const QString &author, const QString &subjectTitle, const QString &date, const QString &room):
        school(school), author(author), subjectTitle(subjectTitle), date(date), room(room) {}

    bool operator==(const painting &other) const {
        return school == other.school &&
                author == other.author &&
                subjectTitle == other.subjectTitle &&
                date == other.date &&
                room == other.room;
    }
};

struct painting_equal {
    bool operator()(const painting &lhs, const painting &rhs) const {
        return lhs == rhs;
    }
};

struct painting_filter {
    QString filter;

    painting_filter(const QString &filter) : filter(filter) {}

    bool operator()(const painting &painting) const {
        return painting.subjectTitle.contains(filter, Qt::CaseInsensitive);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_chartButton_clicked();
    void on_paintingsList_itemSelectionChanged();
    void on_cancelSelectionButton_clicked();
    void on_searchButton_clicked();
    void schoolChartHovered(QPieSlice *slice, bool state);
    void dateChartHovered(QPieSlice *slice, bool state);


private:
    Ui::MainWindow *ui;
    bool enabled = false;
    bool filtered = false;
    set<painting, painting_equal> paintingSet;
    painting_filter filter;

    void updateUI(void);
    void addPainting(const painting &painting);
    void addToSchoolChart(const QString &school);
    void removeToSchoolChart(const QString &school);
    void addToDateChart(const QString &date);
    void removeToDateChart(const QString &date);
    void orderSchoolChartLegend(const QString &school, QPieSlice *slice, QPieSeries *series);
    void orderDateChartLegend(int date, QPieSlice *slice, QPieSeries *series);
    void loadPaintings(void);
    QString formatDate(const QString &date);
    QStringList parseCSV(const QString& string);
};
#endif // MAINWINDOW_H
