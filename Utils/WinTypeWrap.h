/****************************************************************************/
/*                                                                          */
/*      文件名:    WinTypeWrap.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CEasySize;
class CEasyPoint;
class CEasyRect;

class CEasySize : public tagSIZE
{
public:

	// Constructors
	// construct an uninitialized size
	CEasySize() throw();
	// create from two integers
	CEasySize(int initCX, int initCY) throw();
	// create from another size
	CEasySize(SIZE initSize) throw();
	// create from a point
	CEasySize(POINT initPt) throw();
	// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
	CEasySize(DWORD dwSize) throw();

	// Operations
	BOOL operator==(SIZE size) const throw();
	BOOL operator!=(SIZE size) const throw();
	void operator+=(SIZE size) throw();
	void operator-=(SIZE size) throw();
	void SetSize(int CX, int CY) throw();

	// Operators returning CEasySize values
	CEasySize operator+(SIZE size) const throw();
	CEasySize operator-(SIZE size) const throw();
	CEasySize operator-() const throw();

	// Operators returning CEasyPoint values
	CEasyPoint operator+(POINT point) const throw();
	CEasyPoint operator-(POINT point) const throw();

	// Operators returning CEasyRect values
	CEasyRect operator+(const RECT* lpRect) const throw();
	CEasyRect operator-(const RECT* lpRect) const throw();
};

class CEasyPoint : public tagPOINT
{
public:
	// Constructors

	// create an uninitialized point
	CEasyPoint() throw();
	// create from two integers
	CEasyPoint(int initX, int initY) throw();
	// create from another point
	CEasyPoint(POINT initPt) throw();
	// create from a size
	CEasyPoint(SIZE initSize) throw();
	// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
	CEasyPoint(LPARAM dwPoint) throw();


	// Operations

	// translate the point
	void Offset(int xOffset, int yOffset) throw();
	void Offset(POINT point) throw();
	void Offset(SIZE size) throw();
	void SetPoint(int X, int Y) throw();

	BOOL operator==(POINT point) const throw();
	BOOL operator!=(POINT point) const throw();
	void operator+=(SIZE size) throw();
	void operator-=(SIZE size) throw();
	void operator+=(POINT point) throw();
	void operator-=(POINT point) throw();

	// Operators returning CEasyPoint values
	CEasyPoint operator+(SIZE size) const throw();
	CEasyPoint operator-(SIZE size) const throw();
	CEasyPoint operator-() const throw();
	CEasyPoint operator+(POINT point) const throw();

	// Operators returning CEasySize values
	CEasySize operator-(POINT point) const throw();

	// Operators returning CEasyRect values
	CEasyRect operator+(const RECT* lpRect) const throw();
	CEasyRect operator-(const RECT* lpRect) const throw();
};

class CEasyRect :
	public RECT
{	// Constructors
public:
	// uninitialized rectangle
	CEasyRect() throw();
	// from left, top, right, and bottom
	CEasyRect(int l, int t, int r, int b) throw();
	// copy constructor
	CEasyRect(const RECT& srcRect) throw();
	// from a pointer to another rect
	CEasyRect(LPCRECT lpSrcRect) throw();
	// from a point and size
	CEasyRect(POINT point, SIZE size) throw();
	// from two points
	CEasyRect(POINT topLeft, POINT bottomRight) throw();

	// Attributes (in addition to RECT members)

	// retrieves the width
	int Width() const throw();
	// returns the height
	int Height() const throw();
	// returns the size
	CEasySize Size() const throw();
	// reference to the top-left point
	CEasyPoint& TopLeft() throw();
	// reference to the bottom-right point
	CEasyPoint& BottomRight() throw();
	// const reference to the top-left point
	const CEasyPoint& TopLeft() const throw();
	// const reference to the bottom-right point
	const CEasyPoint& BottomRight() const throw();
	// the geometric center point of the rectangle
	CEasyPoint CenterPoint() const throw();
	// swap the left and right
	void SwapLeftRight() throw();
	static void WINAPI SwapLeftRight(LPRECT lpRect) throw();

	// convert between CEasyRect and LPRECT/LPCRECT (no need for &)
	operator LPRECT() throw();
	operator LPCRECT() const throw();

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const throw();
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const throw();
	// returns TRUE if point is within rectangle
	BOOL PtInRect(POINT point) const throw();

	// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) throw();
	void SetRect(POINT topLeft, POINT bottomRight) throw();
	// empty the rectangle
	void SetRectEmpty() throw();
	// copy from another rectangle
	void CopyRect(LPCRECT lpSrcRect) throw();
	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(LPCRECT lpRect) const throw();

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(int x, int y) throw();
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(SIZE size) throw();
	// Inflate rectangle's width and height by moving individual sides.
	// Left side is moved to the left, right side is moved to the right,
	// top is moved up and bottom is moved down.
	void InflateRect(LPCRECT lpRect) throw();
	void InflateRect(int l, int t, int r, int b) throw();

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(int x, int y) throw();
	void DeflateRect(SIZE size) throw();
	void DeflateRect(LPCRECT lpRect) throw();
	void DeflateRect(int l, int t, int r, int b) throw();

	// translate the rectangle by moving its top and left
	void OffsetRect(int x, int y) throw();
	void OffsetRect(SIZE size) throw();
	void OffsetRect(POINT point) throw();
	void NormalizeRect() throw();

	// absolute position of rectangle
	void MoveToY(int y) throw();
	void MoveToX(int x) throw();
	void MoveToXY(int x, int y) throw();
	void MoveToXY(POINT point) throw();

	// set this rectangle to intersection of two others
	BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

	// set this rectangle to bounding union of two others
	BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

	// set this rectangle to minimum of two others
	BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw();

	// Additional Operations
	void operator=(const RECT& srcRect) throw();
	BOOL operator==(const RECT& rect) const throw();
	BOOL operator!=(const RECT& rect) const throw();
	void operator+=(POINT point) throw();
	void operator+=(SIZE size) throw();
	void operator+=(LPCRECT lpRect) throw();
	void operator-=(POINT point) throw();
	void operator-=(SIZE size) throw();
	void operator-=(LPCRECT lpRect) throw();
	void operator&=(const RECT& rect) throw();
	void operator|=(const RECT& rect) throw();

	// Operators returning CEasyRect values
	CEasyRect operator+(POINT point) const throw();
	CEasyRect operator-(POINT point) const throw();
	CEasyRect operator+(LPCRECT lpRect) const throw();
	CEasyRect operator+(SIZE size) const throw();
	CEasyRect operator-(SIZE size) const throw();
	CEasyRect operator-(LPCRECT lpRect) const throw();
	CEasyRect operator&(const RECT& rect2) const throw();
	CEasyRect operator|(const RECT& rect2) const throw();
	CEasyRect MulDiv(int nMultiplier, int nDivisor) const throw();
};


inline CEasySize::CEasySize() throw()
{ /* random filled */ }
inline CEasySize::CEasySize(int initCX, int initCY) throw()
{ cx = initCX; cy = initCY; }
inline CEasySize::CEasySize(SIZE initSize) throw()
{ *(SIZE*)this = initSize; }
inline CEasySize::CEasySize(POINT initPt) throw()
{ *(POINT*)this = initPt; }
inline CEasySize::CEasySize(DWORD dwSize) throw()
{
	cx = (short)LOWORD(dwSize);
	cy = (short)HIWORD(dwSize);
}
inline BOOL CEasySize::operator==(SIZE size) const throw()
{ return (cx == size.cx && cy == size.cy); }
inline BOOL CEasySize::operator!=(SIZE size) const throw()
{ return (cx != size.cx || cy != size.cy); }
inline void CEasySize::operator+=(SIZE size) throw()
{ cx += size.cx; cy += size.cy; }
inline void CEasySize::operator-=(SIZE size) throw()
{ cx -= size.cx; cy -= size.cy; }
inline void CEasySize::SetSize(int CX, int CY) throw()
{ cx = CX; cy = CY; }	
inline CEasySize CEasySize::operator+(SIZE size) const throw()
{ return CEasySize(cx + size.cx, cy + size.cy); }
inline CEasySize CEasySize::operator-(SIZE size) const throw()
{ return CEasySize(cx - size.cx, cy - size.cy); }
inline CEasySize CEasySize::operator-() const throw()
{ return CEasySize(-cx, -cy); }
inline CEasyPoint CEasySize::operator+(POINT point) const throw()
{ return CEasyPoint(cx + point.x, cy + point.y); }
inline CEasyPoint CEasySize::operator-(POINT point) const throw()
{ return CEasyPoint(cx - point.x, cy - point.y); }
inline CEasyRect CEasySize::operator+(const RECT* lpRect) const throw()
{ return CEasyRect(lpRect) + *this; }
inline CEasyRect CEasySize::operator-(const RECT* lpRect) const throw()
{ return CEasyRect(lpRect) - *this; }


inline CEasyPoint::CEasyPoint() throw()
{ /* random filled */ }
inline CEasyPoint::CEasyPoint(int initX, int initY) throw()
{ x = initX; y = initY; }
inline CEasyPoint::CEasyPoint(POINT initPt) throw()
{ *(POINT*)this = initPt; }
inline CEasyPoint::CEasyPoint(SIZE initSize) throw()
{ *(SIZE*)this = initSize; }
inline CEasyPoint::CEasyPoint(LPARAM dwPoint) throw()
{
	x = (short)LOWORD(dwPoint);
	y = (short)HIWORD(dwPoint);
}
inline void CEasyPoint::Offset(int xOffset, int yOffset) throw()
{ x += xOffset; y += yOffset; }
inline void CEasyPoint::Offset(POINT point) throw()
{ x += point.x; y += point.y; }
inline void CEasyPoint::Offset(SIZE size) throw()
{ x += size.cx; y += size.cy; }
inline void CEasyPoint::SetPoint(int X, int Y) throw()
{ x = X; y = Y; }
inline BOOL CEasyPoint::operator==(POINT point) const throw()
{ return (x == point.x && y == point.y); }
inline BOOL CEasyPoint::operator!=(POINT point) const throw()
{ return (x != point.x || y != point.y); }
inline void CEasyPoint::operator+=(SIZE size) throw()
{ x += size.cx; y += size.cy; }
inline void CEasyPoint::operator-=(SIZE size) throw()
{ x -= size.cx; y -= size.cy; }
inline void CEasyPoint::operator+=(POINT point) throw()
{ x += point.x; y += point.y; }
inline void CEasyPoint::operator-=(POINT point) throw()
{ x -= point.x; y -= point.y; }
inline CEasyPoint CEasyPoint::operator+(SIZE size) const throw()
{ return CEasyPoint(x + size.cx, y + size.cy); }
inline CEasyPoint CEasyPoint::operator-(SIZE size) const throw()
{ return CEasyPoint(x - size.cx, y - size.cy); }
inline CEasyPoint CEasyPoint::operator-() const throw()
{ return CEasyPoint(-x, -y); }
inline CEasyPoint CEasyPoint::operator+(POINT point) const throw()
{ return CEasyPoint(x + point.x, y + point.y); }
inline CEasySize CEasyPoint::operator-(POINT point) const throw()
{ return CEasySize(x - point.x, y - point.y); }
inline CEasyRect CEasyPoint::operator+(const RECT* lpRect) const throw()
{ return CEasyRect(lpRect) + *this; }
inline CEasyRect CEasyPoint::operator-(const RECT* lpRect) const throw()
{ return CEasyRect(lpRect) - *this; }


inline CEasyRect::CEasyRect() throw()
{ /* random filled */ }
inline CEasyRect::CEasyRect(int l, int t, int r, int b) throw()
{ left = l; top = t; right = r; bottom = b; }
inline CEasyRect::CEasyRect(const RECT& srcRect) throw()
{ ::CopyRect(this, &srcRect); }
inline CEasyRect::CEasyRect(LPCRECT lpSrcRect) throw()
{ ::CopyRect(this, lpSrcRect); }
inline CEasyRect::CEasyRect(POINT point, SIZE size) throw()
{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
inline CEasyRect::CEasyRect(POINT topLeft, POINT bottomRight) throw()
{ left = topLeft.x; top = topLeft.y;
right = bottomRight.x; bottom = bottomRight.y; }
inline int CEasyRect::Width() const throw()
{ return right - left; }
inline int CEasyRect::Height() const throw()
{ return bottom - top; }
inline CEasySize CEasyRect::Size() const throw()
{ return CEasySize(right - left, bottom - top); }
inline CEasyPoint& CEasyRect::TopLeft() throw()
{ return *((CEasyPoint*)this); }
inline CEasyPoint& CEasyRect::BottomRight() throw()
{ return *((CEasyPoint*)this+1); }
inline const CEasyPoint& CEasyRect::TopLeft() const throw()
{ return *((CEasyPoint*)this); }
inline const CEasyPoint& CEasyRect::BottomRight() const throw()
{ return *((CEasyPoint*)this+1); }
inline CEasyPoint CEasyRect::CenterPoint() const throw()
{ return CEasyPoint((left+right)/2, (top+bottom)/2); }
inline void CEasyRect::SwapLeftRight() throw()
{ SwapLeftRight(LPRECT(this)); }
inline void WINAPI CEasyRect::SwapLeftRight(LPRECT lpRect) throw()
{ int temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
inline CEasyRect::operator LPRECT() throw()
{ return this; }
inline CEasyRect::operator LPCRECT() const throw()
{ return this; }
inline BOOL CEasyRect::IsRectEmpty() const throw()
{ return ::IsRectEmpty(this); }
inline BOOL CEasyRect::IsRectNull() const throw()
{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
inline BOOL CEasyRect::PtInRect(POINT point) const throw()
{ return ::PtInRect(this, point); }
inline void CEasyRect::SetRect(int x1, int y1, int x2, int y2) throw()
{ ::SetRect(this, x1, y1, x2, y2); }
inline void CEasyRect::SetRect(POINT topLeft, POINT bottomRight) throw()
{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
inline void CEasyRect::SetRectEmpty() throw()
{ ::SetRectEmpty(this); }
inline void CEasyRect::CopyRect(LPCRECT lpSrcRect) throw()
{ ::CopyRect(this, lpSrcRect); }
inline BOOL CEasyRect::EqualRect(LPCRECT lpRect) const throw()
{ return ::EqualRect(this, lpRect); }
inline void CEasyRect::InflateRect(int x, int y) throw()
{ ::InflateRect(this, x, y); }
inline void CEasyRect::InflateRect(SIZE size) throw()
{ ::InflateRect(this, size.cx, size.cy); }
inline void CEasyRect::DeflateRect(int x, int y) throw()
{ ::InflateRect(this, -x, -y); }
inline void CEasyRect::DeflateRect(SIZE size) throw()
{ ::InflateRect(this, -size.cx, -size.cy); }
inline void CEasyRect::OffsetRect(int x, int y) throw()
{ ::OffsetRect(this, x, y); }
inline void CEasyRect::OffsetRect(POINT point) throw()
{ ::OffsetRect(this, point.x, point.y); }
inline void CEasyRect::OffsetRect(SIZE size) throw()
{ ::OffsetRect(this, size.cx, size.cy); }
inline void CEasyRect::MoveToY(int y) throw()
{ bottom = Height() + y; top = y; }
inline void CEasyRect::MoveToX(int x) throw()
{ right = Width() + x; left = x; }
inline void CEasyRect::MoveToXY(int x, int y) throw()
{ MoveToX(x); MoveToY(y); }
inline void CEasyRect::MoveToXY(POINT pt) throw()
{ MoveToX(pt.x); MoveToY(pt.y); }
inline BOOL CEasyRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
{ return ::IntersectRect(this, lpRect1, lpRect2);}
inline BOOL CEasyRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
{ return ::UnionRect(this, lpRect1, lpRect2); }
inline void CEasyRect::operator=(const RECT& srcRect) throw()
{ ::CopyRect(this, &srcRect); }
inline BOOL CEasyRect::operator==(const RECT& rect) const throw()
{ return ::EqualRect(this, &rect); }
inline BOOL CEasyRect::operator!=(const RECT& rect) const throw()
{ return !::EqualRect(this, &rect); }
inline void CEasyRect::operator+=(POINT point) throw()
{ ::OffsetRect(this, point.x, point.y); }
inline void CEasyRect::operator+=(SIZE size) throw()
{ ::OffsetRect(this, size.cx, size.cy); }
inline void CEasyRect::operator+=(LPCRECT lpRect) throw()
{ InflateRect(lpRect); }
inline void CEasyRect::operator-=(POINT point) throw()
{ ::OffsetRect(this, -point.x, -point.y); }
inline void CEasyRect::operator-=(SIZE size) throw()
{ ::OffsetRect(this, -size.cx, -size.cy); }
inline void CEasyRect::operator-=(LPCRECT lpRect) throw()
{ DeflateRect(lpRect); }
inline void CEasyRect::operator&=(const RECT& rect) throw()
{ ::IntersectRect(this, this, &rect); }
inline void CEasyRect::operator|=(const RECT& rect) throw()
{ ::UnionRect(this, this, &rect); }
inline CEasyRect CEasyRect::operator+(POINT pt) const throw()
{ CEasyRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
inline CEasyRect CEasyRect::operator-(POINT pt) const throw()
{ CEasyRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
inline CEasyRect CEasyRect::operator+(SIZE size) const throw()
{ CEasyRect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
inline CEasyRect CEasyRect::operator-(SIZE size) const throw()
{ CEasyRect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
inline CEasyRect CEasyRect::operator+(LPCRECT lpRect) const throw()
{ CEasyRect rect(this); rect.InflateRect(lpRect); return rect; }
inline CEasyRect CEasyRect::operator-(LPCRECT lpRect) const throw()
{ CEasyRect rect(this); rect.DeflateRect(lpRect); return rect; }
inline CEasyRect CEasyRect::operator&(const RECT& rect2) const throw()
{ CEasyRect rect; ::IntersectRect(&rect, this, &rect2);
return rect; }
inline CEasyRect CEasyRect::operator|(const RECT& rect2) const throw()
{ CEasyRect rect; ::UnionRect(&rect, this, &rect2);
return rect; }
inline BOOL CEasyRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }

inline void CEasyRect::NormalizeRect() throw()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

inline void CEasyRect::InflateRect(LPCRECT lpRect) throw()
{
	left -= lpRect->left;		top -= lpRect->top;
	right += lpRect->right;		bottom += lpRect->bottom;
}

inline void CEasyRect::InflateRect(int l, int t, int r, int b) throw()
{
	left -= l;			top -= t;
	right += r;			bottom += b;
}

inline void CEasyRect::DeflateRect(LPCRECT lpRect) throw()
{
	left += lpRect->left;	top += lpRect->top;
	right -= lpRect->right;	bottom -= lpRect->bottom;
}

inline void CEasyRect::DeflateRect(int l, int t, int r, int b) throw()
{
	left += l;		top += t;
	right -= r;		bottom -= b;
}

inline CEasyRect CEasyRect::MulDiv(int nMultiplier, int nDivisor) const throw()
{
	return CEasyRect(
		::MulDiv(left, nMultiplier, nDivisor),
		::MulDiv(top, nMultiplier, nDivisor),
		::MulDiv(right, nMultiplier, nDivisor),
		::MulDiv(bottom, nMultiplier, nDivisor));
}