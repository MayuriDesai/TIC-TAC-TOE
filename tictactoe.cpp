#include<windows.h>
#include<tchar.h>
#define DIVISIONS 3

LRESULT CALLBACK WndProc(
	            HWND hwnd,
                UINT iMsg,
				WPARAM wParam,
				LPARAM lParam
				);



int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,LPSTR szcmdLine,int iCmdShow)
{
  TCHAR szAppName[]=TEXT("TC TAC TOE");
  HWND hwnd;
  MSG msg;

  WNDCLASSEX wndClass;
  
  wndClass.cbSize=sizeof(WNDCLASSEX);
  wndClass.style=CS_HREDRAW|CS_VREDRAW;
  wndClass.lpfnWndProc= WndProc;
  wndClass.cbClsExtra=0;
  wndClass.cbWndExtra=0;
  wndClass.hInstance=hInstance;
  wndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
  wndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
  wndClass.hbrBackground=(HBRUSH)GetStockObject(GRAY_BRUSH);
  wndClass.lpszMenuName=NULL;
  wndClass.lpszClassName=szAppName;
  wndClass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);


  

  if(!RegisterClassEx( &wndClass) )
  {
    MessageBox(NULL,TEXT("RegisterClass FAILED"),szAppName,MB_ICONERROR);

    exit (0);
  }


  hwnd= CreateWindowW
	      (
	          szAppName,
              TEXT("TIC TAC TOE"),
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  NULL,
			  NULL,
			  hInstance,
			  NULL
		  );

	if(NULL==hwnd)
	{
		MessageBox(NULL,TEXT("create window failed"),szAppName,MB_ICONERROR);

		return (0);
	}
	

	 ShowWindow(hwnd,iCmdShow);
	 UpdateWindow(hwnd);

	 while(GetMessage(&msg,NULL,0,0))
	  {
		  TranslateMessage(&msg);
		 DispatchMessage(&msg);
	 }
	                
	return msg.wParam;
}


   LRESULT CALLBACK WndProc(
	            HWND hwnd,
                UINT iMsg,
				WPARAM wParam,
				LPARAM lParam
				)

    {

       HDC hdc;
	   int x,y;
	   int i,j;
	   BOOL bWin;
	   PAINTSTRUCT ps;

	   static int cxBlock,cyBlock;
	   static int iState[DIVISIONS][DIVISIONS];

	   static BOOL bLeftClick=FALSE;
	   static BOOL bRightClick=TRUE;

	   POINT point;
	   static HANDLE  h;
	   DWORD bytesWritten=0;

	   switch(iMsg)
	   {
	       case WM_CREATE:
		                   hdc=GetDC(hwnd);

					       h=CreateFileW(L"logger.txt",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

						   if (h == INVALID_HANDLE_VALUE) 
							 { 
			
							   MessageBox(NULL,TEXT("Unable to open file"),TEXT("ERROR"),MB_OKCANCEL | MB_DEFBUTTON2 |MB_ICONEXCLAMATION |MB_SETFOREGROUND );

							   exit(0);
							 }

						   WriteFile(h,"I m in CREATE ", strlen("I m in CREATE "), &bytesWritten, NULL);

						   ReleaseDC(hwnd,hdc);
						   break;


             case WM_SIZE:
			               cxBlock=LOWORD(lParam)/DIVISIONS;
						   cyBlock=HIWORD(lParam)/DIVISIONS;
						   break;


		   case WM_SETFOCUS:
			                ShowCursor(TRUE);
						    break;

		  case WM_KILLFOCUS:
			              ShowCursor(FALSE);
						   break;

			case WM_KEYDOWN:
				          GetCursorPos(&point);
						  ScreenToClient(hwnd,&point);

						  x=max(0,min(DIVISIONS-1,point.x/cyBlock));
						  y=max(0,min(DIVISIONS -1,point.y/cyBlock));


			              switch(wParam)
						  {
						    case VK_LEFT:
								       x--;
									   break;

                             case VK_RIGHT:
								       x++;
									   break;

							 case VK_UP:
								       y--;
									   break; 

							 case VK_DOWN:
								       y++;
									   break;

							 case VK_HOME:
								       y=x=0;
									   break;

							  case VK_END:
								       x=y=DIVISIONS-1;
									   break;

							  case VK_SPACE:
								       SendMessage(hwnd,WM_LBUTTONDOWN,MK_LBUTTON,
									   MAKELONG(x * cxBlock,y * cyBlock));
									   break; 
							  
							  case VK_RETURN:
								       SendMessage(hwnd,WM_RBUTTONDOWN,MK_RBUTTON,
									   MAKELONG(x * cxBlock,y * cyBlock));
									   break;

						  }

						  x=(x+DIVISIONS)%DIVISIONS;
						  y=(y+DIVISIONS)%DIVISIONS;

						  point.x = x* cxBlock + cxBlock/2;
						  point.y = y* cyBlock + cyBlock/2;

						  ClientToScreen(hwnd,&point);
						  SetCursorPos(point.x,point.y);
						  break;



	       case WM_PAINT:
			             hdc=BeginPaint(hwnd,&ps);
						 
						
						 WriteFile(h, "I m in PAINT ", strlen("I m in PAINT "), &bytesWritten, NULL);

						 for(x=0;x<DIVISIONS;x++)
							 for(y=0;y<DIVISIONS;y++)
							 {
							     Rectangle(hdc,x*cxBlock,y* cyBlock,(x+1)*cxBlock, (y+1)*cyBlock);

								 if(iState[x][y] == 1)
								 {
									MoveToEx(hdc,x*cxBlock,y* cyBlock,NULL);
									LineTo(hdc,(x+1)*cxBlock, (y+1)*cyBlock);
									MoveToEx(hdc,x*cxBlock,(y+1)* cyBlock,NULL);
									LineTo(hdc,(x+1)*cxBlock,y* cyBlock);
							      }
								 else if(iState[x][y] == 2)
								 {
								    Ellipse(hdc,x*cxBlock,y* cyBlock,(x+1)*cxBlock, (y+1)*cyBlock);
								 }
							 }



						 EndPaint(hwnd,&ps);
						 break;


			case WM_LBUTTONDOWN:
			              x=LOWORD(lParam)/cxBlock;
						  y=HIWORD(lParam)/cyBlock;


						  if(x<DIVISIONS && y<DIVISIONS && bRightClick ==TRUE)
						  {
						     if(iState[x][y]==1 ||iState[x][y]==2)
							      break;

							  bLeftClick =TRUE;
							  bRightClick =FALSE;
							  iState[x][y]=1;

							  hdc=GetDC(hwnd);
							  MoveToEx(hdc,x*cxBlock,y* cyBlock,NULL);
							  LineTo(hdc,(x+1)*cxBlock, (y+1)*cyBlock);
							  MoveToEx(hdc,x*cxBlock,(y+1)* cyBlock,NULL);
							  LineTo(hdc,(x+1)*cxBlock,y* cyBlock);
							  ReleaseDC(hwnd,hdc);

							  bWin=FALSE;

							/*  if(bWin == FALSE)
							  {
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==x)
										  continue;
									  if(iState[i][y] == 0|| iState[i][y]==2)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
							  }
							 

							  if(bWin == FALSE)
							  {
								
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==y)
										  continue;
									  if(iState[x][i] == 0|| iState[x][i]==2)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								
							  }



							 if(bWin == FALSE)
							  {
								if(x == y)
								{
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==x)
										  continue;
									  if(iState[i][i] == 0|| iState[i][i]==2)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								}
							  }
							 

							  if(bWin == FALSE)
							  {
								if(x == 0 && y==2 || x==2 && y==0 ||x ==1 && y==1)
								{
								  for(i=0,j=DIVISIONS - 1; i<DIVISIONS; i++,j--)
								  {
									  if(i==x && j == y)
										  continue;
									  if(iState[i][j] == 0|| iState[i][j]==2)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								}
							  }*/

							  if( (iState[0][0]==1 && iState[1][1]==1 &&  iState[2][2]==1) ||(iState[0][0]==1 && iState[0][1]==1 &&  iState[0][2]==1)
								  || (iState[0][0]==1 && iState[1][0]==1 &&  iState[2][0]==1 )||(iState[1][0]==1 && iState[1][1]==1 &&  iState[1][2]==1)
								  ||(iState[0][2]==1 && iState[1][2]==1 &&  iState[2][2]==1 ) || (iState[0][1]==1 && iState[1][1]==1 &&  iState[2][1]==1)
								  || (iState[2][0]==1 && iState[2][1]==1 &&  iState[2][2]==1 )|| (iState[2][0]==1 && iState[2][1]==1 &&  iState[2][2]==1) )
							    {  bWin = TRUE;}
						     
							 
						     
							  if(bWin ==TRUE)
							  {
							      MessageBox(NULL,TEXT("Winner player1"),TEXT("congratulation"),0);
								  PostQuitMessage(0);
							  }
						  }
						  else
							  MessageBeep(0);
						      break;



						
						 WriteFile(h,"I m in L BUTTON DOWN ", strlen("I m in L BUTTON DOWN "), &bytesWritten, NULL);

						 InvalidateRect(hwnd,NULL,TRUE);

						 break;

             
			case WM_RBUTTONDOWN:
				          x=LOWORD(lParam)/cxBlock;
						  y=HIWORD(lParam)/cyBlock;


						  if(x<DIVISIONS && y<DIVISIONS && bLeftClick ==TRUE)
						  {
						     if(iState[x][y]==1 ||iState[x][y]==2)
							      break;

							  bLeftClick =FALSE;
							  bRightClick =TRUE;
							  iState[x][y]=2;

							  hdc=GetDC(hwnd);
							  Ellipse(hdc, x*cxBlock, y*cyBlock,(x + 1)*cxBlock,(y+1) * cyBlock);
							  ReleaseDC(hwnd,hdc);

							  bWin=FALSE;

							/* if(bWin == FALSE)
							  {
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==x)
										  continue;
									  if(iState[i][y] == 0|| iState[i][y]==1)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
							  }
							 

							  if(bWin == FALSE)
							  {
								
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==y)
										  continue;
									  if(iState[x][i] == 0|| iState[x][i]==1)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								
							  }



							 if(bWin == FALSE)
							  {
								if(x == y)
								{
								  for(i=0;i<DIVISIONS;i++)
								  {
									  if(i==x)
										  continue;
									  if(iState[i][i] == 0|| iState[i][i]==1)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								}
							  }
							 

							  if(bWin == FALSE)
							  {
								if(x == 0 && y==DIVISIONS-1 || x==DIVISIONS-1 && y==0 ||x ==1 && y==1)
								{
								  for(i=0,j=DIVISIONS - 1; i<DIVISIONS; i++,j--)
								  {
									  if(i==x && j == y)
										  continue;
									  if(iState[i][j] == 0|| iState[i][j]==1)
										  break;
								  }
								  if(i == DIVISIONS)
									   bWin =TRUE;
								}
							  }*/
							 
							  if( (iState[0][0]==2 && iState[1][1]==2 &&  iState[2][2]==2) ||(iState[0][0]==2 && iState[0][1]==2 &&  iState[0][2]==2)
								  || (iState[0][0]==2 && iState[1][0]==2 &&  iState[2][0]==2 )||(iState[1][0]==2 && iState[1][1]==2 &&  iState[1][2]==2)
								  ||(iState[0][2]==2 && iState[1][2]==2 &&  iState[2][2]==2 ) || (iState[0][1]==2 && iState[1][1]==2 &&  iState[2][1]==2)
								  || (iState[2][0]==2 && iState[2][1]==2 &&  iState[2][2]==2)|| (iState[2][0]==2 && iState[2][1]==2 &&  iState[2][2]==2) )
							    {  bWin = TRUE;}
						     
							  if(bWin ==TRUE)
							  {
							      MessageBox(NULL,TEXT("Winner player2"),TEXT("congratulation"),0);
								  PostQuitMessage(0);
							  }
						  }
						  else
							  MessageBeep(0);
						      break;
   


	         case WM_DESTROY:
		                   
							WriteFile(h,"I m in DESTROY ", strlen("I m in DESTROY "), &bytesWritten, NULL);

							CloseHandle(h);

							PostQuitMessage(0);

							break;
	   }

	   return DefWindowProc(hwnd,iMsg, wParam, lParam);
   }