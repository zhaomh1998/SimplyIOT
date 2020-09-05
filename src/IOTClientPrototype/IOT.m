function varargout = IOT(varargin)
% IOT MATLAB code for IOT.fig
%      IOT, by itself, creates a new IOT or raises the existing
%      singleton*.
%
%      H = IOT returns the handle to a new IOT or the handle to
%      the existing singleton*.
%
%      IOT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in IOT.M with the given input arguments.
%
%      IOT('Property','Value',...) creates a new IOT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before IOT_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to IOT_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help IOT

% Last Modified by GUIDE v2.5 20-Aug-2018 22:39:10

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @IOT_OpeningFcn, ...
                   'gui_OutputFcn',  @IOT_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

%% GUI Configurations
function IOT_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to IOT (see VARARGIN)

% Choose default command line output for IOT
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes IOT wait for user response (see UIRESUME)
% uiwait(handles.figure1);
global isConnected logstr rxstr closeReq closeAccept
isConected = 0;
logstr = "LOG";
rxstr = "RX";
closeReq = 0;
closeAccept = 1;
function figure1_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global closeReq closeAccept
closeReq = 1;
while(~closeAccept)
    pause(0.1)
end
% Hint: delete(hObject) closes the figure
delete(hObject);
function varargout = IOT_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



%% Callbacks
function PB_Connect_Callback(hObject, eventdata, handles)
% hObject    handle to PB_Connect (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global esp closeReq closeAccept
set(hObject, 'Enable', 'Off');
esp = tcpip('192.168.4.1', 23, 'NetworkRole', 'client');
esp.Timeout = 1;
try
    logPrintln('TCPIP Connecting', handles);
    pause(0.5)
    fopen(esp);
    logPrintln('ESP Connection Established.', handles);
catch ME
    disp(ME.identifier);
    if(strcmp(esp.status, 'closed'))
    logPrintln('TCPIP Connection to ESP Failed', handles);
    set(hObject, 'Enable', 'On');
    return;
    end
end

while(~strcmp(esp.Status, 'closed'))
    if(closeReq)
        fclose(esp);
        delete(esp);
        clear esp;
        closeAccept = 1;
        return;
    end
    pause(0.1)
    if(esp.BytesAvailable)
        incomingMsg = fgetl(esp);
        rxPrintln(incomingMsg, handles);
        if(incomingMsg(1) == 'c')
            updateCfg(incomingMsg(2:end), handles);
        end
    end
end
set(hObject, 'Enable', 'On');
logPrintln('ESP disconnected', handles);
pause(1)

function CFG_Mode_Callback(hObject, eventdata, handles)
modeCfg = get(hObject,'Value')

function ED_Name_Callback(hObject, eventdata, handles)
nameCfg = get(hObject,'String')

function ED_Pass_Callback(hObject, eventdata, handles)
passCfg = get(hObject,'String')

function ED_Ver_Callback(hObject, eventdata, handles)
verCfg = get(hObject,'String')

function ED_Port_Callback(hObject, eventdata, handles)
portCfg = get(hObject, 'String')

function CK_Newline_Callback(hObject, eventdata, handles)
global isSendNewline 
isSendNewline = get(hObject,'Value')

function ED_Send_Callback(hObject, eventdata, handles)
PB_Send_Callback(handles.PB_Send, eventdata, handles);

function PB_Send_Callback(hObject, eventdata, handles)
global isSendNewline
sendStr = get(handles.ED_Send, 'String')
if(isSendNewline)
else
end




%% Helper Fcns
function logPrintln(newstr, handles)
global logstr
c = clock;
logstr = sprintf('%s\n%s:%s:%s\t\t\t\t%s', logstr,num2str(c(4)),num2str(c(5)),num2str(c(6)),newstr);
set(handles.TX_Log, 'String', logstr);

function rxPrintln(newstr, handles)
global rxstr
c = clock;
rxstr = sprintf('%s\n%s:%s:%s\t\t\t\t%s', rxstr,num2str(c(4)),num2str(c(5)),num2str(c(6)),newstr);
set(handles.TX_RX, 'String', rxstr);

function updateCfg(cfgMsg, handles)
if(cfgMsg(1) == 'M')
    
elseif(cfgMsg(1) == 'N')
    set(handles.ED_Name, 'String', cfgMsg(2:end));
elseif(cfgMsg(1) == 'S')
    set(handles.ED_Pass, 'String', cfgMsg(2:end));
elseif(cfgMsg(1) == 'P')
    set(handles.ED_Port, 'String', cfgMsg(2:end));
elseif(cfgMsg(1) == 'V')
    if(strcmp(cfgMsg(2:end), 'AP'))
        set(handles.CFG_Mode, 'Value', 1);
    elseif(strcmp(cfgMsg(2:end), 'WiFi'))
        set(handles.CFG_Mode, 'Value', 2);
    elseif(strcmp(cfgMsg(2:end), 'BLE'))
        set(handles.CFG_Mode, 'Value', 3);
    end
else
    disp('Following Cfg parse error!');
    cfgMsg
end




%% Unused CreateFcns
function ED_Port_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ED_Port (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function ED_Send_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ED_Send (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function ED_Ver_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ED_Ver (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function ED_Pass_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ED_Pass (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function ED_Name_CreateFcn(hObject, eventdata, handles)
% hObject    handle to ED_Name (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
function CFG_Mode_CreateFcn(hObject, eventdata, handles)
% hObject    handle to CFG_Mode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
