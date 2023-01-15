//#include <stdio.h>
#include <windows.h>
#include <wingdi.h>



struct syGDIst_gdi
{
  HWND hWnd;
  HDC scrDC;
  HDC memDC;
  HBITMAP hbmp;
  HGDIOBJ pre_bmp;
  HPEN hpen;
  HGDIOBJ pre_pen;
  HFONT hfont;
  HGDIOBJ pre_font;
  int width_scr;
  int height_scr;
  int width_bmp;
  int height_bmp;
  int fail_code;
};


int syGDIfc_inis(HWND hWnd, int width_bmp, int height_bmp, struct syGDIst_gdi* st_gdi)
{
  // *st_gdi=calloc(1,sizeof(struct syGDIst_gdi));  //allocate mem for new gdi object
  // if (st_gdi==NULL)
  //   return 30;  //failed to alloc mem of new gdi object
  st_gdi -> fail_code = 0;  //inis failed.
  
  st_gdi -> scrDC = GetDC(hWnd);
  if(st_gdi -> scrDC==NULL)  //get screen DC failed.(0)
    return 0;
  st_gdi -> width_scr = GetDeviceCaps(st_gdi -> scrDC, DESKTOPHORZRES);
  st_gdi -> height_scr = GetDeviceCaps(st_gdi -> scrDC, DESKTOPVERTRES);
  st_gdi -> memDC = CreateCompatibleDC(st_gdi -> scrDC);
  if(st_gdi -> memDC==NULL)  //Create memory DC failed.(-1)
    return -1;
  if(width_bmp==0&&height_bmp==0)
  {
    width_bmp = st_gdi -> width_scr;
    height_bmp = st_gdi -> height_scr;
  }
  st_gdi -> hbmp = CreateCompatibleBitmap(st_gdi -> scrDC, width_bmp, height_bmp);
  if(st_gdi -> hbmp==NULL)  //Create Bitmap DC failed.(-2)
    return -2;
  st_gdi -> pre_bmp = SelectObject(st_gdi -> memDC, st_gdi -> hbmp);
  if( (st_gdi -> pre_bmp==NULL) || (st_gdi -> pre_bmp==HGDI_ERROR) )  //Put bitmap into memory failed.(-3)
    return -3;
  
  SetTextAlign(st_gdi -> memDC,TA_UPDATECP);  //Set text position auto update

  st_gdi -> hWnd = hWnd;
  st_gdi -> width_bmp = width_bmp;
  st_gdi -> height_bmp = height_bmp;
  return 1;
}

int syGDIfc_clr(struct syGDIst_gdi* st_gdi)
{
  st_gdi -> fail_code = -1;  //clr failed.
  SelectObject(st_gdi -> memDC, st_gdi -> pre_bmp);  //take back bitmap from memory.
  if( 0==DeleteObject(st_gdi -> hbmp) )  //Delete bitmap failed.(-4)
    return -4;
  SelectObject(st_gdi -> memDC, st_gdi -> pre_pen);  //take back pen from memory.
  if( 0==DeleteObject(st_gdi -> hpen) )  //Delete pen failed.(-7)
    return -7;
  SelectObject(st_gdi -> memDC, st_gdi -> pre_font);  //take original font back to memory.
  if( 0==DeleteObject(st_gdi -> hfont) )  //Delete font-created failed.(-7)
    return -7;
  if( 0==DeleteDC(st_gdi -> memDC) )  //Delete memory DC failed.(-5)
    return -5;
  if( 0==ReleaseDC(st_gdi -> hWnd, st_gdi -> scrDC) )  //Release (screen) DC failed.(-6)
    return -6;
  return 1;
}

int syGDIfc_delBmp(struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hbmp==NULL)  //no bitmap.
    return 0;
  SelectObject(st_gdi -> memDC, st_gdi -> pre_bmp);  //take back bitmap from memory.
  if( 0==DeleteObject(st_gdi -> hbmp) )  //Delete bitmap failed.(-4)
    return -4;
}

int syGDIfc_putBmp(int width_bmp, int height_bmp, struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hbmp!=NULL)  //Delete original bitmap
    syGDIfc_delBmp(st_gdi);

  st_gdi -> hbmp = CreateCompatibleBitmap(st_gdi -> scrDC, width_bmp, height_bmp);  //Create new bitmap
  if(st_gdi -> hbmp==NULL)  //Create Bitmap DC failed.(-2)
    return -2;

  st_gdi -> pre_bmp = SelectObject(st_gdi -> memDC, st_gdi -> hbmp);
  if( (st_gdi -> pre_bmp==NULL) || (st_gdi -> pre_bmp==HGDI_ERROR) )  //Put bitmap into memory failed.(-3)
    return -3;
}

int syGDIfc_delPen(struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hpen==NULL)  //no pen.
    return 0;
  SelectObject(st_gdi -> memDC, st_gdi -> pre_pen);  //take back pen from memory.
  if( 0==DeleteObject(st_gdi -> hpen) )  //Delete pen failed.(-7)
    return -7;
}

int syGDIfc_putPen(int width, unsigned char r, unsigned char g, unsigned char b, struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hpen!=NULL)  //Delete original pen
    syGDIfc_delPen(st_gdi);
  
  st_gdi -> hpen = CreatePen(PS_SOLID, width, RGB(r,g,b));
  if(st_gdi -> hpen==NULL)  //Create pen failed.(-8)
    return -8;
  st_gdi -> pre_pen = SelectObject(st_gdi -> memDC, st_gdi -> hpen);
  if( (st_gdi -> pre_pen==NULL) || (st_gdi -> pre_pen==HGDI_ERROR) )  //Put bitmap into memory failed.(-9)
    return -9;
}


int syGDIfc_delFont(struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hfont==NULL)  //no font.
    return 0;
  SelectObject(st_gdi -> memDC, st_gdi -> pre_font);  //take original font back to memory.
  if( 0==DeleteObject(st_gdi -> hfont) )  //Delete font-created failed.(-7)
    return -7;
}

int syGDIfc_putFont(int cHeight, int cWidth, int cEscapement, int cOrientation, LPCSTR pszFaceName, char b, char i, char u, char s,struct syGDIst_gdi* st_gdi)
{
  if(st_gdi -> hfont!=NULL)  //Delete pre-created font if exist
    syGDIfc_delFont(st_gdi);
  
  int cWeight; DWORD  bItalic; DWORD bUnderline; DWORD bStrikeOut;
  if (b>0)
    cWeight=700;
  else
    cWeight=400;
  if (i>0)
    bItalic=TRUE;
  else
    bItalic=FALSE;  
  if (u>0)
    bUnderline=TRUE;
  else
    bUnderline=FALSE;  
  if (s>0)
    bStrikeOut=TRUE;
  else
    bStrikeOut=FALSE;
  st_gdi -> hfont = CreateFont(cHeight,cWidth,cEscapement,cOrientation,cWeight,bItalic,bUnderline,bStrikeOut, 
    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE|DEFAULT_PITCH,
    pszFaceName );
  if(st_gdi -> hfont==NULL)  //Create font failed.(-8)
    return -8;
  st_gdi -> pre_font = SelectObject(st_gdi -> memDC, st_gdi -> hfont);
  if( (st_gdi -> pre_font==NULL) || (st_gdi -> pre_font==HGDI_ERROR) )  //Put bitmap into memory failed.(-9)
    return -9;
}


int syGDIfc_line(int o_x, int o_y, int d_x, int d_y,struct syGDIst_gdi* st_gdi)
{
  MoveToEx(st_gdi->memDC, o_x, o_y, NULL);
  LineTo(st_gdi->memDC, d_x, d_y);
}

int syGDIfc_blt(int x, int y, int cx, int cy,struct syGDIst_gdi* st_gdi)
{
  BitBlt(st_gdi->scrDC,x,y,cx,cy,st_gdi->memDC,0,0,SRCCOPY);
}



/*
1.获取设备上下文
  HDC GetDC(
  [in] HWND hWnd );
2.创建与1兼容的内存上下文
  HDC CreateCompatibleDC(
  [in] HDC hdc  );
3.创建与1兼容的位图上下文
  HBITMAP CreateCompatibleBitmap(
  [in] HDC hdc,
  [in] int cx,
  [in] int cy  );
4.将位图放入内存中
  HGDIOBJ SelectObject(
  [in] HDC     hdc,
  [in] HGDIOBJ h  );
5.绘制
6.恢复内存原位图
  SelectObject();
7.删除兼容位图
  BOOL DeleteObject(
  [in] HGDIOBJ ho  );
8.删除兼容内存上下文
  BOOL DeleteDC(
  [in] HDC hdc  );
9.释放显示上下文
  int ReleaseDC(
  [in] HWND hWnd,
  [in] HDC  hDC  );
*/