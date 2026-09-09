/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings.h"

enum {
	MaxSelectedItems = 100,

	LocalEncryptIterCount = 4000, // key derivation iteration count
	LocalEncryptNoPwdIterCount = 4, // key derivation iteration count without pwd (not secure anyway)
	LocalEncryptSaltSize = 32, // 256 bit

	AutoSearchTimeout = 900, // 0.9 secs

	PreloadHeightsCount = 3, // when 3 screens to scroll left make a preload request

	SearchPeopleLimit = 20,

	WebPageUserId = 701000,

	UpdateDelayConstPart = 8 * 3600, // 8 hour min time between update check requests
	UpdateDelayRandPart = 8 * 3600, // 8 hour max - min time between update check requests

	WrongPasscodeTimeout = 1500,

	ChoosePeerByDragTimeout = 1000, // 1 second mouse not moved to choose dialog when dragging a file
};

inline const char *cGUIDStr() {
#ifndef OS_MAC_STORE
	static const char *gGuidStr = "{87A94AB0-E370-4cde-98D3-ACC110C59666}";
#else // OS_MAC_STORE
	static const char *gGuidStr = "{E51FB841-8C0B-4EF9-9E9E-5A0078567666}";
#endif // OS_MAC_STORE

	return gGuidStr;
}

static const char *UpdatesPublicKey = "\
-----BEGIN RSA PUBLIC KEY-----\n\
MIIBCgKCAQEAmkrwTEP3G2Wp2cuHmE5l0BXx4lAYS9EGuzUzvb7KC2zYQS/xd/y8\n\
oZUNr2dh6nY1jOcbZZziYTyeuMiYCU5GY6grd8prOyG916etFrevmXM1UJ164XQr\n\
mWqSXCcblYA/3DHOlNNZ8Iv9FK9iMeC8s16+ZyQBHytsUCQ29Vc5+zWSP/0BFTc9\n\
PfRvA1NpU2NppTPP5+ILwnYDYjZpZJ5qxeKqByKedGAkyRLfnkZmFLeZD41KcP+y\n\
3bjFOmtWYYzAdPVDigQpzpj6d7yczf64eaAunf/T0ZFL/jhFH9azwpbWdHuDy6y+\n\
NXTqVVdYICg9mIFqABTDYAZmJapD32lV4wIDAQAB\n\
-----END RSA PUBLIC KEY-----\
";

static const char *UpdatesPublicBetaKey = "\
-----BEGIN RSA PUBLIC KEY-----\n\
MIIBCgKCAQEAwfPYpnxhHZIC4OsbqkV2k0FP/WozuDLtfWWLZcnj6VDn3cs+Kvla\n\
kl/NJXjo2SMw9fb0RUdw6iIYXoonkSZkdc199ALAkVRGYu1xBdqBql1Ke2c+pUjJ\n\
QLaivse6HvkdreHw8GsjE6Et1yLwDcSzxcK3vcBiCGsR0+ARVRiZ+IXyH3BA21W9\n\
JwS2Yc9Qa7Ur1hmnLuvmy/wdgDagIvkL/Vuc78dHA9XTHZohuJE21PUuApeLtd4I\n\
u0VmXVLYqpR1R5reEipv+BNHXfg5pWrr+nlNNyh8AWARfTuSZTN34/BMBVqV8Vgj\n\
6Ci32WNV6JHjJgETmYERLUOZYML9ic6n4QIDAQAB\n\
-----END RSA PUBLIC KEY-----\
";

#if defined TDESKTOP_API_ID && defined TDESKTOP_API_HASH

constexpr auto ApiId = TDESKTOP_API_ID;
constexpr auto ApiHash = QT_STRINGIFY(TDESKTOP_API_HASH);

#else // TDESKTOP_API_ID && TDESKTOP_API_HASH

// To build your version of Telegram Desktop you're required to provide
// your own 'api_id' and 'api_hash' for the Telegram API access.
//
// How to obtain your 'api_id' and 'api_hash' is described here:
// https://core.telegram.org/api/obtaining_api_id
//
// If you're building the application not for deployment,
// but only for test purposes you can comment out the error below.
//
// This will allow you to use TEST ONLY 'api_id' and 'api_hash' which are
// very limited by the Telegram API server.
//
// Your users will start getting internal server errors on login
// if you deploy an app using those 'api_id' and 'api_hash'.

#error You are required to provide API_ID and API_HASH.

constexpr auto ApiId = 17349;
constexpr auto ApiHash = "344583e45741c457fe1862106095a5eb";

#endif // TDESKTOP_API_ID && TDESKTOP_API_HASH

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
#error "Only little endian is supported!"
#endif // Q_BYTE_ORDER == Q_BIG_ENDIAN

#if (TDESKTOP_ALPHA_VERSION != 0)

// Private key for downloading closed alphas.
#include "../../../DesktopPrivate/alpha_private.h"

#else
static const char *AlphaPrivateKey = "";
#endif

extern QString gKeyFile;
inline const QString &cDataFile() {
	if (!gKeyFile.isEmpty()) return gKeyFile;
	static const QString res(u"data"_q);
	return res;
}

inline const QRegularExpression &cRussianLetters() {
	static QRegularExpression regexp(QString::fromUtf8("[а-яА-ЯёЁ]"));
	return regexp;
}
