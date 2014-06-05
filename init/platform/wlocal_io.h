/**************************************************************************/
/*                                                                        */
/*                 WWIV Initialization Utility Version 5.0                */
/*             Copyright (C)1998-2014, WWIV Software Services             */
/*                                                                        */
/*    Licensed  under the  Apache License, Version  2.0 (the "License");  */
/*    you may not use this  file  except in compliance with the License.  */
/*    You may obtain a copy of the License at                             */
/*                                                                        */
/*                http://www.apache.org/licenses/LICENSE-2.0              */
/*                                                                        */
/*    Unless  required  by  applicable  law  or agreed to  in  writing,   */
/*    software  distributed  under  the  License  is  distributed on an   */
/*    "AS IS"  BASIS, WITHOUT  WARRANTIES  OR  CONDITIONS OF ANY  KIND,   */
/*    either  express  or implied.  See  the  License for  the specific   */
/*    language governing permissions and limitations under the License.   */
/*                                                                        */
/**************************************************************************/

#ifndef __INCLUDED_PLATFORM_WLOCALIO_H__
#define __INCLUDED_PLATFORM_WLOCALIO_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // _WIN32

#include "wfile.h"

class WStatus;
class WSession;

// This C++ class should encompass all Local Input/Output from The BBS.
// You should use a routine in here instead of using printf, puts, etc.
class WLocalIO {

  public:
	static const int scrollDown = 1;
	static const int scrollUp = 0;

  private:
	int  m_nScreenBottom;

  protected:
	int ExtendedKeyWaiting;

#if defined ( _WIN32 )
	COORD  m_cursorPosition;
	HANDLE m_hConOut;
	HANDLE m_hConIn;
	CONSOLE_SCREEN_BUFFER_INFO m_consoleBufferInfo;
#endif

#if defined ( __unix__ ) || defined ( __APPLE__ )
	short m_cursorPositionX;
	short m_cursorPositionY;
#endif

  public:
#if defined ( _WIN32 )
	virtual void set_attr_xy(int x, int y, int a);
	COORD m_originalConsoleSize;
#endif // _WIN32

	// Constructor/Destructor
	WLocalIO();
	WLocalIO( const WLocalIO& copy );
	virtual ~WLocalIO();

	const int GetScreenBottom() const {
		return m_nScreenBottom;
	}
	void SetScreenBottom( int nScreenBottom ) {
		m_nScreenBottom = nScreenBottom;
	}
	virtual void LocalGotoXY(int x, int y);
	virtual int  WhereX();
	virtual int  WhereY();
	virtual void LocalLf();
	virtual void LocalCr();
	virtual void LocalCls();
	virtual void LocalClrEol();
	virtual void LocalBackspace();
	virtual void LocalPutchRaw(unsigned char ch);
	virtual void LocalPutch(unsigned char ch);
	virtual void LocalPuts( const char *pszText );
	virtual void LocalXYPuts( int x, int y, const char *pszText );
	virtual int getchd();
	virtual void LocalScrollScreen(int nTop, int nBottom, int nDirection);
};

#endif // __INCLUDED_PLATFORM_WLOCALIO_H__