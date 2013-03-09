function []=process_result(filename)
    M = csvRead(filename, ";")
    fontS = 4;
    
    disp(filename);
    M1 = M;
    M1(isnan(M1)) = 60000;
    disp("total times:");
    disp(sum(M1, 1)/1000/60/60);
    disp(sum(M1)/1000/60/60);
    
    f=gcf();
    f.figure_size=[1100, 650]
    f.background=-2;
    
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
    
    clf;
    subplot(1,2,1)
    plot([1:N]/N*60,x);
    //xtitle(filename, 'čas [s]', 'št. rešenih primerov');
    xtitle("", 'čas [s]', 'št. rešenih primerov');
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.margins = [0.15, 0.05, 0.125, 0.125]
    
    subplot(1,2,2)
    plot("ln",[1:N]/N*60,x);
    //xtitle(filename, 'čas [s]', 'št. rešenih primerov');
    xtitle("", 'čas [s]', 'št. rešenih primerov');
    a=gca();
    a.font_size = fontS;
    a.x_label.font_size=fontS;
    a.y_label.font_size=fontS;
    a.margins = [0.15, 0.05, 0.125, 0.125]
    a.log_flags="ln";// (l=) log scale on y axis

    legend(['ullmann', 'vf2+subsea', 'vf2'], 4);
    
    xs2png(f,strsubst(filename,".txt",""));
    xs2svg(f,strsubst(filename,".txt",""));
    xs2pdf(f,strsubst(filename,".txt",""));
endfunction


figure(1)
process_result("diploma/data/results_si2_r001.txt")
figure(2)
process_result("diploma/data/results_si2_r005.txt")
figure(3)
process_result("diploma/data/results_si2_r01.txt")

figure(4)
process_result("diploma/data/results_si4_r001.txt")
figure(5)
process_result("diploma/data/results_si4_r005.txt")
figure(6)
process_result("diploma/data/results_si4_r01.txt")

figure(7)
process_result("diploma/data/results_si6_r001.txt")
figure(8)
process_result("diploma/data/results_si6_r005.txt")
figure(9)
process_result("diploma/data/results_si6_r01.txt")



//M = csvRead()

//subplot(3,3,1);


