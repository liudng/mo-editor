// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "FindReplaceDock.hpp"

#include "editor/CodeEditor.hpp"

#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

namespace mo::ui {

FindReplaceDock::FindReplaceDock(QWidget *parent)
    : QDockWidget(parent)
{
    setWindowTitle(tr("Find and Replace"));
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    auto *central = new QWidget(this);
    setWidget(central);

    searchEdit_ = new QLineEdit(central);
    searchEdit_->setPlaceholderText(tr("Search for..."));
    searchEdit_->setClearButtonEnabled(true);
    replaceEdit_ = new QLineEdit(central);
    replaceEdit_->setPlaceholderText(tr("Replace with..."));
    replaceEdit_->setClearButtonEnabled(true);

    caseSensitiveCheck_ = new QCheckBox(tr("Case sensitive"), central);
    wholeWordsCheck_ = new QCheckBox(tr("Whole words"), central);
    regexCheck_ = new QCheckBox(tr("Regex"), central);

    findNextBtn_ = new QPushButton(tr("Find Next"), central);
    findPrevBtn_ = new QPushButton(tr("Find Prev"), central);
    replaceBtn_ = new QPushButton(tr("Replace"), central);
    replaceAllBtn_ = new QPushButton(tr("Replace All"), central);

    statusLabel_ = new QLabel(central);
    statusLabel_->setStyleSheet(QStringLiteral("color: gray;"));

    // --- Layout ---
    // A grid keeps the input rows aligned: the labels share the left
    // column (right-aligned), the edits share a stretching column, and
    // the buttons line up in two columns across both rows.
    auto *findLabel = new QLabel(tr("Find:"), central);
    findLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    auto *replaceLabel = new QLabel(tr("Replace:"), central);
    replaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *grid = new QGridLayout(central);
    grid->setContentsMargins(6, 4, 6, 4);
    grid->setHorizontalSpacing(6);
    grid->setVerticalSpacing(6);

    grid->addWidget(findLabel, 0, 0);
    grid->addWidget(searchEdit_, 0, 1);
    grid->addWidget(findNextBtn_, 0, 2);
    grid->addWidget(findPrevBtn_, 0, 3);

    grid->addWidget(replaceLabel, 1, 0);
    grid->addWidget(replaceEdit_, 1, 1);
    grid->addWidget(replaceBtn_, 1, 2);
    grid->addWidget(replaceAllBtn_, 1, 3);

    grid->setColumnStretch(1, 1);

    auto *optionsRow = new QHBoxLayout;
    optionsRow->addWidget(caseSensitiveCheck_);
    optionsRow->addWidget(wholeWordsCheck_);
    optionsRow->addWidget(regexCheck_);
    optionsRow->addStretch();
    optionsRow->addWidget(statusLabel_);
    grid->addLayout(optionsRow, 2, 0, 1, 4);

    connect(findNextBtn_, &QPushButton::clicked, this, [this]() {
        if (auto *e = currentEditor()) findNext(e);
    });
    connect(findPrevBtn_, &QPushButton::clicked, this, [this]() {
        if (auto *e = currentEditor()) findPrev(e);
    });
    connect(replaceBtn_, &QPushButton::clicked, this, [this]() {
        if (auto *e = currentEditor()) replace(e);
    });
    connect(replaceAllBtn_, &QPushButton::clicked, this, [this]() {
        if (auto *e = currentEditor()) replaceAll(e);
    });
    connect(searchEdit_, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (text.isEmpty()) {
            updateStatus(QString());
        }
    });

    // Enter in search field triggers Find Next.
    connect(searchEdit_, &QLineEdit::returnPressed, this, [this]() {
        if (auto *e = currentEditor()) findNext(e);
    });
}

CodeEditor *FindReplaceDock::currentEditor() const
{
    // The dock's parent is the MainWindow; find its QTabWidget.
    auto *mw = parentWidget();
    if (!mw) {
        return nullptr;
    }
    auto *tw = mw->findChild<QTabWidget *>();
    if (!tw) {
        return nullptr;
    }
    return qobject_cast<CodeEditor *>(tw->currentWidget());
}

QString FindReplaceDock::searchText() const { return searchEdit_->text(); }
QString FindReplaceDock::replaceText() const { return replaceEdit_->text(); }

void FindReplaceDock::focusSearch()
{
    searchEdit_->setFocus();
    searchEdit_->selectAll();
}

void FindReplaceDock::updateStatus(const QString &message)
{
    statusLabel_->setText(message);
}

void FindReplaceDock::findNext(CodeEditor *editor)
{
    if (!editor || searchText().isEmpty()) {
        return;
    }
    editor->findNext(searchText());
    updateStatus(tr("Find Next"));
}

void FindReplaceDock::findPrev(CodeEditor *editor)
{
    if (!editor || searchText().isEmpty()) {
        return;
    }
    editor->findPrev(searchText());
    updateStatus(tr("Find Previous"));
}

void FindReplaceDock::replace(CodeEditor *editor)
{
    if (!editor || searchText().isEmpty()) {
        return;
    }
    auto cursor = editor->textCursor();
    if (cursor.hasSelection() && cursor.selectedText() == searchText()) {
        cursor.insertText(replaceText());
    }
    // Advance to next match.
    editor->findNext(searchText());
    updateStatus(tr("Replaced one occurrence"));
}

void FindReplaceDock::replaceAll(CodeEditor *editor)
{
    if (!editor || searchText().isEmpty()) {
        return;
    }
    const QString findText = searchText();
    const QString replText = replaceText();
    int count = 0;

    QTextCursor cursor(editor->document());
    cursor.beginEditBlock();
    while (!cursor.isNull()) {
        cursor = editor->document()->find(findText, cursor);
        if (cursor.isNull()) {
            break;
        }
        cursor.insertText(replText);
        ++count;
    }
    cursor.endEditBlock();

    updateStatus(tr("Replaced %n occurrence(s)", "", count));
}

} // namespace mo::ui
