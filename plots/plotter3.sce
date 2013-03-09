function [M2]=process_result(filename, s1, s2, s3, s4)
    M = csvRead(filename, ";")
    filename2 = strsubst(filename,".txt","")
    filename2 = strsubst(filename2,"data/results_","")
    fontS = 3;
    fontS1 = 3;
    
    disp(filename);
    M1 = M;
    M1(isnan(M1)) = 60000;
    disp("total times:");
    disp(sum(M1, 1)/1000/60/60);
    disp(sum(M1)/1000/60/60);
    
    
    //disp(M1)
    M2=zeros(10,3)
    for i=0:1:9
        M2(i+1,:) = sum(M1(i*100+1:(i+1)*100, :), 1);
    end
    disp(M2)
    disp(sum(M2,1)/1000/60/60)
    
    subplot(s1,s2,s3)
    plot([20, 40, 60, 80, 100, 200, 400, 600, 800, 1000], M2/1000/100);
    xtitle(filename2, 'število vozlišč', 'čas [s]');
    legend(['ullmann', 'vf2+subsea', 'vf2'], 2);
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.title.font_size=fontS1;
    
    subplot(s1,s2,s4)
    plot([20, 40, 60, 80, 100, 200, 400, 600, 800, 1000], M2/1000/100,"x-");
    xtitle(filename2, 'število vozlišč', 'čas [s]');
    legend(['ullmann', 'vf2+subsea', 'vf2'], 2);
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.title.font_size=fontS1;
    a.log_flags="ll";

    
endfunction


figure(1);
clf();
f = gcf();
f.figure_size=[1100, 1500]
f.background=-2;
m1=process_result("data/results_si2_r001.txt", 3, 2, 1, 2)
m2=process_result("data/results_si2_r005.txt", 3, 2, 3, 4)
m3=process_result("data/results_si2_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
xs2png(f,"results2_si2");
//xs2pdf(f,"results2_si2");
    
    
figure(2)
clf();
f = gcf();
f.figure_size=[1100, 1500]
f.background=-2;
m4=process_result("data/results_si4_r001.txt", 3, 2, 1, 2)
m5=process_result("data/results_si4_r005.txt", 3, 2, 3, 4)
m6=process_result("data/results_si4_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
xs2png(f,"results2_si4");
//xs2pdf(f,"results2_si4");

figure(3)
clf();
f = gcf();
f.figure_size=[1100, 1500]
f.background=-2;
m7=process_result("data/results_si6_r001.txt", 3, 2, 1, 2)
m8=process_result("data/results_si6_r005.txt", 3, 2, 3, 4)
m9=process_result("data/results_si6_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
xs2png(f,"results2_si6");
//xs2pdf(f,"results2_si6");




