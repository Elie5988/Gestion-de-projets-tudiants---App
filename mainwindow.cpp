#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QSound>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1038, 649);
    ui->scrollArea->close();
    ui->scrollArea_2->close();

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::onProjectSelected);
    if (!connectToDatabase()) {
           QMessageBox::critical(this, "Erreur de connexion", "Impossible de se connecter à la base de données.");
           return;
       }

    loadProjects();






}
bool MainWindow::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("nos_projet");
    db.setUserName("root");
    db.setPassword("");
    db.setPort(3307);
    return db.open();
}
void MainWindow::loadProjects() {
    QSqlQuery query("SELECT p.id, p.title, p.description, s.name, p.status FROM projet p JOIN students s ON p.student_id = s.id");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        Project project;
        project.id = query.value(0).toInt();
        project.title = query.value(1).toString();
        project.description = query.value(2).toString();
        project.studentName = query.value(3).toString();
        project.status = query.value(4).toString();
        projects.append(project);
        ui->listWidget->addItem(project.title);
    }
}
void MainWindow::onProjectSelected() {
    int currentRow = ui->listWidget->currentRow();
    if (currentRow >= 0 && currentRow < projects.size()) {
        const Project &project = projects[currentRow];
        ui->descriprionLabel->setText("\t" + project.description);
        ui->etudiantLabel->setText("\t " + project.studentName);
        ui->statusLabel->setText("\t" + project.status);
        ui->label_10->setText(project.title);
    }
}

void MainWindow::onSearchChanged(const QString &text) {
    ui->listWidget->clear();
    for (const Project &project : projects) {
        if (project.title.contains(text, Qt::CaseInsensitive) || project.description.contains(text, Qt::CaseInsensitive)) {
            ui->listWidget->addItem(project.title);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_pushButton_clicked()
{
    ui->scrollArea->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->scrollArea->close();

}

void MainWindow::on_pushButton_4_clicked()
{
    int currentRow = ui->listWidget->currentRow();
    if (currentRow >= 0 && currentRow < projects.size())
    {
        const Project &project = projects[currentRow];

        // Récupérer les valeurs des champs de feedback et grade
        QString feedback = ui->lineEdit_2->text(); // Champ de texte pour le feedback
        QString gradeStr = ui->lineEdit_3->text(); // Champ de texte pour la note
        bool isNumeric;
        int grade = gradeStr.toInt(&isNumeric); // Utiliser int à la place de double

        // Vérifier si les champs ne sont pas vides et si la note est un nombre
        if (feedback.isEmpty() || gradeStr.isEmpty() || !isNumeric)
        {
            QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs correctement.");
            return;
        }

        // Insertion ou mise à jour de l'évaluation dans la table evaluation
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM evaluation WHERE project_id = :project_id");
        query.bindValue(":project_id", project.id);
        query.exec();

        if (query.next() && query.value(0).toInt() > 0)
        {
            // Mettre à jour l'évaluation existante
            query.prepare("UPDATE evaluation SET grade = :grade, feedback = :feedback WHERE project_id = :project_id");
        }
        else
        {
            // Insérer une nouvelle évaluation
            query.prepare("INSERT INTO evaluation (project_id, grade, feedback) VALUES (:project_id, :grade, :feedback)");
        }

        // Lier les valeurs des variables aux paramètres SQL
        query.bindValue(":project_id", project.id); // ID du projet sélectionné
        query.bindValue(":grade", grade);           // Note (grade)
        query.bindValue(":feedback", feedback);     // Commentaire (feedback)

        // Exécuter la requête SQL et gérer les erreurs
        if (!query.exec())
        {
            qDebug() << "Erreur lors de l'insertion ou de la mise à jour de l'évaluation:" << query.lastError().text();
            QMessageBox::critical(this, "Erreur", "Impossible de soumettre l'évaluation.");
        }
        else
        {
            QMessageBox::information(this, "Succès", "Évaluation soumise avec succès !");
            ui->lineEdit_2->clear(); // Réinitialiser les champs après soumission
            ui->lineEdit_3->clear();
            ui->scrollArea->setVisible(false); // Fermer le scrollArea
            //ui->scrollArea_2->setVisible(true); // Afficher le scrollArea de l'évaluation soumise
            on_pushButton_2_clicked();
        }
    }
    else
    {
        QMessageBox::warning(this, "Avertissement", "Veuillez sélectionner un projet avant de soumettre une évaluation.");
    }
}




void MainWindow::on_pushButton_2_clicked() {
    ui->scrollArea_2->show();
    int currentRow = ui->listWidget->currentRow();

    if (currentRow < 0 || currentRow >= projects.size()) {
        return;
    }

    const Project &project = projects[currentRow];
    QSqlQuery query;
    query.prepare("SELECT grade, feedback FROM evaluation WHERE project_id = :id");
    query.bindValue(":id", project.id);

    if (query.exec()) {
        if (query.next()) {
            QString grade = query.value(0).toString();
            QString feedback = query.value(1).toString();
            ui->label_8->setText(feedback);
            ui->label_9->setText(grade);
        } else {
            ui->label_8->setText("Aucun feedback.");
            ui->label_9->setText("Non noté.");
        }
    } else {
        ui->label_8->setText("Erreur lors de la récupération des données.");
        ui->label_9->setText("");
    }

    ui->scrollArea_2->show();
}


void MainWindow::on_pushButton_5_clicked()
{
    ui->scrollArea_2->close();
}

