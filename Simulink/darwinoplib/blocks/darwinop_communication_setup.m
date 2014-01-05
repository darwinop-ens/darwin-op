function varargout = darwinop_communication_setup(varargin)

[varargout{1:nargout}] = feval(varargin{:});

end

function OpenFcn %#ok
% the block is double-clicked in simulink window

hBlk = gcbh;
% find existing figures, if any
f = FindFigure(hBlk);
if isempty(f)
    % figure does not exist yet, create a new one
    CreateFigure(hBlk);
else
    % this figure is already for this block, bring it to front,
    % eventually reflecting changes in block name
    SetFigureName(f);
    figure(f);
end

end

function DeleteFcn %#ok
% the block is deleted
hBlk = gcbh;
% find existing figures, if any
f = FindFigure(hBlk);
if ~isempty(f)
    close(f);
end

end

function NameChangeFcn %#ok
% the block name is changed

hBlk = gcbh;
% find existing figures, if any
f = FindFigure(hBlk);
if ~isempty(f)
    SetFigureName(f);
end

end

function StartFcn %#ok
% the simulation is started
end

function StopFcn %#ok
% the simulation is stopped
end

function CopyFcn %#ok
% the block is copied
% all data copies are automaticaly done
end

function ModelCloseFcn %#ok
% the model is closed

hBlk = gcbh;
% find existing figures, if any
f = FindFigure(hBlk);
if ~isempty(f)
    close(f);
end

end

function f = FindFigure(hBlk)

figures = findall(0,'Tag','darwinop_communication_setup');
for i = 1:length(figures)
    data = guidata(figures(i));
    if isfield(data,'simulink_block') && (data.simulink_block == hBlk)
        f = figures(i);
        return;
    end
end
f=[];

end

function CreateFigure(hBlk)

f = figure('MenuBar','none', ...
           'Toolbar','none', ...
           'Resize','off', ...
           'NumberTitle','off', ...
           'Units','pixels', ...
           'Position',[100,300,800,485], ...
           'Tag','darwinop_communication_setup');

data = guidata(f);

data.simulink_block = hBlk;

[data.mx28_fields,data.cm730_fields,data.fsr_fields,data.vision_fields,data.ids,data.labels,~] = LoadConsts;

data.read_text = uicontrol(f,'Style','text', ...
                             'String',data.labels{2}, ...
                             'HorizontalAlignment','left', ...
                             'Units','pixels', ...
                             'Position',[520,215,270,14]);
data.read_listbox = uicontrol(f,'Style','listbox', ...
                                ...'String',read_item_list, ...
                                ...'Max',length(read_item_list), ...
                                'BackgroundColor','white', ...
                                'Units','pixels', ...
                                'Position',[520,10,270,205]);

data.write_text = uicontrol(f,'Style','text', ...
                              'String',data.labels{3}, ...
                              'HorizontalAlignment','left', ...
                              'Units','pixels', ...
                              'Position',[520,435,270,14]);
data.write_listbox = uicontrol(f,'Style','listbox', ...
                                 ...'String',write_item_list, ...
                                 ...'Max',length(write_item_list), ...
                                 'BackgroundColor','white', ...
                                 'Units','pixels', ...
                                 'Position',[520,230,270,205]);

data.background_color = get(data.read_text,'BackgroundColor');
data.write_color = 'red';
data.read_color = 'green';
data.read_write_color = 'yellow';
data.selected_color = 'cyan';

set(f,'Color',data.background_color);

data.axes = axes('units','pixels', ...
                 'position',[10,10,483,424]);
data.image = imread('darwin.jpg');
imshow(data.image,'Parent',data.axes);

data.id_buttons = zeros(size(data.ids));
data.selected_read_fields = cell(size(data.ids));
data.selected_write_fields = cell(size(data.ids));

for i=1:length(data.ids)
    if data.ids{i}.address == 200
        data.selected_read_fields{i} = zeros(size(data.cm730_fields));
        data.selected_write_fields{i} = zeros(size(data.cm730_fields));
    elseif (data.ids{i}.address == 111) || (data.ids{i}.address == 112)
        data.selected_read_fields{i} = zeros(size(data.fsr_fields));
        data.selected_write_fields{i} = zeros(size(data.fsr_fields));
    elseif data.ids{i}.address == -1
        data.selected_read_fields{i} = zeros(size(data.vision_fields));
        data.selected_write_fields{i} = zeros(size(data.vision_fields));
    else
        data.selected_read_fields{i} = zeros(size(data.mx28_fields));
        data.selected_write_fields{i} = zeros(size(data.mx28_fields));
    end
    if data.ids{i}.address == -1
        width = 40;
        text = 'vision';
    else
        width = 20;
        text = num2str(data.ids{i}.address);
    end
    data.id_buttons(i) = uicontrol(f,'Style','pushbutton', ...
                                     'String',text, ...
                                     'BackgroundColor',data.background_color, ...
                                     'Units','pixels', ...
                                     'Position',[data.ids{i}.posx+8,data.ids{i}.posy-10,width,20], ...
                                     'Callback',{@callback_button_id,i});
end

data.selected_button = uicontrol(f,'Style','pushbutton', ...
                                   'String','', ...
                                   'Enable','off', ...
                                   'BackgroundColor',data.selected_color, ...
                                   'Units','pixels', ...
                                   'Position',[20,60,20,20]);
data.selected_text = uicontrol(f,'Style','text', ...
                                 'String',data.labels{4}, ...
                                 'HorizontalAlignment','left', ...
                                 'BackgroundColor','white', ...
                                 'Units','pixels', ...
                                 'Position',[50,63,80,14]);
data.read_write_button = uicontrol(f,'Style','pushbutton', ...
                                     'String','', ...
                                     'Enable','off', ...
                                     'BackgroundColor',data.read_write_color, ...
                                     'Units','pixels', ...
                                     'Position',[20,90,20,20]);
data.read_write_text = uicontrol(f,'Style','text', ...
                                   'String',data.labels{5}, ...
                                   'HorizontalAlignment','left', ...
                                   'BackgroundColor','white', ...
                                   'Units','pixels', ...
                                   'Position',[50,93,80,14]);
data.write_button = uicontrol(f,'Style','pushbutton', ...
                                'String','', ...
                                'Enable','off', ...
                                'BackgroundColor',data.write_color, ...
                                'Units','pixels', ...
                                'Position',[20,120,20,20]);
data.write_text = uicontrol(f,'Style','text', ...
                              'String',data.labels{6}, ...
                              'HorizontalAlignment','left', ...
                              'BackgroundColor','white', ...
                              'Units','pixels', ...
                              'Position',[50,123,80,14]);
data.read_button = uicontrol(f,'Style','pushbutton', ...
                               'String','', ...
                               'Enable','off', ...
                               'BackgroundColor',data.read_color, ...
                               'Units','pixels', ...
                               'Position',[20,150,20,20]);
data.read_text = uicontrol(f,'Style','text', ...
                             'String',data.labels{7}, ...
                             'HorizontalAlignment','left', ...
                             'BackgroundColor','white', ...
                             'Units','pixels', ...
                             'Position',[50,153,80,14]);
data.noop_button = uicontrol(f,'Style','pushbutton', ...
                               'String','', ...
                               'Enable','off', ...
                               'BackgroundColor',data.background_color, ...
                               'Units','pixels', ...
                               'Position',[20,180,20,20]);
data.noop_text = uicontrol(f,'Style','text', ...
                             'String',data.labels{8}, ...
                             'HorizontalAlignment','left', ...
                             'BackgroundColor','white', ...
                             'Units','pixels', ...
                             'Position',[50,183,80,14]);

data.ip_text = uicontrol(f,'Style','text', ...
                           'String',data.labels{9}, ...
                           'HorizontalAlignment','left', ...
                           'Units','pixels', ...
                           'Position',[10,459,60,14]);
data.ip_edit = uicontrol(f,'Style','edit', ...
                           'BackgroundColor','white', ...
                           'HorizontalAlignment','left', ...
                           'Units','pixels', ...
                           'Position',[72,455,100,22]);

data.port_text = uicontrol(f,'Style','text', ...
                             'String',data.labels{10}, ...
                             'HorizontalAlignment','left', ...
                             'Units','pixels', ...
                             'Position',[181,459,30,14]);
data.port_edit = uicontrol(f,'Style','edit', ...
                             'BackgroundColor','white', ...
                             'HorizontalAlignment','left', ...
                             'Units','pixels', ...
                             'Position',[210,455,40,22]);

data.protocol_text = uicontrol(f,'Style','text', ...
                                 'String',data.labels{11}, ...
                                 'HorizontalAlignment','left', ...
                                 'Units','pixels', ...
                                 'Position',[256,459,55,14]);
data.protocol_popup = uicontrol(f,'Style','popupmenu', ...
                                  'String',{'tcp','udp'}, ...
                                  'BackgroundColor','white', ...
                                  'HorizontalAlignment','left', ...
                                  'Units','pixels', ...
                                  'Position',[310,463,50,14]);

data.sampletime_text = uicontrol(f,'Style','text', ...
                                   'String',data.labels{12}, ...
                                   'HorizontalAlignment','left', ...
                                   'Units','pixels', ...
                                   'Position',[366,459,90,14]);
data.sampletime_edit = uicontrol(f,'Style','edit', ...
                                   'BackgroundColor','white', ...
                                   'HorizontalAlignment','left', ...
                                   'Units','pixels', ...
                                   'Position',[453,455,40,22]);

data.advanced_checkbox = uicontrol(f,'Style','checkbox', ...
                                     'String',data.labels{13}, ...
                                     'Value',0, ...
                                     'Units','pixels', ...
                                     'Position',[520,459,160,14], ...
                                     'Callback',@callback_checkbox_advanced);

data.close_button = uicontrol(f,'Style','pushbutton', ...
                                'String',data.labels{14}, ...
                                'Units','pixels', ...
                                'Position',[692,455,100,25], ...
                                'Callback',@callback_save_and_close);

guidata(f,data);

LoadFigure(f);

callback_button_id(data.id_buttons(1),[],1);

SetFigureName(f);

end

function [mx28_fields,cm730_fields,fsr_fields,vision_fields,ids,labels,lang] = LoadConsts
    try
        if ispref('darwinoplib','consts')
            [mx28_fields,cm730_fields,fsr_fields,vision_fields,ids,labels,lang] = eval(getpref('darwinoplib','consts'));
        else
            [mx28_fields,cm730_fields,fsr_fields,vision_fields,ids,labels,lang] = darwinop_consts;
        end
    catch
        [mx28_fields,cm730_fields,fsr_fields,vision_fields,ids,labels,lang] = darwinop_consts;
    end 
end

function LoadFigure(f)

data = guidata(f);
hBlk = data.simulink_block;

values = get_param(hBlk,'MaskValues');
% see mask properties
% values{1} = IP
% values{2} = Port
% values{3} = Protocol
% values{4} = SampleTime
% values{5} = Frame
% values{6} = ReadIndex
% values{7} = WriteIndex

if isempty(values{1})
    values{1} = '192.168.123.1';
end
if isempty(values{2})
    values{2} = '1234';
end
if isempty(values{3})
    values{3} = 'tcp';
end
if isempty(values{4})
    values{4} = '0.5';
end

set(data.ip_edit,'String',values{1});
set(data.port_edit,'String',values{2});
if strcmp(values{3},'tcp')
    set(data.protocol_popup,'Value',1);
else
    set(data.protocol_popup,'Value',2);
end
set(data.sampletime_edit,'String',values{4});

block_user_data = get_param(hBlk,'UserData');
if ~isempty(block_user_data)
    data.selected_read_fields = block_user_data.selected_read_fields;
    data.selected_write_fields = block_user_data.selected_write_fields;
    % perform update if new ids or new fields are available
    for i=1:length(data.ids)
        if data.ids{i}.address == 200
            ref = zeros(size(data.cm730_fields));
        elseif (data.ids{i}.address == 111) || (data.ids{i}.address == 112)
            ref = zeros(size(data.fsr_fields));
        elseif data.ids{i}.address == -1
            ref = zeros(size(data.vision_fields));
        else
            ref = zeros(size(data.mx28_fields));
        end
        if i <= length(data.selected_read_fields)
            selected_read_fields = data.selected_read_fields{i};
            for j=(length(selected_read_fields)+1):length(ref)
                selected_read_fields(j) = 0;
            end
        else
            selected_read_fields = ref;
        end
        data.selected_read_fields{i} = selected_read_fields;
        if i <= length(data.selected_write_fields)
            selected_write_fields = data.selected_write_fields{i};
            for j=(length(selected_write_fields)+1):length(ref)
                selected_write_fields(j) = 0;
            end
        else
            selected_write_fields = ref;
        end
        data.selected_write_fields{i} = selected_write_fields;
    end
end

for i = 1:length(data.id_buttons)
    read_value = find(data.selected_read_fields{i},1);
    write_value = find(data.selected_write_fields{i},1);
    if isempty(read_value)
        if isempty(write_value)
            set(data.id_buttons(i),'BackgroundColor',data.background_color);
        else
            set(data.id_buttons(i),'BackgroundColor',data.write_color);
        end
    else
        if isempty(write_value)
            set(data.id_buttons(i),'BackgroundColor',data.read_color);
        else
            set(data.id_buttons(i),'BackgroundColor',data.read_write_color);
        end
    end
end

guidata(f,data);

end

function SaveFigure(f)

refresh_listbox;

data = guidata(f);
hBlk = data.simulink_block;
hModel = bdroot(hBlk);
if strcmp(get_param(hModel,'lock'),'on') == 0
    % flush current changes

    protocols = get(data.protocol_popup,'String');
    value_ip = get(data.ip_edit,'String');
    value_port = get(data.port_edit,'String');
    value_protocol = protocols{get(data.protocol_popup,'Value')};
    value_sampletime = get(data.sampletime_edit,'String');

    value_frame = '[ ';
    value_write_index = '[ ';
    value_read_index = '[ ';

    input_index = 1;
    output_index = 1;
    read_position = 1;
    write_position = 1;

    read_mask_display = {''};
    write_mask_display = {''};

    for i = 1:length(data.ids)
        if data.ids{i}.address == 200
            fields = data.cm730_fields;
        elseif (data.ids{i}.address == 111) || (data.ids{i}.address == 112)
            fields = data.fsr_fields;
        elseif data.ids{i}.address == -1
            fields = data.vision_fields;
        else
            fields = data.mx28_fields;
        end
        selected_read_fields = data.selected_read_fields{i};
        read_fields = zeros(1,256);
        for j = 1:length(selected_read_fields)
            field = fields{j};
            if selected_read_fields(j)
                read_fields(field.address+1:field.address+field.size) = ones(1,field.size);
                read_mask_display = [read_mask_display; ...
                                     'port_label(''output'',', ...
                                     num2str(output_index), ...
                                     ',''', ...
                                     data.ids{i}.name, ...
                                     '(', ...
                                     num2str(abs(data.ids{i}.address)), ...
                                     ')/', ...
                                     field.name, ...
                                     ''')' ...
                                    ]; %#ok
                output_index = output_index + 1;
            end
        end
        selected_write_fields = data.selected_write_fields{i};
        write_fields = zeros(1,256);
        for j = 1:length(selected_write_fields)
            field = fields{j};
            if selected_write_fields(j)
                write_fields(field.address+1:field.address+field.size) = ones(1,field.size);
                write_mask_display = [write_mask_display; ...
                                      'port_label(''input'',', ...
                                      num2str(input_index), ...
                                      ',''', ...
                                      data.ids{i}.name, ...
                                      '(', ...
                                      num2str(abs(data.ids{i}.address)), ...
                                      ')/', ...
                                      fields{j}.name, ...
                                      ''')' ...
                  ]; %#ok
                input_index = input_index + 1;
            end
        end
        
        read_ranges = get_ranges(read_fields) - 1;
        if data.ids{i}.address >= 0
            instruction = 2;
        else
            instruction = 17;
        end
        for j = 1:size(read_ranges,1)
            value_frame = [value_frame, ...
                           num2str(instruction),' ', ...
                           num2str(abs(data.ids{i}.address)),' ', ...
                           num2str(read_ranges(j,1)),' ', ...
                           num2str(read_ranges(j,2)-read_ranges(j,1)+1),' ' ...
                          ]; %#ok
            write_position = write_position + 4;
            for k = 1:length(selected_read_fields)
                field = fields{k};
                if (selected_read_fields(k) == 1) && (field.address >= read_ranges(j,1)) && ((field.address+field.size-1) <= read_ranges(j,2))
                    value_read_index = [value_read_index, ...
                                        num2str(read_position + field.address - read_ranges(j,1)),' ', ...
                                        num2str(field.size),';' ...
                                       ]; %#ok
                end
            end
            read_position = read_position + (read_ranges(j,2) - read_ranges(j,1) + 1);
        end
        
        write_ranges = get_ranges(write_fields) - 1;
        if data.ids{i}.address >= 0
            instruction = 3;
        else
            instruction = 18;
        end
        for j = 1:size(write_ranges,1)
            value_frame = [value_frame, ...
                           num2str(instruction),' ', ...
                           num2str(abs(data.ids{i}.address)),' ', ...
                           num2str(write_ranges(j,1)),' ', ...
                           num2str(write_ranges(j,2)-write_ranges(j,1)+1),' ' ...
                          ]; %#ok
            write_position = write_position + 4;
            for k = 1:length(selected_write_fields)
                field = fields{k};
                if (selected_write_fields(k) == 1) && (field.address >= write_ranges(j,1)) && ((field.address+field.size-1) <= write_ranges(j,2))
                    value_write_index = [value_write_index, ...
                                         num2str(write_position + field.address - write_ranges(j,1)),' ', ...
                                         num2str(field.size),';' ...
                                        ]; %#ok
                end
            end
            for k = write_ranges(j,1):write_ranges(j,2)
                value_frame = [value_frame,'0 ']; %#ok
            end
            write_position = write_position + (write_ranges(j,2) - write_ranges(j,1) + 1);
        end
    end
    value_frame(end) = ']';
    value_write_index(end) = ']';
    value_read_index(end) = ']';

    % see mask properties
    % values{1} = IP
    % values{2} = Port
    % values{3} = Protocol
    % values{4} = SampleTime
    % values{5} = Frame
    % values{6} = ReadIndex
    % values{7} = WriteIndex
    values = {
        value_ip, ...
        value_port, ...
        value_protocol, ...
        value_sampletime, ...
        value_frame, ...
        value_read_index, ...
        value_write_index ...
      };

    % break library link not to modify the library block (which leads to error in most cases)
    set_param(hBlk,'LinkStatus','none');

    % disable inputs/outputs draw commands to avoid warnings when the
    % number of inputs/ouputs is modified by the next instruction
    set_param(hBlk,'MaskDisplay','image(''darwin-blk.jpg'')');

    set_param(hBlk,'MaskValues',values);

    set_param(hBlk,'Mask','on');
    set_param(hBlk,'MaskSelfModifiable','on');
    mask_display = [{'image(''darwin-blk.jpg'')'}; read_mask_display; write_mask_display];
    set_param(hBlk,'MaskDisplay',char(mask_display));
    block_user_data.selected_read_fields = data.selected_read_fields;
    block_user_data.selected_write_fields = data.selected_write_fields;
    set_param(hBlk,'UserData',block_user_data);
    set_param(hBlk,'UserDataPersistent', 'on');
end

end

function SetFigureName(f)

data = guidata(f);
set(f,'Name',sprintf(data.labels{1}, getfullname(data.simulink_block)));

end

function r = get_ranges(l)

last_id = 0;
j = 1;
r = [];
for i = 1:length(l)
    if l(i) == 1
        if last_id == 0
            last_id = i;
        end
    elseif last_id ~= 0
        r(j,:) = [last_id,i-1]; %#ok
        last_id = 0;
        j = j + 1;
    end
end

end

function [read_field_list,read_item_list,write_field_list,write_item_list] = filter_fields(fields,advanced)
    prec_address = -2;
    read_item_list = cell(0);
    read_field_list = cell(0);
    write_item_list = cell(0);
    write_field_list = cell(0);
    read_index = 1;
    write_index = 1;
    for field_index = 1:length(fields)
        field = fields{field_index};
        if (advanced) || isempty(find(field.mode == 'A',1))
            read = ~isempty(find(field.mode == 'R',1));
            write = ~isempty(find(field.mode == 'W',1));
            if (prec_address == field.address) || (prec_address == field.address - 1)
                description = ['  ' field.description];
            else
                description = field.description;
                if field.size == 2
                    prec_address = field.address;
                else
                    prec_address = -2;
                end
            end
            if read
                read_field_list{read_index} = field_index;
                read_item_list{read_index} = description;
                read_index = read_index + 1;
            end
            if write
                write_field_list{write_index} = field_index;
                write_item_list{write_index} = description;
                write_index = write_index + 1;
            end
        end
    end
end

function refresh_listbox
    data = guidata(gcf);
    if isfield(data,'old_index')
        read_value = get(data.read_listbox,'Value');
        selected_read_fields = zeros(size(data.selected_read_fields{data.old_index}));
        for i = read_value
            selected_read_fields(data.read_field_list{i}) = 1;
        end
        data.selected_read_fields{data.old_index} = selected_read_fields;

        write_value = get(data.write_listbox,'Value');
        selected_write_fields = zeros(size(data.selected_write_fields{data.old_index}));
        for i = write_value
            selected_write_fields(data.write_field_list{i}) = 1;
        end
        data.selected_write_fields{data.old_index} = selected_write_fields;

        if isempty(read_value)
            if isempty(write_value)
                set(data.id_buttons(data.old_index),'BackgroundColor',data.background_color);
            else
                set(data.id_buttons(data.old_index),'BackgroundColor',data.write_color);
            end
        else
            if isempty(write_value)
                set(data.id_buttons(data.old_index),'BackgroundColor',data.read_color);
            else
                set(data.id_buttons(data.old_index),'BackgroundColor',data.read_write_color);
            end
        end
    end
    
    if data.ids{data.current_index}.address == 200
        fields = data.cm730_fields;
    elseif (data.ids{data.current_index}.address == 111) || (data.ids{data.current_index}.address == 112)
        fields = data.fsr_fields;
    elseif data.ids{data.current_index}.address == -1
        fields = data.vision_fields;
    else
        fields = data.mx28_fields;
    end
    advanced = get(data.advanced_checkbox, 'Value');
    [data.read_field_list,read_item_list,data.write_field_list,write_item_list] = filter_fields(fields,advanced);

    read_value = zeros(size(data.read_field_list));
    selected_read_field = data.selected_read_fields{data.current_index};
    for i = 1:length(data.read_field_list)
        if selected_read_field(data.read_field_list{i}) == 1
            read_value(i) = 1;
        end
    end
    set(data.read_listbox,'String',read_item_list, ...
                          'Max',length(read_item_list), ...
                          'Value',find(read_value));
    write_value = zeros(size(data.write_field_list));
    selected_write_field = data.selected_write_fields{data.current_index};
    for i = 1:length(data.write_field_list)
        if selected_write_field(data.write_field_list{i}) == 1
            write_value(i) = 1;
        end
    end
    set(data.write_listbox,'String',write_item_list, ...
                           'Max',length(write_item_list), ...
                           'Value',find(write_value));
    set(data.id_buttons(data.current_index),'BackgroundColor',data.selected_color);
    data.old_index = data.current_index;
    
    guidata(gcf,data);
end

function callback_button_id(hObj, eventdata, index) %#ok

    data = guidata(gcf);
    data.current_index = index;
    guidata(gcf,data);
    refresh_listbox;

end

function callback_checkbox_advanced(hObj, eventdata) %#ok

    refresh_listbox;

end

function callback_save_and_close(hObj, eventdata) %#ok

    f = gcf;
    SaveFigure(f);
    close(f);

end
