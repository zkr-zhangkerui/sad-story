clc;
clear all;

MAXOBS=60;
obsdata=zeros([MAXOBS,5]);
satid=nan([MAXOBS,1]);
X1=zeros([1,4]);
numX=0;numY=0;numZ=0;numT=0;
sumX=0;sumY=0;sumZ=0;sumT=0;

% 数据文件的路径
filename='CUSV_20212220_BDS_M0.5_I1.0_G2.0.txt'; 
outfile='outpos.txt';

fid=fopen(filename,'r');
fop=fopen(outfile,'w+');

disp(filename);
if(fid==-1 || fop==-1)
    disp('can not find or open the file: %')
    'wrong';
else
    % 文件读取
    num=0;%历元数
    while ~feof(fid)
        tline=fgetl(fid);%读取每一行数据
        if(strcmp(tline(1),'#'))%每一行的第一个是否是#，判断是否是一个新历元
            %epoch, gpsweek, gpssecond, number of obs
            num=num+1;
            obshead=sscanf(tline(2:end),"%f");%该历元的GPST，历元数和观测值数量
            for i=1:obshead(4)
                tline=fgetl(fid);
                satid=tline(1:3); %卫星prn
                % X(m)， Y(m)， Z(m)， pseudorange(m)
                obsdata(i,:)=sscanf(tline(4:end),"%f",[1,5]);%x，y，z坐标，观测值和方差
            end
        end
        %% 定位解算
        % write your code here
        %初始化观测模型和随机模型
        pseudorange=zeros([obshead(4),1]);
        Z=zeros([obshead(4),1]);
        x=zeros([obshead(4),1]);
        y=zeros([obshead(4),1]);
        z=zeros([obshead(4),1]);
        Z0=zeros([obshead(4),1]);
        s=zeros([obshead(4),1]);
        H=zeros([obshead(4),4]);
        X0=[0;0;0;0];%近似值
        number =0;%迭代次数
        X=[10;10;10;1];%解算的值（第一次无解算值先随便给一个）
        for i=1:obshead(4)
            x(i)=obsdata(i,1);
            y(i)=obsdata(i,2);
            z(i)=obsdata(i,3);
            Z0(i)=obsdata(i,4);%观测值
            pseudorange(i)=obsdata(i,5);
        end
        D=diag(pseudorange',0);%方差矩阵
        %迭代
        while abs(norm(X))>1
            for i=1:obshead(4)
                s(i)=sqrt((x(i)-X0(1))^2+(y(i)-X0(2))^2+(z(i)-X0(3))^2);%距离
                H(i,:)=[(X0(1)-x(i))/s(i),(X0(2)-y(i))/s(i),(X0(3)-z(i))/s(i),1];
            end
            t0=X0(4);%钟差
            Z=Z0-s-t0;
            X=inv(H'/D*H)*H'/D*Z;%新一次解算的结果
            th=norm(X);
            X1=X+X0;%新一次解算的xyz及钟差的值
            X0=X1;
            number=number+1;
        end
        %验后单位权中误差
        v=(H/(H'/D*H)*H'/D-D)*Z;
        a=sqrt(v'/D*v/(obshead(4)-4));
        %估计参数精度评定
        V1=inv(H'/D*H);%验前
        %if (a^2*(obshead(4)-4))
        V2=inv(H'/D*H)*(a^2);%验后
        %% 数据输出
        outhead(num,:)=[num,obshead(2),obshead(3),obshead(4)];
        fprintf(fop,'# %3d %3d %10.3f %14.3f\r\n ',outhead(num,:));
        fprintf(fop,'%16s %16s %16s %16s\r\n', 'X(m)', 'Y(m)', 'Z(m)','T(m)');
        outdata(num,:)=[X1(1),X1(2),X1(3),X1(4)];
        fprintf(fop,'%14.3f %14.3f %14.3f %14.3f\r\n',outdata(num,:));
        fprintf(fop,'验后单位权中误差: %10.3f\r\n ',a);%验前单位权中误差是1
        fprintf(fop,'验前估计方差：\r\n');
        for i=1:4
            outnum=V1(i,:);
            fprintf(fop,'%14.3f %14.3f %14.3f %14.3f\r\n',outnum);
        end
        fprintf(fop,'验后估计方差：\r\n');
        for i=1:4
            outnum=V2(i,:);
            fprintf(fop,'%14.3f %14.3f %14.3f %14.3f\r\n',outnum);
        end
        
        %% 清空数据，准备读取下一历元
        obsdata = obsdata * 0; %数据清空，读取下一历元
    end
    disp('process over');
end
fclose(fid); fclose(fop);


%计算标准差和均值
for i=1:num
    numX=numX+outdata(i,1);
    numY=numY+outdata(i,2);
    numZ=numZ+outdata(i,3);
    numT=numT+outdata(i,4);
end
meanX=numX/num;
meanY=numY/num;
meanZ=numZ/num;
meanT=numT/num;

for i=1:num
    sumX=sumX+(outdata(i,1)-meanX)^2;
    sumY=sumY+(outdata(i,2)-meanY)^2;
    sumZ=sumZ+(outdata(i,3)-meanZ)^2;
    sumT=sumT+(outdata(i,4)-meanT)^2;
end
stdX=sqrt(sumX/num);
stdY=sqrt(sumY/num);
stdZ=sqrt(sumZ/num);
stdT=sqrt(sumT/num);

%表格展示均值和标准差
name={'X坐标';'Y坐标';'Z坐标';'钟差T'};
mean={meanX;meanY;meanZ;meanT};
std={stdX;stdY;stdZ;stdT};
T=table(name,mean,std);
writetable(T, 'xyzTable.xlsx');

%%WGS84坐标系转换到站心坐标系
%站心坐标系的原点坐标
orgxyz(1)=-1.13291501648681e+06;
orgxyz(2)=6.09252850388968e+06;
orgxyz(3)=1.50463316777129e+06;
enu=zeros(num,3);
numx=0;numy=0;numz=0;
rmsx=0;rmsy=0;rmsz=0;

for i=1:num
    %xyz(i,1:3)=outdata(i,1:3);
    xyz=outdata(i,1:3);
    enu(i,:) = xyz2enu(xyz,orgxyz);
    EX(i)=enu(i,1);numx=numx+EX(i);rmsx=rmsx+EX(i)^2;
    EY(i)=enu(i,2);numy=numy+EY(i);rmsy=rmsy+EY(i)^2;
    EZ(i)=enu(i,3);numz=numz+EZ(i);rmsz=rmsz+EZ(i)^2;
    data(i,:)=outhead(i,2:3);
    data_series1(i,1:6)=gps2cal(data(i,:));%时间转换,gps时转换成公历时
end
%计算站心坐标系下的均值和rms
meanx=numx/num;
meany=numy/num;
meanz=numz/num;
rmsX=sqrt(rmsx/num);
rmsY=sqrt(rmsy/num);
rmsZ=sqrt(rmsz/num);

%表格展示结果
name={'E坐标';'N坐标';'U坐标'};
mean={meanx;meany;meanz};
rms={rmsX;rmsY;rmsZ};
T1=table(name,mean,rms);
writetable(T1, 'enuTable.xlsx');

data_series=datenum(data_series1);%生成时间序列

%绘制时间序列图
figure;
hold on;
px=plot(data_series,EX);
meanx_vector = meanx * ones(size(data_series));
plot(data_series,meanx_vector);
datetick('x','HH/MM/SS');
title("站心坐标E的时间序列图(2021.8.10)");
xlabel("小时/分/秒");
ylabel("E");
legend('E','mean');
hold off;

figure;
hold on;
py=plot(data_series,EY);
meany_vector = meany * ones(size(data_series));
plot(data_series,meany_vector);
datetick('x','HH/MM/SS');
title("站心坐标N的时间序列图(2021.8.10");
xlabel("小时/分/秒");
ylabel("N");
legend('N','mean');
hold off;

figure;
hold on;
pz=plot(data_series,EZ);
meanz_vector = meanz * ones(size(data_series));
plot(data_series,meanz_vector);
datetick('x','HH/MM/SS');
title("站心坐标U的时间序列图(2021.8.10)");
xlabel("小时/分/秒");
ylabel("U");
legend('U','mean');
hold off;

%绘制散点图
figure;
scatter(EX,EY,"filled");
xlabel("E/m");
ylabel("N/m");
zlabel("U/m");
title("站心坐标系下的散点图");

A=[1,2,3;4,6,8;9,12,14];
inv(A);