// SPDX-FileCopyrightText: 2026 Liu Dong <liudng@hotmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QByteArray>
#include <QtTest>

#include "mo/services/EncodingDetector.hpp"

using mo::services::EncodingDetector;

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
    // A longer GB18030 sample so libuchardet has enough statistical signal
    // to commit to a CJK encoding. "你好世界，这是一个测试。" in GB18030:
    //   你好世界，这是一个测试。
    QByteArray phrase = QByteArray::fromHex(
        "c4e3bac3cac0bde7a3acd5e2cad7d2bbb8f6b2e2cad4a1a3");
    QByteArray data;
    for (int i = 0; i < 8; ++i) {
        data.append(phrase);
    }

    const auto result = EncodingDetector::detect(data);
    // libuchardet may return any of the overlapping GB* encoding names
    // depending on version and input length. Accept any of them as a
    // successful CJK detection rather than asserting a specific one.
    const auto lower = result.toLower();
    QVERIFY(lower == QStringLiteral("gb18030")
            || lower == QStringLiteral("gb2312")
            || lower == QStringLiteral("gbk"));
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
