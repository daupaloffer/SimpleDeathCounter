#include "main_widget.h"
#include "ui_main_widget.h"
#include <QHotkey>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

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

      cout << pastDeaths << endl;
      deathsFile.close();
   }
   else {
      cout << "Deaths file does not exist, creating fresh.." << endl;
      ofstream deathsFile(deathsFileName);
      deathsFile << "0";
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

      cout << shortcutCombo.toString().toStdString() << endl;
      configFile.close();
   }
   else {
      this->deathShortcut->setShortcut(QKeySequence("Ctrl+Shift+Up"), true);

      cout << "Config file does not exist, creating fresh.." << endl;
      ofstream configFile(configFileName);
      configFile << "Ctrl+Shift+Up";
      configFile.close();
   }

   //persistentDeathScribe.open(deathsFileName, std::ios::trunc);
   //persistentDeathScribe << currentDeaths;
   //persistentDeathScribe.flush();
   cout << "Main widget started" << endl;
}

MainWidget::~MainWidget() {
   cout << "Closing widget, saving data.." << endl;
   //ofstream deathsFile("deaths.txt");
   ofstream configFile("config.txt");

   //deathsFile << to_string(currentDeaths);
   configFile << deathShortcut->shortcut().toString().toStdString();

   //deathsFile.close();
   //persistentDeathScribe.close();
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
   //persistentDeathScribe.write(to_string(currentDeaths).c_str(), to_string(currentDeaths).size());
   //persistentDeathScribe << currentDeaths;
   //persistentDeathScribe.flush();

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
   //persistentDeathScribe << "";
   //persistentDeathScribe << currentDeaths;
   //persistentDeathScribe.flush();

   cout << "Incremented deaths to: " + to_string(currentDeaths) << endl;
}
