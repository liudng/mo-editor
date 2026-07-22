// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTextStream>
#include <QtTest>

#include "mo/models/Document.hpp"

class TestDocument : public QObject {
    Q_OBJECT
  private slots:
    void testLoadExistingFile();
    void testSave();
    void testModifiedState();
};

void TestDocument::testLoadExistingFile() {
    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    {
        QTextStream out(&tmp);
        out << QStringLiteral("Hello, Mo!\n");
    }
    tmp.close();

    mo::models::Document doc;
    QVERIFY(doc.load(tmp.fileName()));
    QCOMPARE(doc.content(), QStringLiteral("Hello, Mo!\n"));
    QCOMPARE(doc.filePath(), tmp.fileName());
    QVERIFY(!doc.isModified());
}

void TestDocument::testSave() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.path() + QStringLiteral("/out.txt");

    mo::models::Document doc;
    doc.setContent(QStringLiteral("line one\nline two\n"));
    QVERIFY(doc.saveAs(path));
    QVERIFY(QFile::exists(path));
    QVERIFY(!doc.isModified());
    QCOMPARE(doc.filePath(), path);

    mo::models::Document reloaded;
    QVERIFY(reloaded.load(path));
    QCOMPARE(reloaded.content(), QStringLiteral("line one\nline two\n"));
}

void TestDocument::testModifiedState() {
    mo::models::Document doc;
    QVERIFY(!doc.isModified());

    doc.setContent(QStringLiteral("edited"));
    QVERIFY(doc.isModified());

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.close();
    QVERIFY(doc.saveAs(tmp.fileName()));
    QVERIFY(!doc.isModified());

    // save() without a file path returns false; saveAs is required first.
    mo::models::Document empty;
    QVERIFY(!empty.save());
}

QTEST_GUILESS_MAIN(TestDocument)
#include "test_document.moc"
