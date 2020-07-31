x = importdata("X.txt");
x_e = importdata("Xe.txt");

y = importdata("Y.txt");
y_e = importdata("Ye.txt");

a = importdata("A.txt");
a_e = importdata("Ae.txt");

s = importdata("S.txt");
s_e = importdata("Se.txt");


t = 0:1:500;


%figure(1);
subplot(4,1,1);
plot(x, "b");
hold on
title("X");
plot(x_e, "g");
hold off

%figure(2);
subplot(4,1,2);
plot(y, "b");
hold on
title("Y");
plot(y_e, "g");
hold off

%figure(3);
subplot(4,1,3);
plot(a, "b");
hold on
title("A");
plot(a_e, "g");
hold off

%figure(4);
subplot(4,1,4);
plot(s, "b");
hold on
title("S");
plot(s_e, "g");
hold off
