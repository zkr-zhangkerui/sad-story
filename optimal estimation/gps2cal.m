function cal=gps2cal(gpst)
% gps2cal	��GPS�ܺ����ڵ���ת��������GPSʱ��
%  cal=gps2cal(week,sec)  ���صĹ�����1x6����6�зֱ�Ϊ������ʱ����
%  gpst��1x2����2�зֱ�ΪGPS�ܺ����ڵ���

% GPS��MJD44244��ʼ
mjd=44244+(gpst(1)*86400*7+gpst(2))/86400;
cal=mjd2cal(mjd);
