#pragma once
#ifndef _UNISTD_H
#define _UNISTD_H   

/* This is intended as a drop-in replacement for unistd.h on Windows.
 * Please add functionality as neeeded.
 * https://stackoverflow.com/a/826027/1202830
 */

 /* Values for the second argument to access.
	These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
	//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

/* read, write, and close are NOT being #defined here, because while there are file handle specific versions for Windows, they probably don't work for sockets. You need to look at your app and consider whether to call e.g. closesocket(). */

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#endif /* unistd.h  */