clc;
clear all;

MAXOBS=60;
obsdata=zeros([MAXOBS,5]);
satid=nan([MAXOBS,1]);
X=zeros([1,5]);dX=zeros([5,1]);
satid={};
% 数据文件的路径
% filename = 'D:/data/CUSV20210222_BDS.txt'
filename='./100COBS.txt'; 
outfile='outpos.txt';

fid=fopen(filename,'r');
fop=fopen(outfile,'w+');

disp(filename);
if(fid==-1 || fop==-1)
    disp('can not find or open the file: %')
    'wrong';
else
    % 文件读取
    while ~feof(fid)
        tline=fgetl(fid);
        if(strcmp(tline(1),'#'))
            % gpsweek, gpssecond, number of obs
            obshead=sscanf(tline(2:end),"%f");
            for i=1:obshead(3)
                tline=fgetl(fid);
                satid{i}=tline(1:3); %暂不存储卫星prn
                % X(m)， Y(m)， Z(m)， pseudorange(m)
                obsdata(i,:)=sscanf(tline(4:end),"%f",[1,5]);
            end
        end
        %% 定位解算
        % write your code here
        H=zeros(obshead(3),5);R=zeros(obshead(3),obshead(3));v=zeros(obshead(3),1);
       
        
        %% 数据输出
        outdata=[obshead(1),obshead(2),X(1),X(2),X(3),X(4),X(5)];
        fprintf(fop,'%d %10.3f %14.3f %14.3f %14.3f %9.3f %9.3f\r\n',outdata);
        %% 清空数据，准备读取下一历元
        obsdata = obsdata * 0; %数据清空，读取下一历元
        
    end
    disp('process over');
end
fclose(fid); fclose(fop);

%% compare refernce result with output.txt

