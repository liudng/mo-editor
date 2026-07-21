// SPDX-FileCopyrightText: 2026 liudng <liudng@users.noreply.github.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QByteArray>
#include <QtTest>

#include "hello/services/EncodingDetector.hpp"

using hello::services::EncodingDetector;

class TestEncodingDetector : public QObject {
    Q_OBJECT
  private slots:
    void testUtf8Detection();
    void testGb18030Detection();
    void testAscii();
    void testEmptyInput();
};

void TestEncodingDetector::testUtf8Detection() {
    QByteArray data;
    data += "ASCII prefix ";
    data += QByteArray::fromHex("e4bda0e5a5bd");  // "你好" in UTF-8

    const auto result = EncodingDetector::detect(data);
    QCOMPARE(result.toUpper(), QStringLiteral("UTF-8"));
}

void TestEncodingDetector::testGb18030Detection() {
    // "你好" in GB18030 (EUC-CN).
    QByteArray data = QByteArray::fromHex("c4e3bac3");
    // Repeat the bytes so libuchardet has enough signal to commit.
    data.append(data);
    data.append(data);

    const auto result = EncodingDetector::detect(data);
    QVERIFY(result.compare(QStringLiteral("GB18030"), Qt::CaseInsensitive) == 0
            || result.compare(QStringLiteral("GB2312"), Qt::CaseInsensitive) == 0
            || result.compare(QStringLiteral("GBK"), Qt::CaseInsensitive) == 0);
}

void TestEncodingDetector::testAscii() {
    QByteArray data = "plain ascii content without any multi-byte chars";
    const auto result = EncodingDetector::detect(data);
    QVERIFY(result.compare(QStringLiteral("ASCII"), Qt::CaseInsensitive) == 0
            || result.compare(QStringLiteral("UTF-8"), Qt::CaseInsensitive) == 0);
}

void TestEncodingDetector::testEmptyInput() {
    // The implementation falls back to UTF-8 for empty input.
    QCOMPARE(EncodingDetector::detect(QByteArray()), QStringLiteral("UTF-8"));
}

QTEST_GUILESS_MAIN(TestEncodingDetector)
#include "test_encoding_detector.moc"
