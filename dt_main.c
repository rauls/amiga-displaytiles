/*********************************
*                                *
*  (c) 1993 Raul A. Sobon			*
*                                *
*  This source is copyright 1993 *
*  in all respects.              *
*                                *
*  DeskTop				 			 	*
*********************************/

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>

#include <dos/dos.h>				// WHY!! 
#include <dos/dosextens.h>
#include <dos/stdio.h>				// doses stdio

#include <intuition/intuitionbase.h>
#include <intuition/classes.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/scale.h>

#include "dt_globals.h"

#include <proto/exec.h>				// use amiga library stuff
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/diskfont.h> 


#include <stdio.h>					// and the thing we all use!
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dt_gad.h"				// gadgets include


static char  *version_string = { VERSTAG };
int	windowflag=0;

/*
 * --- Message info
 */
ULONG			IClass;
UWORD			Qualifier, Code;

struct		Gadget	*IObject;
ULONG			version;
WORD			maxcolors,maxdepth,
				oldx,oldy;


	WORD	scale_code = 1;
	BOOL	jump_screen = FALSE;

struct TextFont	*thinpazfont8;



//======================================================================================


/*
 * --- Close the libraries which are opened by me.
 */
void CloseLibraries( void ){
	if (DiskFontBase)		CloseLibrary( (struct Library *) DiskFontBase );
	if (GadToolsBase)		CloseLibrary(( struct Library * )GadToolsBase );
	if (IntuitionBase)	CloseLibrary(( struct Library * )IntuitionBase );
	if (DosBase)			CloseLibrary(( struct Library * )DosBase );
	if (GfxBase)			CloseLibrary(( struct Library * )GfxBase );
	if (AslBase)			CloseLibrary(( struct Library * )AslBase );
}






/*
 * --- Deallocate and close all resources I grabbed before.
 */
LONG ExitProgram( LONG code ) {
	if ( thinpazfont8 )
		CloseFont( thinpazfont8 );

	CloseInputWindow();

	CloseLibraries();

	Exit( code );
}




/*
 * --- Open lots of libraries that I need.
 */
long OpenLibraries( void ){

	if ( !(GfxBase = (struct GfxBase *) OpenLibrary((UBYTE *) "graphics.library" , 36l ))) {
		Printf("\graphics.library\n");
		return FALSE;
		}

	if ( !(DosBase = (struct DosBase *) OpenLibrary((UBYTE *) "dos.library", 36l ))) {
		Printf("\tdos.library\n");
		return FALSE;
		}

	if ( !(IntuitionBase = (struct IntuitionBase *) OpenLibrary((UBYTE *) "intuition.library", 36l ))) {
		Printf("\tintuition.library\n");
		return FALSE;
		}

	if ( !(AslBase = (struct Library *) OpenLibrary((UBYTE *) "asl.library", 37l ))) {
		Printf("\tasl.library\n");
		return FALSE;
		}

	if ( !(GadToolsBase = (struct Library *) OpenLibrary((UBYTE *) "gadtools.library", 36l ))) {
		Printf("\tgadtools.library\n");
		return FALSE;
		}

	if ( !(DiskFontBase = (struct Library *) OpenLibrary((UBYTE *) "diskfont.library", 36l ))) {
		WriteStr("\tdiskfont.library\n");
		return FALSE;
		}
	return TRUE;
}



int OpenInputDisplay( void ){
	maxdepth=Scr->RastPort.BitMap->Depth;
	maxcolors=1<<maxdepth;
	if ( ! ( VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
		return( 1L );
	return OpenInputWindow();
}



/*
 * --- Allocate and open all resources I need   :-)
 */
long InitProgram( void ){
	long	rc=0;

	Scr = IntuitionBase->ActiveScreen;

	if ( ! (thinpazfont8  = OpenDiskFont( &thinpaz8 )) ) {
			;
		}

    return( rc );
}





/*
 * --- Reads a message from the window message port.
 * --- Returns TRUE if a message was read and puts the
 * --- message data in the globals. Return FALSE if there
 * --- was no message at the port.
 */
long ReadIMsg( struct Window *iwnd )
{
    struct IntuiMessage *imsg;

    if ( imsg = GT_GetIMsg( iwnd->UserPort )) {

        IClass      =   imsg->Class;
        Qualifier   =   imsg->Qualifier;
        Code        =   imsg->Code;
        IObject     =   imsg->IAddress;

        GT_ReplyIMsg( imsg );

        return TRUE;
    }
    return FALSE;
}



/*
 * --- Clears all message from a message port.
 */
void ClearMsgPort( struct MsgPort *mport )
{
    struct IntuiMessage  *msg;

    while ( msg = GT_GetIMsg( mport )) GT_ReplyIMsg( msg );
}






/**********************************************************************
 *
 *						Draw3DOutBox
 *	Description : just like normal bevelbox except inside color is selectable
 *				  plus it looks MONUMENTAL!
 *								¯¯¯¯¯¯¯¯¯¯¯
 *  Returns		: NULL
 *	Globals		: usual
 *
 */
void Draw3DBox(
	struct	RastPort *rp,		// window rastport to draw into
	UWORD	xpos,				// X coordinate to place MEGA-BEVEL
	UWORD	ypos,				// Y coordinate to place MEGA-BEVEL
	UWORD	xsize,				// X size of MEGA-BEVEL
	UWORD	ysize,				// X size of MEGA-BEVEL
	UWORD	shine_pen,			// top-left pen
	UWORD	body_pen,			// central pen
	UWORD	shadow_pen			// bot-right pen
){

#define	LineDraw(x,y,x2,y2)	Move( rp, x , y); \
							Draw( rp, x2, y2 )

	xsize--;ysize--;

	SetAPen( rp, body_pen );
	RectFill( rp, xpos, ypos, xpos+xsize, ypos+ysize);

	LineDraw( xpos, ypos, xpos+1, ypos+1 );
	LineDraw( xpos+xsize-1, ypos+ysize-1, xpos+xsize, ypos+ysize );

	SetAPen( rp, shine_pen );
	LineDraw( xpos+0, ypos+0, xpos+xsize-1, ypos );
	LineDraw( xpos+0, ypos+1, xpos+xsize-2, ypos+1 );
	LineDraw( xpos+0, ypos+0, xpos, ypos+ysize-1 );
	LineDraw( xpos+1, ypos+0, xpos+1, ypos+ysize-2 );

	SetAPen( rp, shadow_pen );
	LineDraw( xpos+1, ypos+ysize-0, xpos+xsize-1, ypos+ysize-0 );
	LineDraw( xpos+2, ypos+ysize-1, xpos+xsize-2, ypos+ysize-1 );
	LineDraw( xpos+xsize-0, ypos+1, xpos+xsize-0, ypos+ysize-1 );
	LineDraw( xpos+xsize-1, ypos+2, xpos+xsize-1, ypos+ysize-2 );

	SetAPen( rp, 2 );
	if ( shine_pen > shadow_pen ){
		LineDraw( xpos+0, ypos+0, xpos+2, ypos+0 );
		LineDraw( xpos+0, ypos+0, xpos+0, ypos+2 );
	}
	else {
		LineDraw( xpos+xsize, ypos+ysize, xpos+xsize-2, ypos+ysize );
		LineDraw( xpos+xsize, ypos+ysize, xpos+xsize-0, ypos+ysize-2 );
	}
}






/**********************************************************************
 *
 *						Draw3DLine
 *	Description : just like normal line except its 3D (sorta!) (horizontal)
 *				  plus it looks MONUMENTAL!
 *								¯¯¯¯¯¯¯¯¯¯¯
 *  Returns		: NULL
 *	Globals		: usual
 *
 */
void Draw3DLine(
	struct	RastPort *rp,		// window rastport to draw into
	UWORD	xpos,				// X coordinate to place MEGA-BEVEL
	UWORD	ypos,				// Y coordinate to place MEGA-BEVEL
	UWORD	xsize,				// X size of MEGA-BEVEL
	UWORD	shine_pen,			// top-left pen
	UWORD	shadow_pen			// bot-right pen
){

	xsize--;

	SetAPen( rp, shine_pen );
	LineDraw( xpos+0, ypos+0, xpos+xsize, ypos+0 );
	SetAPen( rp, shadow_pen );
	LineDraw( xpos+0, ypos+1, xpos+xsize, ypos+1 );
}



//#define	BITMAPSCALE	1

#define	SCALE_NUM	20
#define	SCALE_DEN	100
#define	MAX_X			640
#define	MAX_Y			512
#define	SCALE			10/100		// 1:5

#define	ICON_XS		(MAX_X*SCALE)
#define	ICON_YS		(MAX_Y*SCALE)

#define	RB	8

long ReDrawScreen(
	struct Screen *scr,
	WORD	x,
	WORD	y
){
	LONG	xdest,ydest,xstop,ystop;
	LONG	xs,ys,xdel,ydel,
			rgb[3];
	LONG	col=0,lastcol=0;
	struct	BitScaleArgs	scale_inf;
	UBYTE	pentable[256];

	//========>  setup palette pen table
	for ( col=0; col< (1<<Scr->BitMap.Depth); col++ ){
		GetRGB32( scr->ViewPort.ColorMap, col, 1, &rgb[0] );
		pentable[col] = FindColor( Scr->ViewPort.ColorMap, rgb[0], rgb[1], rgb[2], -1 );
	}

	//========>  setup coordinates etc..
	xdel  = (scr->Width<<RB)/(MAX_X*SCALE);
	ydel  = (scr->Height<<RB)/(MAX_Y*SCALE);
	xstop = (scr->Width<<RB);
	ystop = (scr->Height<<RB);
	ydest = y;

	//========>  resize screen to thumbnail using OS scaling facilities
	switch( scale_code ){
		case 0 :
			scale_inf.bsa_SrcBitMap = scr->RastPort.BitMap;
			scale_inf.bsa_DestBitMap = InputWnd->RPort->BitMap;
			scale_inf.bsa_SrcX = 0;
			scale_inf.bsa_SrcY = 0;
			scale_inf.bsa_DestX = x+InputWnd->LeftEdge;
			scale_inf.bsa_DestY = y+InputWnd->TopEdge;
			scale_inf.bsa_SrcWidth = scr->Width;
			scale_inf.bsa_SrcHeight = scr->Height;
			scale_inf.bsa_XSrcFactor = (scr->Width<<RB)/ICON_XS;
			scale_inf.bsa_YSrcFactor = (scr->Height<<RB)/ICON_YS;
			scale_inf.bsa_XDestFactor = 1<<RB;
			scale_inf.bsa_YDestFactor = 1<<RB;
			scale_inf.bsa_Flags = NULL;
			BitMapScale( &scale_inf );
			break;
		case 1:
			//========>  resize screen to thumbnail using custom code
			for ( ys=0;ys<ystop;ys+=ydel){
				xdest = x;
				for ( xs=0;xs<xstop;xs+=xdel){
					col = pentable[ ReadPixel( &scr->RastPort, (xs>>RB),(ys>>RB) ) ];
					if( col != lastcol )
						SetAPen( InputWnd->RPort, (UBYTE)col );
					WritePixel( InputWnd->RPort, xdest++, ydest );
					lastcol = col;
				}
				ydest++;
			}
			break;
	}
	return 0;
}



#define	MAX_STRLEN	10
void ReDrawBox(
	struct Screen *scr,
	WORD	x,
	WORD	y
){
	UBYTE	stlen=0,*string;

	Draw3DBox( InputWnd->RPort, x-3, y-3, ICON_XS+6, ICON_YS+6, 2, 0, 1 );
	Draw3DBox( InputWnd->RPort, x-3, y-3+ICON_YS+4, ICON_XS+6,12, 1, 0, 2 );
	Move( InputWnd->RPort, x, y+ICON_YS+9 );

	if( scr->Title ) {
		string = scr->Title;
		stlen=strlen( string );
	}
	else
	if( scr->DefaultTitle ) {
		string = scr->DefaultTitle;
		stlen=strlen( string );
	}	
	else
		string = NULL;

	if (  stlen > MAX_STRLEN )
		stlen = MAX_STRLEN;
	else
	if( string && stlen==0 )
		stlen = MAX_STRLEN;

	SetAPen( InputWnd->RPort, 1 );
	SetFont( InputWnd->RPort, thinpazfont8 );
	Text( InputWnd->RPort, string, stlen );
}



long HowManyScreens( void ){
	struct Screen *scr;
	long	cnt=0;

	scr = IntuitionBase->FirstScreen;
	while( scr ){
		cnt++;
		scr = scr->NextScreen;
	}
	return cnt;
}






#define	BOX_LEFT	20
#define	BOX_TOP		10
#define	BOX_XDELTA	18
#define	BOX_YDELTA	23

struct Screen	*GetScreenMatrix( WORD testx, WORD testy ){
	struct Screen *scr;
	WORD	matrixX=1,matrixY=1;
	WORD	x=BOX_LEFT,y=BOX_TOP;
	WORD	screencount,currentscreen;

	screencount = HowManyScreens();

		if ( screencount > 9 ) {
			matrixX = (screencount/4)+1; matrixY = 4;
		}
		else
		if ( screencount > 4 ) {
			matrixX = (screencount/3)+1; matrixY = 3;
		}
		if ( screencount == 4 ||
			 screencount == 3 ) {
			matrixX = 2; matrixY = 2;
		}
		else {
			matrixX = 1; matrixY = screencount;
		}


	scr = IntuitionBase->FirstScreen;
	currentscreen = 0;
	while( scr ){
		if( !(currentscreen%matrixY) && currentscreen ) {
			x+= BOX_LEFT+(BOX_XDELTA+ICON_YS);
			y = BOX_TOP;
		}

		if( ((testx>x) && (testx<(x+ICON_XS))) &&
			 ((testy>y) && (testy<(y+ICON_YS))) )
			return scr;

		y+= (ICON_YS)+BOX_YDELTA;
		scr = scr->NextScreen;
		currentscreen++;
	}
	return NULL;
}





long DrawAllScreens( void ){
	struct Screen *scr;
	WORD	x=BOX_LEFT,y=BOX_TOP,yoff;
static
	WORD	matrixX=1,matrixY=1;
static
	WORD	lastscreencount=0;
	WORD	screencount,currentscreen;
	
	screencount = HowManyScreens();

	SetWindowPointer( InputWnd, WA_BusyPointer, TRUE, TAG_DONE );

	if( screencount != lastscreencount ) {
		if ( InputWnd ) {
			oldx = InputWnd->LeftEdge;
			oldy = InputWnd->TopEdge;
			CloseInputWindow();
		}

		if ( screencount > 9 ) {
			matrixX = (screencount/4)+1; matrixY = 4;
		}
		else
		if ( screencount > 4 ) {
			matrixX = (screencount/3)+1; matrixY = 3;
		}
		if ( screencount == 4 ||
			 screencount == 3 ) {
			matrixX = 2; matrixY = 2;
		}
		else {
			matrixX = 1; matrixY = screencount;
		}

		InputWidth = ((BOX_LEFT)+((BOX_XDELTA+(ICON_XS))*matrixX));
		InputHeight = (11+((BOX_YDELTA+(ICON_YS))*matrixY));
		InputLeft = oldx;
		InputTop = oldy;
		OpenInputDisplay();
		SetFont( InputWnd->RPort, thinpazfont8 );
		Draw3DBox( InputWnd->RPort, InputWnd->BorderLeft, InputWnd->BorderTop, InputWidth-8, InputHeight-InputWnd->BorderBottom, 2, 3, 1 );
	}

	scr = IntuitionBase->FirstScreen;
	y = BOX_TOP+InputWnd->BorderTop;
	currentscreen = 0;
	while( scr ){
		if( !(currentscreen%matrixY) && currentscreen ) {
			x+= (BOX_XDELTA)+ICON_XS;
			y = BOX_TOP+InputWnd->BorderTop;
		}
		ReDrawBox( scr, x, y );
		ReDrawScreen( scr, x, y );
		y+= (BOX_YDELTA)+ICON_YS;
		scr = scr->NextScreen;
		currentscreen++;
	}
	lastscreencount = screencount;

	SetWindowPointer( InputWnd, TAG_DONE );

	return 0;
}







/**********************************************************************
 *
 *						InputMenus()
 *	Description : Handles all menu events
 *  Returns		: BOOL
 *	Globals		:
 *
 */
BOOL HandleInputMenus( void ){
	struct	MenuItem		*Next;
	UWORD	menu, item, sub;
	BOOL	running = TRUE;


    while( Code != MENUNULL ) {
        Next = ItemAddress(  InputMenus, Code );
        menu = MENUNUM( Code );
        item = ITEMNUM( Code );
        sub  = SUBNUM( Code );
        switch ( menu ) {
            case    0:
                switch ( item ) {
					case	0:	if ( Next->Flags&CHECKED )
										jump_screen = TRUE;
								else	jump_screen = FALSE;
								break;
					case	1:	scale_code = sub; break;
					case	2:	break;
					case	3:	break;	//save
					case	4:	break;
					case	5:	running = FALSE; break;

					//	switch ( sub ) {
					//		case	0:	scale_code1 = 0;
					//		case	1:
					//			break;
					//	}
					//	break;
				}
			break;
		}
		Code = Next->NextSelect;
	}
	return running;
}




/* ===================================== */
int main(int argc,char *argv[]) {
	struct	Screen	*showscr;
	BOOL	running = TRUE;
	BOOL	realactive = FALSE;

	if ( ! OpenLibraries() ) {
		Printf("Could not open requested Libraries.\n");
		ExitProgram( 20l );
		}

	version=GfxBase->LibNode.lib_Version;

	if ( version < 37){
		WriteStr("Need AmigaOS Kernal V37 or higher !!\012");
		Exit(20);
	}

						// actual program is here
	InitProgram();

	DrawAllScreens();

//	printf("\nAmigaOS V%ld  MaxColors %d\n",version,maxcolors);

	while ( running ) {
		WaitPort( InputWnd->UserPort );
		while ( ReadIMsg( InputWnd )) {
			switch ( IClass ) {
				case	IDCMP_CLOSEWINDOW:			// Key short cuts
					running=FALSE;
					break;

				case	IDCMP_MOUSEBUTTONS:
					switch ( Code ){
						case SELECTUP:
							showscr = GetScreenMatrix( InputWnd->MouseX, InputWnd->MouseY );
							if( showscr ) {
								ScreenToFront( showscr );
								if( jump_screen ) {
									CloseInputWindow(); Scr = showscr;
									OpenInputDisplay();
									Draw3DBox( InputWnd->RPort, InputWnd->BorderLeft, InputWnd->BorderTop, InputWidth-8, InputHeight-InputWnd->BorderBottom, 2, 3, 1 );
									DrawAllScreens();
									realactive = FALSE;
								}
								if( showscr->FirstWindow )
									ActivateWindow( showscr->FirstWindow );
							}
							break;
						case SELECTDOWN:
							break;
					}
					break;

				case	IDCMP_GADGETUP:
					switch ( IObject->GadgetID ) {
						case	GD_f1:
							DrawAllScreens();
							break;
						case	GD_f2:
							break;
					}
					break;

				case	IDCMP_VANILLAKEY:			// Key short cuts
					switch ( Code ) {
						case	'q':	running=FALSE;	break;
						case	'u':	DrawAllScreens();	break;
					}
					break;

				case	IDCMP_MENUPICK:
					running = HandleInputMenus();
					break;

				case	IDCMP_CHANGEWINDOW:
					DrawAllScreens();
					realactive = FALSE;
					break;


				case	IDCMP_ACTIVEWINDOW:
					ClearMsgPort( InputWnd->UserPort );
					if ( realactive )
						DrawAllScreens();
					else
						realactive = TRUE;
					break;

				case	IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( InputWnd );
					GT_EndRefresh( InputWnd, TRUE);
					//DrawAllScreens();
					break;

			}
		}
	}

	ExitProgram(0);
}


