#include "main_widget.h"
#include "ui_main_widget.h"
#include <QHotkey>
#include <iostream>
#include <fstream>
#include <string>

#if __has_include(<filesystem>)
   #include <filesystem>
   namespace filesystem = std::filesystem;
#elif __has_include(<experimental/filesystem>)
   #include <experimental/filesystem>
   namespace filesystem = std::experimental::filesystem;
#endif

using namespace std;

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget), deathShortcut(new QHotkey(this)) {
   ui->setupUi(this);
   setWindowTitle("SimpleDeathCounter");

   connect(this->ui->keySequenceEdit, &QKeySequenceEdit::editingFinished, this, &MainWidget::onSetShortcut);
   connect(this->ui->setDeathsButton, &QPushButton::clicked, this, &MainWidget::onSetDeaths);
   connect(this->deathShortcut, &QHotkey::activated, this, &MainWidget::incrementDeaths);

   //// Data and config file management
   string deathsFileName = "deaths.txt";
   string configFileName = "config.txt";
   bool deathsFileExists = filesystem::exists(deathsFileName);
   bool configFileExists = filesystem::exists(configFileName);

   // Deaths file
   if (deathsFileExists) {
      ifstream deathsFile(deathsFileName);
      string pastDeaths;

      // This gets the first line of the file seemingly, and I'm not sure how to get other lines - for now this is okay, but I
      // will need to create a way to loop through lines in the future if I add more config values
      getline(deathsFile, pastDeaths);

      // Convert past deaths string to int
      currentDeaths = stoi(pastDeaths);
      this->ui->currentDeathsLabel->setNum(currentDeaths);

      deathsFile.close();
   }
   else {
      currentDeaths = 0;

      cout << "Deaths file does not exist, creating fresh.." << endl;
      ofstream deathsFile(deathsFileName);
      deathsFile << 0;
      deathsFile.close();
   }

   // Config file
   if (configFileExists) {
      ifstream configFile(configFileName);
      string storedShortcutCombo;

      getline(configFile, storedShortcutCombo);
      QKeySequence shortcutCombo = QString::fromStdString(storedShortcutCombo);

      this->deathShortcut->setShortcut(shortcutCombo, true);
      this->ui->currentShortcutLabel->setText(shortcutCombo.toString());

      configFile.close();
   }
   else {
      this->deathShortcut->setShortcut(QKeySequence("Ctrl+Shift+Up"), true);

      cout << "Config file does not exist, creating fresh.." << endl;
      ofstream configFile(configFileName);
      configFile << "Ctrl+Shift+Up";
      configFile.close();
   }

   cout << "Main widget started" << endl;
}

MainWidget::~MainWidget() {
   cout << "Closing widget, saving data.." << endl;

   ofstream configFile("config.txt");
   configFile << deathShortcut->shortcut().toString().toStdString();
   configFile.close();

   cout << "Data saved" << endl;

   delete this->deathShortcut;
   delete ui;
   cout << "Main widget closed" << endl;
}

void MainWidget::onSetDeaths() {
   currentDeaths = this->ui->deathsSpinBox->value();
   this->ui->currentDeathsLabel->setNum(currentDeaths);

   ofstream deathsFile("deaths.txt");
   deathsFile << currentDeaths;
   deathsFile.close();

   cout << "Set deaths to: " + to_string(currentDeaths) << endl;
}

void MainWidget::onSetShortcut() {
   QKeySequence newSequence = this->ui->keySequenceEdit->keySequence();
   this->deathShortcut->setShortcut(newSequence, true);
   this->ui->currentShortcutLabel->setText(newSequence.toString());
   this->ui->keySequenceEdit->clearFocus();

   cout << "Death shortcut set" << endl;
}

void MainWidget::incrementDeaths() {
   currentDeaths++;
   this->ui->currentDeathsLabel->setNum(currentDeaths);

   ofstream deathsFile("deaths.txt");
   deathsFile << currentDeaths;
   deathsFile.close();

   cout << "Incremented deaths to: " + to_string(currentDeaths) << endl;
}
