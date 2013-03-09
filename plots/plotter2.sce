function []=process_result(filename, s1, s2, s3, s4)
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
       
    T=5000
    N=6000;
    fact=T/N
    for j=1:1:size(M,2)
        x1 = [1:N];
        for i=1:1:N
            x1(i) = sum(M(:,j) < i*fact);
        end
        if j == 1 then
            x = x1;
        else
            x = [x; x1];
        end
    end
    
    subplot(s1,s2,s3)
    plot([1:N]/N*60,x);
    //xtitle(filename, 'čas [s]', 'št. rešenih primerov');
    xtitle(filename2, 'čas [s]', 'št. rešenih primerov');
    legend(['ullmann', 'vf2+subsea', 'vf2'], 4);
    
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.title.font_size=fontS1;
    a.margins = [0.15, 0.05, 0.125, 0.15]
    
    subplot(s1,s2,s4)
    plot("ln",[1:N]/N*60,x);
    //xtitle(filename, 'čas [s]', 'št. rešenih primerov');
    xtitle(filename2, 'čas [s]', 'št. rešenih primerov');
    legend(['ullmann', 'vf2+subsea', 'vf2'], 4);
    
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.title.font_size=fontS1;
    a.margins = [0.15, 0.05, 0.125, 0.15]
    a.log_flags="ln";// (l=) log scale on y axis


    
    //xs2png(f,strsubst(filename,".txt",""));
    //xs2pdf(f,strsubst(filename,".txt",""));
endfunction


figure(1);
clf();
f = gcf();
process_result("data/results_si2_r001.txt", 3, 2, 1, 2)
process_result("data/results_si2_r005.txt", 3, 2, 3, 4)
process_result("data/results_si2_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
f.background=-2;
f.figure_size=[1100, 1500];
xs2png(f,"results_si2");
//xs2pdf(f,"results_si2");
    
    
figure(2)
clf();
f = gcf();
process_result("data/results_si4_r001.txt", 3, 2, 1, 2)
process_result("data/results_si4_r005.txt", 3, 2, 3, 4)
process_result("data/results_si4_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
f.background=-2;
xs2png(f,"results_si4");
//xs2pdf(f,"results_si4");

figure(3)
clf();
f = gcf();
process_result("data/results_si6_r001.txt", 3, 2, 1, 2)
process_result("data/results_si6_r005.txt", 3, 2, 3, 4)
process_result("data/results_si6_r01.txt", 3, 2, 5, 6)
f.figure_size=[1100, 1500];
f.background=-2;
xs2png(f,"results_si6");
//xs2pdf(f,"results_si6");




