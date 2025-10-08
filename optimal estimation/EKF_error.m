clc;
clear all;

%%模型的诊断与适应 在每个历元增加构建假设检验量进行诊断
% 自定义 F 分布的累积分布函数
function cdf = cdf_f(x, df1, df2)
    cdf = betainc((df1 * x) / (df1 * x + df2), df1 / 2, df2 / 2);
end
function iserror=diagnosis(V,obsdata,D0)
    iserror=0;
    alpha=0.1;%设置误警概率为0.5%
    t=V'*inv(D0)*V/sum(any(obsdata,2));
    %大分母自由度的F分布近似
    d2 = 1e6;% 分母自由度设为1e6
    % 手动计算 F 分布的临界值
    th = fzero(@(x) cdf_f(x, sum(any(obsdata,2)), d2) - (1 - alpha), [0, 100]);
    if t>th
        iserror=-1;
    end
end

%每个历元进行解算
function [X0,D0,Dx,V,H,D_delat]=EKF(obshead,obsdata,satid,C,var_e,state_change,X0,D0,I,A)
    true_num=sum(any(obsdata,2));
    %假设一
    H=zeros(true_num,8);%设计矩阵
    Z0=zeros(true_num,1);%代入近似值后的计算结果%
    V=zeros(true_num,1);%残差
    %Z=zeros(obshead(3),1);
    p0=zeros(true_num,1);K=zeros(8,true_num);
    D_delat=zeros(true_num,true_num);

    f1=@(t) ((I+A*(obshead(2)+1-t))*C*var_e*C'*(I+A*(obshead(2)+1-t))');
    Dw=integral(f1,obshead(2),obshead(2)+1,'ArrayValued', true);
    X=state_change*X0;
    Dx=state_change*D0*state_change'+Dw;
    for i=1:true_num
        %观测噪声矩阵
        D_delat(i,i)=obsdata(i,5);
        % 测量更新所用矩阵
        p0(i)=sqrt((X(1)-obsdata(i,1))^2+(X(2)-obsdata(i,2))^2+(X(3)-obsdata(i,3))^2);
        Z0(i)=p0(i)+X(4)+X(5);
        H(i,1)=(X(1)-obsdata(i,1))/p0(i);
        H(i,2)=(X(2)-obsdata(i,2))/p0(i);
        H(i,3)=(X(3)-obsdata(i,3))/p0(i);
        H(i,4)=1;
        if satid{i}(1)=='G'
            H(i,5)=0;
        else
            H(i,5)=1;
        end
        % 假设一：
        H(i,6)=0;H(i,7)=0;H(i,8)=0;
        %假设二：
        %假设三：
        %H(i,6)=0;H(i,7)=0;H(i,8)=0;H(i,9)=0;H(i,10)=0;
    end
    
    V=obsdata(1:true_num,4)-Z0;
    K=Dx*H'*inv(H*Dx*H'+D_delat);
    X0=X+K*V;
    D0=(I-K*H)*Dx*(I-K*H)'+K*D_delat*K';
end

MAXOBS=60;
obsdata=zeros([MAXOBS,5]);
satid=nan([MAXOBS,1]);
dX=zeros([5,1]);
satid={};
alpha=0.1;

%历元差
delta_t=1;
%输出矩阵 llh+time+钟差+ISB
X1=zeros(9,1);
X1_xyz=zeros(3,1);
X1_enu=zeros(3,1);
%历元数
num=0;
%异常值个数
error_num=0;


% % %假设一：状态矩阵 位置 钟差 ISB 速度 速度，钟差和ISB服从随机游走/一阶的高斯马尔可夫
% %X0=zeros([8,1]);
X0=[-2161300;4382200;4085800;10;20;100;100;100];
D0=diag([10,10,10,10,10,100,100,100]);
%系统噪声
var_e=diag([100,100,100,100,100]);%分别为钟差，系统偏差，速度（xyz）的白噪声方差
C=zeros(8,5);
C(4,1)=1;C(5,2)=1;C(6,3)=1;C(7,4)=1;C(8,5)=1;
A=zeros(8,8);A(1,6)=1;A(2,7)=1;A(3,8)=1;A(4,4)=-1;A(5,5)=-1;%A(6,6)=-1;A(7,7)=-1;A(8,8)=-1;
%状态转移矩阵
I=diag(ones(8,1));
state_change=zeros(8,8);
state_change=I+A*delta_t;

% % 假设二：状态矩阵 位置 钟差 ISB 均认为只有白噪声 
% 初始值
% X0=[-2161300;4382200;4085800;10;20];
% D0=diag([10,10,10,100,100]);
% %系统噪声
% var_e=diag([100,100,100,100,100]);%分别为位置 钟差 ISB的白噪声方差
% C=zeros(5,5);
% C(1,1)=1;C(2,2)=1;C(3,3)=1;C(4,4)=1;C(5,5)=1;
% A=zeros(5,5);%A(4,4)=-1;A(5,5)=-1;
% %状态转移矩阵
% I=diag(ones(5,1));
% state_change=zeros(5,5);
% state_change=I+A*delta_t;

%假设三：状态矩阵 位置 钟差 速度 钟速 均是一阶高斯马尔可夫
% %e,n,u方向经度较好 但是钟差发散
% %X0=zeros([8,1]);
% X0=[-2161300;4382200;4085800;10;20;10;10;10;10;10];
% D0=diag([10,10,10,10,10,100,100,100,100,100]);
% %系统噪声
% var_e=diag([100,100,100,100,100]);%分别为速度（xyz）,钟速的白噪声方差
% C=zeros(10,5);
% C(6,1)=1;C(7,2)=1;C(8,3)=1;C(9,4)=1;C(10,5)=1;
% A=zeros(10,10);
% A(1,6)=-1;A(2,7)=-1;A(3,8)=-1;A(4,9)=-1;A(5,10)=-1;
% %状态转移矩阵
% I=diag(ones(10,1));
% state_change=zeros(10,10);
% state_change=I+A*delta_t;

% 数据文件的路径
%filename='E:\新建文件夹\大二下\最优估计\异常数据的探测和剔除\100COBS_1.txt'; 
filename='E:\新建文件夹\大二下\最优估计\异常数据的探测和剔除\100COBS_2.txt';
outfile='E:\新建文件夹\大二下\最优估计\OutPos.txt';
outfile1='E:\新建文件夹\大二下\最优估计\OutPos1.txt';

fid=fopen(filename,'r');
fop=fopen(outfile,'w+');
fop1=fopen(outfile1,'w+');

disp(filename);
if(fid==-1 || fop==-1||fop1==-1)
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
        num=num+1;
       
        [X0,D0,Dx,V,H,D_delat]=EKF(obshead,obsdata,satid,C,var_e,state_change,X0,D0,I,A);

        %模型的诊断
        time=0;%迭代次数
        Dv=H*Dx*H'+D_delat;
        iserror=diagnosis(V,obsdata,Dv);
        while (iserror)<0&&(time<obshead(3))
        %if iserror<0
            time=time+1;
            % if time>5
            %     alpha=0.3;
            % end
            error_num=error_num+1;
            disp(time);
            Ci=zeros(sum(any(obsdata,2)),1);
            Ti=zeros(sum(any(obsdata,2)),1);
            %假设只有一个异常
            for i=1:sum(any(obsdata,2))
                Ci(i)=1;
                ti=abs(V(i))/sqrt(Dv(i,i));
                %ti=abs(Ci'*inv(Dv)*V/sqrt(Ci'*inv(Dv)*Ci));
                cdf_normal = @(z) 0.5 * (1 + erf(z / sqrt(2)));
                thi = fzero(@(z) cdf_normal(z) - (1 - alpha), [0, 100]);
                if ti>thi
                    Ti(i)=ti;
                end
            end
            if sum(Ti)==0%没有检测到异常
                %disp(num);
                iserror=0;
            else
                %obsdata(find(Ti(:,1)==max(Ti)),:)=[];
                obsdata(find(Ti(:,1)==max(Ti)),5)=1e6;
                [X0,D0,Dx,V,H,D_delat]=EKF(obshead,obsdata,satid,C,var_e,state_change,X0,D0,I,A);
                Dv=H*Dx*H'+D_delat;
                iserror=diagnosis(V,obsdata,Dv);
            end

        end
        %将结果转化为经纬度,enu
        X1(1:3,num)=xyz2llh(X0(1:3,1));
        X1(4,num)=obshead(2);
        X1(5,num)=X0(4);
        X1(6,num)=X0(5);
        X1(7,num)=X0(6);
        X1(8,num)=X0(7);
        X1(9,num)=X0(8);
        X1_xyz(:,num)=llh2xyz(X1(1:3,num));

        %% 数据输出
        outdata=[obshead(1),obshead(2),X1(1,num),X1(2,num),X1(3,num),X0(4),X0(5)];
        %fprintf(fop,'%d %10.3f %14.3f %14.3f %14.3f %9.3f %9.3f\r\n',outdata);
        fprintf(fop1,'%d %10.3f %14.3f %14.3f %14.3f %9.3f %9.3f\r\n',outdata);
        %% 清空数据，准备读取下一历元
        obsdata = obsdata * 0; %数据清空，读取下一历元
    end
    disp('process over');
end
fclose(fid); fclose(fop);

%% compare refernce result with output.txt
%读取参考值
llh_true=zeros(size(X1,2),5);
xyz_true=zeros(size(X1,2),3);
enu_true=zeros(size(X1,2),3);
Num=0;
filename1='E:\新建文件夹\大二下\最优估计\异常数据的探测和剔除\100C3640.19o.gnss';
fid=fopen(filename1,'r');
disp(filename);
if(fid==-1 || fop==-1)
    disp('can not find or open the file: %')
    'wrong';
else
    while ~feof(fid)
        tline=fgetl(fid);
        % 预处理字符串：替换逗号为空格，去除多余分隔符
        tline = strrep(tline, ',', ' ');       % 处理逗号分隔符
        tline = regexprep(tline, '\s+', ' ');  % 合并多个空格
        truedata=sscanf(tline(1:end),"%f",[1,5]);
        if (mod((truedata(2)-108899),1)==0)&&(truedata(2)>=108899)
            Num=Num+1;
            llh_true(Num,:)=sscanf(tline(1:end),"%f",[1,5]);
            index=find(X1(4,:)==llh_true(Num,2));
            xyz_true(Num,:)=llh2xyz(llh_true(Num,3:5));
            if isempty(index)
                continue;
            end
            X1_enu(:,Num)=xyz2enu(X1_xyz(1:3,index)',xyz_true(Num,:));
        end
    end
end

%以真值为基站坐标转换到enu
%绘图比较结算结果
%钟差随时间变化图
figure;
subplot(2,1,1);
plot(X1(5,:));
title('钟差');
xlabel('历元数');
ylabel('dt/m');

subplot(2,1,2);
plot(X1(6,:));
title('ISB');
xlabel('历元数');
ylabel('ISB/m');

%enu方向随时间变化图
figure;
subplot(3, 1, 1);
plot(X1_enu(1,:));
%ylim([40,40.1]);
title('东');
xlabel('时间/s');
ylabel('E/m');
hold off;

subplot(3, 1, 2);
plot(X1_enu(2,:));
%ylim([116.1,116.2]);
title('北');
xlabel('历元数');
ylabel('N/m');
hold off;

subplot(3, 1, 3); 
plot(X1_enu(3,:));
%ylim([-100,100]);
title('天');
xlabel('历元数');
ylabel('U/m');
hold off;

%计算rms
sum_l1=0;sum_l2=0;sum_h=0;sum_e=0;sum_n=0;sum_u=0;
rms_l1=0;rms_l2=0;rms_h=0;rms_e=0;rms_n=0;rms_u=0;
error=0;
for i=1:size(X1,2)
    index=find(X1(4,:)==llh_true(i,2));
    if isempty(index)
        continue;
    end
    sum_l1=sum_l1+(llh_true(i,3)-X1(1,index))^2;
    sum_l2=sum_l2+(llh_true(i,4)-X1(2,index))^2;
    sum_h=sum_h+(llh_true(i,5)-X1(3,index))^2;
    sum_e=sum_e+(X1_enu(1,index))^2;
    sum_n=sum_n+(X1_enu(2,index))^2;
    sum_u=sum_u+(X1_enu(3,index))^2;
end
rms_l1=sqrt(sum_l1/num);
rms_l2=sqrt(sum_l2/num);
rms_h=sqrt(sum_h/num);
rms_e=sqrt(sum_e/num);
rms_n=sqrt(sum_n/num);
rms_u=sqrt(sum_u/num);

%表格展示rms
name={'E';'N';'U'};
rms={rms_e;rms_n;rms_u};
T=table(name,rms);
writetable(T, 'rms_3.xlsx');

I=diag([1,1,1]);C=[1;1;1];var_e=1;
f1=@(t) (I*C*var_e*C'*I');
Dw=integral(f1,0,1,'ArrayValued', true);