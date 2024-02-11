#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , paintingSet(set<painting, painting_equal>())
    , filter(painting_filter(""))
{
    ui->setupUi(this);

    QPieSeries *pieSeriesSchool = new QPieSeries();
    QPieSeries *pieSeriesDate = new QPieSeries();
    pieSeriesSchool->setName("Grafico Scuole");
    pieSeriesSchool->setPieSize(1.0f);
    pieSeriesDate->setName("Grafico Date");
    pieSeriesDate->setPieSize(1.0f);

    auto schoolChart = ui->schoolChart->chart();
    auto dateChart = ui->dateChart->chart();

    dateChart->legend()->setAlignment(Qt::AlignRight);
    dateChart->addSeries(pieSeriesDate);
    dateChart->createDefaultAxes();
    ui->dateChart->setRenderHint(QPainter::Antialiasing, true);

    schoolChart->legend()->setAlignment(Qt::AlignRight);
    schoolChart->addSeries(pieSeriesSchool);
    schoolChart->createDefaultAxes();
    ui->schoolChart->setRenderHint(QPainter::Antialiasing, true);

    ui->paintingsList->horizontalHeader()->setStretchLastSection(true);

    ui->chartHoverLabel->hide();
    connect(pieSeriesSchool, SIGNAL(hovered(QPieSlice*,bool)), this, SLOT(schoolChartHovered(QPieSlice*,bool)));
    connect(pieSeriesDate, SIGNAL(hovered(QPieSlice*,bool)), this, SLOT(dateChartHovered(QPieSlice*,bool)));

    loadPaintings();
    on_chartButton_clicked();
    updateUI();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::schoolChartHovered(QPieSlice *slice, bool state) {
    if (state) {
        ui->chartHoverLabel->setText(slice->label().split(":").at(0) + ": " + QString::number(slice->percentage() * 100) + "%");
        ui->chartHoverLabel->show();
    } else {
        ui->chartHoverLabel->hide();
        ui->chartHoverLabel->setText("");
    }
}

void MainWindow::dateChartHovered(QPieSlice *slice, bool state) {
    if (state) {
        ui->chartHoverLabel->setText(slice->label());
        ui->chartHoverLabel->show();
    } else {
        ui->chartHoverLabel->hide();
        ui->chartHoverLabel->setText("");
    }
}

void MainWindow::updateUI(void) {
    if (enabled) {
        ui->schoolLineEdit->setReadOnly(true);
        ui->authorLineEdit->setReadOnly(true);
        ui->subjectTitleLineEdit->setReadOnly(true);
        ui->DateLineEdit->setReadOnly(true);
        ui->RoomLineEdit->setReadOnly(true);
    } else {
        ui->paintingsList->setCurrentItem(nullptr);
        ui->paintingEditLabel->setText("Aggiungendo un Dipinto");
        ui->removeButton->setEnabled(false);
        ui->cancelSelectionButton->setEnabled(false);
        ui->paintingsList->setCurrentItem(nullptr);
        ui->schoolLineEdit->setReadOnly(false);
        ui->schoolLineEdit->setText("");
        ui->schoolLineEdit->setPlaceholderText("scuola");
        ui->authorLineEdit->setReadOnly(false);
        ui->authorLineEdit->setText("");
        ui->authorLineEdit->setPlaceholderText("autore");
        ui->subjectTitleLineEdit->setReadOnly(false);
        ui->subjectTitleLineEdit->setText("");
        ui->subjectTitleLineEdit->setPlaceholderText("soggetto/titolo");
        ui->DateLineEdit->setReadOnly(false);
        ui->DateLineEdit->setText("");
        ui->DateLineEdit->setPlaceholderText("data");
        ui->RoomLineEdit->setReadOnly(false);
        ui->RoomLineEdit->setText("");
        ui->RoomLineEdit->setPlaceholderText("sala");
    }
    enabled = !enabled;
}

void MainWindow::on_addButton_clicked(void) {
    if (enabled) {
        auto school = ui->schoolLineEdit->text().trimmed();
        auto author = ui->authorLineEdit->text().trimmed();
        auto subjectTitle = ui->subjectTitleLineEdit->text().trimmed();
        auto date = ui->DateLineEdit->text().trimmed();
        auto room = ui->RoomLineEdit->text().trimmed();

        if (school.isEmpty() || author.isEmpty() || subjectTitle.isEmpty() || date.isEmpty() || room.isEmpty())
            return;

        const painting painting(school, author, subjectTitle, date, room);
        if (paintingSet.add(painting)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Gestionale Dipinti");
            msgBox.setText("Il dipinto è stato aggiunto con successo.");
            msgBox.exec();
            if (!filtered || filter(painting))
               addPainting(painting);
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Gestionale Dipinti");
            msgBox.setText("Il dipinto è già presente.");
            msgBox.exec();
        }

        enabled = false;
    }
    updateUI();
}

void MainWindow::on_removeButton_clicked() {
    auto paintingList = ui->paintingsList;
    auto list = paintingList->selectedItems();

    if (list.size() != 5)
        return;

    const painting painting(list.at(0)->text(), list.at(1)->text(), list.at(2)->text(), list.at(3)->text(), list.at(4)->text());
    if (paintingSet.remove(painting)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Gestionale Dipinti");
        msgBox.setText("Il dipinto è stato rimosso con successo.");
        msgBox.exec();
    }

    paintingList->removeRow(paintingList->selectedItems().at(0)->row());

    removeToSchoolChart(painting.school);
    removeToDateChart(painting.date);

    updateUI();
}

void MainWindow::addPainting(const painting &painting) {
    auto row = ui->paintingsList->rowCount();
    ui->paintingsList->insertRow(row);

    ui->paintingsList->setItem(row, 0, new QTableWidgetItem(painting.school));
    ui->paintingsList->setItem(row, 1, new QTableWidgetItem(painting.author));
    ui->paintingsList->setItem(row, 2, new QTableWidgetItem(painting.subjectTitle));
    ui->paintingsList->setItem(row, 3, new QTableWidgetItem(painting.date));
    ui->paintingsList->setItem(row, 4, new QTableWidgetItem(painting.room));

    addToSchoolChart(painting.school);
    addToDateChart(painting.date);
}

void MainWindow::on_paintingsList_itemSelectionChanged() {
    if (enabled)
        updateUI();

    auto paintingList = ui->paintingsList;
    auto list = paintingList->selectedItems();

    if (list.size() != 5)
        return;

    ui->paintingEditLabel->setText("Dipinto N. " +  QString::number(paintingList->row(list.at(0)) + 1));
    ui->schoolLineEdit->setText(list.at(0)->text());
    ui->authorLineEdit->setText(list.at(1)->text());
    ui->subjectTitleLineEdit->setText(list.at(2)->text());
    ui->DateLineEdit->setText(list.at(3)->text());
    ui->RoomLineEdit->setText(list.at(4)->text());

    ui->removeButton->setEnabled(true);
    ui->cancelSelectionButton->setEnabled(true);
}


void MainWindow::on_cancelSelectionButton_clicked() {
    updateUI();
}

void MainWindow::on_searchButton_clicked() {
    auto searchString = ui->searchLineEdit->text().trimmed();
    if (!filtered && searchString.isEmpty())
        return;

    auto schoolSeries = static_cast<QPieSeries *>(ui->schoolChart->chart()->series().at(0));
    auto dateSeries = static_cast<QPieSeries *>(ui->dateChart->chart()->series().at(0));

    if (filtered && searchString.isEmpty()) {
        schoolSeries->clear();
        dateSeries->clear();
        ui->paintingsList->setRowCount(0);
        filtered = false;

        for (auto curr = paintingSet.begin(); curr != paintingSet.end(); ++curr) {
            addPainting(*curr);
        }
    } else {
        if (filtered && filter.filter == searchString)
            return;

        schoolSeries->clear();
        dateSeries->clear();
        ui->paintingsList->setRowCount(0);
        filter.filter = searchString;

        const set<painting, painting_equal> filteredpaintingSet = filter_out(paintingSet, filter);
        filtered = true;
        for (auto curr = filteredpaintingSet.begin(); curr != filteredpaintingSet.end(); ++curr) {
            addPainting(*curr);
        }
    }
    if (!enabled)
        updateUI();
}

void MainWindow::on_chartButton_clicked() {
    if (ui->schoolChart->isVisible()) {
        ui->schoolChart->hide();
        ui->dateChart->show();
        ui->chartLabel->setText("Grafico Date Numero Dipinti");
    } else {
        ui->schoolChart->show();
        ui->dateChart->hide();
        ui->chartLabel->setText("Grafico Scuole Percentuale Dipinti");
    }
}

void MainWindow::addToSchoolChart(const QString &school) {
    auto series = static_cast<QPieSeries *>(ui->schoolChart->chart()->series()[0]);
    auto slices = series->slices();
    auto sum = ui->paintingsList->rowCount();
    bool check = true;
    QPieSlice *pieSlice;
    for (int i = 0; i < slices.size(); i++) {
        pieSlice = slices.at(i);
        if (school == pieSlice->label().split(":").at(0)) {
            pieSlice->setValue(pieSlice->value() + 1);
            check = false;
        }
        auto num = QString::number(pieSlice->value() / sum * 100.0, 'f', 2);
        pieSlice->setLabel(pieSlice->label().left(pieSlice->label().indexOf(":")) + ": " + num + "%");
    }

    if (check) {
        pieSlice = new QPieSlice(school, 1);
        QRandomGenerator *random = QRandomGenerator::global();
        auto color = QColor(50,
                            (50 + series->count() * random->bounded(2, 10)) % 255,
                            random->bounded(100, 255),
                            random->bounded(100, 255));
        pieSlice->setColor(color);
        auto num = QString::number(pieSlice->value() / sum * 100.0, 'f', 2);
        pieSlice->setLabel(pieSlice->label().left(pieSlice->label().indexOf(":")) + ": " + num + "%");
        orderSchoolChartLegend(school.split(":").at(0), pieSlice, series);
    }
}

void MainWindow::removeToSchoolChart(const QString &school) {
    auto series = static_cast<QPieSeries *>(ui->schoolChart->chart()->series()[0]);
    auto slices = series->slices();
    auto sum = ui->paintingsList->rowCount();
    QPieSlice *pieSlice;
    QString string;
    for (int i = 0; i < slices.size(); i++) {
        pieSlice = slices.at(i);
        string = pieSlice->label();
        if (school == pieSlice->label().split(":").at(0)) {
            pieSlice->setValue(pieSlice->value() - 1);
            if (pieSlice->value() == 0) {
                series->remove(pieSlice);
                continue;
            }
        }
        auto num = QString::number(pieSlice->value() / sum * 100.0, 'f', 2);
        pieSlice->setLabel(pieSlice->label().left(pieSlice->label().indexOf(":")) + ": " + num + "%");
    }
}

void MainWindow::addToDateChart(const QString &date) {
    auto formattedDate = formatDate(date);
    QPieSeries * series = static_cast<QPieSeries *>(ui->dateChart->chart()->series()[0]);
    auto slices = series->slices();
    QPieSlice *pieSlice;
    QString string;
    for (int i = 0; i < slices.size(); i++) {
        pieSlice = slices.at(i);
        string = pieSlice->label();
        if (string.split(":").at(0) == formattedDate) {
            pieSlice->setValue(pieSlice->value() + 1);
            pieSlice->setLabel(formattedDate + ": " + QString::number(pieSlice->value()));
            return;
        }
    }

    pieSlice = new QPieSlice(formattedDate, 1);
    pieSlice->setLabel(formattedDate + ": " + QString::number(pieSlice->value()));
    QRandomGenerator *random = QRandomGenerator::global();
    auto color = QColor(50,
                        (50 + series->count() * random->bounded(2, 10)) % 255,
                        random->bounded(100, 255),
                        random->bounded(100, 255));
    pieSlice->setColor(color);
    orderDateChartLegend(formattedDate.split("-").at(0).toInt(), pieSlice, series);
}

void MainWindow::removeToDateChart(const QString &date) {
    auto formattedDate = formatDate(date);
    auto series = static_cast<QPieSeries *>(ui->dateChart->chart()->series()[0]);
    auto slices = series->slices();
    QPieSlice *pieSlice;
    QString string;
    for (int i = 0; i < slices.size(); i++) {
        pieSlice = slices.at(i);
        string = pieSlice->label();
        if (string.contains(formattedDate)) {
            pieSlice->setValue(pieSlice->value() - 1);
            if (pieSlice->value() == 0) {
                series->remove(pieSlice);
            } else
                pieSlice->setLabel(formattedDate + ": " + QString::number(pieSlice->value()));
            break;
        }
    }
}

QString MainWindow::formatDate(const QString &date) {
    QString result = "";
    bool check = false;
    for (int i = 0; i < date.size(); ++i) {
        if (date.at(i).isDigit()) {
            result += date.at(i);
            check = true;
        } else if (check)
            break;
    }

    if (result.size() == 0)
        return "Data Non Numerica";
    else if (result.size() < 4)
        return result;
    else {
        if (result.at(2).unicode() >= 53)
            return result.left(2) + "50-" + QString::number(result.leftRef(2).toInt() + 1) + "00";
        else
            return result.left(2) + "00-" + result.left(2) + "50";
    }
}

void MainWindow::loadPaintings(void) {
    QFile file("dipinti_uffizi.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        std::cerr << "Cannot open file for reading: "
        << qPrintable(file.errorString())
        << std::endl;
        return;
    }
    QTextStream in(&file);

    QString line;
    QStringList list;
    auto paintingList = ui->paintingsList;

    if (!in.atEnd()) {
        line = in.readLine();
        list = parseCSV(line);

        if (list.size() != 5) {
            std::cerr << "Error while reading cvs file, wrong format" << std::endl;
            return;
        }

        paintingList->setHorizontalHeaderLabels(list);
    }

    while(!in.atEnd()) {
        line = in.readLine();
        list = parseCSV(line);
        if (list.size() == 5) {
            const painting painting(list[0], list[1], list[2], list[3], list[4]);
            if (paintingSet.add(painting))
                addPainting(painting);
        } else {
            std::cerr << "Error while reading cvs file, wrong format, line not added" << std::endl;
        }
    }
}

void MainWindow::orderSchoolChartLegend(const QString &school, QPieSlice *slice, QPieSeries *series) {
    int i = 0;
    auto list = series->slices();
    for (; i < list.size(); ++i) {
        if (school.compare(list.at(i)->label().split(":").at(0), Qt::CaseInsensitive) < 0)
            break;
    }

    series->insert(i, slice);
}

void MainWindow::orderDateChartLegend(int date, QPieSlice *slice, QPieSeries *series) {
    int i = 0;
    auto list = series->slices();
    for (; i < list.size(); ++i) {
        if (date < list.at(i)->label().split("-").at(0).toInt())
            break;
    }

    series->insert(i, slice);
}

QStringList MainWindow::parseCSV(const QString &string) {
    bool state = true;
    QStringList fields;
    QString value;

    for (int i = 0; i < string.size(); i++) {
        const QChar current = string.at(i);

        if (state) {
            if (current == ',') {
                fields.append(value.trimmed());
                value.clear();
            } else if (current == '"') {
                state = false;
                value += current;
            } else
                value += current;
        } else {
            if (current == '"') {
                if (i < string.size()) {
                    if (i+1 < string.size() && string.at(i+1) == '"') {
                        value += '"';
                        i++;
                    } else {
                        state = true;
                        value += '"';
                    }
                }
            } else
                value += current;
        }
    }

    if (!value.isEmpty())
        fields.append(value.trimmed());

    for (int i=0; i < fields.size(); ++i)
        if (fields[i].length() >= 1 && fields[i].at(0) == '"') {
            fields[i] = fields[i].mid(1);
            if (fields[i].length() >= 1 && fields[i].right(1) == '"')
                fields[i] = fields[i].left(fields[i].length()-1);
        }

    return fields;
}
