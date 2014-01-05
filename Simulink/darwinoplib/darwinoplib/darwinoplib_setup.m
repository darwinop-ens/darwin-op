function darwinoplib_setup()

scriptname = mfilename('fullpath');
[curpath,~,~] = fileparts(scriptname);
tgtpath = curpath(1:end-length('\darwinoplib'));
addpath(tgtpath);
addpath(fullfile(tgtpath, 'darwinoplib'));
addpath(fullfile(tgtpath, 'blocks'));
addpath(fullfile(tgtpath, 'darwinoplibdemos'));
addpath(fullfile(tgtpath, 'putty'));

savepath;

disp('DarwinOP Target Path Setup Complete.');

blockspath = fullfile(tgtpath, 'blocks');

listing = dir(blockspath);
ref = 'darwinop_consts';
language_files = cell(0);
languages = cell(0);
i=1;
for f=1:length(listing)
    filename = listing(f).name;
    if strncmp(filename, ref, length(ref)) && (filename(end-1) == '.') && ...
       ((filename(end) == 'M') || (filename(end) == 'm'))
        language_files{i} = filename(1:end-2);
        [~,~,~,~,~,~,language] = eval(filename(1:end-2));
        languages{i} = language;
        i = i+1;
    end
end

if isempty(languages)
    error('failed to detect languages');
end

fprintf('Select library language:\n');
for i=1:length(languages)
    fprintf('  [%d] %s\n', i, languages{i});
end

while true
    i = input(sprintf('1-%d:',length(languages)), 's');
    try
        i = eval(i);
        if (i >= 1) && (i <= length(languages)) && (abs(round(i)-i) < 16*eps)
            fprintf('Selected "%s"\n',languages{i});
            if ispref('darwinoplib','language')
                setpref('darwinoplib','language',languages{i});
            else
                addpref('darwinoplib','language',languages{i});
            end
            if ispref('darwinoplib','consts')
                setpref('darwinoplib','consts',language_files{i});
            else
                addpref('darwinoplib','consts',language_files{i});
            end
            fprintf('Done.\n');
            return;
        end
    catch
    end
end
