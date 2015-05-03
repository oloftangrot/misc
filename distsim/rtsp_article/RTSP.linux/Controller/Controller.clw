; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CControllerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Controller.h"

ClassCount=3
Class1=CControllerApp
Class2=CControllerDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_CONTROLLER_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Resource4=IDR_MENU

[CLS:CControllerApp]
Type=0
HeaderFile=Controller.h
ImplementationFile=Controller.cpp
Filter=N
LastObject=CControllerApp

[CLS:CControllerDlg]
Type=0
HeaderFile=ControllerDlg.h
ImplementationFile=ControllerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CControllerDlg

[CLS:CAboutDlg]
Type=0
HeaderFile=ControllerDlg.h
ImplementationFile=ControllerDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_CONTROLLER_DIALOG]
Type=1
Class=CControllerDlg
ControlCount=9
Control1=ID_PING,button,1342242817
Control2=ID_INITIALIZE,button,1342242816
Control3=ID_START,button,1342242816
Control4=ID_HALT,button,1342242816
Control5=IDC_LIST,listbox,1352745856
Control6=IDC_STATIC,static,1342308352
Control7=IDC_FEDERATION_STATUS,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[MNU:IDR_MENU]
Type=1
Class=CControllerDlg
Command1=ID_OPEN
Command2=ID_EXIT
Command3=ID_HELP
Command4=ID_ABOUT
CommandCount=4

