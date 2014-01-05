function darwinop_communication(block)
setup(block);

end

function setup(block)

% Register original number of ports based on settings in Mask Dialog
values = get_param(block.BlockHandle,'MaskValues');

% see mask properties
% values{1} = IP
% values{2} = Port
% values{3} = Protocol
% values{4} = SampleTime
% values{5} = Frame
% values{6} = ReadIndex
% values{7} = WriteIndex

block.NumInputPorts = size(eval(values{7}),1);
block.NumOutputPorts = size(eval(values{6}),1);

% Setup port properties to be inherited or dynamic
block.SetPreCompInpPortInfoToDynamic;
block.SetPreCompOutPortInfoToDynamic;

% Override input port properties
for i = 1:block.NumInputPorts
    block.InputPort(i).DatatypeID  = 0;  % double
    block.InputPort(i).Complexity  = 'Real';
    block.InputPort(i).SamplingMode = 'Sample'; % sample based
    block.InputPort(i).SampleTime = [-1 0];
    block.InputPort(i).DirectFeedthrough = false;
end

% Override output port properties
for i = 1:block.NumOutputPorts
    block.OutputPort(i).DatatypeID  = 0; % double
    block.OutputPort(i).Complexity  = 'Real';
    block.OutputPort(i).SamplingMode = 'Sample'; % sample based
    block.OutputPort(i).SampleTime = [eval(values{4}) 0];
end

block.NumDialogPrms     = 7;
block.DialogPrmsTunable = {'Nontunable','Nontunable','Nontunable','Nontunable','Nontunable','Nontunable','Nontunable'};

% No parallel queries
block.SupportsMultipleExecInstances(false);

% Specify if Accelerator should use TLC or call back into
% M-file
block.SetAccelRunOnTLC(false);

block.RegBlockMethod('CheckParameters',          @CheckPrms);
block.RegBlockMethod('ProcessParameters',        @ProcessPrms);
block.RegBlockMethod('Start',                    @Start);
block.RegBlockMethod('PostPropagationSetup',     @DoPostPropagationSetup);
block.RegBlockMethod('SetInputPortSampleTime',   @SetInputPortSampleTime);
block.RegBlockMethod('SetOutputPortSampleTime',  @SetOutputPortSampleTime);
block.RegBlockMethod('SetInputPortSamplingMode', @SetInputPortSamplingMode);
block.RegBlockMethod('Outputs',                  @Outputs);
block.RegBlockMethod('Terminate',                @Terminate);
block.RegBlockMethod('WriteRTW',                 @WriteRTW);

end

function CheckPrms(block)

%IP = block.DialogPrm(1).Data;
Port  = block.DialogPrm(2).Data;
Protocol  = block.DialogPrm(3).Data;
SampleTime   = block.DialogPrm(4).Data;
Frame   = block.DialogPrm(5).Data;
ReadIndex   = block.DialogPrm(6).Data;
WriteIndex   = block.DialogPrm(7).Data;

% TODO checks
if (Port <= 0) || (Port > 65535)
    error('incorrect port number, it should be in range [1..65535]');
end

if (Protocol ~= 1) && (Protocol ~= 2)
    error('invalid communication protocol');
end

if ~isreal(SampleTime) || ~isfloat(SampleTime) || (length(SampleTime) ~= 1)
    error('invalid sample time, it should be a floating point scalar');
end

if ~isreal(Frame) || ~isfloat(Frame)
    error('invalid frame, it should be a floating point vector');
end

if ~isreal(ReadIndex) || ~isfloat(ReadIndex) || (size(ReadIndex,2) ~= 2)
    error('invalid read index, it should be a floating point matrix');
end

if ~isreal(WriteIndex) || ~isfloat(WriteIndex) || (size(WriteIndex,2) ~= 2)
    error('invalid read index, it should be a floating point matrix');
end

end

function ProcessPrms(block)

%% Update run time parameters
block.AutoUpdateRuntimePrms;

end

function DoPostPropagationSetup(block)

% Register all tunable parameters as runtime parameters.
block.AutoRegRuntimePrms;

% 1 internal state for holding tcp/udp object
block.NumDWorks = 1;
% 1 = tcp/udp object
block.Dwork(1).Usage = 'DState';
block.Dwork(1).UsedAsDiscState = true;
block.Dwork(1).Dimensions = 1;
block.Dwork(1).DatatypeID = 0; % double
block.Dwork(1).Complexity = 'real';
block.Dwork(1).Name = 'NetObj';

end

function Start(block)

if strcmp(get_param(gcs,'RTWCGKeepContext'),'on')
    disp('### Detecting a RTW build');
    % do not start the simulation locally
    % why is this function called when the RTW model is being built?
    return;
else
    disp('### Detecting a normal simulation');
end

IP = block.DialogPrm(1).Data;
Port  = block.DialogPrm(2).Data;
Protocol  = block.DialogPrm(3).Data;
SampleTime   = block.DialogPrm(4).Data;
Frame   = block.DialogPrm(5).Data;
ReadIndex   = block.DialogPrm(6).Data;
%WriteIndex   = block.DialogPrm(7).Data;

InputBufferSize = max(sum(ReadIndex,2)) - 1;

if Protocol == 1
    NetObj = tcpip(IP, Port);
else
    NetObj = udp(IP, Port);
end
NetObj.Timeout = SampleTime;
NetObj.InputBufferSize = InputBufferSize;
NetObj.OutputBufferSize = length(Frame);

try
    try
        fopen(NetObj);
    catch
        StartInstrumentation;
    end
    if ~strcmp(NetObj.status, 'open')
        fopen(NetObj);
    end
catch
    error('network communication error: failed to open the communication');
end

global NetObjects;
if isempty(NetObjects)
    NetObjects = [NetObj]; %#ok
    block.Dwork(1).Data = 1;
else
    for i = 1:length(NetObjects)
        if strcmp(NetObjects(i).Status,'closed')
            NetObjects(i) = NetObj;
            block.Dwork(1).Data = i;
            return;
        end
    end
    NetObjects = [NetObjects NetObj];
    block.Dwork(1).Data = length(NetObjects);
end

end

function StartInstrumentation

  % get current settings from Simulink
  DarwinOPIP = get_string_param('DarwinOPIP');
  DarwinOPPort = get_string_param('DarwinOPPort');
  DarwinOPTimeout = str2double(get_string_param('DarwinOPTimeout'));
  %DarwinOPDisconnect = str2double(get_string_param('DarwinOPDisconnect'));
  DarwinOPUser = get_string_param('DarwinOPUser');
  DarwinOPPassword = get_string_param('DarwinOPPassword');
  %DarwinOPWork = get_string_param('DarwinOPWork');

  % get current script directory
  scriptName = mfilename('fullpath');
  [scriptDirectory,~,~] = fileparts(scriptName);

  % build ssh command line
  sshName = fullfile(scriptDirectory, '..', 'putty', 'plink.exe');
  sshCmd = {sshName, ...
            '-l', ...
            DarwinOPUser, ...
            '-pw', ...
            DarwinOPPassword, ...
            '-P', ...
            DarwinOPPort, ...
            '-ssh', ...
            '-4', ...
            '-noagent', ...
            DarwinOPIP, ...
           };
  % initialize ssh process
  ssh_pb = java.lang.ProcessBuilder(sshCmd);
  ssh_pb.redirectErrorStream(true);

  ssh_proc = [];
  new_line = char(10);

  try
    % start ssh process
    disp('### connecting to darwin');
    ssh_proc = ssh_pb.start();
    ssh_os = java.io.PrintWriter(ssh_proc.getOutputStream());
    ssh_wait_ready(ssh_proc, DarwinOPTimeout);

    % configure remote shell
    disp('### configuring remote shell');
    ssh_os.write(['export PS1=\$', ...
                  new_line]);
    ssh_os.flush();
    ssh_wait_ready(ssh_proc, DarwinOPTimeout); % skip '$' in command
    ssh_wait_ready(ssh_proc, DarwinOPTimeout);

    disp('### kill existing programs');
    ssh_os.write(['kill `lsof -t /dev/ttyUSB0`', ...
                  new_line]);
    ssh_os.flush();
    ssh_wait_ready(ssh_proc, DarwinOPTimeout);

    disp('### launching program');
    ssh_os.write(['/darwin/Linux/project/instrumentation/instrumentation', ...
                  '&', ...
                  new_line]);
    ssh_os.flush();
    ssh_wait_ready(ssh_proc, DarwinOPTimeout);

    disp('### wait initialization time');
    ssh_os.write(['sleep 5', ...
                  new_line]);
    ssh_os.flush();
    ssh_wait_ready(ssh_proc, DarwinOPTimeout);

    % close SSH connection
    disp('### closing connection');
    ssh_os.write(['exit', ...
                  new_line]);
    ssh_os.flush();
    % wait for the plink process to terminate
    ssh_proc.waitFor();
    % print ssh status
    if ssh_proc.exitValue() == 0
      disp('### SSH command success');
    else
      disp('### SSH command failure, return value is %d', ssh_proc.exitValue());
      error('SSH command failure, return value is %d', ssh_proc.exitValue());
    end
  catch me
    if ssh_proc ~= [] %#ok
      ssh_proc.destroy();
    end
    rethrow(me);
  end
end

function val = get_string_param(name)
  val = get_param(gcs,name);
  switch class(val)
    case 'char'
      return;
    case {'single','double'}
      val = sprintf('%d', val);
    otherwise
      disp(sprintf('### unknown setting class %s', class(val))); %#ok
      error('unknown setting class %s', class(val));
  end
end

function ssh_wait_ready(ssh_proc, timeout)
  % get ssh input stream
  ssh_is = ssh_proc.getInputStream();
  % time reference
  tic;
  % check for timeout
  while toc < timeout
    if ssh_is.available()
      c = ssh_is.read();
      if c == 36 % '$'
        return;
      else
        fprintf('%c', c);
      end
    else
      pause(0.01);
    end
  end
  disp('### SSH command timeout');
  error('SSH command timeout');
end

function SetInputPortSamplingMode(block, idx, mode)
block.InputPort(idx).SamplingMode = mode;

for i = 1:block.NumOutputPorts
    block.OutputPort(i).SamplingMode = 'Sample';
end

end

function SetInputPortSampleTime(block, idx, time)

%fprintf('in %d: %d,%d\n',idx,time(1),time(2));
block.InputPort(idx).SampleTime = time;

end

function SetOutputPortSampleTime(block, idx, time)

%fprintf('out %d: %d,%d\n',idx,time(1),time(2));
block.InputPort(idx).SampleTime = time;

end

function Outputs(block)

Frame = block.DialogPrm(5).Data;
ReadIndex = block.DialogPrm(6).Data;
WriteIndex = block.DialogPrm(7).Data;

global NetObjects;
NetObj = NetObjects(block.Dwork(1).Data);

for i=1:size(WriteIndex,1)
    if WriteIndex(i,2) == 1
        val = uint8(block.InputPort(i).Data);
        Frame(WriteIndex(i,1)) = val;
    else
        j = WriteIndex(i,1);
        val = uint16(block.InputPort(i).Data);
        Frame(j) = bitand(val,255);
        Frame(j + 1) = bitand(val,65280) / 256;
    end
end

try
    fwrite(NetObj,uint8(Frame)); %(1:NetObj.InputBufferSize)));
    Data = fread(NetObj);
catch
    error('network communication error: write/read failure');
end

if length(Data) ~= NetObj.InputBufferSize
    error('network communication error: insufficient bytes are returned (%d of %d)',length(Data),NetObj.InputBufferSize);
end

for i=1:size(ReadIndex,1)
    if ReadIndex(i,2) == 1
        block.OutputPort(i).Data = double(Data(ReadIndex(i,1)));
    else
        j = ReadIndex(i,1);
        block.OutputPort(i).Data = double(Data(j)) + 256*double(Data(j + 1));
    end
end

NetObjects(block.Dwork(1).Data) = NetObj;

end

function Terminate(block)
    global NetObjects;
    if block.Dwork(1).Data > 0
        NetObj = NetObjects(block.Dwork(1).Data);
        try
            fclose(NetObj);
        catch
            error('network communication error: failed to close the communication');
        end
        NetObjects(block.Dwork(1).Data) = NetObj;
    end
end

function WriteRTW(block)

    %IP = block.DialogPrm(1).Data;
    %Port  = block.DialogPrm(2).Data;
    %Protocol  = block.DialogPrm(3).Data;
    %SampleTime   = block.DialogPrm(4).Data;
    Frame   = block.DialogPrm(5).Data;
    ReadIndex   = block.DialogPrm(6).Data;
    WriteIndex   = block.DialogPrm(7).Data;

    % first pass to get number of bus operations
    OperationCount = 0;
    i=1;
    while (i<=length(Frame))
        if Frame(i) == 2
            i = i+4;
        else
            i = i+4+Frame(i+3);
        end
        OperationCount = OperationCount + 1;
    end
    OperationKind = zeros(1,OperationCount);
    OperationID = zeros(1,OperationCount);
    OperationAddress = zeros(1,OperationCount);
    OperationSize = zeros(1,OperationCount);
    InputCount = size(WriteIndex,1);
    InputOperationIndex = zeros(1,InputCount);
    InputOperationAddress = zeros(1,InputCount);
    InputOperationSize = zeros(1,InputCount);
    OutputCount = size(ReadIndex,1);
    OutputOperationIndex = zeros(1,OutputCount);
    OutputOperationAddress = zeros(1,OutputCount);
    OutputOperationSize = zeros(1,OutputCount);

    i=1;
    OperationIndex = 1;
    InputIndex = 1;
    OutputIndex = 1;
    ReadLen = 0;
    while (i<=length(Frame))
        Kind = Frame(i);
        OperationKind(OperationIndex) = Kind;
        ID = Frame(i+1);
        OperationID(OperationIndex) = ID;
        Addr = Frame(i+2);
        OperationAddress(OperationIndex) = Addr;
        Len = Frame(i+3);
        OperationSize(OperationIndex) = Len;
        if Kind == 2
            % read operation
            for j=1:size(ReadIndex,1)
                if ((ReadIndex(j,1) - 1)>= ReadLen) && ...
                   ((ReadIndex(j,1) - 1 + ReadIndex(j,2)) <= (ReadLen + Len))
                    OutputOperationIndex(OutputIndex) = OperationIndex-1;
                    OutputOperationAddress(OutputIndex) = Addr + ReadIndex(j,1) - 1 - ReadLen;
                    OutputOperationSize(OutputIndex) = ReadIndex(j,2);
                    OutputIndex = OutputIndex+1;
                end
            end
            ReadLen = ReadLen + Len;
            % skip the header bytes
            i = i+4;
        elseif Kind == 3
            % write operation
            for j=1:size(WriteIndex,1)
                if (WriteIndex(j,1) >= (i+4)) && ...
                   ((WriteIndex(j,1) + WriteIndex(j,2)) <= (i+4+Len))
                    InputOperationIndex(InputIndex) = OperationIndex-1;
                    InputOperationAddress(InputIndex) = Addr + WriteIndex(j,1) - (i+4);
                    InputOperationSize(InputIndex) = WriteIndex(j,2);
                    InputIndex = InputIndex+1;
                end
            end
            % skip the header and data bytes
            i = i+4+Len;
        elseif Kind == 17
            % internal read operation
            % TODO translation in C++
            error('embedded code generation for vision is not supported yet');
            % skip the header bytes
            %i = i+4;
        elseif Kind == 18
            % internal write operation
            % TODO translation in C++
            error('embedded code generation for vision is not supported yet');
            % skip the header and data bytes
            %i = i+4+Len;
        end
        OperationIndex = OperationIndex+1;
    end

    block.WriteRTWParam('matrix', 'OperationCount', int32(length(OperationKind)));
    block.WriteRTWParam('matrix', 'OperationKind', int32(OperationKind));
    block.WriteRTWParam('matrix', 'OperationID', int32(OperationID));
    block.WriteRTWParam('matrix', 'OperationAddress', int32(OperationAddress));
    block.WriteRTWParam('matrix', 'OperationSize', int32(OperationSize));

    block.WriteRTWParam('matrix', 'InputCount', int32(length(InputOperationIndex)));
    block.WriteRTWParam('matrix', 'InputOperationIndex', int32(InputOperationIndex));
    block.WriteRTWParam('matrix', 'InputOperationAddress', int32(InputOperationAddress));
    block.WriteRTWParam('matrix', 'InputOperationSize', int32(InputOperationSize));

    block.WriteRTWParam('matrix', 'OutputCount', int32(length(OutputOperationIndex)));
    block.WriteRTWParam('matrix', 'OutputOperationIndex', int32(OutputOperationIndex));
    block.WriteRTWParam('matrix', 'OutputOperationAddress', int32(OutputOperationAddress));
    block.WriteRTWParam('matrix', 'OutputOperationSize', int32(OutputOperationSize));

end
