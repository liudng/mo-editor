// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FindReplaceDialog.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace hello::ui {

FindReplaceDialog::FindReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Find and Replace"));
    setModal(false);

    searchEdit_ = new QLineEdit(this);
    replaceEdit_ = new QLineEdit(this);

    caseSensitiveCheck_ = new QCheckBox(tr("Case sensitive"), this);
    wholeWordsCheck_ = new QCheckBox(tr("Whole words"), this);
    regexCheck_ = new QCheckBox(tr("Regular expression"), this);

    auto *findBtn = new QPushButton(tr("Find"), this);
    auto *replaceBtn = new QPushButton(tr("Replace"), this);
    auto *replaceAllBtn = new QPushButton(tr("Replace All"), this);

    auto *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(new QLabel(tr("Find:")));
    searchLayout->addWidget(searchEdit_);

    auto *replaceLayout = new QHBoxLayout;
    replaceLayout->addWidget(new QLabel(tr("Replace:")));
    replaceLayout->addWidget(replaceEdit_);

    auto *optionsGroup = new QGroupBox(tr("Options"), this);
    auto *optionsLayout = new QVBoxLayout(optionsGroup);
    optionsLayout->addWidget(caseSensitiveCheck_);
    optionsLayout->addWidget(wholeWordsCheck_);
    optionsLayout->addWidget(regexCheck_);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(findBtn);
    btnLayout->addWidget(replaceBtn);
    btnLayout->addWidget(replaceAllBtn);
    btnLayout->addStretch();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(replaceLayout);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addLayout(btnLayout);

    connect(findBtn, &QPushButton::clicked, this, &FindReplaceDialog::find);
    connect(replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
}

QString FindReplaceDialog::searchText() const { return searchEdit_->text(); }
QString FindReplaceDialog::replaceText() const { return replaceEdit_->text(); }
bool FindReplaceDialog::caseSensitive() const { return caseSensitiveCheck_->isChecked(); }
bool FindReplaceDialog::wholeWords() const { return wholeWordsCheck_->isChecked(); }
bool FindReplaceDialog::useRegex() const { return regexCheck_->isChecked(); }

void FindReplaceDialog::find()
{
    emit findRequested(searchText(), caseSensitive(), wholeWords(), useRegex());
}

void FindReplaceDialog::replace()
{
    emit replaceRequested(searchText(), replaceText(),
                          caseSensitive(), wholeWords(), useRegex());
}

void FindReplaceDialog::replaceAll()
{
    emit replaceAllRequested(searchText(), replaceText(),
                             caseSensitive(), wholeWords(), useRegex());
}

} // namespace hello::ui
